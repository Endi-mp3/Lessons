#include <stdio.h>

int string2num(const char* string, unsigned long *out)
{
	if (out == NULL) // check that out is not empty
		return -1;

	unsigned long num = 0; // initialize result. *out can be used without variable on stack
	unsigned char digit;
	for(int i = 0; string[i] != 0; i++) { // we consider that string has null-terminator
		digit = (string[i] - 0x30); // Calc digit by decreasing on '0'
		if (digit > 9) // check that digit boundaries not broken
			return -1; // return with error
		num = num * 10 + digit; // add 0 to the end of num and add digit after that
	}
	*out = num; // set pointer's value
	return 0;
}

int main(int argc, const char* argv[])
{
	unsigned long test;
	int r = string2num(argv[1], &test);

	printf("%lu\n", test);
	return r;
}
