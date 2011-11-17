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

	return true;
}
