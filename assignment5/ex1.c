#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t sigint_count = 0;

// handler Ctrl + C
void handle_sigint(int signum) {
    sigint_count++;
    printf("SIGINT received (%d).\n", sigint_count);

    if (sigint_count >= 3) {
        printf("received SIGINT 3 times. exit\n");
        exit(0);
    }
}

int main() {
    // sign up signal handler for SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("SIGINT is blocked\n");
    printf("ctrl + c 3 times to exit\n");

    printf("running...\n");

    while (1) {}

    return 0;
}
