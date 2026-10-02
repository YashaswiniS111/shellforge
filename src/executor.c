#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"

int execute_pipeline(Token tokens[], int count)
{
    int command_count = 1;
    int background = 0;

    /*
     * Count commands and check for background execution.
     */
    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_PIPE)
        {
            command_count++;
        }

        if (tokens[i].type == TOKEN_BACKGROUND)
        {
            background = 1;
        }
    }

    /*
     * A pipeline with N commands needs N-1 pipes.
     */
    int pipes[MAX_TOKENS][2];
    pid_t pids[MAX_TOKENS];

    /*
     * Create required pipes.
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
     * Create one child for every command.
     */
    while (command_index < command_count)
    {
        char *args[MAX_TOKENS];
        int argc = 0;

        /*
         * Collect arguments until PIPE, BACKGROUND, or END.
         */
        while (token_index < count &&
               tokens[token_index].type != TOKEN_PIPE &&
               tokens[token_index].type != TOKEN_BACKGROUND &&
               tokens[token_index].type != TOKEN_END)
        {
            args[argc++] = tokens[token_index].value;
            token_index++;
        }

        args[argc] = NULL;

        /*
         * Empty command check.
         */
        if (argc == 0)
        {
            fprintf(stderr, "shellforge: empty command\n");

            for (int i = 0; i < command_count - 1; i++)
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

            for (int i = 0; i < command_count - 1; i++)
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
             * Connect stdin from previous pipe.
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
             * Connect stdout to next pipe.
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
             * Close all pipe descriptors in child.
             */
            for (int i = 0; i < command_count - 1; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            /*
             * Execute external command.
             */
            execvp(args[0], args);

            perror("execvp");
            exit(127);
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
     * Parent closes all pipes.
     */
    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /*
     * Background commands must not be waited for.
     */
    if (background)
    {
        printf("[background] process started: %d\n", pids[command_count - 1]);
        return 0;
    }

    /*
     * Foreground command: wait for every child.
     */
    int last_status = 0;

    for (int i = 0; i < command_count; i++)
    {
        int status;

        if (waitpid(pids[i], &status, 0) < 0)
        {
            perror("waitpid");
            last_status = 1;
            continue;
        }

        /*
         * Return status of the last command.
         */
        if (i == command_count - 1)
        {
            if (WIFEXITED(status))
            {
                last_status = WEXITSTATUS(status);
            }
            else
            {
                last_status = 1;
            }
        }
    }

    return last_status;
}

int execute_external(Token tokens[], int count)
{
    return execute_pipeline(tokens, count);
}
