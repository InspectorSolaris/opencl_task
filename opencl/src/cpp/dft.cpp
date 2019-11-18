#include "dft.h"

constexpr long double Pi = 3.141592653589793238462643383279502884L;

void DFT_Base
(
	unsigned long long int size,
	complex * arr,
	bool invert
)
{
	unsigned long long int n = size;

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
		long double ang = 2 * Pi / len * (invert ? -1 : +1);

		complex wlen = {cos(ang), sin(ang)};

		for(unsigned long long int i = 0; i < n; i += len)
		{
			complex w = {1, 0};

			for(unsigned long long int j = 0; j < len / 2; ++j)
			{
				complex u = arr[i + j];
				complex v = arr[i + j + len / 2] * w;

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
	unsigned long long int size,
	complex * arr,
	bool invert
)
{
	unsigned long long int n = size;

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
		long double ang = 2 * Pi / len * (invert ? -1 : +1);
		
		complex wlen = {cos(ang), sin(ang)};

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

				double * a1_real = (double *)&_mm256_add_pd(_mm256_load_pd(u_real), _mm256_load_pd(v_real));
				double * a1_imag = (double *)&_mm256_add_pd(_mm256_load_pd(u_imag), _mm256_load_pd(v_imag));
				double * a2_real = (double *)&_mm256_sub_pd(_mm256_load_pd(u_real), _mm256_load_pd(v_real));
				double * a2_imag = (double *)&_mm256_sub_pd(_mm256_load_pd(u_imag), _mm256_load_pd(v_imag));

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

			double * a_real = (double *)&_mm256_div_pd(_mm256_load_pd(buffer_a_real), _mm256_set1_pd((double)n));
			double * a_imag = (double *)&_mm256_div_pd(_mm256_load_pd(buffer_a_imag), _mm256_set1_pd((double)n));

			for(unsigned long long int j = 0; j < 4 && i + j < n; ++j)
			{
				arr[i + j].real = a_real[j];
				arr[i + j].imag = a_imag[j];
			}
		}
	}

	return;
}