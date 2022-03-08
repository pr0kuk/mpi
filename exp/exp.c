#define MAXCHAR 10
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
double Fact(int n, int s)
{
    int i = 0, t = 1;
    for(i = n; i < s; i++)
        t = t * i;
    return t;
}



int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }
    int n = atoi(argv[1]), my_rank = 0, commsize = 0, i = 0;
    long int c_k = 0, sumc_k = 0, sum = 0;
    char Result[MAXCHAR * 10], RResult[MAXCHAR];
    double tstart = 0, tfinish = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
 
    if (my_rank == 0)
        tstart = MPI_Wtime();

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    char chcksum[MAXCHAR];
    for (i = 0; i < MAXCHAR; i++)
        chcksum[i] = '0';
    for (i = my_rank; i <= n; i += commsize) {
        int l = 0, j = 0;
        c_k = Fact(i, n);
        long int tt = c_k;
        char ckchr[MAXCHAR], ckch[MAXCHAR];
        for (j = 0; j < MAXCHAR; j++)
            ckchr[j] = '0', ckch[j] = '0';
        while (c_k != 0) {
            ckchr[l] = (char)(c_k % 10 + '0');
            c_k /= 10;
            l++;
        }
        for(j = 0; j < MAXCHAR; j++) {
            if (chcksum[j] + ckchr[j] - '0' - '0' > 9) {
                chcksum[j] = (ckchr[j] + chcksum[j] - '0' - '0') % 10 + '0';
                chcksum[j+1]++;
            }
            else
                chcksum[j] = chcksum[j] + ckchr[j] - '0';
        }

    }
    int j = 0;
    char ans[MAXCHAR], nmr[MAXCHAR], nmrr[MAXCHAR];
    for (i = 0; i < MAXCHAR; i++)
        nmr[i] = '0', ans[i] = '0';
    MPI_Gather(&chcksum, MAXCHAR, MPI_CHAR, &Result, MAXCHAR, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {   
        long int t, len_nmr = 0, len_dnmr = 0, dnmr = 0, lnmr;
        for (i = 0; i < commsize; i++){
            for(j = 0; j < MAXCHAR; j++) {
                if (Result[j+i*MAXCHAR] + nmr[j] - '0' - '0' > 9) {
                    nmr[j] = (nmr[j] + Result[j+i*MAXCHAR] - '0' - '0') % 10 + '0';
                    nmr[j+1]++;
                }
                else
                    nmr[j] += Result[j+i*MAXCHAR] - '0';
            }
        }
        while(nmr[MAXCHAR-len_nmr-1] == '0')
            len_nmr++;
        len_nmr = MAXCHAR - len_nmr;
        strncpy(nmrr, nmr, MAXCHAR); 
        for (i = 0; i < MAXCHAR; i++)
            nmrr[i] = nmr[MAXCHAR-i-1];
        for (i = 0; i < len_nmr; i++)
            nmr[i] = nmrr[MAXCHAR - len_nmr + i];
        char * pnmr = (char*)malloc(len_dnmr);
        char * ppnmr = (char*)malloc(len_dnmr+1);
        dnmr = Fact(1, n);
        t = dnmr;
        while(t != 0) {
            t /= 10;
            len_dnmr++;
        }
        j = 0;
        for (i = 0; j < MAXCHAR; i++) {
            strncpy(pnmr, nmr + j, len_dnmr);
            lnmr = atol(pnmr);
            if (lnmr / dnmr == 0) {
                strncpy(ppnmr, nmr + j, len_dnmr + 1);
                lnmr = atoi(ppnmr);
                nmr[j] = '0';
                nmr[j+1] = '0' + lnmr % dnmr;
                ans[i] = (char)(lnmr / dnmr + '0');
            }
            else {
                nmr[j] = '0' + lnmr % dnmr; 
                ans[i] = (char)(lnmr / dnmr + '0');
            }
            if (nmr[j] == '0')
                j++;
            if (j==len_nmr - 1)
                ans[++i] = '.';
        }
        free(pnmr);
        free(ppnmr);
        for (i = 0; i < MAXCHAR; i++)
            printf("%c", ans[i]);
        printf("- ANS\n");
        tfinish = MPI_Wtime();
        printf("Time %f\n", tfinish - tstart);  
    }

    MPI_Finalize();
    return 0;
}
