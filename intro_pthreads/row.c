#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem;
long double ans = 0;
int n;
int nproc;
void* slave_thr(void* my_rank) {
    long double psum = 0;
    int i = 0;
    for (i = *(int*)my_rank + 1; i <= n; i += nproc)
        psum += 1/(long double)i;
    sem_wait(&sem);
    ans += psum;
    sem_post(&sem);
    printf("my rank is: %d, my part sum: %Lf\n", *(int*)my_rank, psum);
}

int main(int argc, char *argv[])
{
    
    int i = 0;
    nproc = atoi(argv[1]);
    n = atoi(argv[2]);
    pthread_t thr[nproc];
    int num[nproc];
    sem_init(&sem, 0, 1);

    for (i = 0; i < nproc; i++){
        num[i] = i;
        pthread_create(&thr[i], NULL, slave_thr, num+i);
    }
    for(i = 0; i< nproc; ++i) {
        pthread_join(thr[i], NULL);
    }
    printf("ANS:%Lf\n", ans);
    return 0;
}