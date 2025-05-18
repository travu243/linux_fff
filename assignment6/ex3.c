#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Process started (PID: %d)\n", getpid());
    printf("running...\n");
//    for (int i = 0; i < 10; i++) {
//        printf("Working... %d\n", i + 1);
//        sleep(1);
//    }

    while(1);
//    printf("Process done\n");
    return 0;
}
