#include "user_drawer.h"

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

UserDrawer::UserDrawer(xn::DepthGenerator depthGenerator, xn::UserGenerator userGenerator, Recorder* recorder) :
  depthGenerator(depthGenerator),
  userGenerator(userGenerator),
  recorder(recorder) {
}

void UserDrawer::draw() {

	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	userGenerator.GetUsers(aUsers, nUsers);
	XnUserID user = aUsers[0];

	if (userGenerator.GetSkeletonCap().IsTracking(user))
	{
		// Record
		XnPoint3D pts[15];

		XnSkeletonJointPosition joint;

		static const XnSkeletonJoint joints[15] = {
			XN_SKEL_HEAD, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER, XN_SKEL_RIGHT_SHOULDER,
			XN_SKEL_LEFT_ELBOW, XN_SKEL_RIGHT_ELBOW, XN_SKEL_LEFT_HAND, XN_SKEL_RIGHT_HAND,
			XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP, XN_SKEL_LEFT_KNEE, XN_SKEL_RIGHT_KNEE,
			XN_SKEL_LEFT_FOOT, XN_SKEL_RIGHT_FOOT, XN_SKEL_TORSO};

		for(int i = 0; i < 15; i++) {
			userGenerator.GetSkeletonCap().GetSkeletonJointPosition(user, joints[i], joint);
			pts[i] = joint.position;
		}
		depthGenerator.ConvertRealWorldToProjective(15, pts, pts);

		recorder->record(pts);

		glBegin(GL_LINES);

		glColor4f(1, 1, 1, 1);

		drawLimb(user, XN_SKEL_HEAD, XN_SKEL_NECK);

		drawLimb(user, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
		drawLimb(user, XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
		drawLimb(user, XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);

		drawLimb(user, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
		drawLimb(user, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
		drawLimb(user, XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

		drawLimb(user, XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
		drawLimb(user, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);
	
		drawLimb(user, XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
		drawLimb(user, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
		drawLimb(user, XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

		drawLimb(user, XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
		drawLimb(user, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
		drawLimb(user, XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

		drawLimb(user, XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);

		glEnd();
	}
}

void UserDrawer::drawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2) {
	if (!userGenerator.GetSkeletonCap().IsTracking(player)) {
		printf("not tracked!\n");
		return;
	}

	XnSkeletonJointPosition joint1, joint2;
	userGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	userGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5) {
		return;
	}

	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;

	depthGenerator.ConvertRealWorldToProjective(2, pt, pt);

	glVertex3i(pt[0].X, pt[0].Y, 0);
	glVertex3i(pt[1].X, pt[1].Y, 0);
}
