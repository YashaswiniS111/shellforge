#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"

int execute_external(Token tokens[], int count)
{
    pid_t pid;
    int status;

    char *args[MAX_TOKENS];
    int argc = 0;

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_END)
            break;

        args[argc++] = tokens[i].value;
    }

    args[argc] = NULL;

    if (argc == 0)
        return 0;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    return 0;
}
