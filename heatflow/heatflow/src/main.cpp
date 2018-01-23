#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <chrono>

using namespace std::chrono;

#include <Window.hpp>

#include <CL/cl.hpp>

cl::Platform platform;
cl::Device device;
cl::Context context;
cl::Program program;
cl::Kernel stepKernel;
cl::Buffer heatBuffer[2];
cl::CommandQueue queue;

class MainWindow : public DisplayWindow
{
public:
	void resizeEvent(int width, int height);
	void mouseDownEvent(Window::Button button);
	void keyDownEvent(Window::Key key);
};

void MainWindow::resizeEvent(int width, int height)
{
	printf("size %d,%d\n", width, height);
}

void MainWindow::mouseDownEvent(Window::Button button)
{
	if(button == Window::Button_Right)
		exit(0);
}

void MainWindow::keyDownEvent(Window::Key key)
{
	if(key == Window::Key::Key_Q)
	exit(0);
}

void step(float* in, float* out, int width, int height)
{
	float K = 0.5;
	#pragma omp parallel for num_threads(4)
	for (int y = 1; y < height-1; y++) {
		for (int x = 1; x < width-1; x++) {
			float surroundings =
				in[(y-1)*width+x-1] + in[(y-1)*width+x]   + in[(y-1)*width+x+1] +
				in[y*width+x-1]   +                in[y*width+x+1]   +
				in[(y+1)*width+x-1] + in[(y+1)*width+x]   + in[(y+1)*width+x+1];
			surroundings *= 0.125;
			out[y*width+x] = in[y*width+x] - (K * (in[y*width+x] - surroundings));
		}
	}
}

void initCL(int width, int height, float* buffer)
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if(platforms.size() == 0)
	{
		std::cout << "Error: No OpenCL platform found" << std::endl;
		exit(1);
	}
	platform = platforms[0];

	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	device = devices[0];

	context = cl::Context({device});
	cl::Program::Sources sources;
	std::ifstream in("kernels.cl");
	std::string kernelCode((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	sources.push_back({kernelCode.c_str(),kernelCode.length()});

        program = cl::Program(context,sources);
        if(program.build({device},"-cl-fast-relaxed-math -cl-mad-enable")!=CL_SUCCESS)
	{
        	std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";
        	exit(1);
        }
	stepKernel = cl::Kernel(program,"heatstep");
	queue = cl::CommandQueue(context, device);
	heatBuffer[0] = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float)*width*height);
	heatBuffer[1] = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float)*width*height);

	queue.enqueueWriteBuffer(heatBuffer[0],CL_TRUE,0,sizeof(float)*width*height, buffer);
	queue.enqueueWriteBuffer(heatBuffer[1],CL_TRUE,0,sizeof(float)*width*height, buffer);
	stepKernel.setArg(2, width);
	stepKernel.setArg(3, height);
	
}

void stepCL(int num)
{
	stepKernel.setArg(0, heatBuffer[0]);
	stepKernel.setArg(1, heatBuffer[1]);
	
	for(int i=0;i<num;i++)
	{
		queue.enqueueNDRangeKernel(stepKernel,cl::NullRange,cl::NDRange(800),cl::NDRange(16));
		stepKernel.setArg(0, heatBuffer[i%2]);
		stepKernel.setArg(1, heatBuffer[(i+1)%2]);
	}
}

void readback(float* buffer, int width, int height)
{
	queue.enqueueReadBuffer(heatBuffer[0],CL_TRUE,0,sizeof(float)*width*height, buffer);
}

int main()
{
	int width = 1200*3;
	int height = 800*3;
	float* buffer[2] = {new float [width*height],
		new float [width*height]};
	for(int i=0;i<width*height;i++)
		buffer[0][i] = 0.f;
	for(int i=0;i<width;i++)
		buffer[0][i] = 1.f;
	memcpy(buffer[1],buffer[0],sizeof(float)*width*height);

	initCL(width, height, buffer[0]);

	int simulationTimes = 1000;
	auto t1 = high_resolution_clock::now();
	#if 1
	stepCL(simulationTimes);
	readback(buffer[0], width, height);
	#else
	for(int i=0;i<simulationTimes;i++)
		step(buffer[i%2], buffer[!(i%2)], width, height);
	#endif
	auto t2 = high_resolution_clock::now();
	auto time_span = duration_cast<duration<double>>(t2 - t1);

	std::cout << "Simulation tooks " << time_span.count() << " seconds. "
		<< simulationTimes/time_span.count() << "simulations/s" << std::endl;
	
	MainWindow window;
	window.createWindow("SimpleRay - Basic Windowing", 1200, 800);
	window.generateRenderTexture(buffer[0], width, height);

	window.loop();
	window.terminate();

	delete [] buffer[0];
	delete [] buffer[1];
	return 0;
}
