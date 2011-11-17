#ifndef WINDOW_H
#define WINDOW_H

#include "canvas.h"
#include "input_handler.h"

class Window {
public:

	Window();

	InputHandler* windowHandler();
	Canvas* windowCanvas();
};

#endif
