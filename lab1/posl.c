#include <stdio.h>
#include <stdlib.h>
long double phi(long double x) {
    return (x*x*x) / (12 * (0.6*0.6));
}

long double psi(long double t) {
    return 0.6 * (t*t*t) / 12;
}

int init_grid(long double* grid, int n, int m, long double h, long double tao) {
    int i = 0, j = 0, nm = n * m;
    for (i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            *(grid+i*m+j) = 0;}}
    for (i = 0; i < m; i++)
        *(grid + i) = phi(i*h);
    for (i = 0; i < n; i++)
        *(grid+i*m) = psi(i*tao);
}

int print_grid(long double* grid, int n, int m) {
    int i = 0, j = 0;
    for (i = n-1; i >= 0; i--) {
        for(j = 0; j < m; j++)
            printf("%Lf ", *(grid+i*m+j));
        printf("\n");
    }
}
int fkm(long double t, long double x) {
    return t*x;
}

int krest(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * 2 * tao - 0.6*tao*(*(grid+t*m+x+1) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t-1)*m+x); 
}

int ugolok(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * tao - 0.6*tao*(*(grid+t*m+x) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t)*m+x); 
}


int main(int argc, char* argv[])
{
    int n = 200, m = 200;
    long double tao = 0.01, h = 0.01;
    int i = 0, j = 0;
    long double* grid = (long double*)calloc(n*m, sizeof(long double));
    if (grid == NULL) {
        perror("MALLOC: ");
        return -1;
    }
    init_grid(grid, n, m, h, tao);
    for (i = 1; i < m; i++)
        ugolok(grid, n, m, h, tao, 0, i);
    for (i = 1; i < (n-1); i++) {
        for (j = 1; j < (m-1); j++)
            krest(grid, n , m, h, tao, i, j);
        ugolok(grid, n, m, h, tao, i, m-1);
    }
    print_grid(grid, n, m);
}