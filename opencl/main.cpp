#include <iostream>
#include <fstream>
#include <vector>
#include <CL/opencl.h>

#include "dft.h"
#include "complex.h"

constexpr const char * ipath = "io/i.io";
constexpr const char * opath = "io/o.io";

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

void PrintComplexArray
(
	unsigned long long int size,
	complex * arr,
	std::ostream & stream
)
{
	for(unsigned long long int i = 0; i < size; ++i)
	{
		stream << "(" << arr[i].real << ", " << arr[i].imag << ") ";
	}

	stream << '\n';

	return;
}

int main()
{
	std::ofstream idata(ipath);
	std::ofstream odata(opath);

	const unsigned long long int n = 10;
	const unsigned long long int size[n] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

	for(unsigned long long int i = 0; i < n; ++i)
	{
		std::vector<complex> arr(size[i]);

		for(unsigned long long int j = 0; j < size[i]; ++j)
		{
			arr[j].real = rand() % 100;
			arr[j].imag = 0;
		}

		PrintString			("Data size: ",			idata);
		PrintInt			(size[i],				idata);
		PrintString			("\n",					idata);
		PrintComplexArray	(size[i], arr.data(),	idata);

		std::vector<complex> base(arr.begin(), arr.end());
		std::vector<complex> avx(arr.begin(), arr.end());
		std::vector<complex> gpu(arr.begin(), arr.end());
		std::vector<complex> cpu(arr.begin(), arr.end());

		DFT_Base(size[i], base.data(), false);
		DFT_Base(size[i], base.data(), true);
	}

	cl_platform_id platform[2] = {0, 0};
	cl_uint platforms_available = 0;

	clGetPlatformIDs(2, platform, &platforms_available);

	cl_device_id gpu = 0;
	cl_device_id cpu = 0;

	clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, 1, &gpu, nullptr);
	clGetDeviceIDs(platform[1], CL_DEVICE_TYPE_CPU, 1, &cpu, nullptr);

	cl_context gpu_context = clCreateContext(nullptr, 1, &gpu, nullptr, nullptr, nullptr);
	cl_context cpu_context = clCreateContext(nullptr, 1, &cpu, nullptr, nullptr, nullptr);

	cl_command_queue gpu_cq = clCreateCommandQueue(gpu_context, gpu, 0, nullptr);
	cl_command_queue cpu_cq = clCreateCommandQueue(cpu_context, cpu, 0, nullptr);



	return 0;
}