#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main() {
    sigset_t curr_mask;
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGINT);
    sigaddset(&block, SIGUSR1);

    // block SIGINT
    sigprocmask(SIG_BLOCK, &block, NULL);

    // get current mask
    if (sigprocmask(SIG_SETMASK, NULL, &curr_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("blocked signals:\n");
    for (int i = 1; i < NSIG; ++i) {
        if (sigismember(&curr_mask, i)) {
            printf(" - [%d] %s\n", i, strsignal(i));
        }
    }

    return 0;
}
