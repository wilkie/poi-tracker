#include "kinect.h"

bool Kinect::has_failed(XnStatus rc, const char* msg) {
	if (rc != XN_STATUS_OK) {
		printf("%s failed: %s\n", msg, xnGetStatusString(rc));
		return true;
	}
	return false;
}

bool Kinect::has_errors(XnStatus rc, xn::EnumerationErrors errors, const char* msg) {
	if (rc == XN_STATUS_NO_NODE_PRESENT) {
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		return true;
	}
	return false;
}

Kinect::Kinect() {
}

bool Kinect::initialize() {
	XnStatus rc = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	// Initialize OpenNI
	rc = g_Context.InitFromXmlFile("poi-tracker.xml", g_ScriptNode, &errors);

	if (has_errors(rc, errors, "InitFromXmlFile")) {
		return false;
	}

	if (has_failed(rc, "InitFromXmlFile")) {
		return false;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	if (has_failed(rc, "Find depth generator")) {
		return false;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_HANDS, g_HandsGenerator);
	if (has_failed(rc, "Find hands generator")) {
		return false;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_GESTURE, g_GestureGenerator);
	if (has_failed(rc, "Find gesture generator")) {
		return false;
	}

	XnCallbackHandle h;
	if (g_HandsGenerator.IsCapabilitySupported(XN_CAPABILITY_HAND_TOUCHING_FOV_EDGE)) {
		g_HandsGenerator.GetHandTouchingFOVEdgeCap().RegisterToHandTouchingFOVEdge(&Kinect::touchingCallback, this, h);
	}

	XnCallbackHandle hGestureIntermediateStageCompleted, hGestureProgress, hGestureReadyForNextIntermediateStage;
	g_GestureGenerator.RegisterToGestureIntermediateStageCompleted(&Kinect::gestureIntermediateCallback, this, hGestureIntermediateStageCompleted);
	g_GestureGenerator.RegisterToGestureReadyForNextIntermediateStage(&Kinect::gestureReadyForNextIntermediateCallback, this, hGestureReadyForNextIntermediateStage);
	g_GestureGenerator.RegisterGestureCallbacks(NULL, &Kinect::gestureProgressCallback, this, hGestureProgress);

	return true;
}

void Kinect::cleanup() {
	g_ScriptNode.Release();
	g_DepthGenerator.Release();
	g_HandsGenerator.Release();
	g_GestureGenerator.Release();
	g_Context.Release();
}

void Kinect::touchingCallback(xn::HandTouchingFOVEdgeCapability &generator, XnUserID id, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	//k->g_pDrawer->SetTouchingFOVEdge(id);
}

void Kinect::gestureIntermediateCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: Intermediate stage complete (%f, %f, %f)\n",
		strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}

void Kinect::gestureProgressCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: progress: %f (%f, %f, %f)\n",
		strGesture, fProgress, pPosition->X, pPosition->Y, pPosition->Z);
}

void Kinect::gestureReadyForNextIntermediateCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: Ready for next intermediate stage (%f, %f, %f)\n",
		strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}
