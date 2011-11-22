#include "presenter.h"

#include <GL/glut.h>

Presenter* Presenter::self;

Presenter::Presenter(Canvas* c, Kinect* k) {
	// Store the canvas
	canvas = c;
	kinect = k;

	// Store ourself
	self = this;

	// Tell glut about our functions
	glutDisplayFunc(&Presenter::glutDisplay);
	glutIdleFunc(&Presenter::glutIdle);
}

void Presenter::draw() {
	canvas->clear();

	kinect->update();

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
