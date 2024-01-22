#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>
#include "LineParser.h"
#include <fcntl.h>

#define MAX_INPUT_SIZE 2048

void printPrompt();
cmdLine *parseInput(char *input);
void execute(cmdLine *pCmdLine, int debugFlag);

int main(int argc, char *argv[])
{
    int debugFlag = 0; // Initialize the debug flag to 0 (disabled)

    // Check if the "-d" flag is provided
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        debugFlag = 1; // Enable debug mode
    }

    while (1)
    {
        printPrompt();

        char input[MAX_INPUT_SIZE];
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) // checks if the user entered anything at all
        {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Check if the command is "quit"
        if (strcmp(input, "quit\n") == 0)
        {
            break; // Exit the infinite loop
        }

        // Parse the input
        cmdLine *parsedCmd = parseCmdLines(input);

        // Execute the command
        execute(parsedCmd, debugFlag);

        // Free resources
        freeCmdLines(parsedCmd);
    }

    printf("Exiting shell normally.\n");
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

cmdLine *parseInput(char *input)
{
    cmdLine *parsedCmd = parseCmdLines(input);
    if (parsedCmd == NULL)
    {
        fprintf(stderr, "Error parsing command line.\n");
        exit(EXIT_FAILURE);
    }
    return parsedCmd;
}

void execute(cmdLine *pCmdLine, int debugFlag)
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
        if (debugFlag)
        {
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }

        // Redirect input if needed
        if (pCmdLine->inputRedirect != NULL)
        {
            int input_fd = open(pCmdLine->inputRedirect, O_RDONLY);
            if (input_fd == -1)
            {
                perror("open inputRedirect");
                _exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        // Redirect output if needed
        if (pCmdLine->outputRedirect != NULL)
        {
            int output_fd = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1)
            {
                perror("open outputRedirect");
                _exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        // Execute the command
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        {
            perror("execvp");
            _exit(EXIT_FAILURE); // _exit is used to terminate the process if execvp fails
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
