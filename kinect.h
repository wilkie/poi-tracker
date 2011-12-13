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

// Drawers
#include "point_drawer.h"
#include "user_drawer.h"
#include "recorder.h"

class Kinect {
public:
	Kinect();
	bool initialize(char* xml_path);
	void cleanup();

	void update();

	unsigned int width();
	unsigned int height();

	PointDrawer* pointDrawer();
	UserDrawer* userDrawer();

	Recorder* recorder();

private:
	unsigned int _width;
	unsigned int _height;
	
	XnBool g_bNeedPose;
	char g_strPose[20];
	XnBool g_bDrawBackground;
	XnBool g_bDrawPixels;
	XnBool g_bDrawSkeleton;
	XnBool g_bPrintID;
	XnBool g_bPrintState;

	// OpenNI
	xn::EnumerationErrors errors;

	xn::Context g_Context;
	xn::ScriptNode g_ScriptNode;
	xn::DepthGenerator g_DepthGenerator;
	xn::ImageGenerator g_ImageGenerator;
	xn::HandsGenerator g_HandsGenerator;
	xn::UserGenerator g_UserGenerator;
	xn::GestureGenerator g_GestureGenerator;

	// NITE
	XnVSessionManager* g_pSessionManager;
	XnVFlowRouter* g_pFlowRouter;

	// Drawer
	PointDrawer* g_pDrawer;
	UserDrawer* g_userDrawer;

	// Recorder
	Recorder* _recorder;

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

	static void XN_CALLBACK_TYPE userCalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie);
	static void XN_CALLBACK_TYPE userCalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie);
	static void XN_CALLBACK_TYPE userPoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie);
	static void XN_CALLBACK_TYPE userNew(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
	static void XN_CALLBACK_TYPE userLost(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
};

#endif
