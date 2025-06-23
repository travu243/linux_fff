#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

void* thread_func(void* arg) {
    int core_id = *(int*)arg;

    printf("thread %ld is running on CPU %d\n", (long)pthread_self(),sched_getcpu());

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
    }
    else printf("thread %ld is seted for run on core %d\n",(long)pthread_self(),sched_getcpu());

    while (1) {
        printf("thread running on core %d\n", sched_getcpu());
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t t1;
    int core = 0;

    pthread_create(&t1, NULL, thread_func, &core);

    pthread_join(t1, NULL);

    return 0;
}
