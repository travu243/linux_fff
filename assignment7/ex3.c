#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int buffer = 0;
int data_ready = 0;

pthread_mutex_t lock;         // Mutex to protect buffer
pthread_cond_t cond;          // Condition variable

void* producer(void* arg) {
    sleep(1);

    pthread_mutex_lock(&lock);
    buffer = 1111; // Produce data
    data_ready = 1;
    printf("produced data = %d\n", buffer);
    pthread_cond_signal(&cond); // Signal the consumer
    pthread_mutex_unlock(&lock);

    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&lock);
    while (data_ready == 0) {
        pthread_cond_wait(&cond, &lock); // Wait until data is ready
    }
    printf("consumed data = %d\n", buffer);
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t prod, cons;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
