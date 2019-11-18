#include "complex.h"

__kernel void DFT_CL
(
	__global unsigned long long int size,
	__global complex * arr,
	__global bool invert
)
{
	__local unsigned long long int n = size;
	__local complex * a = arr;
	__local bool inv = invert;




}