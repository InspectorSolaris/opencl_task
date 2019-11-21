#include "dft.h"
#include "operations.h"

constexpr long double Pi = 3.141592653589793238462643383279502884L;

void DFT_Base
(
	const unsigned long long int size,
	complex * const arr,
	const bool invert
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
		const long double ang = 2 * Pi / len * (invert ? +1 : -1);

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

	if(invert)
	{
		for(unsigned long long int i = 0; i < n; ++i)
		{
			arr[i].real /= n;
			arr[i].imag /= n;
		}
	}

	return;
}

void DFT_AVX
(
	const unsigned long long int size,
	complex * const arr,
	const bool invert
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
		const long double ang = 2 * Pi / len * (invert ? +1 : -1);
		
		const complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
			complex w = {1, 0};

			for(unsigned long long int j = 0; j < len / 2; j += 4)
			{
				double u_real[4] = {0, 0, 0, 0};
				double u_imag[4] = {0, 0, 0, 0};
				double v_real[4] = {0, 0, 0, 0};
				double v_imag[4] = {0, 0, 0, 0};

				for(unsigned long long int k = 0; k < 4 && j + k < len / 2; ++k)
				{
					u_real[k] = arr[i + j + k].real;
					u_imag[k] = arr[i + j + k].imag;
					v_real[k] = (arr[i + j + k + len / 2] * w).real;
					v_imag[k] = (arr[i + j + k + len / 2] * w).imag;

					w *= wlen;
				}

				const __m256d u_real_vec = _mm256_load_pd(u_real);
				const __m256d u_imag_vec = _mm256_load_pd(u_imag);
				const __m256d v_real_vec = _mm256_load_pd(v_real);
				const __m256d v_imag_vec = _mm256_load_pd(v_imag);

				const double * const a1_real = (double *)&_mm256_add_pd(u_real_vec, v_real_vec);
				const double * const a1_imag = (double *)&_mm256_add_pd(u_imag_vec, v_imag_vec);
				const double * const a2_real = (double *)&_mm256_sub_pd(u_real_vec, v_real_vec);
				const double * const a2_imag = (double *)&_mm256_sub_pd(u_imag_vec, v_imag_vec);

				for(unsigned long long int k = 0; k < 4 && j + k < len / 2; ++k)
				{
					arr[i + j + k]				= {a1_real[k], a1_imag[k]};
					arr[i + j + k + len / 2]	= {a2_real[k], a2_imag[k]};
				}
			}
		}
	}

	if(invert)
	{
		for(unsigned long long int i = 0; i < n; i += 4)
		{
			double buffer_a_real[4] = {0, 0, 0, 0};
			double buffer_a_imag[4] = {0, 0, 0, 0};

			for(unsigned long long int j = 0; j < 4 && i + j < n; ++j)
			{
				buffer_a_real[j] = arr[i + j].real;
				buffer_a_imag[j] = arr[i + j].imag;
			}

			const double * const a_real = (double *)&_mm256_div_pd(_mm256_load_pd(buffer_a_real), _mm256_set1_pd((double)n));
			const double * const a_imag = (double *)&_mm256_div_pd(_mm256_load_pd(buffer_a_imag), _mm256_set1_pd((double)n));

			for(unsigned long long int j = 0; j < 4 && i + j < n; ++j)
			{
				arr[i + j].real = a_real[j];
				arr[i + j].imag = a_imag[j];
			}
		}
	}

	return;
}

void DFT_MP
(
	const unsigned long long int size,
	complex * const arr,
	const bool invert
)
{
	const unsigned long long int n = size;
	const unsigned long long int lg_n = Lg2N(n);

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
		const long double ang = 2 * Pi / len * (invert ? +1 : -1);

		const complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
#pragma omp parallel for
			for(long long int j = 0; j < len / 2; ++j)
			{
				const complex u = arr[i + j];
				const complex v = arr[i + j + len / 2] * Pow(wlen, j);

				arr[i + j] = u + v;
				arr[i + j + len / 2] = u - v;
			}
		}
	}

	if(invert)
	{
#pragma omp parallel for
		for(long long int i = 0; i < n; ++i)
		{
			arr[i].real /= n;
			arr[i].imag /= n;
		}
	}

	return;
}
