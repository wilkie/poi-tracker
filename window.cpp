#include "window.h"

Window::Window(unsigned int width, unsigned int height) {
	// Initialize GLUT
	canvas = new Canvas(width, height);

	// Bind to input devices

	// Create a Presenter
	presenter = new Presenter(canvas);
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
