#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
int nproc;
void* slave_thr(void* my_rank){
    printf("Hello World, my rank is %d, total number of executors is %d\n", *(int*)my_rank, nproc);
}

int main(int argc, char *argv[])
{
    int i = 0;
    nproc = atoi(argv[1]);
    pthread_t thr[nproc];
    int num[nproc];
    printf("nproc:%d\n", nproc);
    for (i = 0; i < nproc; i++){
        num[i] = i;
        pthread_create(&thr[i], NULL, slave_thr, num+i);
    }
    for(i = 0; i< nproc; ++i) {
        pthread_join(thr[i], NULL);
    }
    return 0;
}       