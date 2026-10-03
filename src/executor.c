#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "executor.h"
#include "jobs.h"

int execute_pipeline(Token tokens[], int count)
{
    int command_count = 1;
    int background = 0;

    /*
     * Count commands and detect background execution.
     */
    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_PIPE)
            command_count++;

        if (tokens[i].type == TOKEN_BACKGROUND)
            background = 1;
    }

    int pipes[MAX_TOKENS][2];
    pid_t pids[MAX_TOKENS];

    /*
     * Create pipes.
     */
    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    int command_index = 0;
    int token_index = 0;

    /*
     * Build a command string for the job table.
     */
    char command_string[256];
    command_string[0] = '\0';

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END ||
            tokens[i].type == TOKEN_BACKGROUND)
        {
            break;
        }

        if (tokens[i].type == TOKEN_PIPE)
        {
            strncat(command_string, "| ",
                    sizeof(command_string) -
                    strlen(command_string) - 1);
        }
        else
        {
            strncat(command_string, tokens[i].value,
                    sizeof(command_string) -
                    strlen(command_string) - 1);

            strncat(command_string, " ",
                    sizeof(command_string) -
                    strlen(command_string) - 1);
        }
    }

    /*
     * Remove trailing space.
     */
    size_t command_length = strlen(command_string);

    if (command_length > 0 &&
        command_string[command_length - 1] == ' ')
    {
        command_string[command_length - 1] = '\0';
    }

    /*
     * Create one child per command.
     */
    while (command_index < command_count)
    {
        char *args[MAX_TOKENS];
        int argc = 0;

        while (token_index < count &&
               tokens[token_index].type != TOKEN_PIPE &&
               tokens[token_index].type != TOKEN_BACKGROUND &&
               tokens[token_index].type != TOKEN_END)
        {
            args[argc++] = tokens[token_index].value;
            token_index++;
        }

        args[argc] = NULL;

        if (argc == 0)
        {
            fprintf(stderr,
                    "shellforge: empty command\n");

            for (int i = 0;
                 i < command_count - 1;
                 i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            return 1;
        }

        pids[command_index] = fork();

        if (pids[command_index] < 0)
        {
            perror("fork");

            for (int i = 0;
                 i < command_count - 1;
                 i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            return 1;
        }

        /*
         * Child process.
         */
        if (pids[command_index] == 0)
        {
            /*
             * Put all commands in the pipeline
             * into the same process group.
             */
            if (command_index == 0)
            {
                setpgid(0, 0);
            }
            else
            {
                setpgid(0, pids[0]);
            }

            /*
             * Connect stdin.
             */
            if (command_index > 0)
            {
                if (dup2(pipes[command_index - 1][0],
                         STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Connect stdout.
             */
            if (command_index < command_count - 1)
            {
                if (dup2(pipes[command_index][1],
                         STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Close pipe descriptors.
             */
            for (int i = 0;
                 i < command_count - 1;
                 i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            execvp(args[0], args);

            perror("execvp");
            exit(127);
        }

        /*
         * Parent places child in the same process group.
         */
        if (command_index == 0)
        {
            setpgid(pids[command_index],
                    pids[command_index]);
        }
        else
        {
            setpgid(pids[command_index],
                    pids[0]);
        }

        /*
         * Move past PIPE or BACKGROUND.
         */
        if (token_index < count &&
            (tokens[token_index].type == TOKEN_PIPE ||
             tokens[token_index].type == TOKEN_BACKGROUND))
        {
            token_index++;
        }

        command_index++;
    }

    /*
     * Parent closes pipes.
     */
    for (int i = 0;
         i < command_count - 1;
         i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    pid_t pgid = pids[0];

    /*
     * Background process.
     */
    if (background)
    {
        int job_id = job_add(pgid,
                             command_string,
                             JOB_RUNNING);

        if (job_id < 0)
        {
            /*
             * If the job table is full,
             * wait for the processes.
             */
            for (int i = 0;
                 i < command_count;
                 i++)
            {
                waitpid(pids[i], NULL, 0);
            }

            return 1;
        }

        printf("[%d] %d %s\n",
               job_id,
               pgid,
               command_string);

        return 0;
    }

    /*
     * Foreground process.
     */
    int last_status = 0;

    for (int i = 0;
         i < command_count;
         i++)
    {
        int status;

        if (waitpid(pids[i],
                    &status,
                    WUNTRACED) < 0)
        {
            perror("waitpid");
            last_status = 1;
            continue;
        }

        if (WIFSTOPPED(status))
        {
            job_add(pgid,
                    command_string,
                    JOB_STOPPED);

            printf("\n[stopped] %s\n",
                   command_string);

            last_status = 1;
        }
        else if (i == command_count - 1)
        {
            if (WIFEXITED(status))
                last_status = WEXITSTATUS(status);
            else
                last_status = 1;
        }
    }

    return last_status;
}

int execute_external(Token tokens[], int count)
{
    return execute_pipeline(tokens, count);
}
