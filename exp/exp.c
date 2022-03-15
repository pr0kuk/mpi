#define MAXCHAR 4096
#define max(a,b) (a > b) ? a : b
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "big_int.h"

void fact(big_int * c_k, int s, int n)
{
    int i = 0, t = s, lent = 0;
    char * odin = "1";
    while (t > 0)
        t /= 10, lent++;
    t = s;
    char * rr = (char*)malloc(lent);
    for (i = 0; i < lent; i++)
        rr[lent-i-1] = t % 10 + '0', t /= 10;
    rr[lent] = 0;
    big_int * od = dcreate(odin), *ss = dcreate(rr);
    for (i = s; i <= n; i++) {
       multiply(c_k, ss);
       add(ss, od);
    }
    free(rr);
}

long int calc_n(long int p)
{
    long i = 1, l = 0, n = 1;
    for (i = 1; l + 1 < p; i++) {
        n *= i;
        for (;n >= 100; l++)
            n /= 10;
    }
    return i;
}



int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }

    int my_rank = 0, commsize = 0, i = 0;
    double tstart = 0, tfinish = 0, t1 = 0, t2 = 0, t3 = 0;
    if (my_rank == 0)
        tstart = MPI_Wtime();
    long int p = atol(argv[1]), n = 0;

    //char Result[MAXCHAR * 10];
    char * Result = (char*)malloc(MAXCHAR * 4), *sumch_k = (char*)malloc(MAXCHAR);
    big_int* c_k = dcreate("1"), * sum_k = dcreate("0");

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    
    if (my_rank == 0) {
        t3 = MPI_Wtime();
        n = calc_n(p);
        t1 = MPI_Wtime();
        printf("\nn is %ld\n", n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = my_rank + 1; i <= n; i += commsize) {
        c_k = dcreate("1");
        fact(c_k, i, n);
        add(sum_k, c_k);
        sumch_k = dprint(sum_k);
    }

    //printf("summs %s %d\n", sumch_k, my_rank);
    //printf("strlen %ld\n", strlen(sumch_k));
    MPI_Gather(sumch_k, MAXCHAR, MPI_CHAR, Result, MAXCHAR, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        t2 = MPI_Wtime();
        big_int * summa0 = dcreate(Result), * dnmr = dcreate("1"), * nmr = summa0, *rem, * chast, * des = dcreate("10");
        char ans[MAXCHAR];
        for (i = 1; i < commsize; i++)
            add(summa0, dcreate(Result + MAXCHAR * i));
        double tfacts = MPI_Wtime();
        fact(dnmr, 1, n);
        double tfacte = MPI_Wtime();
        printf("strlen dnmr %ld\n", strlen(dprint(dnmr)));
        for (i = 0; i < p + 2; i++) {
            if (i == 1)
                ans[i++] = '.';
            chast = division(nmr, dnmr, &rem);
            nmr = rem;  
            multiply(nmr, des);
            ans[i] = dprint(chast)[0];
        }
        tfinish = MPI_Wtime();
        //printf("tfact %f\n", -tfacts + tfacte); 
        printf("calc_n %f\n", t1 - t3);  
        printf("tmain %f\n", t2 - t1);  
        printf("tdivision %f\n", tfinish - t2);
        printf("tgeneral %f\n", tfinish - t3);
        for (i = 0; i < p + 2; i++)
            printf("%c", ans[i]);
        printf("\n");
    }
    free(sumch_k);
    free(Result);
    MPI_Finalize();
    return 0;
}