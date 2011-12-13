#include "window.h"

Window::Window(unsigned int width, unsigned int height, Kinect* k) {
	// Initialize GLUT
	canvas = new Canvas(width, height);

	// Bind to input devices
	handler = new InputHandler();

	// Create a Presenter
	presenter = new Presenter(canvas, k);
}

Window::Window(unsigned int width, unsigned int height, Player* p) {
	// Initialize GLUT
	canvas = new Canvas(width, height);

	// Bind to input devices
	handler = new InputHandler();

	// Create a Presenter
	presenter = new Presenter(canvas, p);
}

InputHandler* Window::windowHandler() {
	return handler;
}

Canvas* Window::windowCanvas() {
	return canvas;
}

Presenter* Window::windowPresenter() {
	return presenter;
}
