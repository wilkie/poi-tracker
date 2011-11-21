#include "window.h"

Window::Window() {
	// Initialize GLUT
	canvas = new Canvas();

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
