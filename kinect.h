#// OpenNI
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

	// TODO: Kill these globals with fire
	xn::Context g_Context;
	xn::ScriptNode g_ScriptNode;
	xn::DepthGenerator g_DepthGenerator;
	xn::HandsGenerator g_HandsGenerator;
	xn::GestureGenerator g_GestureGenerator;

	bool has_failed(XnStatus rc, const char* msg);
	bool has_errors(XnStatus rc, xn::EnumerationErrors errors, const char* msg);
};
