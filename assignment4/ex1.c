#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);

    printf("pid %d\n", getpid());

    printf("wait SIGUSR1 or SIGUSR2\n");

    while (1) {
        pause();  // pause for signal
    }

    return 0;
}


