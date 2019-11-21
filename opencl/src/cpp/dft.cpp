#include "dft.h"
#include "operations.h"

constexpr long double Pi = 3.141592653589793238462643383279502884L;

void DFT_Base
(
	const unsigned long long int size,
	complex * const arr
)
{
	const const unsigned long long int n = size;

	for(unsigned long long int i = 1, j = 0; i < n; ++i)
	{
		unsigned long long int bit = n >> 1ULL;

		for(; j >= bit; bit >>= 1)
		{
			j -= bit;
		}

		j += bit;

		if(i < j)
		{
			std::swap(arr[i], arr[j]);
		}
	}

	for(unsigned long long int len = 2; len <= n; len <<= 1ULL)
	{
		const long double ang = 2 * Pi / len * -1;

		const complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
			complex w = {1, 0};

			for(unsigned long long int j = 0; j < len / 2; ++j)
			{
				const complex u = arr[i + j];
				const complex v = arr[i + j + len / 2] * w;

				arr[i + j] = u + v;
				arr[i + j + len / 2] = u - v;

				w *= wlen;
			}
		}
	}

	return;
}

void DFT_AVX
(
	const unsigned long long int size,
	complex * const arr
)
{
	const unsigned long long int n = size;

	for(unsigned long long int i = 1, j = 0; i < n; ++i)
	{
		unsigned long long int bit = n >> 1ULL;

		for(; j >= bit; bit >>= 1)
		{
			j -= bit;
		}

		j += bit;

		if(i < j)
		{
			std::swap(arr[i], arr[j]);
		}
	}

	for(unsigned long long int len = 2; len <= n; len <<= 1ULL)
	{
		const long double ang = 2 * Pi / len * -1;
		
		const complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
			complex w = {1, 0};

			w /= wlen;

			for(unsigned long long int j = 0; j < len / 2; j += 4)
			{
				//const complex u = arr[i + j];
				//const complex v = arr[i + j + len / 2] * w;

				//arr[i + j] = u + v;
				//arr[i + j + len / 2] = u - v;

				//w *= wlen;

				const unsigned long long int k = std::min(4ULL, len / 2 - j);

				const	__m256 u_vec = _mm256_load_ps((const float *)(arr + i + j));
						__m256 v_vec = _mm256_load_ps((const float *)(arr + i + j + len / 2));
				const	__m256 w_vec = _mm256_setr_ps
				(
					(w *= wlen).real,
					(w).imag,
					(w *= wlen).real,
					(w).imag,
					(w *= wlen).real,
					(w).imag,
					(w *= wlen).real,
					(w).imag
				);

				v_vec = _mm256_permute_ps
				(
					_mm256_hadd_ps
					(
						_mm256_mul_ps
						(
							_mm256_mul_ps
							(
								v_vec,
								w_vec
							),
							_mm256_setr_ps
							(
								+1.0,
								-1.0,
								+1.0,
								-1.0,
								+1.0,
								-1.0,
								+1.0,
								-1.0
							)
						),
						_mm256_mul_ps
						(
							v_vec,
							_mm256_permute_ps
							(
								w_vec,
								0b10110001
							)
						)
					),
					0b11011000
				);

				const float * const ans1 = (const float *)&_mm256_add_ps(u_vec, v_vec);
				const float * const ans2 = (const float *)&_mm256_sub_ps(u_vec, v_vec);

				for(unsigned long long int x = 0; x < k; ++x)
				{
					arr[i + j + x]				= {ans1[2 * x + 0], ans1[2 * x + 1]};
					arr[i + j + x + len / 2]	= {ans2[2 * x + 0], ans2[2 * x + 1]};
				}
			}
		}
	}

	return;
}

void DFT_MP
(
	const unsigned long long int size,
	complex * const arr
)
{
	const unsigned long long int n = size;
	const unsigned long long int lg_n = Lg2N(n);

	complex * cpow = new complex[n / 2];

#pragma omp parallel for
	for(long long int i = 0; i < n; ++i)
	{
		unsigned long long int rev_i = RevBits(i, lg_n);

		if(rev_i < i)
		{
			std::swap(arr[i], arr[rev_i]);
		}
	}

	for(unsigned long long int len = 2; len <= n; len <<= 1ULL)
	{
		const long double ang = 2 * Pi / len * -1;

		const complex wlen = {cos(ang), sin(ang)};

	#pragma omp parallel for
		for(long long int i = 0; i < len / 2; ++i)
		{
			cpow[i] = Pow(wlen, i);
		}
		
	#pragma omp parallel for
		for(long long int i = 0; i < n; i += len)
		{
		#pragma omp parallel for
			for(long long int j = 0; j < len / 2; ++j)
			{
				const complex u = arr[i + j];
				const complex v = arr[i + j + len / 2] * cpow[j];

				arr[i + j] = u + v;
				arr[i + j + len / 2] = u - v;
			}
		}
	}

	delete[] cpow;

	return;
}
