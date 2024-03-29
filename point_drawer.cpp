#include "point_drawer.h"

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

PointDrawer::PointDrawer(XnUInt32 nHistory, xn::DepthGenerator depthGenerator, xn::ImageGenerator imageGenerator, Recorder* recorder)
  : XnVPointControl("PointDrawer"),
  m_nHistorySize(nHistory),
  m_DepthGenerator(depthGenerator),
  m_ImageGenerator(imageGenerator),
  m_bDrawDM(false),
  m_bFrameID(false), 
  recorder(recorder) {
	m_pfPositionBuffer = new XnFloat[nHistory * 3];
}

PointDrawer::~PointDrawer() {
	std::map<XnUInt32, std::list<XnPoint3D> >::iterator iter;
	for (iter = m_History.begin(); iter != m_History.end(); ++iter) {
		iter->second.clear();
	}
	m_History.clear();

	delete [] m_pfPositionBuffer;
}

void PointDrawer::SetDepthMap(XnBool bDrawDM) {
	m_bDrawDM = bDrawDM;
}

void PointDrawer::SetFrameID(XnBool bFrameID) {
	m_bFrameID = bFrameID;
}

static XnBool bShouldPrint = false;
void PointDrawer::OnPointCreate(const XnVHandPointContext* cxt) {
	printf("** %d\n", cxt->nID);
	// Create entry for the hand
	m_History[cxt->nID].clear();
	bShouldPrint = true;
	OnPointUpdate(cxt);
	bShouldPrint = true;
}

void PointDrawer::OnPointUpdate(const XnVHandPointContext* cxt) {
	// positions are kept in projective coordinates, since they are only used for drawing
	XnPoint3D ptProjective(cxt->ptPosition);

	if (bShouldPrint)printf("Point (%f,%f,%f)", ptProjective.X, ptProjective.Y, ptProjective.Z);
	m_DepthGenerator.ConvertRealWorldToProjective(1, &ptProjective, &ptProjective);
	if (bShouldPrint)printf(" -> (%f,%f,%f)\n", ptProjective.X, ptProjective.Y, ptProjective.Z);

	// Add new position to the history buffer
	m_History[cxt->nID].push_front(ptProjective);
	// Keep size of history buffer
	if (m_History[cxt->nID].size() > m_nHistorySize)
		m_History[cxt->nID].pop_back();
	bShouldPrint = false;
}

void PointDrawer::OnPointDestroy(XnUInt32 nID) {
	// No need for the history buffer
	m_History.erase(nID);
}

#define MAX_DEPTH 10000
float g_pDepthHist[MAX_DEPTH];
unsigned int getClosestPowerOfTwo(unsigned int n) {
	unsigned int m = 2;
	while(m < n) m<<=1;

	return m;
}

