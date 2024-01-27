#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIDE 0
#define OUTPUT_SIDE 1
#define BUFFER_SIZE 80

int main(int argc, char *argv[])
{
    //used this site to understand the concept: https://tldp.org/LDP/lpg/node11.html#:~:text=To%20create%20a%20simple%20pipe,be%20used%20for%20the%20pipeline.
    int fd[2];
    char readbuffer[BUFFER_SIZE];
    pipe(fd);

    pid_t childpid = fork();
        
    if(childpid == -1)
    {
        perror("fork");
        exit(1);
    }

    if(childpid == 0)
    {
        /* Child process closes up input side of pipe */
        close(fd[INPUT_SIDE]);

        printf("Input child's message to parent: ");
        char* message =(char *)malloc(BUFFER_SIZE);
        fgets(message, BUFFER_SIZE, stdin);

        /* Send the message through the output side of pipe */
        write(fd[OUTPUT_SIDE], message, strlen(message) + 1);
        
        free(message);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        close(fd[OUTPUT_SIDE]);

        /* Read in a string from the pipe */
        if(read(fd[INPUT_SIDE], readbuffer, sizeof(readbuffer)) == -1) {
            perror("parent reading error");
            exit(1);
        }

        printf("Parent reads: %s", readbuffer);
    }
    
    exit(0);
}