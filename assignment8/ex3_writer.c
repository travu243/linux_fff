#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SHM_KEY  0x1234

int main() {
    // create shared memory
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // attach memory area to process area
    char *shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    // write mess to memory area
    const char *message = "hello aaaaaaaa";
    strncpy(shmaddr, message, SHM_SIZE);
    printf("writer: %s\n", message);

    // wait for reader read message
    printf("writer: Press Enter to detach and remove shared memory...\n");
    getchar();

    // detach memory and remove it
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL); // delete shared memory

    return 0;
}
