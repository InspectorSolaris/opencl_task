#ifndef _PRINT_H_

#include <ostream>

#include "complex.h"

void PrintInt
(
	long long int a,
	std::ostream & stream
);

void PrintString
(
	const char * string,
	std::ostream & stream
);

void PrintComplex
(
	complex & z,
	std::ostream & stream
);

void PrintComplexArray
(
	unsigned long long int size,
	complex * arr,
	std::ostream & stream
);

#endif // !_PRINT_H_