#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]) {
    int n = atoi(argv[1]), i = 0;
    int* mas = (int*)malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        mas[i] = 1;
    FILE* fd = fopen("in_gen.txt", "w+");
    for (i = 0; i < n; i++)
        fprintf(fd, "%d ", mas[i]);
    return 0;
}