#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 10
#define NUM_INCREMENTS 100000

int counter = 0; // Shared resource

void* increment(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        counter++; // No mutex used, race condition happens here
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("without mutex\n");
    printf("final counter value: %d\n", counter);

    return 0;
}
