#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#include "complex.h"

unsigned long long Lg2N
(
	unsigned long long n
);

unsigned long long RevBits
(
	unsigned long long x,
	unsigned long long lg_n
);

complex Pow
(
	const complex & n,
	unsigned long long x
);

#endif // !_OPERATIONS_H_