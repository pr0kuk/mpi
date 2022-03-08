#include "big_int.h"

void mem_error();
void add4mul(big_int *bi1, const big_int *bi2);
void badd(unsigned char *bytes, int *psize, unsigned char summand);
void bmul(unsigned char *bytes, int *psize, unsigned char factor);
void dmul(unsigned char *dbytes, int *psize);
void dadd(unsigned char *dbytes, int *psize, unsigned char summand);

void mem_error()
{
	printf("Недостаточно памяти. Работа программы прекращена\n");
	exit(1);
}

unsigned int count_bits(const big_int *bi)
{
    unsigned int result = bi->size * 8;
    unsigned char byte = bi->bytes[bi->size - 1];
    for (int i = 7; i >= 0; i--)
        if (byte >> i)
           return result + i - 7;
    return 0;
}

big_int *simple_division2pow(const big_int *bi, unsigned int power, unsigned int count)
{   
    int size = bi->size;
    unsigned char *bytes = bi->bytes;
    if (!power)
        return create(bytes, size);        
    if (power >= count)
        return from_byte(0);
    int rem = power % 8;
    if (!rem)
    {
        int rem_size = power / 8;
        return create(bytes + rem_size, size - rem_size);        
    }
    int rem_size = power / 8 + 1, add = 8 - rem;
    unsigned int quot_count = count - power;
    int quot_size = quot_count / 8;
    if (quot_count % 8)
        quot_size++;
    unsigned char *quot_bytes;
    if (!(quot_bytes = malloc(quot_size)))
        mem_error();
    int delta = rem_size - 1; 
    for (int i = 0; i < quot_size; i++)
    {
        quot_bytes[i] = bytes[delta + i] >> rem;
        if (rem_size + i < size)
            quot_bytes[i] |= bytes[rem_size + i] << add;
    }
    big_int *quot_bi;
    if (!(quot_bi = malloc(sizeof(big_int))))
        mem_error();
    quot_bi->bytes = quot_bytes;
    quot_bi->size = quot_size;
    return quot_bi;
}

big_int *divide(big_int *bi1, const big_int *bi2, big_int **remainder)
{
    big_int *p = division(bi1, bi2, remainder);
    if (!p)
        return 0;
    free(bi1->bytes);
    bi1->bytes = p->bytes;
    bi1->size = p->size;
    free(p);
    return bi1;
}

big_int *division(const big_int *bi1, const big_int *bi2, big_int **remainder)
{
    unsigned char *bytes1 = bi1->bytes, *bytes2 = bi2->bytes;
    int size1 = bi1->size, size2 = bi2->size;
    if (bytes2[size2 - 1] == 0)
        return 0;
    unsigned int count2 = count_bits(bi2); 
    int r = count2 % 8 - 1;
    if (r < 0)
        r = 7;
    if (bytes2[size2 - 1] == (unsigned char) 1 << r)
    {
        int zero = 1;
        for (int i = 0; i < size2 - 1; i++)
            if (bytes2[i] != 0)
            {
                zero = 0;
                break;
            }
        if (zero)
            return division2pow(bi1, count2 - 1, remainder);         
    }
    big_int *rem = create(bytes1, size1);
    big_int *quot = from_byte(0);   
    unsigned int count1 = count_bits(bi1);
    while (count1 > count2)
    {
        big_int *temp = simple_division2pow(rem, count2, count1);
        add(quot, temp);
        deduct(rem, multiply(temp, bi2));
        delete(temp);
        count1 = count_bits(rem);
    }
    if (deduct(rem, bi2))
    {
        big_int *one = from_byte(1);
        add(quot, one);
        delete(one);
    }
    if (remainder)
        *remainder = rem;
    else
        delete(rem);
    return quot;
}

big_int *divide2pow(big_int *bi, unsigned int power, big_int **remainder)
{
    big_int *p = division2pow(bi, power, remainder);
	free(bi->bytes);
	bi->bytes = p->bytes;
	bi->size = p->size;
	free(p);
	return bi;
}

