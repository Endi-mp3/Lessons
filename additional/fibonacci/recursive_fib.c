#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/**
 * @function fibonacci
 * @brief	Calculate fibonacci value for target position in fibonacci's row
 * @param	target
 * @return	calculated fibonacci value
 */
uint64_t fibonacci(uint8_t target);


/**
 * @function convert_char_to_u8
 * @brief	Converts char array to uint8_t value and validate result of conversion
 * @param	arg char array
 * @return	Positive uint8_t value or negative error code.
 */
int convert_char_to_u8(const char* arg);

int main(int argc, const char * argv[])
{
	int res = -1;
	int tgt = 0;
	if (argc < 2)
	{
		printf("Enter target number from 1..255\n");
		scanf("%d", &tgt);
		if (tgt > 255 || tgt < 1) {
			perror("Argument is wrong. It must be valid number from 1..255");
			return -1;
		}
		printf("Fib[%i]=%llu\n", tgt, fibonacci(tgt));
		return 0;
	}

	for(int i = 1; i < argc; i++) {
		tgt = convert_char_to_u8(argv[i]);
		if (tgt > 0) {
			printf("Fib[%i]=%llu\n", tgt, fibonacci(tgt));
		}
	}

	return res;
}

bool isDigit(const char c)
{
	return ((c >= '0') && (c <= '9'));
}

int convert_char_to_u8(const char* arg)
{
	int res = 0;
	int len = strnlen(arg, 4);
	if (len > 3) {
		perror("Argument to big. Maximum target can be 255");
		return -1;
	}
	for(int i = 0; i < len; i++) {
		if ( !isDigit(arg[i]) ) {
			perror("Argument is wrong. It must be valid number from 1..255");
			return -2;
		}

		// "1 5 7"
		//		V   +1
		//      1
		//		<-	*10
		//	  1 0
		//		V	+5
		//	  1 5
		//		<-	*10
		//  1 5 0
		//		V	+7
		//	1 5 7

		//					arg = "157";
		// i = 0, res = 0, arg[i] = '1'
		//		0 *= 10 => res = 0
		//		0 += '1' - '0' <=> 	0 += 1 => res = 1;
		// i = 1, res = 1, arg[i] = '5'
		//		1 *= 10 => res = 10
		//		10 += '5' - '0' <=> 10 += 5 => res = 15;
		// i = 2, res = 15, arg[i] = '7'
		//		15 *= 10 => res = 150
		//		150 += '7' - '0' <=> 150 += 7 => res = 157
		res *= 10;
		res += (int)arg[i] - (int)'0';
	}
	if (res > 255) {
		perror("Argument is wrong. It must be valid number from 1..255");
		return -3;
	}
	return res;
}

uint64_t fibonacci(uint8_t target)
{
	uint64_t res = 0;
	// with recursive§
	return target;
}


