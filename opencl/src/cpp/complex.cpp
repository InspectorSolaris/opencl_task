#include "complex.h"

complex & operator+
(
	complex & larg,
	complex & rarg
)
{
	larg.real += rarg.real;
	larg.imag += rarg.imag;

	return larg;
}

complex & operator-
(
	complex & larg,
	complex & rarg
)
{
	larg.real -= rarg.real;
	larg.imag -= rarg.imag;

	return larg;
}

complex & operator*
(
	complex & larg,
	complex & rarg
)
{
	double a = larg.real;
	double b = larg.imag;
	double c = rarg.real;
	double d = rarg.imag;

	double real = a * c - b * d;
	double imag = b * c + a * d;

	larg = {real, imag};

	return larg;
}

complex & operator/
(
	complex & larg,
	complex & rarg
)
{
	double a = larg.real;
	double b = larg.imag;
	double c = rarg.real;
	double d = rarg.imag;

	double real = (a * c + b * d) / (c * c + d * d);
	double imag = (b * c - a * d) / (c * c + d * d);

	larg = {real, imag};

	return larg;
}

complex & operator*=
(
	complex & larg,
	complex & rarg
)
{
	return larg * rarg;
}

complex & operator/=
(
	complex & larg,
	complex & rarg
)
{
	return larg / rarg;
}