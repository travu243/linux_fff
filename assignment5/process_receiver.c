#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler_initial(int signo) {
    printf("Received SIGUSR1 (blocked but queued)\n");
}

void handler_debug(int signo) {
    printf("SIGUSR1 received (not blocked)\n");
}

int main() {
    sigset_t block_set, old_set;

    printf("pid = %d\n", getpid());

    // initial handler
    signal(SIGUSR1, handler_initial);

    // mask block SIGUSR1
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);

    // Block SIGUSR1
    sigprocmask(SIG_BLOCK, &block_set, &old_set);
    printf("doing important work... SIGUSR1 is blocked.\n");

    sleep(10);  // do something

    // switch handler for debug
    printf("finished\n");
    signal(SIGUSR1, handler_debug);

    // Unblock SIGUSR1
    sigprocmask(SIG_SETMASK, &old_set, NULL);

    pause();
    return 0;
}
