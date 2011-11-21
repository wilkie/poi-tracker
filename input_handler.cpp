#include "input_handler.h"

#include <GL/glut.h>

InputHandler::InputHandler() {
	// Tell GLUT about our handlers
	glutKeyboardFunc(&InputHandler::glutKeyboard);
}

void InputHandler::glutKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27: // (escape) -- Exit
			break;
		case 'p':
			// Pause
			break;
		case 'd':
			// Depth Map Toggle
			break;
		case 's':
			// Smoothing Toggle
			break;
		case 'e':
			// End Session
			break;
	}
}
