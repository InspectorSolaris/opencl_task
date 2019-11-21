#include "complex.h"

complex operator+
(
	const complex & larg,
	const complex & rarg
)
{
	complex res = {larg.real + rarg.real, larg.imag + rarg.imag};

	return res;
}

complex operator-
(
	const complex & larg,
	const complex & rarg
)
{
	complex res = {larg.real - rarg.real, larg.imag - rarg.imag};

	return res;
}

complex operator*
(
	const complex & larg,
	const complex & rarg
)
{
	float a = larg.real;
	float b = larg.imag;
	float c = rarg.real;
	float d = rarg.imag;

	float real = a * c - b * d;
	float imag = b * c + a * d;

	complex res = {real, imag};

	return res;
}

complex operator/
(
	const complex & larg,
	const complex & rarg
)
{
	float a = larg.real;
	float b = larg.imag;
	float c = rarg.real;
	float d = rarg.imag;

	float real = (a * c + b * d) / (c * c + d * d);
	float imag = (b * c - a * d) / (c * c + d * d);

	complex res = {real, imag};

	return res;
}

complex & operator*=
(
	complex & larg,
	const complex & rarg
)
{
	larg = larg * rarg;

	return larg;
}

complex & operator/=
(
	complex & larg,
	const complex & rarg
)
{
	larg = larg / rarg;

	return larg;
}