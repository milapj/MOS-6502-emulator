#include <stdio.h>

int is_prime(unsigned int num)
{
	unsigned int factor;

	if (!num) {
		return 0;
	}

	for (factor = 2; factor * factor <= num; factor++) {
		if (num % factor == 0) {
			return 0;
		}
	}

	return 1;
}

int
main()
{
	unsigned int num = 0;
	while (printf("Enter a number, and I'll tell you if it's prime: "), scanf("%u", &num) != EOF) {
		if (is_prime(num)) {
			printf("%u is prime!\n\n", num);
		} else {
			printf("%u isn't prime!\n\n", num);
		}
	}
	return 0;
}
