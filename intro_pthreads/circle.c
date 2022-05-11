#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
int s = 0;
int nproc;
sem_t sem;


void* slave_thr(void* my_rank){
    sem_wait(&sem);
    s++;
    printf("My rank is %d, current number is %d\n", *(int*)my_rank, s);
    sem_post(&sem);
}

int main(int argc, char *argv[])
{

    int i = 0;
    nproc = atoi(argv[1]);
    pthread_t thr[nproc];
    int num[nproc];
    sem_init(&sem, 0, 1);
    printf("nproc:%d\n", nproc);
    for (i = 0; i < nproc; i++){
        num[i] = i;
        pthread_create(&thr[i], NULL, slave_thr, num+i);
    }
    for(i = 0; i< nproc; ++i) {
        pthread_join(thr[i], NULL);
    }
    printf("Final number is %d\n", s);
    return 0;
}