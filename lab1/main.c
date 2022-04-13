#include <stdio.h>
#include <stdlib.h>
long double phi(int x) {
    return x;
}

long double psi(int t) {
    return t;
}

int init_grid(long double* grid, int n, int m) {
    int i = 0, j = 0, nm = n * m;
    for (i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            *(grid+i*m+j) = 0;}}
    for (i = 0; i < m; i++)
        *(grid + i) = phi(i);
    for (i = 0; i < n; i++)
        *(grid+i*m) = psi(i);
}

int print_grid(long double* grid, int n, int m) {
    int i = 0, j = 0;
    for (i = 0; i < n; i++) {
        for(j = 0; j < m; j++)
            printf("%Lf ", *(grid+i*m+j));
        printf("\n");
    }
}
int fkm(int t, int x) {
    return t+x;
}

int krest(long double * grid, int n, int m, int h, int tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t,x) * 2 * tao - tao*(*(grid+t*m+x+1) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t-1)*m+x); 
}

int ugolok(long double * grid, int n, int m, int h, int tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t,x) * tao - tao*(*(grid+t*m+x) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t)*m+x); 
}


int main(int argc, char* argv[])
{
    int n = 5, m = 7, tao = 1, h = 1;
    int i = 0, j = 0;
    long double* grid = (long double*)calloc(n*m, sizeof(long double));
    init_grid(grid, n, m);
    print_grid(grid, n, m);
    printf("\n\n");
    for (i = 0; i < n; i++)
        ugolok(grid, n, m, h, tao, i, 1);
    for (i = 1; i < m; i++)
        ugolok(grid, n, m, h, tao, 0, i);
    for (i = 1; i < n; i++)
        for (j = 2; j < m; j++)
            krest(grid, n , m, h, tao, i, j);
    print_grid(grid, n, m);
}