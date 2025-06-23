#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define STACK_SIZE 1024*1024
sem_t sem;

int thread_func(void* arg) {
    printf("thread: hello\n");
    sleep(1);
    sem_post(&sem);  // signal done
    return 0;
}

int main() {
    void* stack = malloc(STACK_SIZE);
    sem_init(&sem, 1, 0);  // shared semaphore

    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
    clone(thread_func, stack + STACK_SIZE, flags, NULL);

    sem_wait(&sem);  // wait for thread to finish
    printf("Main: Thread has finished.\n");

    sem_destroy(&sem);
    free(stack);
    return 0;
}
