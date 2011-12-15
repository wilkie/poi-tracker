#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

class Player {
public:
	Player(char* file);
	void play();
	void draw();

	int width();
	int height();

private:
	FILE* _input;
	int _width;
	int _height;
	unsigned int _start_time;
	unsigned int _play_time;
	
	float _this_frame[15][3];
	float _next_frame[15][3];
	unsigned int _next_time;

	void _nextFrame(unsigned int &elapsed, float joints[15][3]);
	void _drawFrame();
	void _drawLimb(size_t idx1, size_t idx2);
};

#endif
