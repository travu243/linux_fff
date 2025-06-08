#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t sigint_count = 0;

// handler ctrl + c
void handle_sigint(int signum) {
    sigint_count++;
    printf("SIGINT received (%d time(s)).\n", sigint_count);

    if (sigint_count >= 3) {
        printf("received SIGINT 3 times. exiting\n");
        exit(0);
    }
}

int main() {
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    //sa.sa_flags = SA_SIGINFO;

    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction fail !!\n");
    }

    printf("SIGINT is blocked\n");
    printf("ctrl + c 3 times to exit\n");

    printf("running\n");

    while (1) {}

    return 0;
}
