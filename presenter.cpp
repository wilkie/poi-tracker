#include "presenter.h"

#include <GL/glut.h>

Presenter::Presenter(Canvas* c) {
	// Store the canvas
	canvas = c;

	// Store ourself
	self = this;

	// Tell glut about our functions
	glutDisplayFunc(&Presenter::glutDisplay);
	glutIdleFunc(&Presenter::glutIdle);
}

void Presenter::draw() {
}

void Presenter::run() {
	canvas->loop();
}

void Presenter::glutDisplay() {
}

void Presenter::glutIdle() {
}
