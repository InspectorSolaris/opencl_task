#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <CL/opencl.h>

#include "dft.h"
#include "complex.h"
#include "benchmark.h"
#include "print.h"

constexpr const char * ipath = "io/i.io";
constexpr const char * opath = "io/o.io";

std::string GetFile
(
	std::string file_path
)
{
	std::stringstream sstr;
    sstr << std::ifstream(file_path).rdbuf();

    return sstr.str();
}

double Deviation
(
	const std::vector<complex> & a,
	const std::vector<complex> & b
)
{
	double sum = 0;

	if(a.size() != b.size())
	{
		return -1;
	}

	for(unsigned long long int i = 0; i < a.size(); ++i)
	{
		const complex sub = a[i] - b[i];

		sum += (double)(sub).real * (sub).real;
		sum += (double)(sub).imag * (sub).imag;
	}

	return std::sqrt(sum / a.size());
}

int main()
{
	std::ifstream idata(ipath);
	std::ofstream odata(opath);

	const unsigned long long int t = 1024;
	const unsigned long long int n = 1024;

	std::vector<complex> arr(n);

	for(unsigned long long int i = 0; i < n; ++i)
	{
		idata >> arr[i].real;

		arr[i].imag = 0;
	}

	std::vector<complex> arr_base = arr;
	std::vector<complex> arr_avx = arr;
	std::vector<complex> arr_mp = arr;

	unsigned long long int base_time = BenchmarkDFT(t, DFT_Base, arr.size(), arr);
	unsigned long long int avx_time = BenchmarkDFT(t, DFT_AVX, arr.size(), arr);
	unsigned long long int mp_time = BenchmarkDFT(t, DFT_MP, arr.size(), arr);

	DFT_Base(arr_base.size(), arr_base.data());
	DFT_AVX(arr_avx.size(), arr_avx.data());
	DFT_MP(arr_mp.size(), arr_mp.data());

	double base_deviation = Deviation(arr_base, arr_base);
	double avx_deviation = Deviation(arr_base, arr_avx);
	double mp_deviation = Deviation(arr_base, arr_mp);

	odata << "Base (" << base_time << "mics)(" << base_deviation << ")\n" << arr_base << '\n';
	odata << "AVX (" << avx_time << "mics)(" << avx_deviation << ")\n" << arr_avx << '\n';
	odata << "MP (" << mp_time << "mics)(" << mp_deviation << ")\n" << arr_mp << '\n';

	return 0;
}