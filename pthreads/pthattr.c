#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#define NUM_THREADS 4

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
    int tid;
    double stuff;
} thread_data_t;

/* thread function */
void*thr_func(void* arg) {
    int scopei = -908, detachi = -908, inheriti = -908;
    struct sched_param param;
    void** stackaddri = (void**)malloc(8);
    size_t stacksizei = -908;
    pthread_attr_t scope, detach, stackaddr, stacksize, inherit, sched;
    thread_data_t *data = (thread_data_t *)arg;

    pthread_attr_init(&scope);
    pthread_attr_init(&detach);
    pthread_attr_init(&stackaddr);
    pthread_attr_init(&stacksize);
    pthread_attr_init(&inherit);
    pthread_attr_init(&sched);

    pthread_attr_getscope(&scope, &scopei);
    pthread_attr_getdetachstate(&detach,&detachi);
    pthread_attr_getdetachstate(&detach,&detachi);
    pthread_attr_getstack(&stackaddr,stackaddri, &stacksizei);
    pthread_attr_getinheritsched(&inherit,&inheriti);
    pthread_attr_getschedparam(&sched, &param);

    printf("thread id: %d\n", data->tid);
    printf("scope:%d\ndetach:%d\nstacksize:%ld\ninherit:%d\nschedpolicy:%d\n\n", scopei, detachi, stacksizei, inheriti, param.sched_priority);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    pthread_t thr[NUM_THREADS];
    int i, rc;
    /* create a thread_data_t argument array */
    thread_data_t thr_data[NUM_THREADS];
    /* create threads */
    for(i = 0; i< NUM_THREADS; ++i) {
        thr_data[i].tid = i;
        if((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    /* block until all threads complete */
    for(i = 0; i< NUM_THREADS; ++i)
        pthread_join(thr[i], NULL);
    return EXIT_SUCCESS;
}