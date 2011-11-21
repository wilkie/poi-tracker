#ifndef WINDOW_H
#define WINDOW_H

#include "canvas.h"
#include "presenter.h"
#include "input_handler.h"

class Window {
public:

	Window();

	InputHandler* windowHandler();
	Canvas* windowCanvas();
	Presenter* windowPresenter();

private:

	Canvas* canvas;
	Presenter* presenter;
	InputHandler* handler;
};

#endif