big_int *division2pow(const big_int *bi, unsigned int power, big_int **remainder)
{   
    int size = bi->size;
    unsigned char *bytes = bi->bytes;
    if (!power)
    {
        if (remainder)
            *remainder = from_byte(0);
        return create(bytes, size);        
    }
    unsigned int count = count_bits(bi);
    if (power >= count)
    {
        if (remainder)
            *remainder = create(bytes, size);
        return from_byte(0);
    }
    int rem = power % 8;
    if (!rem)
    {
        int rem_size = power / 8;
        if (remainder)
            *remainder = create(bytes, rem_size);
        return create(bytes + rem_size, size - rem_size);        
    }
    int rem_size = power / 8 + 1, add = 8 - rem;
    if (remainder)
    {
        unsigned char *rem_bytes;
        if (!(rem_bytes = malloc(rem_size)))
            mem_error();
        memcpy(rem_bytes, bytes, rem_size);
        unsigned char *byte = rem_bytes + rem_size - 1;
        *byte = (unsigned char) (*byte << add) >> add;
        if(!*byte && rem_size != 1)
        {
            *remainder = create(rem_bytes, rem_size);
            free(rem_bytes);
        }
        else
        {          
            if (!(*remainder = malloc(sizeof(big_int))))
                mem_error();
            (*remainder)->bytes = rem_bytes;
            (*remainder)->size = rem_size;
        }
    }
    unsigned int quot_count = count - power;
    int quot_size = quot_count / 8;
    if (quot_count % 8)
        quot_size++;
    unsigned char *quot_bytes;
    if (!(quot_bytes = malloc(quot_size)))
        mem_error();
    int delta = rem_size - 1; 
    for (int i = 0; i < quot_size; i++)
    {
        quot_bytes[i] = bytes[delta + i] >> rem;
        if (rem_size + i < size)
            quot_bytes[i] |= bytes[rem_size + i] << add;
    }
    big_int *quot_bi;
    if (!(quot_bi = malloc(sizeof(big_int))))
        mem_error();
    quot_bi->bytes = quot_bytes;
    quot_bi->size = quot_size;
    return quot_bi;
}

big_int *two_pow(unsigned int power)
{
    int size = ++power / 8;
    int rem = power % 8;
    if (rem)
        size++;
    unsigned char *bytes;
    if (!(bytes = calloc(size, 1)))
        mem_error();
    if (rem)
        bytes[size - 1] = 1 << (rem - 1);
    else
        bytes[size - 1] = 128;
    big_int *bi;
    if (!(bi = malloc(sizeof(big_int))))
        mem_error();
    bi->bytes = bytes;
    bi->size = size;
    return bi;     
}

big_int *deduct(big_int *bi1, const big_int *bi2)
{
	big_int *p = difference(bi1, bi2);
	if (!p)
		return 0;
	free(bi1->bytes);
	bi1->bytes = p->bytes;
	bi1->size = p->size;
	free(p);
	return bi1;
}

big_int *difference(const big_int *bi1, const big_int *bi2)
{
	int size1 = bi1->size, size2 = bi2->size;
	if (size2 > size1)
		return 0;
	unsigned char *bytes1 = bi1->bytes, *bytes2 = bi2->bytes, *bytes;
	if (!(bytes = malloc(size1)))
		mem_error();
	int buf = 0, i;
	for (i = 0; i < size2; i++)
	{
		buf += *(bytes1 + i) - *(bytes2 + i);
		if (buf < 0)
		{
			*(bytes + i) = 256 + buf;
			buf = -1;
		}
		else
		{			
			*(bytes + i) = buf;
			buf = 0;
		}
	}
	for (; i < size1; i++)
	{
		buf += *(bytes1 + i);
		if (buf < 0)
		{
			*(bytes + i) = 256 + buf;
			buf = -1;
		}
		else
		{			
			*(bytes + i) = buf;
			buf = 0;
		}
	}
	if (buf)
	{
		free(bytes);
		return 0;
	}
	while (--i)
		if (*(bytes + i))
			break;
	i++;
	if (i != size1)
		if (!(bytes = realloc(bytes, i)))
			mem_error();
	big_int *result;
	if (!(result = malloc(sizeof(big_int))))
		mem_error();
	result->bytes = bytes;
	result->size = i;
	return result;
}

