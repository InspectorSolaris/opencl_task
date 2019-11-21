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

std::ostream & operator<<
(
	std::ostream & stream,
	const complex z
	)
{
	stream << z.real << std::showpos << z.imag << std::noshowpos << 'i';

	return stream;
}

std::ostream & operator<<
(
	std::ostream & stream,
	const std::vector<complex> & vec
	)
{
	for(unsigned long long int i = 0; i < vec.size(); ++i)
	{
		stream << vec[i] << ' ';
	}

	return stream;
}