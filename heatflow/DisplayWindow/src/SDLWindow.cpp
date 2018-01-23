#ifndef __ANDROID__

#include "SDLWindow.hpp"

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <SDLScancodeTable.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
using namespace std;

SDLWindow::SDLWindow()
{
	int size = sizeof(Window::sdlScancodeTable)/sizeof(SDL_Scancode);
	for(int i=0;i<size;i++)
		scancodeToKeyTable.insert({Window::sdlScancodeTable[i],(Window::Key)i});
}

bool SDLWindow::createWindow(const char* title,int width,int height)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "Failed to init SDL Error : " << SDL_GetError() << endl;
		return false;
	}

	//Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(window == NULL)
	{
		cout << "Failed to create window. Error : " << SDL_GetError() << endl;
		return false;
	}

	SDL_SetWindowSize(window, width, height);
	context = SDL_GL_CreateContext(window);
	if(context == NULL)
	{
		cout << "Failed to create OpenGL context. Error : " << SDL_GetError() << endl;
		return false;
	}

	//NOTE:Do we really want VSync?
	if(SDL_GL_SetSwapInterval(1) < 0)
		cout << "Warning: unable to activate Vsync. Error : " << SDL_GetError() << endl;

	//This inits The OpenGL enviroment WindowBase needs
	if(WindowBase::initGL() == false)
	{
		std::cout << "Error: Failed to init OpenGL stuff for WindowBase" << std::endl;
		return false;
	}

	return true;
}

void SDLWindow::loop()
{
	bool quit = false;
	SDL_Event event;
	SDL_StartTextInput();

	while(true)
	{
		while(SDL_PollEvent(&event)!= 0)
		{
			if(event.type == SDL_QUIT)
			{
				quit = true;
				break;
			}
			else if(event.type == SDL_WINDOWEVENT)
			{
				if(event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					//HACK: On windows, a unwanted resize event will accure after creating the window.
					//Ignore it.
					#if defined(__WIN32) || defined(__WIN64)
						static bool firstResizeEvent = true;
						if(!firstResizeEvent)
							resizeEvent(getWidth(),getHeight());
						else
							firstResizeEvent = false;
					#else
						resizeEvent(getWidth(),getHeight());
					#endif
				}
			}
			else if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				Window::Button button = Window::Button_Left;
				if(event.button.button == SDL_BUTTON_LEFT)
					button = Window::Button_Left;
				if(event.button.button == SDL_BUTTON_MIDDLE)
					button = Window::Button_Middle;
				if(event.button.button == SDL_BUTTON_RIGHT)
					button = Window::Button_Right;
				mouseDownEvent(button);
			}
			else if(event.type == SDL_KEYDOWN)
			{
				Window::Key key = Window::Key_Unknown;
				auto iterator = scancodeToKeyTable.find(event.key.keysym.scancode);
				if(iterator == scancodeToKeyTable.end())
				{
					cerr << "Warning: An unknown key was pressed. Please check the scan code list." << endl;
					key = Window::Key_Unknown;
				}
				else
					key = (*iterator).second;

				//HACK: Fix 2 SDL_KEYDOWN event occur when 1 key press on linux
				#ifdef __linux__
				static bool lastSet = false;
				if(lastSet==false)
					lastSet = true;
				else
				{
					lastSet = false;
					continue;
				}
				#endif
				keyDownEvent(key);
			}

		}
		if(quit == true)
			break;

		renderEvent();

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		render();

		SDL_GL_SwapWindow(window);
	}
	SDL_StopTextInput();
}

void SDLWindow::terminate()
{
	//Call this before doing anything. This will delete all OpenGL stuff WindowBase created.
	WindowBase::terminateGL();

	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}

int SDLWindow::getWidth()
{
	int w,h;
	SDL_GetWindowSize(window, &w, &h);
	return w;
}

int SDLWindow::getHeight()
{
	int w,h;
	SDL_GetWindowSize(window, &w, &h);
	return h;
}

void SDLWindow::getCursorPos(int* x, int* y)
{
	SDL_GetMouseState(x,y);
}

#endif
