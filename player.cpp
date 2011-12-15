#include "player.h"

// Sleep Support
#ifdef _WIN32
	#include<Windows.h>
#else
#endif

// Timer Support
#ifdef _WIN32
	#include <Mmsystem.h>
#else
#endif

// OpenGL
#include <GL/gl.h>

Player::Player(char* file) {
	_input = fopen(file, "r");

	fscanf(_input, "%d %d\n", &_width, &_height);
	_start_time = 0;
	_play_time = 0;
	_next_time = 0;
}

void Player::draw() {
	if (_start_time == 0) {
		#ifdef _WIN32
			_start_time = timeGetTime();
		#else
		#endif
	}
	
	unsigned int curr_time;

	#ifdef _WIN32
		curr_time = timeGetTime();
	#else
	#endif

	unsigned int _play_time = curr_time - _start_time;
	unsigned int elapsed;

	// Find next frame

	// Is this the first frame?
	if (feof(_input)) {
		// Done
	}
	else if (_next_time == 0) {
		// Play first frame
		if (!feof(_input)) {
			_nextFrame(elapsed, _this_frame);
		}

		// Get next frame
		if (!feof(_input)) {
			_nextFrame(_next_time, _next_frame);
		}
	}
	else if (_play_time >= _next_time) {
		// We can display the next frame
		memcpy(_this_frame, _next_frame, sizeof(_this_frame));
		
		// Get next frame
		while(!feof(_input) && _play_time >= _next_time) {
			_nextFrame(elapsed, _next_frame);
			_next_time += elapsed;
		}
	}

	printf("%d %d %d\n", _play_time, _next_time, feof(_input));
	_drawFrame();
}

enum Joints {
	HEAD = 0,
	NECK,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	LEFT_ELBOW,
	RIGHT_ELBOW,
	LEFT_HAND,
	RIGHT_HAND,
	LEFT_HIP,
	RIGHT_HIP,
	LEFT_KNEE,
	RIGHT_KNEE,
	LEFT_FOOT,
	RIGHT_FOOT,
	TORSO
};

void Player::_drawFrame() {
	glBegin(GL_LINES);

	glColor4f(1, 1, 1, 1);
	
	_drawLimb(HEAD, NECK);

	_drawLimb(NECK, LEFT_SHOULDER);
	_drawLimb(LEFT_SHOULDER, LEFT_ELBOW);
	_drawLimb(LEFT_ELBOW, LEFT_HAND);

	_drawLimb(NECK, RIGHT_SHOULDER);
	_drawLimb(RIGHT_SHOULDER, RIGHT_ELBOW);
	_drawLimb(RIGHT_ELBOW, RIGHT_HAND);

	_drawLimb(LEFT_SHOULDER, TORSO);
	_drawLimb(RIGHT_SHOULDER, TORSO);
	
	_drawLimb(TORSO, LEFT_HIP);
	_drawLimb(LEFT_HIP, LEFT_KNEE);
	_drawLimb(LEFT_KNEE, LEFT_FOOT);

	_drawLimb(TORSO, RIGHT_HIP);
	_drawLimb(RIGHT_HIP, RIGHT_KNEE);
	_drawLimb(RIGHT_KNEE, RIGHT_FOOT);

	_drawLimb(LEFT_HIP, RIGHT_HIP);

	glEnd();
}

void Player::_drawLimb(size_t idx1, size_t idx2) {
	glVertex3i(_this_frame[idx1][0], _this_frame[idx1][1], 0);
	glVertex3i(_this_frame[idx2][0], _this_frame[idx2][1], 0);
}

void Player::_nextFrame(unsigned int &elapsed, float joints[15][3]) {
	if(!feof(_input)) {
		fscanf(_input, "%d ", &elapsed);

		for(int i = 0; i < 15; i++) {
			fscanf(_input, "{%f, %f, %f}, ", &joints[i][0], &joints[i][1], &joints[i][2]);
		}

		fscanf(_input, "\n");
	}
}

void Player::play() {
	unsigned int elapsed;
	float joints[15][3];

	while(!feof(_input)) {
		_nextFrame(elapsed, joints);
	}
}

int Player::width() {
	return _width;
}

int Player::height() {
	return _height;
}
