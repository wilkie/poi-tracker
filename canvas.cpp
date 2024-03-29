#include "canvas.h"

// For OpenNI constants
#include "kinect.h"

// GLUT
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// OpenGL
#include <GL/gl.h>

Canvas::Canvas(unsigned int width, unsigned int height) {
	// Create a window, initialize some GL shenanigans
	int argc = 0;
	const char* argv[] = {"./foo"};

	this->width = width;
	this->height = height;

	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Poi-tracker");

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void Canvas::loop() {
	glutMainLoop();
}

void Canvas::clear() {
	// Clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Set up ortho mode
	glOrtho(0, width, height, 0, -1.0, 1.0);

	glDisable(GL_TEXTURE_2D);
}

void Canvas::swap() {
	glutSwapBuffers();
}

void Canvas::redisplay() {
	glutPostRedisplay();
}
