#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int data_ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* producer(void* arg) {
    sleep(1);
    pthread_mutex_lock(&mutex);
    printf("producer: produced data\n");
    data_ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (data_ready == 0) {
        printf("consumer: waiting for data...\n");
        pthread_cond_wait(&cond, &mutex);
    }
    printf("consumer: consumed data\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, consumer, NULL);
    pthread_create(&t2, NULL, producer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}

