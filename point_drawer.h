#ifndef POINT_DRAWER_H
#define POINT_DRAWER_H

#include <map>
#include <list>

#include <XnCppWrapper.h>
#include <XnVPointControl.h>

#include "recorder.h"

class PointDrawer : public XnVPointControl {
public:

	PointDrawer(XnUInt32 nHistorySize, xn::DepthGenerator depthGenerator, xn::ImageGenerator imageGenerator, Recorder* recorder);
	virtual ~PointDrawer();

	void Update(XnVMessage* pMessage);
	void OnPointCreate(const XnVHandPointContext* cxt);
	void OnPointUpdate(const XnVHandPointContext* cxt);
	void OnPointDestroy(XnUInt32 nID);
	void Draw();
	void SetDepthMap(XnBool bDrawDM);
	void SetFrameID(XnBool bFrameID);
	void SetTouchingFOVEdge(XnUInt32 nID);

protected:

	XnBool IsTouching(XnUInt32 nID);

	// Number of previous Hand Positions
	XnUInt32 m_nHistorySize;

	// Previous Hand Positions
	std::map<XnUInt32, std::list<XnPoint3D> > m_History;
	std::list<XnUInt32> m_TouchingFOVEdge;

	// Source of the depth map
	xn::DepthGenerator m_DepthGenerator;
	xn::ImageGenerator m_ImageGenerator;
	XnFloat* m_pfPositionBuffer;

	Recorder* recorder;

	XnBool m_bDrawDM;
	XnBool m_bFrameID;
};

#endif
