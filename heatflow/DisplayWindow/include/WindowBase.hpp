#ifndef WINDOWBASE_H
#define WINDOWBASE_H

#if defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
	#ifndef __WINDOWS__
		#define __WINDOWS__
	#endif
	#include <windows.h>
#endif

#include "GLdef.hpp"

namespace Incarnate
{
class Film;
}

typedef unsigned int GLuint;

namespace Window
{
	enum Button
	{
		Button_Left,
		Button_Middle,
		Button_Right
	};

	enum Key
	{
		Key_Unknown = -1,
		Key_A = 0,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,
		Key_Num0,
		Key_Num1,
		Key_Num2,
		Key_Num3,
		Key_Num4,
		Key_Num5,
		Key_Num6,
		Key_Num7,
		Key_Num8,
		Key_Num9,
		Key_Escape,
		Key_LControl,
		Key_LShift,
		Key_LAlt,
		Key_LSystem,
		Key_RControl,
		Key_RShift,
		Key_RAlt,
		Key_RSystem,
		Key_Menu,
		Key_LBracket,
		Key_RBracket,
		Key_SemiColon,
		Key_Comma,
		Key_Period,
		Key_Quote,
		Key_Slash,
		Key_BackSlash,
		Key_Tilde,
		Key_Equal,
		Key_Dash,
		Key_Space,
		Key_Return,
		Key_BackSpace,
		Key_Tab,
		Key_PageUp,
		Key_PageDown,
		Key_End,
		Key_Home,
		Key_Insert,
		Key_Delete,
		Key_Add,
		Key_Subtract,
		Key_Multiply,
		Key_Divide,
		Key_Left,
		Key_Right,
		Key_Up,
		Key_Down,
		Key_NumPad0,
		Key_NumPad1,
		Key_NumPad2,
		Key_NumPad3,
		Key_NumPad4,
		Key_NumPad5,
		Key_NumPad6,
		Key_NumPad7,
		Key_NumPad8,
		Key_NumPad9,
		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,
		Key_F13,
		Key_F14,
		Key_F15,
		Key_Pause
	};

	class Shader;
	class Surface;
}

class WindowBase
{
public:
	WindowBase(const WindowBase&) = delete;
	WindowBase();
	virtual ~WindowBase();

	void generateRenderTexture(const unsigned char* buffer, int width, int height);
	void generateRenderTexture(const float* buffer, int width, int height);
	void generateRenderTexture(const Incarnate::Film* buffer, int width, int height);
	void render();
	void setGamma(float gamma);
	virtual void loop() = 0;
	virtual void terminate(){};

	virtual void renderEvent(){};

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual void getCursorPos(int* x, int* y){};

protected:
	bool initGL();
	void terminateGL();

	Window::Shader* shader;
	Window::Surface* surface;
};

#endif
