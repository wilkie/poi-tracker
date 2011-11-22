#ifndef TASK_H
#define TASK_H

#include "kinect.h"

class Task {
public:

	Task(Kinect* k);

	void run();
	void draw();
};

#endif
