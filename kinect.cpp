#include "kinect.h"

Kinect::Kinect() {
}

// Callback: New user was detected
void XN_CALLBACK_TYPE Kinect::userNew(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d New User %d\n", epochTime, nId);
	// New user found
	if (k->g_bNeedPose)
	{
		k->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(k->g_strPose, nId);
	}
	else
	{
		k->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE Kinect::userLost(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Lost user %d\n", epochTime, nId);	
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE Kinect::userPoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
	k->g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	k->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE Kinect::userCalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Calibration started for user %d\n", epochTime, nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE Kinect::userCalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
		k->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("%d Calibration failed for user %d\n", epochTime, nId);
		if (k->g_bNeedPose)
		{
			k->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(k->g_strPose, nId);
		}
		else
		{
			k->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

bool Kinect::initialize(char* xml_path) {
	XnStatus rc = XN_STATUS_OK;
	
	g_bNeedPose = FALSE;
	g_strPose[0] = '\0';
	g_bDrawBackground = TRUE;
	g_bDrawPixels = TRUE;
	g_bDrawSkeleton = TRUE;
	g_bPrintID = TRUE;
	g_bPrintState = TRUE;

	// Initialize OpenNI
	rc = g_Context.InitFromXmlFile(xml_path, g_ScriptNode, &errors);

	_recorder = new Recorder("recording.poi");

	if (has_errors(rc, errors, "InitFromXmlFile")) {
		printf("File to load was: %s\n", xml_path);
		return false;
	}

	if (has_failed(rc, "InitFromXmlFile")) {
		printf("File to load was: %s\n", xml_path);
		return false;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	if (has_failed(rc, "Find depth generator")) {
		return false;
	}

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_ImageGenerator);
	if (has_failed(rc, "Find image generator")) {
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
		g_HandsGenerator.GetHandTouchingFOVEdgeCap().RegisterToHandTouchingFOVEdge(
			&Kinect::touchingCallback, this, h);
	}

	XnCallbackHandle hGestureIntermediateStageCompleted, hGestureProgress, hGestureReadyForNextIntermediateStage;
	g_GestureGenerator.RegisterToGestureIntermediateStageCompleted(
		&Kinect::gestureIntermediateCallback, this, hGestureIntermediateStageCompleted);
	g_GestureGenerator.RegisterToGestureReadyForNextIntermediateStage(
		&Kinect::gestureReadyForNextIntermediateCallback, this, hGestureReadyForNextIntermediateStage);
	g_GestureGenerator.RegisterGestureCallbacks(NULL, 
		&Kinect::gestureProgressCallback, this, hGestureProgress);

	// NITE Initialization
	g_pSessionManager = new XnVSessionManager();
	rc = g_pSessionManager->Initialize(&g_Context, "Click,Wave", "RaiseHand");
	if (has_failed(rc, "SessionManager::Initialize")) {
		return false;
	}

	g_pSessionManager->RegisterSession(this, 
		&Kinect::sessionStarting, 
		&Kinect::sessionEnding, 
		&Kinect::focusProgress);

	g_pDrawer = new PointDrawer(20, g_DepthGenerator, g_ImageGenerator, _recorder);
	g_pFlowRouter = new XnVFlowRouter();
	g_pFlowRouter->SetActive(g_pDrawer);
	
	g_pSessionManager->AddListener(g_pFlowRouter);

	g_pDrawer->RegisterNoPoints(NULL, &noHands);
	g_pDrawer->SetDepthMap(true);

	// Skeleton Tracking
	rc = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (has_failed(rc, "Find user generator")) {
		return false;
	}
	rc = g_UserGenerator.Create(g_Context);
	if (has_failed(rc, "Find user generator")) {
		return false;
	}

	XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	
	rc = g_UserGenerator.RegisterUserCallbacks(&Kinect::userNew, &Kinect::userLost, this, hUserCallbacks);
	if (has_failed(rc, "Register to user callbacks")) {
		return false;
	}

	rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(&Kinect::userCalibrationStart, this, hCalibrationStart);
	if (has_failed(rc, "Register to calibration start")) {
		return false;
	}
	
	rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(&Kinect::userCalibrationComplete, this, hCalibrationComplete);
	if (has_failed(rc, "Register to calibration complete")) {
		return false;
	}

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration()) {
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
			printf("Pose required, but not supported\n");
			return false;
		}

		rc = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(&Kinect::userPoseDetected, this, hPoseDetected);
		if (has_failed(rc, "Register to pose detected")) {
			return false;
		}
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	g_userDrawer = new UserDrawer(g_DepthGenerator, g_UserGenerator, _recorder);

	rc = g_Context.StartGeneratingAll();
	if (has_failed(rc, "StartGenerating")) {
		return false;
	}

	XnMapOutputMode mode;
	g_DepthGenerator.GetMapOutputMode(mode);

	_width = mode.nXRes;
	_height = mode.nYRes;

	return true;
}

void Kinect::cleanup() {
	g_ScriptNode.Release();
	g_DepthGenerator.Release();
	g_HandsGenerator.Release();
	g_GestureGenerator.Release();
	g_Context.Release();
}

// Errors

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

// Callbacks

void __stdcall Kinect::touchingCallback(xn::HandTouchingFOVEdgeCapability &generator, XnUserID id, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	//k->g_pDrawer->SetTouchingFOVEdge(id);
}

void __stdcall Kinect::gestureIntermediateCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: Intermediate stage complete (%f, %f, %f)\n",
		strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}

void __stdcall Kinect::gestureProgressCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: progress: %f (%f, %f, %f)\n",
		strGesture, fProgress, pPosition->X, pPosition->Y, pPosition->Z);
}

void __stdcall Kinect::gestureReadyForNextIntermediateCallback(xn::GestureGenerator &generator, const XnChar* strGesture, const XnPoint3D* pPosition, void* pCookie) {
	Kinect* k = (Kinect*)pCookie;
	printf("Gesture %s: Ready for next intermediate stage (%f, %f, %f)\n",
		strGesture, pPosition->X, pPosition->Y, pPosition->Z);
}

void __stdcall Kinect::focusProgress(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress, void* UserCxt) {
	Kinect* k = (Kinect*)UserCxt;
	printf("NITE %s: Focus Progress (%f, %f, %f: %f)\n",
		strFocus, ptPosition.X, ptPosition.Y, ptPosition.Z, fProgress);
}

void __stdcall Kinect::sessionStarting(const XnPoint3D& ptPosition, void* UserCxt) {
	Kinect* k = (Kinect*)UserCxt;
	printf("NITE : Session Starting (%f, %f, %f)\n",
		ptPosition.X, ptPosition.Y, ptPosition.Z);
}

void __stdcall Kinect::sessionEnding(void* UserCxt) {
	Kinect* k = (Kinect*)UserCxt;
	printf("NITE : Session Ending\n");
}

void __stdcall Kinect::noHands(void* UserCxt) {
	Kinect* k = (Kinect*)UserCxt;
	printf("NITE : No Hands\n");
}

unsigned int Kinect::width() {
	return this->_width;
}

unsigned int Kinect::height() {
	return this->_height;
}

PointDrawer* Kinect::pointDrawer() {
	return g_pDrawer;
}

UserDrawer* Kinect::userDrawer() {
	return g_userDrawer;
}

Recorder* Kinect::recorder() {
	return _recorder;
}

void Kinect::update() {
	// Read next available data
	g_Context.WaitOneUpdateAll(g_DepthGenerator);

	// Update NITE tree
	g_pSessionManager->Update(&g_Context);

//	PrintSessionState(g_SessionState);
}
