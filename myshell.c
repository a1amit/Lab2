#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>
#include "LineParser.h"

#define MAX_INPUT_SIZE 2048

void printPrompt();
cmdLine *parseInput(char *input);
void execute(cmdLine *pCmdLine);

int main()
{
    while (1)
    {
        printPrompt();

        char input[MAX_INPUT_SIZE];
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Remove newline character if present
        size_t input_len = strlen(input);
        if (input_len > 0 && input[input_len - 1] == '\n')
        {
            input[input_len - 1] = '\0';
        }

        // Check if the command is "quit"
        if (strcmp(input, "quit") == 0)
        {
            printf("Exiting shell normally.\n");
            break;
        }

        // Parse the input
        cmdLine *parsedCmd = parseCmdLines(input);
        if (parsedCmd == NULL)
        {
            fprintf(stderr, "Error parsing command line.\n");
            exit(EXIT_FAILURE);
        }

        // Execute the command
        execute(parsedCmd);

        // Free resources
        freeCmdLines(parsedCmd);
    }

    return 0;
}

void printPrompt()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s$ ", cwd);
    }
    else
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
}

void execute(cmdLine *pCmdLine)
{
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0)
    {
        // Child process
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        int status;
        waitpid(child_pid, &status, 0);

        if (status == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
}
