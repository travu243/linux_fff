#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_FILE "/tmp/spinlock_shm"

int main() {
    int fd = open(SHM_FILE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    void *addr = mmap(NULL, sizeof(pthread_spinlock_t),
                      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    pthread_spinlock_t *lock = (pthread_spinlock_t *)addr;

    printf("proc_b: trying to acquire spinlock...\n");
    pthread_spin_lock(lock);
    printf("proc_b: spinlock acquired!\n");

    pthread_spin_unlock(lock);
    printf("proc_b: spinlock released.\n");

    munmap(addr, sizeof(pthread_spinlock_t));
    close(fd);
    return 0;
}
