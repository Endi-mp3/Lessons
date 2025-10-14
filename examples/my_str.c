#include <stdio.h>


int string2num(const char* string, unsigned long *out)
{
	unsigned long num = 0;
	unsigned char digit; // переводиться как цифра
	for(int i = 0; string[i] != 0; i++) {
		digit = (string[i] - 0x30);
		if (digit > 9)
			return -1;
		num = num * 10 + digit;
	}
	*out = num;
	return 0;
}

int main(int argc, const char* argv[])
{
	unsigned long test;
	int r = string2num(argv[1], &test);
	printf("%lu\n", test);
	return r;
}
