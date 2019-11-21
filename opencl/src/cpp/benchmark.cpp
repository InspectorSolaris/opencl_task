#include "benchmark.h"

unsigned long long int BenchmarkDFT
(
	unsigned long long int t,
	void DFT
	(
		const unsigned long long int size,
		complex * const arr
	),
	const unsigned long long int size,
	const std::vector<complex> arr
)
{
	unsigned long long int time = 0;

	for(unsigned long long int i = 0; i < t; ++i)
	{
		std::vector<complex> data = arr;

		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		DFT(size, data.data());

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	return time / t;
}