big_int *multiply(big_int *bi1, const big_int *bi2)
{
	big_int *p = product(bi1, bi2);
	free(bi1->bytes);
	bi1->bytes = p->bytes;
	bi1->size = p->size;
	free(p);
	return bi1;
}

big_int *product(const big_int *bi1, const big_int *bi2)
{
	int size1 = bi1->size, size2 = bi2->size, size = size1 + size2;
	unsigned char *bytes1 = bi1->bytes, *bytes2 = bi2->bytes, *bytes;
	big_int *temp, *result;
	if (!(result = malloc(sizeof(big_int))))
		mem_error();	
	if (size1 == 1 && *bytes1 == 0 || size2 == 1 && *bytes2 == 0)
	{
		if (!(result->bytes = malloc(1)))
			mem_error();	
		*result->bytes = 0;
		result->size = 1;	
		return result;
	}
	if (!(temp = malloc(sizeof(big_int))))
		mem_error();
	if (!(temp->bytes = malloc(size1 + 1)))
		mem_error();
	temp->size = size1;
	memcpy(temp->bytes, bytes1, size1);
	bmul(temp->bytes, &temp->size, *(bytes2 + (--size2)));
	if (!(bytes = calloc(size, 1)))
		mem_error();
	result->bytes = bytes + size - temp->size;
	result->size = temp->size;
	memcpy(result->bytes, temp->bytes, temp->size);
	while (size2--)
	{
		result->bytes--;
		result->size++;
		if (*(bytes2 + size2))
		{
			temp->size = size1;
	    	memcpy(temp->bytes, bytes1, size1);
	    	bmul(temp->bytes, &temp->size, *(bytes2 + size2));
	    	add4mul(result, temp);
	    }
	}
	delete(temp);
	if (result->size < size)
	{
		unsigned char *newbytes;
		if (!(newbytes = malloc(result->size)))
			mem_error();
		memcpy(newbytes, result->bytes, result->size);
		result->bytes = newbytes;
		free(bytes);
	}
	return result;
}

void add4mul(big_int *bi1, const big_int *bi2)
{
	unsigned char *bytes1 = bi1->bytes, *bytes2 = bi2->bytes;
	int size1 = bi1->size, size2 = bi2->size;
	int buf = 0, i;
	for (i = 0; i < size2; i++)
	{
		*(bytes1 + i) = buf += *(bytes1 + i) + *(bytes2 + i);
		buf >>= 8;
	}
	for (; i < size1; i++)
	{
		*(bytes1 + i) = buf += *(bytes1 + i);
		buf >>= 8;
	}
	if (buf)
	{
		for (int j = 0; j < size1; j++)
			*(bytes1 + j - 1) = *(bytes1 + j);
		*(bytes1 + size1 - 1) = 1;
		bi1->bytes--;
		bi1->size++;
	}
}

big_int *add(big_int *bi1, const big_int *bi2)
{
	big_int *s = sum(bi1, bi2);
	free(bi1->bytes);
	bi1->bytes = s->bytes;
	bi1->size = s->size;
	free(s);
	return bi1;
}

void dout(const big_int *bi)
{
	char *str = dprint(bi);
	printf("%s", str);
	free(str);
}

void hout(const big_int *bi)
{
	char *str = hprint(bi);
	printf("%s", str);
	free(str);
}

big_int *from_byte(unsigned char byte)
{
    big_int *bi;
	if (!(bi = malloc(sizeof(big_int))))
		mem_error();
	if (!(bi->bytes = malloc(1)))
		mem_error();
	*(bi->bytes) = byte;
	bi->size = 1;
	return bi;
}

