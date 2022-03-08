#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LOG_256_10 0.4152410119
#define LOG_100_256 1.204119983

typedef struct
{
	unsigned char *bytes;
	int size;
} big_int;

big_int *from_byte(unsigned char byte);
big_int *divide(big_int *bi1, const big_int *bi2, big_int **remainder);
big_int *division(const big_int *bi1, const big_int *bi2, big_int **remainder);
big_int *divide2pow(big_int *bi, unsigned int power, big_int **remainder);
big_int *division2pow(const big_int *bi, unsigned int power, big_int **remainder);
unsigned int count_bits(const big_int *bi);
big_int *two_pow(unsigned int power);
big_int *deduct(big_int *bi1, const big_int *bi2);
big_int *difference(const big_int *bi1, const big_int *bi2);
big_int *multiply(big_int *bi1, const big_int *bi2);
big_int *product(const big_int *bi1, const big_int *bi2);
big_int *add(big_int *bi1, const big_int *bi2);
void hout(const big_int *bi);
void dout(const big_int *bi);
big_int *create(const void *bytes, int size);
big_int *hcreate(const char *string);
big_int *dcreate(const char *string);
char *hprint(const big_int *bi);
char *dprint(const big_int *bi);
big_int *sum(const big_int *bi1, const big_int *bi2);
int compare(const big_int *bi1, const big_int *bi2);
void delete(big_int *bi);
