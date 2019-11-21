#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <vector>
#include <chrono>

#include "complex.h"

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
);


#endif // !_BENCHMARK_H_