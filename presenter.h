#ifndef PRESENTER_H
#define PRESENTER_H

#include "canvas.h"
#include "kinect.h"

class Presenter {
public:
	Presenter(Canvas* c, Kinect* t);

	void draw();
	void run();

private:

	Canvas* canvas;
	Kinect* kinect;
	static Presenter* self;

	static void glutDisplay();
	static void glutIdle();
};

#endif
