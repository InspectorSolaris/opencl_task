#include "complex.h"

#define Pi 3.141592653589793238462643383279502884L

unsigned long long Lg2N
(
	unsigned long long n
)
{
	unsigned long long lg_n = 0;

	while((1 << lg_n) < n)
	{
		++lg_n;
	}

	return lg_n;
}

unsigned long long RevBits
(
	unsigned long long n,
	unsigned long long lg_n
)
{
	int rev = 0;

	for(unsigned long long i = 0; i < lg_n; ++i)
	{
		if(n & (1 << i))
		{
			rev |= 1 << (lg_n - 1 - i);
		}
	}

	return rev;
}

complex BinaryPow
(
	complex & n,
	unsigned long long x
)
{
	if(x == 0) return {1, 0};
	else if(x == 1) return n;
	else if(x % 2) return BinaryPow(n, x - 1) * n;
	else
	{
		complex a = BinaryPow(n, x / 2);

		return a * a;
	}
}

__kernel void DFT_CL
(
	__global const unsigned long long int size,
	__global const complex * arr_global,
	__global const bool invert
)
{
	__local const unsigned long long int n = size;
	__local const unsigned long long int lg_n = Lg2N(n);
	__local const complex * a = arr_global;
	__local const bool inv = invert;

	__local complex * arr = new complex[n];
	
	unsigned long long ind = get_global_id(0);
	unsigned long long rev_ind = RevBits(ind);

	if(ind < rev_ind)
	{
		arr[ind] = a[rev_ind];
	}

	for(unsigned long long len = 2; len <= n; len <<= 1)
	{
		long double ang = 2 * Pi / len * (inv ? -1 : +1);

		__local complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
			if(i <= ind && ind < i + len / 2)
			{
				complex w = BinaryPow(wlen, ind - i);

				complex u = arr[i + ind];
				complex v = arr[i + ind + len / 2] * w;

				arr[i + ind] = u + v;
				arr[i + ind + len / 2] = u - v;
			}
		}
	}

	if(inv)
	{
		arr[ind] /= n;
	}

	a[rev_ind] = arr[ind];

	return;
}