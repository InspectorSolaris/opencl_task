#include "operations.h"

unsigned long long int Lg2N
(
	unsigned long long int n
)
{
	unsigned long long int lg_n = 0;

	while((1ULL << lg_n) < n)
	{
		++lg_n;
	}

	return lg_n;
}

unsigned long long int RevBits
(
	unsigned long long int x,
	unsigned long long int lg_n
)
{
	unsigned long long int rev = 0;

	for(unsigned long long int i = 0; i < lg_n; ++i)
	{
		if(x & (1ULL << i))
		{
			rev |= 1ULL << (lg_n - 1 - i);
		}
	}

	return rev;
}

complex Pow
(
	const complex & n,
	unsigned long long int x
)
{
	if(x == 0) return {1, 0};
	else if(x == 1) return n;
	else if(x % 2) return n * Pow(n, x - 1);
	else
	{
		complex a = Pow(n, x / 2);

		return a * a;
	}
}