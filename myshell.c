#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>
#include <signal.h>
#include "LineParser.h"

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
    if (strcmp(pCmdLine->arguments[0], "cd") == 0)
    {
        // Handle "cd" command separately
        if (chdir(pCmdLine->arguments[1]) == -1)
        {
            perror("chdir");
            fprintf(stderr, "Error: Unable to change directory to %s\n", pCmdLine->arguments[1]);
        }
    } else if(strcmp(pCmdLine->arguments[0], "nuke") == 0) {
        // Handle "nuke" command seperateley
        int pidToNuke = atoi(pCmdLine->arguments[1]);

        // send the signal to the process with wrapper kill
        if(kill((pid_t)pidToNuke, SIGKILL) == 0)
            fprintf(stdout, "Nuked process %d\n", pidToNuke);
        else 
            fprintf(stderr, "Error: Unable to nuke process %d\n", pidToNuke);
        
    } else if(strcmp(pCmdLine->arguments[0], "wakeup") == 0) {
        // Handle "wakeup" command seperateley
        int pidToWake = atoi(pCmdLine->arguments[1]);

        // send the signal to the process with wrapper kill
        if(kill((pid_t)pidToWake, SIGCONT) == 0)
            fprintf(stdout, "Woke up process %d\n", pidToWake);
        else 
            fprintf(stderr, "Error: Unable to wake up process %d\n", pidToWake);

    }
    else
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

            // Handle input redirection
            if (pCmdLine->inputRedirect != NULL)
            {
                FILE *inputFile = fopen(pCmdLine->inputRedirect, "r");
                if (inputFile == NULL)
                {
                    perror("fopen");
                    _exit(EXIT_FAILURE);
                }
                dup2(fileno(inputFile), STDIN_FILENO);
                fclose(inputFile);
            }

            // Handle output redirection
            if (pCmdLine->outputRedirect != NULL)
            {
                FILE *outputFile = fopen(pCmdLine->outputRedirect, "w");
                if (outputFile == NULL)
                {
                    perror("fopen");
                    _exit(EXIT_FAILURE);
                }
                dup2(fileno(outputFile), STDOUT_FILENO);
                fclose(outputFile);
            }

            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
            {
                perror("execvp");
                _exit(EXIT_FAILURE); // Use _exit to terminate the process if execvp fails
            }
        }
        else
        {
            // Parent process
            if (pCmdLine->blocking)
            {
                int status;
                waitpid(child_pid, &status, 0);

                if (status == -1)
                {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
