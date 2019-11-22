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
	const unsigned long long int lg_n = 10;

	std::vector<complex> arr(n);

	for(unsigned long long int i = 0; i < n; ++i)
	{
		idata >> arr[i].real;

		arr[i].imag = 0;
	}

	std::vector<complex> arr_base = arr;
	std::vector<complex> arr_avx = arr;
	std::vector<complex> arr_mp = arr;
	std::vector<cl_double2> arr_cl(n);

	for (unsigned long long int i = 0; i < n; ++i)
	{
		arr_cl[i].x = 1;
		arr_cl[i].y = 0;
	}

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

	cl_int errcode_ret = 0;

	cl_platform_id platform = 0;

	clGetPlatformIDs(1, &platform, nullptr);

	cl_device_id device = 0;

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

	cl_context context = clCreateContext(0, 1, &device, nullptr, nullptr, nullptr);

	const unsigned long long int files_amount = 1;

	std::string source = GetFile("src/cl/dft.cl");

	const char * sources[files_amount] = { source.c_str() };

	unsigned long long int source_size[files_amount] = { source.size() };

	cl_program program = clCreateProgramWithSource(context, files_amount, sources, source_size, &errcode_ret);

	errcode_ret = clBuildProgram(program, 1, &device, "-cl-std=CL1.2", nullptr, nullptr);

	//{
	//	cl_build_status status = 0;
	//	char srcs[8192];

	//	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 8192, srcs, 0);
	//	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, 8192, &status, 0);

	//	odata << srcs << ' ' << status << '\n';
	//}

	cl_mem cl_n = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), nullptr, &errcode_ret);
	cl_mem cl_lg_n = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), nullptr, &errcode_ret);
	cl_mem cl_arr = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_double2) * n, nullptr, &errcode_ret);

	cl_command_queue comand_queue = clCreateCommandQueue(context, device, 0, nullptr);

	cl_event wb_events[3] = {0, 0, 0};

	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_n, CL_FALSE, 0, sizeof(cl_uint), &n, 0, nullptr, &wb_events[0]);
	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_lg_n, CL_FALSE, 0, sizeof(cl_uint), &lg_n, 0, nullptr, &wb_events[1]);
	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_arr, CL_FALSE, 0, sizeof(cl_double2) * n, arr_cl.data(), 0, nullptr, &wb_events[2]);

	cl_kernel kernel = clCreateKernel(program, "DFT_CL", &errcode_ret);

	errcode_ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&cl_n);
	errcode_ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&cl_lg_n);
	errcode_ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cl_arr);

	cl_event kernel_event = 0;

	errcode_ret = clEnqueueNDRangeKernel(comand_queue, kernel, 1, nullptr, &n, nullptr, 3, wb_events, &kernel_event);

	errcode_ret = clEnqueueReadBuffer(comand_queue, cl_arr, CL_TRUE, 0, sizeof(cl_double2) * n, arr_cl.data(), 1, &kernel_event, nullptr);

	clFinish(comand_queue);



	return 0;
}