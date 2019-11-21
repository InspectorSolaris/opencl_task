#ifndef _PRINT_H_

#include <ostream>
#include <vector>

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

std::ostream & operator<<
(
	std::ostream & stream,
	const complex z
	);

std::ostream & operator<<
(
	std::ostream & stream,
	const std::vector<complex> & vec
	);

#endif // !_PRINT_H_