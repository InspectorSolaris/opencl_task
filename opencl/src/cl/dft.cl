typedef double2 complex;

complex Add
(
	const complex * larg,
	const complex * rarg
)
{
	complex res = (complex)(larg->x + rarg->x, larg->y + rarg->y);

	return res;
}

complex Sub
(
	const complex * larg,
	const complex * rarg
)
{
	complex res = (complex)(larg->x - rarg->x, larg->y - rarg->y);

	return res;
}

complex Mul
(
	const complex * larg,
	const complex * rarg
)
{
	long double a = larg->x;
	long double b = larg->y;
	long double c = rarg->x;
	long double d = rarg->y;

	long double x = a * c - b * d;
	long double y = b * c + a * d;

	complex res = (complex)(x, y);

	return res;
}

complex Div
(
	const complex * larg,
	const complex * rarg
)
{
	long double a = larg->x;
	long double b = larg->y;
	long double c = rarg->x;
	long double d = rarg->y;

	long double x = (a * c + b * d) / (c * c + d * d);
	long double y = (b * c - a * d) / (c * c + d * d);

	complex res = (complex)(x, y);

	return res;
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
		if(x & (1UL << i))
		{
			rev |= 1 << (lg_n - 1 - i);
		}
	}

	return rev;
}

complex Pow
(
	const complex * n,
	unsigned long long x
)
{
	complex pow = (complex)(1, 0);

	for(unsigned long long int i = 0; i < x; ++i)
	{
		pow = Mul(&pow, n);
	}

	return pow;
}

__kernel void DFT_CL
(
	__constant const unsigned long long int * n,
	__constant const unsigned long long int * lg_n,
	__global complex * arr,
	__constant const int * inv
)
{
	const unsigned long long int private_lg_n = *lg_n;
	unsigned long long int ind = get_global_id(0);
	unsigned long long int rev_ind = RevBits(ind, private_lg_n);

	if(rev_ind < ind)
	{
		const complex a = arr[ind];

		arr[ind] = arr[rev_ind];
		arr[rev_ind] = a;
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	for(unsigned long long len = 2; len <= *n; len <<= 1)
	{
		const long double ang = 2 / len * (inv ? +1 : -1);

		const complex wlen = (complex)(cospi((double)ang), sinpi((double)ang));

		if(ind / (len / 2) % 2 == 0)
		{
			const complex w = Pow(&wlen, ind % (len / 2));
			const complex x = arr[ind + len / 2];

			const complex u = arr[ind];
			const complex v = Mul(&x, &w);

			const complex a = Add(&u, &v);
			const complex b = Sub(&u, &v);

			//arr[ind] = Add(&u, &v);
			//arr[ind + len / 2] = Sub(&u, &v);
		}

		barrier(CLK_GLOBAL_MEM_FENCE);
	}

	if(*inv)
	{
		const complex a = arr[ind];
		const complex b = (complex)(*n, 0);

		//arr[ind] = Div(&a, &b);
	}

	return;
}