#include "print.h"

void PrintInt
(
	long long int a,
	std::ostream & stream
)
{
	stream << a;

	return;
}

void PrintString
(
	const char * string,
	std::ostream & stream
)
{
	stream << string;

	return;
}

void PrintComplex
(
	complex & z,
	std::ostream & stream
)
{
	stream << "(" << z.real << "," << z.imag << ") ";

	return;
}

void PrintComplexArray
(
	unsigned long long int size,
	complex * arr,
	std::ostream & stream
)
{
	for(unsigned long long int i = 0; i < size; ++i)
	{
		PrintComplex(arr[i], stream);
	}

	stream << '\n';

	return;
}