#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig);

int main(int argc, char **argv)
{
    printf("Starting the program\n");

    // Set signal handlers
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);

    while (1)
    {
        sleep(1);
    }

    return 0;
}

void handler(int sig)
{
    // Print the received signal with a message for debbugging purposes
    printf("\nReceived Signal: %s\n", strsignal(sig));

    // Set the default signal handler to handle the signal
    signal(sig, SIG_DFL);

    // Raise the signal again so that the default signal handler can handle it
    raise(sig);

    // After handling SIGCONT, reinstate the custom handler for SIGTSTP
    if (sig == SIGCONT)
    {
        signal(SIGTSTP, handler);
    }
    // After handling SIGTSTP, reinstate the custom handler for SIGCONT
    else if (sig == SIGTSTP)
    {
        signal(SIGCONT, handler);
    }
}
