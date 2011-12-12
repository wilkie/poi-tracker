#ifndef KINECT_H
#define KINECT_H

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

// Drawer
#include "point_drawer.h"

class Kinect {
public:
	Kinect();
	bool initialize();
	void cleanup();

	void update();

	unsigned int width();
	unsigned int height();

	PointDrawer* pointDrawer();

private:
	unsigned int _width;
	unsigned int _height;

	// OpenNI
	xn::EnumerationErrors errors;

	xn::Context g_Context;
	xn::ScriptNode g_ScriptNode;
	xn::DepthGenerator g_DepthGenerator;
	xn::ImageGenerator g_ImageGenerator;
	xn::HandsGenerator g_HandsGenerator;
	xn::GestureGenerator g_GestureGenerator;

	// NITE
	XnVSessionManager* g_pSessionManager;
	XnVFlowRouter* g_pFlowRouter;

	// Drawer
	PointDrawer* g_pDrawer;

	bool has_failed(XnStatus rc, const char* msg);
	bool has_errors(XnStatus rc, xn::EnumerationErrors errors, const char* msg);

	static void __stdcall touchingCallback(xn::HandTouchingFOVEdgeCapability&, XnUserID, const XnPoint3D*, XnFloat, XnDirection, void*);
	static void __stdcall gestureIntermediateCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, void*);
	static void __stdcall gestureProgressCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, XnFloat, void*);
	static void __stdcall gestureReadyForNextIntermediateCallback(xn::GestureGenerator&, const XnChar*, const XnPoint3D*, void*);

	// NITE Callbacks
	static void __stdcall focusProgress(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress, void* UserCxt);
	static void __stdcall sessionStarting(const XnPoint3D& ptPosition, void* UserCxt);
	static void __stdcall sessionEnding(void* UserCxt);
	static void __stdcall noHands(void* UserCxt);
};

#endif
