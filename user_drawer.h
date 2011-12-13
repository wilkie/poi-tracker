#ifndef USER_DRAWER_H
#define USER_DRAWER_H

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

#include "recorder.h"

class UserDrawer {
public:
	UserDrawer(xn::DepthGenerator depthGenerator, xn::UserGenerator userGenerator, Recorder* recorder);

	void draw();

private:

	void drawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2);

	xn::UserGenerator userGenerator;
	xn::DepthGenerator depthGenerator;

	Recorder* recorder;
};

#endif
