#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child working
        while (1) {
            printf("Child working...\n");
            sleep(1);
        }
    } else {
        sleep(5);
        kill(pid, SIGKILL);
        wait(NULL);
        printf("Child was killed due to timeout\n");
    }

    return 0;
}