GLuint initTexture(void** buf, int& width, int& height) {
	GLuint texID = 0;
	glGenTextures(1,&texID);

	width = getClosestPowerOfTwo(width);
	height = getClosestPowerOfTwo(height); 
	*buf = new unsigned char[width*height*4];
	glBindTexture(GL_TEXTURE_2D,texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texID;
}

GLfloat texcoords[8];
void DrawRectangle(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY) {
	GLfloat verts[8] = {	topLeftX, topLeftY,
		topLeftX, bottomRightY,
		bottomRightX, bottomRightY,
		bottomRightX, topLeftY
	};
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glFlush();
}

void DrawTexture(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY) {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	DrawRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

// --------------------------------
// Drawing
// --------------------------------

#define YUV422_U  0
#define YUV422_Y1 1
#define YUV422_V  2
#define YUV422_Y2 3
#define YUV422_BPP 4
#define YUV_RED   0
#define YUV_GREEN 1
#define YUV_BLUE  2
#define YUV_ALPHA  3
#define YUV_RGBA_BPP 4

#if (XN_PLATFORM == XN_PLATFORM_WIN32)

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBAImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize) {
	const XnUInt8* pYUVLast = pYUVImage + nYUVSize - 8;
	XnUInt8* pRGBLast = pRGBAImage + nRGBSize - 16;

	const __m128 minus16 = _mm_set_ps1(-16);
	const __m128 minus128 = _mm_set_ps1(-128);
	const __m128 plus113983 = _mm_set_ps1(1.13983F);
	const __m128 minus039466 = _mm_set_ps1(-0.39466F);
	const __m128 minus058060 = _mm_set_ps1(-0.58060F);
	const __m128 plus203211 = _mm_set_ps1(2.03211F);
	const __m128 zero = _mm_set_ps1(0);
	const __m128 plus255 = _mm_set_ps1(255);

	// define YUV floats
	__m128 y;
	__m128 u;
	__m128 v;

	__m128 temp;

	// define RGB floats
	__m128 r;
	__m128 g;
	__m128 b;

	// define RGB integers
	__m128i iR;
	__m128i iG;
	__m128i iB;

	XnUInt32* piR = (XnUInt32*)&iR;
	XnUInt32* piG = (XnUInt32*)&iG;
	XnUInt32* piB = (XnUInt32*)&iB;

	while (pYUVImage <= pYUVLast && pRGBAImage <= pRGBLast)
	{
		// process 4 pixels at once (values should be ordered backwards)
		y = _mm_set_ps(pYUVImage[YUV422_Y2 + YUV422_BPP], pYUVImage[YUV422_Y1 + YUV422_BPP], pYUVImage[YUV422_Y2], pYUVImage[YUV422_Y1]);
		u = _mm_set_ps(pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U],  pYUVImage[YUV422_U]);
		v = _mm_set_ps(pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V],  pYUVImage[YUV422_V]);

		u = _mm_add_ps(u, minus128); // u -= 128
		v = _mm_add_ps(v, minus128); // v -= 128

		/*

		http://en.wikipedia.org/wiki/YUV

		From YUV to RGB:
		R =     Y + 1.13983 V
		G =     Y - 0.39466 U - 0.58060 V
		B =     Y + 2.03211 U

		*/ 

		temp = _mm_mul_ps(plus113983, v);
		r = _mm_add_ps(y, temp);

		temp = _mm_mul_ps(minus039466, u);
		g = _mm_add_ps(y, temp);
		temp = _mm_mul_ps(minus058060, v);
		g = _mm_add_ps(g, temp);

		temp = _mm_mul_ps(plus203211, u);
		b = _mm_add_ps(y, temp);

		// make sure no value is smaller than 0
		r = _mm_max_ps(r, zero);
		g = _mm_max_ps(g, zero);
		b = _mm_max_ps(b, zero);

		// make sure no value is bigger than 255
		r = _mm_min_ps(r, plus255);
		g = _mm_min_ps(g, plus255);
		b = _mm_min_ps(b, plus255);

		// convert floats to int16 (there is no conversion to uint8, just to int8).
		iR = _mm_cvtps_epi32(r);
		iG = _mm_cvtps_epi32(g);
		iB = _mm_cvtps_epi32(b);

		// extract the 4 pixels RGB values.
		// because we made sure values are between 0 and 255, we can just take the lower byte
		// of each INT16
		pRGBAImage[0] = piR[0];
		pRGBAImage[1] = piG[0];
		pRGBAImage[2] = piB[0];
		pRGBAImage[3] = 255;

		pRGBAImage[4] = piR[1];
		pRGBAImage[5] = piG[1];
		pRGBAImage[6] = piB[1];
		pRGBAImage[7] = 255;

		pRGBAImage[8] = piR[2];
		pRGBAImage[9] = piG[2];
		pRGBAImage[10] = piB[2];
		pRGBAImage[11] = 255;

		pRGBAImage[12] = piR[3];
		pRGBAImage[13] = piG[3];
		pRGBAImage[14] = piB[3];
		pRGBAImage[15] = 255;

		// advance the streams
		pYUVImage += 8;
		pRGBAImage += 16;
	}
}

#else // not Win32

