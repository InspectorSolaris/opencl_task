#ifndef _DFT_H_
#define _DFT_H_

#include <algorithm>
#include <cmath>
#include <intrin.h>
#include <CL/cl.h>
#include <omp.h>

#include "complex.h"

void DFT_Base
(
	const unsigned long long int size,
	complex * const arr
);

void DFT_AVX
(
	const unsigned long long int size,
	complex * const arr
);

void DFT_MP
(
	const unsigned long long int size,
	complex * const arr
);

#endif // !_DFT_H_