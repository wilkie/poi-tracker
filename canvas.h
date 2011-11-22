#ifndef CANVAS_H
#define CANVAS_H

class Canvas {
public:
	Canvas(unsigned int width, unsigned int height);

	void loop();

	void clear();
	void swap();
	void redisplay();

private:

	unsigned int width;
	unsigned int height;
};

#endif