void YUV444ToRGBA(XnUInt8 cY, XnUInt8 cU, XnUInt8 cV,
					XnUInt8& cR, XnUInt8& cG, XnUInt8& cB, XnUInt8& cA) {
	XnInt32 nC = cY - 16;
	XnInt16 nD = cU - 128;
	XnInt16 nE = cV - 128;

	nC = nC * 298 + 128;

	cR = XN_MIN(XN_MAX((nC            + 409 * nE) >> 8, 0), 255);
	cG = XN_MIN(XN_MAX((nC - 100 * nD - 208 * nE) >> 8, 0), 255);
	cB = XN_MIN(XN_MAX((nC + 516 * nD           ) >> 8, 0), 255);
	cA = 255;
}

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize) {
	const XnUInt8* pCurrYUV = pYUVImage;
	XnUInt8* pCurrRGB = pRGBImage;
	const XnUInt8* pLastYUV = pYUVImage + nYUVSize - YUV422_BPP;
	XnUInt8* pLastRGB = pRGBImage + nRGBSize - YUV_RGBA_BPP;

	while (pCurrYUV <= pLastYUV && pCurrRGB <= pLastRGB)
	{
		YUV444ToRGBA(pCurrYUV[YUV422_Y1], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		YUV444ToRGBA(pCurrYUV[YUV422_Y2], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		pCurrYUV += YUV422_BPP;
	}
}

#endif

void DrawImage(const xn::ImageMetaData& im) {
	static bool bInitialized = false;	
	static GLuint imageTexID;
	static unsigned char* pImageTexBuf;
	static int texWidth, texHeight;

	float topLeftX;
	float topLeftY;
	float bottomRightY;
	float bottomRightX;
	float texXpos;
	float texYpos;

	if(!bInitialized) {
		XnUInt16 nXRes = im.XRes();
		XnUInt16 nYRes = im.YRes();
		printf("%d %d\n", nXRes, nYRes);
		texWidth =  getClosestPowerOfTwo(nXRes);
		texHeight = getClosestPowerOfTwo(nYRes);
		printf("%d %d\n", texWidth, texHeight);

		imageTexID = initTexture((void**)&pImageTexBuf, texWidth, texHeight) ;

		bInitialized = true;

		topLeftX = nXRes;
		topLeftY = 0;
		bottomRightY = nYRes;
		bottomRightX = 0;
		texXpos =(float)nXRes/texWidth;
		texYpos  =(float)nYRes/texHeight;

		memset(texcoords, 0, 8*sizeof(float));
		texcoords[0] = texXpos, texcoords[1] = texYpos, texcoords[2] = texXpos, texcoords[7] = texYpos;
	}
	
	unsigned char* pDestImage = pImageTexBuf;
	const XnUInt8* pImage = im.Data();
	XnPixelFormat pf = im.PixelFormat();
	
	for (XnUInt16 nY = im.YOffset(); nY < im.YRes() + im.YOffset(); nY++) {
		XnUInt8* pTexture = &pImageTexBuf[nY * texWidth * 3];

		if (pf == XN_PIXEL_FORMAT_YUV422) {
			YUV422ToRGB888(pImage, pTexture, im.XRes()*2, texWidth * 3);
			pImage += im.XRes()*2;
		}
		else {
			for (XnUInt16 nX = 0; nX < im.XRes(); nX++, pTexture+=3) {
				switch (pf) {
					case XN_PIXEL_FORMAT_RGB24:
						pTexture[0] = pImage[0];
						pTexture[1] = pImage[1];
						pTexture[2] = pImage[2];
						pImage+=3; 
						break;
					case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
						pTexture[0] = pTexture[1] = pTexture[2] = *pImage;
						pImage+=1; 
						break;
					case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
						XnUInt16* p16 = (XnUInt16*)pImage;
						pTexture[0] = pTexture[1] = pTexture[2] = (*p16) >> 2;
						pImage+=2; 
						break;
				}
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, imageTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageTexBuf);
	
	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glEnable(GL_TEXTURE_2D);
	DrawTexture(im.XRes(), im.YRes(), 0, 0);	
	glDisable(GL_TEXTURE_2D);
}

void DrawDepthMap(const xn::DepthMetaData& dm) {
	static bool bInitialized = false;	
	static GLuint depthTexID;
	static unsigned char* pDepthTexBuf;
	static int texWidth, texHeight;

	float topLeftX;
	float topLeftY;
	float bottomRightY;
	float bottomRightX;
	float texXpos;
	float texYpos;

	if(!bInitialized) {
		XnUInt16 nXRes = dm.XRes();
		XnUInt16 nYRes = dm.YRes();
		texWidth =  getClosestPowerOfTwo(nXRes);
		texHeight = getClosestPowerOfTwo(nYRes);

		depthTexID = initTexture((void**)&pDepthTexBuf,texWidth, texHeight) ;

		bInitialized = true;

		topLeftX = nXRes;
		topLeftY = 0;
		bottomRightY = nYRes;
		bottomRightX = 0;
		texXpos =(float)nXRes/texWidth;
		texYpos  =(float)nYRes/texHeight;

		memset(texcoords, 0, 8*sizeof(float));
		texcoords[0] = texXpos, texcoords[1] = texYpos, texcoords[2] = texXpos, texcoords[7] = texYpos;

	}

	unsigned int nValue = 0;
	unsigned int nHistValue = 0;
	unsigned int nIndex = 0;
	unsigned int nX = 0;
	unsigned int nY = 0;
	unsigned int nNumberOfPoints = 0;
	XnUInt16 g_nXRes = dm.XRes();
	XnUInt16 g_nYRes = dm.YRes();

	unsigned char* pDestImage = pDepthTexBuf;

	const XnUInt16* pDepth = dm.Data();

	// Calculate the accumulative histogram
	memset(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));
	for (nY=0; nY<g_nYRes; nY++) {
		for (nX=0; nX<g_nXRes; nX++) {
			nValue = *pDepth;

			if (nValue != 0) {
				g_pDepthHist[nValue]++;
				nNumberOfPoints++;
			}

			pDepth++;
		}
	}

	for (nIndex=1; nIndex<MAX_DEPTH; nIndex++) {
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}

	if (nNumberOfPoints) {
		for (nIndex=1; nIndex<MAX_DEPTH; nIndex++) {
			g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}

	pDepth = dm.Data(); 

	nIndex = 0;
	// Prepare the texture map
	for (nY=0; nY<g_nYRes; nY++) {
		for (nX=0; nX < g_nXRes; nX++, nIndex++) {
			nValue = *pDepth;

			if (nValue != 0) {
				nHistValue = g_pDepthHist[nValue];

				pDestImage[0] = nHistValue; 
				pDestImage[1] = nHistValue;
				pDestImage[2] = nHistValue;
			}
			else {
				pDestImage[0] = 0;
				pDestImage[1] = 0;
				pDestImage[2] = 0;
			}

			pDepth++;
			pDestImage+=3;
		}

		pDestImage += (texWidth - g_nXRes) *3;
	}

	glBindTexture(GL_TEXTURE_2D, depthTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pDepthTexBuf);

	// Display the OpenGL texture map
	glColor4f(0.5,0.5,0.5,1);

	glEnable(GL_TEXTURE_2D);
	DrawTexture(dm.XRes(),dm.YRes(),0,0);	
	glDisable(GL_TEXTURE_2D);
}

#ifdef USE_GLUT
void glPrintString(void *font, char *str) {
	size_t i,l = strlen(str);

	for(i=0; i<l; i++)
	{
		glutBitmapCharacter(font,*str++);
	}
}
void DrawFrameID(XnUInt32 nFrameID) {
	glColor4f(1,0,0,1);
	glRasterPos2i(20, 50);
	XnChar strLabel[20];
	sprintf(strLabel, "%d", nFrameID);
	glPrintString(GLUT_BITMAP_HELVETICA_18, strLabel);
}
#endif

// Colors for the points
XnFloat Colors[][3] = {
	{0.5,0.5,0.5},	// Grey
	{0,1,0},	// Green
	{0,0.5,1},	// Light blue
	{1,1,0},	// Yellow
	{1,0.5,0},	// Orange
	{1,0,1},	// Purple
	{1,1,1}		// White. reserved for the primary point
};
XnUInt32 nColors = 6;

XnBool PointDrawer::IsTouching(XnUInt32 id) {
	for (std::list<XnUInt32>::const_iterator iter = m_TouchingFOVEdge.begin(); iter != m_TouchingFOVEdge.end(); ++iter) {
		if (*iter == id)
			return TRUE;
	}
	return FALSE;
}

void PointDrawer::Draw() {
	std::map<XnUInt32, std::list<XnPoint3D> >::const_iterator PointIterator;

	// Go over each existing hand
	for (PointIterator = m_History.begin();
			PointIterator != m_History.end();
			++PointIterator) {
		// Clear buffer
		XnUInt32 nPoints = 0;
		XnUInt32 i = 0;
		XnUInt32 Id = PointIterator->first;

		// Go over all previous positions of current hand
		std::list<XnPoint3D>::const_iterator PositionIterator;
		for (PositionIterator = PointIterator->second.begin();
				PositionIterator != PointIterator->second.end();
				++PositionIterator, ++i) {
			// Add position to buffer
			XnPoint3D pt(*PositionIterator);
			m_pfPositionBuffer[3*i] = pt.X;
			m_pfPositionBuffer[3*i + 1] = pt.Y;
			m_pfPositionBuffer[3*i + 2] = 0;//pt.Z();
		}

		// Set color
		XnUInt32 nColor = Id % nColors;
		XnUInt32 nSingle = GetPrimaryID();
		if (Id == GetPrimaryID()) {
			nColor = 6;
		}

		// Draw buffer:
		glColor4f(Colors[nColor][0],
				Colors[nColor][1],
				Colors[nColor][2],
				1.0f);
		glPointSize(2);
		glVertexPointer(3, GL_FLOAT, 0, m_pfPositionBuffer);
		glDrawArrays(GL_LINE_STRIP, 0, i);


		if (IsTouching(Id)) {
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		glPointSize(8);
		glDrawArrays(GL_POINTS, 0, 1);
		glFlush();
	}
}
void PointDrawer::SetTouchingFOVEdge(XnUInt32 nID) {
	m_TouchingFOVEdge.push_front(nID);
}

// Handle a new Message
void PointDrawer::Update(XnVMessage* pMessage) {
	// PointControl's Update calls all callbacks for each hand
	XnVPointControl::Update(pMessage);
	
	if (!m_bDrawDM) {
		// Draw depth map
		xn::DepthMetaData depthMD;
		m_DepthGenerator.GetMetaData(depthMD);
		DrawDepthMap(depthMD);
	}
	else {
		// Draw image map
		xn::ImageMetaData imageMD;
		m_ImageGenerator.GetMetaData(imageMD);
		DrawImage(imageMD);
	}

#ifdef USE_GLUT
	if (m_bFrameID)
	{
		// Print out frame ID
		xn::DepthMetaData depthMD;
		m_DepthGenerator.GetMetaData(depthMD);
		DrawFrameID(depthMD.FrameID());
	}
#endif
	// Draw hands
	Draw();
	m_TouchingFOVEdge.clear();
}

/*void PrintSessionState(SessionState eState)
  {
  glColor4f(1,0,1,1);
  glRasterPos2i(20, 20);
  XnChar strLabel[200];

  switch (eState)
  {
  case IN_SESSION:
  sprintf(strLabel, "Tracking hands"); break;
  case NOT_IN_SESSION:
  sprintf(strLabel, "Perform click or wave gestures to track hand"); break;
  case QUICK_REFOCUS:
  sprintf(strLabel, "Raise your hand for it to be identified, or perform click or wave gestures"); break;
  }

  glPrintString(GLUT_BITMAP_HELVETICA_18, strLabel);
  }
 */
