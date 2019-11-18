#ifndef _DFT_H_
#define _DFT_H_

#include <algorithm>
#include <cmath>
#include <intrin.h>
#include <CL/cl.h>

#include "complex.h"

void DFT_Base
(
	unsigned long long int size,
	complex * arr,
	bool invert
);

void DFT_AVX
(
	unsigned long long int size,
	complex * arr,
	bool invert
);

#endif // !_DFT_H_