#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Signal handler function
void handle_signal(int sig) {
    switch (sig) {
        case SIGUSR1:
            printf("SIGUSR1: aaaaaaa\n");
            break;
        case SIGUSR2:
            printf("SIGUSR2: bye bye\n");
            exit(0);
            break;
        default:
            break;
    }
}

int main() {
    // Register signal handlers using signal()
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);

    printf("Process ID: %d\n", getpid());

    printf("Waiting for SIGUSR1 or SIGUSR2...\n");

    // Keep the program running
    while (1) {
        pause();  // Suspend until signal arrives
    }

    return 0;
}


