#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <receiver_pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);

    printf("sending SIGUSR1 to pid %d\n", pid);
    kill(pid, SIGUSR1);
    return 0;
}
