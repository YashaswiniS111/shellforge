#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"

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

    if (strcmp(command, "exit") == 0)
        return 1;

    return 0;
}

int builtin_pwd(Token tokens[], int count)
{
    (void)tokens;

    /*
     * count includes the TOKEN_END token.
     *
     * pwd
     * -> pwd, END
     * -> count = 2
     */
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

    printf("%s\n", cwd);

    return 0;
}

int builtin_cd(Token tokens[], int count)
{
    /*
     * count includes TOKEN_END.
     *
     * cd
     * -> cd, END
     * -> count = 2
     *
     * cd /
     * -> cd, /, END
     * -> count = 3
     *
     * Therefore count > 3 means too many arguments.
     */
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

int builtin_exit(Token tokens[], int count)
{
    (void)tokens;

    /*
     * exit
     * -> exit, END
     * -> count = 2
     */
    if (count > 2)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }

    exit(0);
}
