#ifndef _COMPLEX_H_
#define _COMPLEX_H_

typedef struct
{
	double real;
	double imag;
} complex;

complex & operator+
(
	complex & larg,
	complex & rarg
);

complex & operator-
(
	complex & larg,
	complex & rarg
);

complex & operator*
(
	complex & larg,
	complex & rarg
);

complex & operator/
(
	complex & larg,
	complex & rarg
);

complex & operator*=
(
	complex & larg,
	complex & rarg
);

complex & operator/=
(
	complex & larg,
	complex & rarg
);

#endif // !_COMPLEX_H_