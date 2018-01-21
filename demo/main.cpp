#include <CL/cl.hpp>

#include <iostream>
#include <exception>
#include <string>
#include <fstream>

std::string loadFile(std::string path)
{
	std::ifstream t(path);
	if(t.good() == false)
		throw std::runtime_error("Cannot open file: " + path);
	std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
	return str;
}

std::vector<int> range(int n)
{
	std::vector<int> vec(n);
	for(int i=0;i<n;i++)
		vec[i] = i;
	return vec;
}

int main()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if(platforms.size() == 0)
	{
		std::cout << "No OpenCL platform avliable" << std::endl;
		return 0;
	}
	
	auto platform = platforms[0];
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if(devices.size() == 0)
	{
		std::cout << "No device in platform" << std::endl;
		return 0;
	}
	auto device = devices[0];
	std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
	
	std::string src = loadFile("kernel.cl");
	cl::Context context(device);
	cl::Program::Sources sources;

	sources.push_back({src.c_str(),src.length()});
	cl::Program program(context,sources);
	if(program.build({device}) != CL_SUCCESS)
	{
		std::cout<<"Error building: "
			<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) <<std::endl;
		return 0;
	}
	
	cl::Buffer bufferA(context,CL_MEM_READ_WRITE,sizeof(int)*10);
	cl::Buffer bufferB(context,CL_MEM_READ_WRITE,sizeof(int)*10);
	cl::Buffer bufferC(context,CL_MEM_READ_WRITE,sizeof(int)*10);
	cl::CommandQueue queue(context,device);
	
	std::vector<int> A = range(10);
	std::vector<int> B = range(10);
	queue.enqueueWriteBuffer(bufferA,CL_TRUE,0,sizeof(int)*10,&A[0]);
	queue.enqueueWriteBuffer(bufferB,CL_TRUE,0,sizeof(int)*10,&B[0]);
	
	cl::Kernel kernel(program,"add");
	kernel.setArg(0,bufferA);
	kernel.setArg(1,bufferB);
	kernel.setArg(2,bufferC);
	queue.enqueueNDRangeKernel(kernel,
			cl::NullRange,
			cl::NDRange(10),
			cl::NullRange);
	
	std::vector<int> C(10);
	queue.enqueueReadBuffer(bufferC,CL_TRUE,0,sizeof(int)*10,&C[0]);
	
	std::cout<< "result: ";
	for(auto v : C)
		std::cout << v << " ";
	std::cout << std::endl;
	
}

