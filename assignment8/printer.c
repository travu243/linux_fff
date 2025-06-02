#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 15
#define NUM_PRINTERS 3
#define QUEUE_SIZE NUM_THREADS

// Queue đơn giản cho thread ID
int task_queue[QUEUE_SIZE];
int front = 0, rear = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printer_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t printer_sem;

int printer_status[NUM_PRINTERS] = {1, 1, 1}; // 1: free, 0: busy

void enqueue(int task_id) {
    task_queue[rear++] = task_id;
}

int dequeue() {
    return (front < rear) ? task_queue[front++] : -1;
}

int get_available_printer() {
    for (int i = 0; i < NUM_PRINTERS; i++) {
        if (printer_status[i] == 1) {
            printer_status[i] = 0;
            return i;
        }
    }
    return -1;
}

void release_printer(int printer_id) {
    printer_status[printer_id] = 1;
}

void* worker(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        int task_id = dequeue();
        pthread_mutex_unlock(&queue_mutex);

        if (task_id == -1)
            break;

        // wait for printer
        sem_wait(&printer_sem);

        pthread_mutex_lock(&printer_mutex);
        int printer_id = get_available_printer();
        pthread_mutex_unlock(&printer_mutex);

        printf("thread %d acquired printer-%d\n", task_id, printer_id + 1);

        sleep(rand() % 3 + 1); // time simulator

        printf("thread %d released printer-%d\n", task_id, printer_id + 1);

        pthread_mutex_lock(&printer_mutex);
        release_printer(printer_id);
        pthread_mutex_unlock(&printer_mutex);

        sem_post(&printer_sem); // free printer
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t threads[5]; // num of worker thread
    sem_init(&printer_sem, 0, NUM_PRINTERS);

    // push 15 task to queue
    for (int i = 0; i < NUM_THREADS; i++) {
        enqueue(i + 1);
    }

    // create worker thread
    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    // wait for threads finish
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&printer_sem);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&printer_mutex);

    printf("completed\n");
    return 0;
}
