#ifndef RECORDER_H
#define RECORDER_H

// OpenNI
#ifdef _WIN32
	#include <XnOpenNI.h>
	#include <XnCppWrapper.h>
	#include <XnHash.h>
	#include <XnLog.h>
#else
	#include <ni/XnOpenNI.h>
	#include <ni/XnCppWrapper.h>
	#include <ni/XnHash.h>
	#include <ni/XnLog.h>
#endif

// NITE
#ifdef _WIN32
	#include <XnVNite.h>
#else
	#include <nite/XnVNite.h>
#endif

class Recorder {
public:
	Recorder();
	void record(XnPoint3D pts[13]);

private:
	FILE* _output;
};

#endif
