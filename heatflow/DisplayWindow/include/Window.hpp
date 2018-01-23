#ifndef WINDOW_H
#define WINDOW_H

#ifdef __ANDROID__
	#include "AndroidWindow.hpp"
    typedef AndroidWindow DisplayWindow;
#else
    #include "SDLWindow.hpp"
    typedef SDLWindow DisplayWindow;
#endif

#endif
