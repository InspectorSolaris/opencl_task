#ifndef _COMPLEX_H_
#define _COMPLEX_H_

typedef struct
{
	float real;
	float imag;
} complex;

complex operator+
(
	const complex & larg,
	const complex & rarg
);

complex operator-
(
	const complex & larg,
	const complex & rarg
);

complex operator*
(
	const complex & larg,
	const complex & rarg
);

complex operator/
(
	const complex & larg,
	const complex & rarg
);

complex & operator*=
(
	complex & larg,
	const complex & rarg
);

complex & operator/=
(
	complex & larg,
	const complex & rarg
);

#endif // !_COMPLEX_H_