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

Canvas::Canvas() {
	// Create a window, initialize some GL shenanigans
	int argc = 0;
	const char* argv[] = {"./foo"};

	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Poi-tracker");

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}