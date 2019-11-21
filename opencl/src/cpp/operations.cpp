#include "operations.h"

unsigned long long Lg2N
(
	unsigned long long n
)
{
	unsigned long long lg_n = 0;

	while((1ULL << lg_n) < n)
	{
		++lg_n;
	}

	return lg_n;
}

unsigned long long RevBits
(
	unsigned long long x,
	unsigned long long lg_n
)
{
	int rev = 0;

	for(unsigned long long i = 0; i < lg_n; ++i)
	{
		if(x & (1ULL << i))
		{
			rev |= 1 << (lg_n - 1 - i);
		}
	}

	return rev;
}

complex Pow
(
	const complex & n,
	unsigned long long x
)
{
	complex pow = {1, 0};

	for(unsigned long long int i = 0; i < x; ++i)
	{
		pow *= n;
	}

	return pow;
}