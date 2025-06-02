#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 6
#define MAX_CONCURRENT 3

sem_t semaphore;

void* thread_func(void* arg) {
    int id = *(int*)arg;

    printf("thread %d waiting...\n", id);
    sem_wait(&semaphore);

    printf("thread %d entered\n", id);
    sleep(2);

    printf("thread %d leaving\n", id);
    sem_post(&semaphore);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    sem_init(&semaphore, 0, MAX_CONCURRENT);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore);
    return 0;
}