big_int *create(const void *bytes, int size)
{
	const unsigned char *b = (const unsigned char*) bytes;
	while (--size)
		if (*(b + size))
			break;
	size++;
	big_int *bi;
	if (!(bi = malloc(sizeof(big_int))))
		mem_error();
	unsigned char *new_bytes;
	if (!(new_bytes = malloc(size)))
		mem_error();
	bi->bytes = memcpy(new_bytes, bytes, size);
	bi->size = size;
	return bi;
}

big_int *hcreate(const char *string)
{
	if (!string)
		return 0;
	int len = strlen(string);
	if (!len)
		return 0;
	if (*string == '0' && len != 1)
		return 0;
	char odd = len % 2;
	int size = len / 2;
	if (odd)
		size++;
	unsigned char *bytes;
	if (!(bytes = malloc(size)))
		mem_error();
	int count = size - 1;
	int buf;	
	if (odd)
	{
		if (!isxdigit(*string))
		{
			free(bytes);
			return 0;
		}
		sscanf(string, "%1x", &buf);
		*(bytes + count) = buf;
		string++;
		count--;
	}
	for (; count >= 0; count--)
	{
		if (!isxdigit(*string) || !isxdigit(*(string + 1)))
		{
			free(bytes);
			return 0;
		}
		sscanf(string, "%2x", &buf);
		*(bytes + count) = buf;
		if (count)	
		{
			string++;
			string++;
		}
	}	
	big_int *bi;
	if (!(bi = malloc(sizeof(big_int))))
		mem_error();
	bi->bytes = bytes;
	bi->size = size;
	return bi;
}

void badd(unsigned char *bytes, int *psize, unsigned char summand)
{
	int buf = summand;
	for (int i = 0; i < *psize; i++)
	{
		if (!buf)
			return;
		*(bytes + i) = buf += *(bytes + i);
		buf >>= 8; 
	}
	if (buf)
	{
		*(bytes + *psize) = buf;
		(*psize)++;
	}
}

void bmul(unsigned char *bytes, int *psize, unsigned char factor)
{
	int buf = 0;
	for (int i = 0; i < *psize; i++)
	{
		*(bytes + i) = buf += *(bytes + i) * factor;
		buf >>= 8; 
	}
	if (buf)
	{
		*(bytes + *psize) = buf;
		(*psize)++;
	}
}

big_int *dcreate(const char *string)
{
	if (!string)
		return 0;
	int len = strlen(string);
	if (!len)
		return 0;
	if (*string == '0' && len != 1)
		return 0;
	char odd = len % 2;
	int size = len * LOG_256_10 + 1;
	int buf;	
	if (odd)
	{
		if (!isdigit(*string))
			return 0;
		sscanf(string, "%1d", &buf);
		string++;
	}
	else
	{
		if (!isdigit(*string) || !isdigit(*(string + 1)))
			return 0;
		sscanf(string, "%2d", &buf);
		string++;
		string++;
	}
	unsigned char *bytes;
	if (!(bytes = malloc(size)))
		mem_error();
	*bytes = buf;
	size = 1;
	while (*string)
	{
		if (!isxdigit(*string) || !isxdigit(*(string + 1)))
		{
			free(bytes);
			return 0;
		}
		sscanf(string, "%2d", &buf);
		bmul(bytes, &size, 100);
        if (buf)
        	badd(bytes, &size, buf);
		string++;
        string++;     
	}
	if (!(bytes = realloc(bytes, size)))
		mem_error();
	big_int *bi;
	if (!(bi = malloc(sizeof(big_int))))
		mem_error();
	bi->bytes = bytes;
	bi->size = size;
	return bi;
}

char *hprint(const big_int *bi)
{
	int count = bi->size;
	int len = count * 2;
	count--;
	unsigned char *bytes = bi->bytes;
	char odd = *(bytes + count) <= 0xf;
	if (!odd)
		len++;
	char *string, *str;
	if (!(str = string = malloc(len)))
		mem_error();
	if (odd)
	{
		sprintf(string, "%x", *(bytes + count));
		count--;
		str++;
	}
	for (; count >= 0; count--)
	{
		unsigned char c = *(bytes + count);
		if (c <= 0xf)
			sprintf(str, "0%x", c);
		else
			sprintf(str, "%x", c);
		str++;
		str++;
	}
	*str = '\0';
	return string;
}

