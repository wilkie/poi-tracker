#include "recorder.h"

Recorder::Recorder(char* path) {
	_output = fopen(path, "w+");
}

void Recorder::record(XnPoint3D pts[15]) {
	fprintf(_output, "%d ", 0);
	for (int i = 0; i < 15; i++) {
		fprintf(_output, "{%f, %f, %f}, ", pts[i].X, pts[i].Y, pts[i].Z);
	}
	fprintf(_output, "\n");
}
