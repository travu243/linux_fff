#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    sigset_t block_set, pending_set;

    // init signal set
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);

    // block SIGINT
    if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("SIGINT is blocked\n");

    // time to press ctrl+c
    sleep(5);

    // check if SIGINT is pending
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

    // unblock SIGINT if needed
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    return 0;
}