void dmul(unsigned char *dbytes, int *psize)
{
	int buf = 0;
	for (int i = 0; i < *psize; i++)
	{
		*(dbytes + i) = (buf += *(dbytes + i) * 256) % 100;
		buf /= 100;
	}
	*(dbytes + *psize) = buf % 100;
	(*psize)++;
	buf /= 100;
	if (buf)
	{
		*(dbytes + *psize) = buf;
		(*psize)++;
	}
}

void dadd(unsigned char *dbytes, int *psize, unsigned char summand)
{
	int buf = summand;
	for (int i = 0; i < *psize; i++)
	{
		if (!buf)
			return;
		*(dbytes + i) = (buf += *(dbytes + i)) % 100;
		buf /= 100; 
	}
	if (buf)
	{
		*(dbytes + *psize) = buf;
		(*psize)++;
	}
}

char *dprint(const big_int *bi)
{
	int count = bi->size;
	int size = count * LOG_100_256 + 1;
	count--;
	unsigned char *dbytes;
	if (!(dbytes = malloc(size)))
		mem_error();
	unsigned char *bytes = bi->bytes;
	*dbytes = 0;
	size = 1;
	unsigned char buf = *(bytes + count);
	if (buf)
		dadd(dbytes, &size, buf);
	while (count--)
	{  
		dmul(dbytes, &size);
		buf = *(bytes + count);
        if (buf)
        	dadd(dbytes, &size, buf);
	}
	buf = *(dbytes + size - 1);
	char odd = buf < 10;
	char *string, *str;
	if (odd)
	{
		if (!(string = str = malloc(2 * size)))
			mem_error();
		sprintf(str, "%d", buf);
		size--;
		str++;
	}
	else
	{
		if (!(string = str = malloc(2 * size + 1)))
			mem_error();
	}
	while (size--)
	{
		buf = *(dbytes + size);
		if (buf < 10)
			sprintf(str, "0%d", buf);
		else
			sprintf(str, "%d", buf);
		str++;
		str++;
	}
	*str = '\0';	
	free(dbytes);
	return string;
}

big_int *sum(const big_int *bi1, const big_int *bi2)
{
	unsigned char *max, *min, *bytes;
	int s_max, s_min;
	if (bi1->size >= bi2->size)
	{
		max = bi1->bytes;
		min = bi2->bytes;
		s_max = bi1->size;
		s_min = bi2->size;
	}
	else
	{
		max = bi2->bytes;
		min = bi1->bytes;
		s_max = bi2->size;
		s_min = bi1->size;
	}
	int size = s_max;
	if (!(bytes = malloc(size)))
		mem_error();
	int buf = 0, i;
	for (i = 0; i < s_min; i++)
	{
		*(bytes + i) = buf += *(max + i) + *(min + i);
		buf >>= 8;
	}
	for (; i < s_max; i++)
	{
		*(bytes + i) = buf += *(max + i);
		buf >>= 8;
	}
	if (buf)
	{
		size++;
		if (!(bytes = realloc(bytes, size)))
			mem_error();
		*(bytes + size - 1) = 1;
	}
	big_int *bi;
	if (!(bi = malloc(sizeof(big_int))))
		mem_error();
	bi->bytes = bytes;
	bi->size = size;
	return bi;
}

int compare(const big_int *bi1, const big_int *bi2)
{
	if (bi1 == bi2)
		return 0;
	int s = bi1->size, result = s - bi2->size;
	if (result)
		return result;
	unsigned char *b1 = bi1->bytes, *b2 = bi2->bytes;
	while (s--)
		if (result = *(b1 + s) - *(b2 + s))
			return result;
	return 0;
}

void delete(big_int *bi)
{
	free(bi->bytes);
	free(bi);
}
