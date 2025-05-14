#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t sigint_count = 0;

// Handler cho Ctrl + C
void handle_sigint(int signum) {
    sigint_count++;
    printf("SIGINT received (%d time(s)).\n", sigint_count);

    if (sigint_count >= 3) {
        printf("Received SIGINT 3 times. Exiting...\n");
        exit(0);
    }
}

int main() {
    // Đăng ký signal handler cho SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("SIGINT is blocked from terminating the program.\n");
    printf("Press Ctrl + C three times to exit.\n");

    printf("Running...\n");

    while (1) {}

    return 0;
}
