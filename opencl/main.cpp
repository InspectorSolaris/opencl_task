#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <CL/opencl.h>

#include "dft.h"
#include "complex.h"
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

int main()
{
	std::ifstream idata(ipath);
	std::ofstream odata(opath);

	const unsigned long long int t = 1024;
	const unsigned long long int n = 1024;
	const unsigned long long int lg_n = 10;
	const bool not_true = false;

	std::vector<complex> arr(n);
	std::vector<complex> arr_AVX(n);
	std::vector<complex> arr_MP(n);
	std::vector<cl_double2> arr_CL(n);

	for(unsigned long long int i = 0; i < n; ++i)
	{
		idata >> arr[i].real;

		arr[i].imag = 0;

		arr_AVX[i] = arr[i];
		arr_MP[i] = arr[i];
		arr_CL[i].x = arr[i].real;
		arr_CL[i].y = arr[i].imag;
	}

	cl_int errcode_ret = 0;

	cl_platform_id platform = 0;

	clGetPlatformIDs(1, &platform, nullptr);

	cl_device_id device = 0;

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

	cl_context context = clCreateContext(0, 1, &device, nullptr, nullptr, nullptr);

	const unsigned long long int files_amount = 1;

	std::string source = GetFile("src/cl/dft.cl");

	const char * sources[files_amount] = {source.c_str()};

	unsigned long long int source_size[files_amount] = {source.size()};

	cl_program program = clCreateProgramWithSource(context, files_amount, sources, (const unsigned int *)source_size, &errcode_ret);
	
	errcode_ret = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

	{
		cl_build_status status = 0;
		char srcs[8192];

		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 8192, srcs, 0);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, 8192, &status, 0);
	
		odata << srcs;
	}

	cl_mem cl_n		= clCreateBuffer(context, CL_MEM_READ_ONLY,		sizeof(cl_ulong),		nullptr, &errcode_ret);
	cl_mem cl_lg_n	= clCreateBuffer(context, CL_MEM_READ_ONLY,		sizeof(cl_ulong),		nullptr, &errcode_ret);
	cl_mem cl_arr	= clCreateBuffer(context, CL_MEM_READ_WRITE,	sizeof(cl_double2) * n,	nullptr, &errcode_ret);
	cl_mem cl_inv	= clCreateBuffer(context, CL_MEM_READ_ONLY,		sizeof(cl_int),			nullptr, &errcode_ret);

	cl_command_queue comand_queue = clCreateCommandQueue(context, device, 0, nullptr);

	cl_event wb_events[4];

	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_n,		CL_FALSE, 0, sizeof(cl_uint),			&n,				0, nullptr, &wb_events[0]);
	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_lg_n,	CL_FALSE, 0, sizeof(cl_uint),			&lg_n,			0, nullptr, &wb_events[1]);
	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_arr,	CL_FALSE, 0, sizeof(cl_double2) * n,	arr_CL.data(),	0, nullptr, &wb_events[2]);
	errcode_ret = clEnqueueWriteBuffer(comand_queue, cl_inv,	CL_FALSE, 0, sizeof(cl_int),			&not_true,		0, nullptr, &wb_events[3]);

	cl_kernel kernel = clCreateKernel(program, "DFT_CL", &errcode_ret);

	errcode_ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&cl_n);
	errcode_ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&cl_lg_n);
	errcode_ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cl_arr);
	errcode_ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&cl_inv);

	cl_event kernel_event;

	errcode_ret = clEnqueueNDRangeKernel(comand_queue, kernel, 1, nullptr, (unsigned int *)&n, nullptr, 4, wb_events, &kernel_event);

	errcode_ret = clEnqueueReadBuffer(comand_queue, cl_arr, CL_TRUE, 0, sizeof(cl_double2) * n, arr_CL.data(), 1, &kernel_event, nullptr);

	clFinish(comand_queue);

	return 0;
}