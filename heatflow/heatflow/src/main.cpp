#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

using namespace std::chrono;

#include <Window.hpp>

#include <CL/cl.hpp>

cl::Platform platform;
cl::Device device;
cl::Context clctx;
cl::Program program;
cl::Kernel stepKernel;
cl::Buffer heatBuffer[2];
cl::CommandQueue queue;

class MainWindow : public DisplayWindow
{
public:
	MainWindow() = default;
	void initData();
	virtual ~MainWindow();
	void resizeEvent(int width, int height);
	void mouseDownEvent(Window::Button button);
	void keyDownEvent(Window::Key key);
	void renderEvent();

	void initCL(int width, int height, float* buffer);
	void stepCL(int num);
	void readback(float* buffer, int width, int height);
	void step(float* in, float* out, int width, int height);

	float* buffer[2];
	int simWidth;
	int simHeight;
};

static bool useGPU = true;

void MainWindow::initData()
{
	int width = getWidth()*1.8;
	int height = getHeight()*1.8;
	buffer[0] = new float [width*height];
	buffer[1] = new float [width*height];

	for(int i=0;i<width*height;i++)
		buffer[0][i] = 0.f;
	for(int i=0;i<width;i++)
		buffer[0][i] = 1.f;
	for(int i=0;i<width;i++)
		buffer[0][(height-1)*width+i] = 1.f;
	memcpy(buffer[1],buffer[0],sizeof(float)*width*height);

	initCL(width, height, buffer[0]);

	simWidth = width;
	simHeight = height;
}

MainWindow::~MainWindow()
{
	delete [] buffer[0];
	delete [] buffer[1];
}

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

void MainWindow::step(float* in, float* out, int width, int height)
{
	float K = 0.95;
	#pragma omp parallel for
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


void MainWindow::renderEvent()
{
	int simulationTimes = 50;
	auto t1 = high_resolution_clock::now();
	if(useGPU)
	{
		stepCL(simulationTimes);
		readback(buffer[0], simWidth, simHeight);
	}
	else
	{
		for(int i=0;i<simulationTimes;i++)
			step(buffer[i%2], buffer[!(i%2)], simWidth, simHeight);
	}
	auto t2 = high_resolution_clock::now();
	auto time_span = duration_cast<duration<double>>(t2 - t1);

	std::cout << "Simulation tooks " << time_span.count() << " seconds. On " << (useGPU?"OpenCL ":"CPU ")
		<< simulationTimes/time_span.count()*simulationTimes << "simulations/s\r" << std::flush;
	generateRenderTexture(buffer[0], simWidth, simHeight);
}

void MainWindow::initCL(int width, int height, float* buffer)
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

	clctx = cl::Context({device});
	cl::Program::Sources sources;
	std::ifstream in("../kernels.cl");
	std::string kernelCode((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	sources.push_back({kernelCode.c_str(),kernelCode.length()});

        program = cl::Program(clctx,sources);
        if(program.build({device},"-cl-fast-relaxed-math -cl-mad-enable")!=CL_SUCCESS)
	{
        	std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";
        	exit(1);
        }
	stepKernel = cl::Kernel(program,"heatstep");
	queue = cl::CommandQueue(clctx, device);
	heatBuffer[0] = cl::Buffer(clctx, CL_MEM_READ_WRITE, sizeof(float)*width*height);
	heatBuffer[1] = cl::Buffer(clctx, CL_MEM_READ_WRITE, sizeof(float)*width*height);

	queue.enqueueWriteBuffer(heatBuffer[0],CL_TRUE,0,sizeof(float)*width*height, buffer);
	queue.enqueueWriteBuffer(heatBuffer[1],CL_TRUE,0,sizeof(float)*width*height, buffer);
	stepKernel.setArg(2, width);
	stepKernel.setArg(3, height);	
}

void MainWindow::stepCL(int num)
{
	stepKernel.setArg(0, heatBuffer[0]);
	stepKernel.setArg(1, heatBuffer[1]);
	
	for(int i=0;i<num;i++)
	{
		queue.enqueueNDRangeKernel(stepKernel,cl::NullRange,cl::NDRange(simHeight),cl::NDRange(32));
		stepKernel.setArg(0, heatBuffer[(i+1)%2]);
		stepKernel.setArg(1, heatBuffer[(i+2)%2]);
	}
}

void MainWindow::readback(float* buffer, int width, int height)
{
	queue.enqueueReadBuffer(heatBuffer[0],CL_TRUE,0,sizeof(float)*width*height, buffer);
}

int main(int argc, char** argv)
{

	if(argc == 1)
		useGPU = false;
	else
	{
		useGPU = false;
		if(std::string(argv[1]) == "ocl")
			useGPU = true;
	}
	
	MainWindow window;
	window.createWindow("SimpleRay - Basic Windowing", 1200, 800);
	window.initData();

	window.loop();
	window.terminate();
	return 0;
}
