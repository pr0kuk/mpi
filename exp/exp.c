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
    for (t = s; t > 0; lent++, t/= 10);
    char * rr = (char*)malloc(lent);
    for (i = 0, t = s; i < lent; i++)
        rr[lent-i-1] = t % 10 + '0', t /= 10;
    rr[lent] = 0;
    big_int * od = dcreate("1"), *ss = dcreate(rr);
    for (i = s; i <= n; i++) {
       multiply(c_k, ss);
       add(ss, od);
    }
    free(rr);
}

long int calc_n(long int p)
{
    long i = 1, l = 0, n = 1;
    for (; l + 1 < p; n*=i++)
        for (;n >= 100; l++, n/=10);
    return i + 2;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }

    int my_rank = 0, commsize = 0, i = 0, j = 0;
    double tstart = 0, tfinish = 0, tcalcs = 0, tcalcf = 0, tgather = 0;
    if (my_rank == 0)
        tstart = MPI_Wtime();
    long int p = atol(argv[1]), n = 0;
    big_int* c_k = dcreate("1"), * sum_k = dcreate("0"), * dnmr = dcreate("1"), * nmr, * rem, * chast, * des = dcreate("10");
    char Result[MAXCHAR * 10], part_ans[MAXCHAR];
    char * ans = (char*)malloc(MAXCHAR), *sumch_k = (char*)malloc(MAXCHAR);
    memset(part_ans, 0, MAXCHAR);
    tstart = MPI_Wtime();
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    
    if (my_rank == 0) {
        memset(Result, 0, MAXCHAR * 5);
        n = calc_n(p);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    tcalcs = MPI_Wtime();
//COMPUTING ROW MEMBERS
    for (i = my_rank + 1; i <= n; i += commsize) {
        c_k = dcreate("1");
        fact(c_k, i, n);
        add(sum_k, c_k);
        sumch_k = dprint(sum_k);
    }
//DIVISION
    fact(dnmr, 1, n);
    nmr = dcreate(sumch_k);
    for (i = 0; i < p + 3; i++) {
        chast = division(nmr, dnmr, &rem);
        nmr = rem;  
        multiply(nmr, des);
        part_ans[i] = dprint(chast)[0];
    }
    tcalcf = MPI_Wtime();
//GATHERING & SUMMING
    MPI_Gather(part_ans, MAXCHAR, MPI_CHAR, Result, MAXCHAR, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        tgather = MPI_Wtime();
        big_int * summa0 = dcreate("0");
        for (i = 0; i < commsize; i++) {
            for (j = 0;; j++) {
                if ((Result+MAXCHAR*i+j)[0] != '0') {
                    add(summa0, dcreate(Result + MAXCHAR * i + j)); 
                    break; 
                }
            }
        }
        tfinish = MPI_Wtime();
//OUTPUT
        ans = dprint(summa0);
        printf("\n");
        for (i = 0; i < p + 2; i++) {
            if (i == 1)
                printf(".");
            printf("%c", ans[i]);
        }
        printf("\n");
        printf("tgeneral %lf\n", tfinish - tstart);
        printf("tmain %lf\n", tcalcf - tcalcs);
    }

//ENDING
    free(sumch_k);
    MPI_Finalize();
    return 0;
}