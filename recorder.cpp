#include "recorder.h"

#ifdef _WIN32
	#include <Mmsystem.h>
#else
#endif

Recorder::Recorder(char* path, int width, int height) {
	_output = fopen(path, "w+");
	_last_time = 0;
	fprintf(_output, "%d %d\n", width, height);
}

void Recorder::record(XnPoint3D pts[15]) {
	if (_last_time == 0) {
		#ifdef _WIN32
			_last_time = timeGetTime();
		#else
		#endif
	}

	unsigned int curr_time;

	#ifdef _WIN32
		curr_time = timeGetTime();
	#else
	#endif

	fprintf(_output, "%d ", curr_time - _last_time);
	for (int i = 0; i < 15; i++) {
		fprintf(_output, "{%f, %f, %f}, ", pts[i].X, pts[i].Y, pts[i].Z);
	}
	fprintf(_output, "\n");

	_last_time = curr_time;
}
