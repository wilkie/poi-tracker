// OpenNI
#include <ni/XnOpenNI.h>
#include <ni/XnCppWrapper.h>
#include <ni/XnHash.h>
#include <ni/XnLog.h>

// NITE
#include <nite/XnVNite.h>

// TODO: Kill these globals with fire
xn::Context g_Context;
xn::ScriptNode g_ScriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::HandsGenerator g_HandsGenerator;
xn::GestureGenerator g_GestureGenerator;

bool has_failed(XnStatus rc, const char* msg) {
	if (rc != XN_STATUS_OK) {
		printf("%s failed: %s\n", msg, xnGetStatusString(rc));
		return true;
	}
	return false;
}

bool has_errors(XnStatus rc, xn::EnumerationErrors errors, const char* msg) {
	if (rc == XN_STATUS_NO_NODE_PRESENT) {
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		return true;
	}
	return false;
}

int main(int argc, char* argv[]) {
	XnStatus rc = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	// Initialize OpenNI
	rc = g_Context.InitFromXmlFile("poi-tracker.xml", g_ScriptNode, &errors);

	if (has_errors(rc, errors, "InitFromXmlFile")) {
		return -1;
	}

	if (has_failed(rc, "InitFromXmlFile")) {
		return -1;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	if (has_failed(rc, "Find depth generator")) {
		return -1;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_HANDS, g_HandsGenerator);
	if (has_failed(rc, "Find hands generator")) {
		return -1;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_GESTURE, g_GestureGenerator);
	if (has_failed(rc, "Find gesture generator")) {
		return -1;
	}

	return 0;
}
