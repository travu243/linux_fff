#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* thread_function(void* arg) {
    int thread_num = *((int*)arg);
    printf("thread %d aaa\n", thread_num);

    pthread_t tid = pthread_self();
    printf("thread id: %lu\n", tid);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;
    int id1 = 1, id2 = 2;

    // thread 1
    if (pthread_create(&thread1, NULL, thread_function, &id1) != 0) {
        perror("failed 1");
        exit(EXIT_FAILURE);
    }

    // thread 2
    if (pthread_create(&thread2, NULL, thread_function, &id2) != 0) {
        perror("failed 2");
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // pthread_detach(thread1);
    // pthread_detach(thread2);

    printf("all finished.\n");

    return 0;
}

