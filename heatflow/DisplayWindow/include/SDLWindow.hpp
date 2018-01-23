#include "WindowBase.hpp"

#include <unordered_map>

typedef void* SDL_GLContext;
class SDL_Window;

class SDLWindow : public WindowBase
{
public:
	SDLWindow();
	bool createWindow(const char* title,int width,int height);
	void loop();
	void terminate();

	int getWidth();
	int getHeight();

	virtual void resizeEvent(int width, int height){}
	virtual void mouseDownEvent(Window::Button button){}
	virtual void keyDownEvent(Window::Key key){};
	virtual void renderEvent(){};

	void getCursorPos(int* x, int* y);

protected:
	SDL_Window* window = NULL;
	SDL_GLContext context = NULL;

	std::unordered_map<int,Window::Key> scancodeToKeyTable;
};
