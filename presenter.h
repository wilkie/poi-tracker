#ifndef PRESENTER_H
#define PRESENTER_H

#include "canvas.h"

class Presenter {
public:
	Presenter();

	void draw();
	void run();

private:

	Canvas* canvas;
};

#endif
