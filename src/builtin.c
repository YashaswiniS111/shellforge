#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "builtin.h"
#include "jobs.h"

int is_builtin(const char *command)
{
    if (command == NULL)
        return 0;

    if (strcmp(command, "pwd") == 0)
        return 1;

    if (strcmp(command, "cd") == 0)
        return 1;

    if (strcmp(command, "echo") == 0)
        return 1;

    if (strcmp(command, "jobs") == 0)
        return 1;

    if (strcmp(command, "fg") == 0)
        return 1;

    if (strcmp(command, "bg") == 0)
        return 1;

    if (strcmp(command, "exit") == 0)
        return 1;

    return 0;
}

int builtin_pwd(Token tokens[], int count)
{
    (void)tokens;

    if (count > 2)
    {
        fprintf(stderr, "pwd: too many arguments\n");
        return 1;
    }

    char cwd[4096];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("pwd");
        return 1;
    }

    printf("Yashaswini\n");
    printf("%s\n", cwd);

    return 0;
}

int builtin_cd(Token tokens[], int count)
{
    if (count > 3)
    {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }

    const char *directory;

    if (count == 2)
    {
        directory = getenv("HOME");

        if (directory == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }
    else
    {
        directory = tokens[1].value;
    }

    if (chdir(directory) != 0)
    {
        perror("cd");
        return 1;
    }

    return 0;
}

int builtin_echo(Token tokens[], int count)
{
    for (int i = 1; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END)
            break;

        printf("%s", tokens[i].value);

        if (i + 1 < count &&
            tokens[i + 1].type != TOKEN_END)
        {
            printf(" ");
        }
    }

    printf("\n");

    return 0;
}

int builtin_jobs(Token tokens[], int count)
{
    (void)tokens;

    if (count > 2)
    {
        fprintf(stderr, "jobs: too many arguments\n");
        return 1;
    }

    jobs_print();

    return 0;
}

int builtin_fg(Token tokens[], int count)
{
    if (count != 3)
    {
        fprintf(stderr, "fg: usage: fg <job_id>\n");
        return 1;
    }

    int job_id = atoi(tokens[1].value);

    if (job_id <= 0)
    {
        fprintf(stderr, "fg: invalid job id\n");
        return 1;
    }

    job_t *job = job_find(job_id);

    if (job == NULL)
    {
        fprintf(stderr, "fg: job not found\n");
        return 1;
    }

    pid_t pgid = job->pgid;

    if (kill(-pgid, SIGCONT) < 0)
    {
        perror("fg");
        return 1;
    }

    job_continue(pgid);

    int status;

    if (waitpid(-pgid, &status, WUNTRACED) < 0)
    {
        perror("fg: waitpid");
        return 1;
    }

    if (WIFSTOPPED(status))
    {
        job_stop(pgid);
    }
    else if (WIFEXITED(status) || WIFSIGNALED(status))
    {
        job_done(pgid);
        job_remove(job_id);
    }

    return 0;
}

int builtin_bg(Token tokens[], int count)
{
    if (count != 3)
    {
        fprintf(stderr, "bg: usage: bg <job_id>\n");
        return 1;
    }

    int job_id = atoi(tokens[1].value);

    if (job_id <= 0)
    {
        fprintf(stderr, "bg: invalid job id\n");
        return 1;
    }

    job_t *job = job_find(job_id);

    if (job == NULL)
    {
        fprintf(stderr, "bg: job not found\n");
        return 1;
    }

    if (kill(-job->pgid, SIGCONT) < 0)
    {
        perror("bg");
        return 1;
    }

    job_continue(job->pgid);

    return 0;
}

int builtin_exit(Token tokens[], int count)
{
    (void)tokens;

    if (count > 2)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }

    exit(0);
}

int execute_builtin(Token tokens[], int count)
{
    if (count <= 0 || tokens[0].type == TOKEN_END)
        return 1;

    if (strcmp(tokens[0].value, "pwd") == 0)
        return builtin_pwd(tokens, count);

    if (strcmp(tokens[0].value, "cd") == 0)
        return builtin_cd(tokens, count);

    if (strcmp(tokens[0].value, "echo") == 0)
        return builtin_echo(tokens, count);

    if (strcmp(tokens[0].value, "jobs") == 0)
        return builtin_jobs(tokens, count);

    if (strcmp(tokens[0].value, "fg") == 0)
        return builtin_fg(tokens, count);

    if (strcmp(tokens[0].value, "bg") == 0)
        return builtin_bg(tokens, count);

    if (strcmp(tokens[0].value, "exit") == 0)
        return builtin_exit(tokens, count);

    return 1;
}
