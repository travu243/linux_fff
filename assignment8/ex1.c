#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 10
#define NUM_INCREMENTS 10000

int counter = 0; // Shared resource
pthread_mutex_t counter_mutex;

void* increment(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        pthread_mutex_lock(&counter_mutex);   // Lock before modifying
        counter++;
        pthread_mutex_unlock(&counter_mutex); // Unlock after modifying
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&counter_mutex, NULL); // Initialize mutex

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("with mutex \n");
    printf("final counter value: %d\n", counter);
    pthread_mutex_destroy(&counter_mutex); // Destroy mutex

    return 0;
}

