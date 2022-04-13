#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]) {
    char * filename = argv[2];
    int n = atoi(argv[1]), i = 0;
    int* mas = (int*)malloc(n*sizeof(int));
    for (i = 0; i < n; i++) {
        if (i % 4 == 0)
            mas[i] = (i/4) * 4 + 1;
        if (i%4 == 1)
            mas[i] = (i/4) * 4;
        if (i % 4 == 2 || i % 4 == 3)
            mas[i] = i;
    }
    FILE* fd = fopen(filename, "w+");
    for (i = 0; i < n; i++)
        fprintf(fd, "%d ", mas[i]);
    return 0;
}