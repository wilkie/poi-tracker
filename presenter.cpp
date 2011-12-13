#include "presenter.h"

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

Presenter* Presenter::self;

Presenter::Presenter(Canvas* c, Kinect* k) {
	// Store the canvas
	canvas = c;
	kinect = k;
	player = NULL;

	// Store ourself
	self = this;

	// Tell glut about our functions
	glutDisplayFunc(&Presenter::glutDisplay);
	glutIdleFunc(&Presenter::glutIdle);
}

Presenter::Presenter(Canvas* c, Player* p) {
	// Store the canvas
	canvas = c;
	player = p;
	kinect = NULL;

	// Store ourself
	self = this;

	// Tell glut about our functions
	glutDisplayFunc(&Presenter::glutDisplay);
	glutIdleFunc(&Presenter::glutIdle);
}

void Presenter::draw() {
	canvas->clear();

	if (kinect != NULL) {
		kinect->update();
		kinect->pointDrawer()->Draw();
		kinect->userDrawer()->draw();
	}
	else {
		player->draw();
	}

	canvas->swap();
}

void Presenter::run() {
	canvas->loop();
}

void Presenter::glutDisplay() {
	self->draw();
}

void Presenter::glutIdle() {
	self->canvas->redisplay();
}
