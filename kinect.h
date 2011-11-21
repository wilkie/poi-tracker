#ifndef KINECT_H
#define KINECT_H

// OpenNI
#include <ni/XnOpenNI.h>
#include <ni/XnCppWrapper.h>
#include <ni/XnHash.h>
#include <ni/XnLog.h>

// NITE
#include <nite/XnVNite.h>

class Kinect {
public:
	Kinect();
	bool initialize();
	void cleanup();

private:
	xn::EnumerationErrors errors;

	xn::Context g_Context;
	xn::ScriptNode g_ScriptNode;
	xn::DepthGenerator g_DepthGenerator;
	xn::HandsGenerator g_HandsGenerator;
	xn::GestureGenerator g_GestureGenerator;

	bool has_failed(XnStatus rc, const char* msg);
	bool has_errors(XnStatus rc, xn::EnumerationErrors errors, const char* msg);

	static void touchingCallback(xn::HandTouchingFOVEdgeCapability&, XnUserID, const XnPoint3D*, XnFloat, XnDirection, void*);
	static void gestureIntermediateCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, void*);
	static void gestureProgressCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, XnFloat, void*);
	static void gestureReadyForNextIntermediateCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, void*);
};

#endif
