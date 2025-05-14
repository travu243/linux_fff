#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t block_set, pending_set;

    // Step 1: Initialize the signal set and add SIGINT to it
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    // Step 2: Block SIGINT
    if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("SIGINT is now blocked. Try pressing Ctrl+C...\n");

    // Give user time to press Ctrl+C
    sleep(5);

    // Step 3: Check if SIGINT is pending
    if (sigpending(&pending_set) < 0) {
        perror("sigpending");
        exit(EXIT_FAILURE);
    }

    if (sigismember(&pending_set, SIGINT)) {
        printf("SIGINT is pending. Exiting program.\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("No pending SIGINT. Continuing program.\n");
    }

    // Optional: Unblock SIGINT if needed
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    return 0;
}
