#define MAX_DIGITS 6
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/*int get_n(int fd)
{
    int n = 0;
    char buf[1];
    while(read(fd, buf, 1) > 0)
        if (buf[0] == ' ')
            n++;
    printf("n is %d\n", n);
    return n;
}

int process_in(int* sizes, int fd, int n)
{
    char buf[1], buf2[MAX_DIGITS];
    int i = 0, j = 0;
    memset(buf2, 0, MAX_DIGITS);
    lseek(fd, 0, SEEK_SET);
    while(read(fd, buf, 1) > 0) {
        if (buf[0] == '\n')
            break;
        if (buf[0] == ' ') {
            sizes[i++] = atoi(buf2);
            memset(buf2, 0, MAX_DIGITS);
            j = 0;
        }
        else
            buf2[j++] = buf[0];
    }
    return 0;
}

int main()
{
    int fd = open("in.txt", 0);
    int* sizes;
    int n = get_n(fd), i = 0, sum = 0;
    sizes = (int*)malloc(n*sizeof(int));
    process_in(sizes, fd, n);
    for (i = 0; i < n; i++) {
        printf("%d ", sizes[i]);
        sum += sizes[i];
    }
    printf("\n");
    int** mas = (int**)malloc(sum * sizeof(int));
}*/

int main(int argc, char* argv[])
{
    int n = argv[1], i = 0, n2 = 0;
    int* mas = (int*)malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        mas[i] = i;
    n2 = n / 2 + 1;
    int* mas1 = (int*)malloc(n2*sizeof(int)), mas2 = (int*)malloc(n2*sizeof(int));
}