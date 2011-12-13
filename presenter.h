#ifndef PRESENTER_H
#define PRESENTER_H

#include "canvas.h"
#include "kinect.h"
#include "player.h"

class Presenter {
public:
	Presenter(Canvas* c, Kinect* t);
	Presenter(Canvas* c, Player* p);

	void draw();
	void run();

private:

	Canvas* canvas;
	Kinect* kinect;
	Player* player;
	static Presenter* self;

	static void glutDisplay();
	static void glutIdle();
};

#endif
