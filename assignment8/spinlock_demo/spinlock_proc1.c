#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>

#define SHM_FILE "/tmp/spinlock_shm"

int main() {
    int fd = open(SHM_FILE, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    ftruncate(fd, sizeof(pthread_spinlock_t));

    void *addr = mmap(NULL, sizeof(pthread_spinlock_t),
                      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    pthread_spinlock_t *lock = (pthread_spinlock_t *)addr;
    pthread_spin_init(lock, PTHREAD_PROCESS_SHARED);

    printf("proc_a: locking spinlock for 10 seconds...\n");
    pthread_spin_lock(lock);
    printf("proc_a: spinlock acquired.\n");

    sleep(10);

    pthread_spin_unlock(lock);
    printf("proc_a: spinlock released.\n");

    munmap(addr, sizeof(pthread_spinlock_t));
    close(fd);
    return 0;
}
