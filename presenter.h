#ifndef PRESENTER_H
#define PRESENTER_H

#include "canvas.h"
#include "task.h"

class Presenter {
public:
	Presenter(Canvas* c, Task* t);

	void draw();
	void run();

private:

	Canvas* canvas;
	Task* task;
	static Presenter* self;

	static void glutDisplay();
	static void glutIdle();
};

#endif
