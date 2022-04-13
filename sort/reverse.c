#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]) {
    int n = atoi(argv[1]), i = 0;
    char* filename = argv[2];
    int* mas = (int*)malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        mas[i] = n - i;
    FILE* fd = fopen(filename, "w+");
    for (i = 0; i < n; i++)
        fprintf(fd, "%d ", mas[i]);
    return 0;
}