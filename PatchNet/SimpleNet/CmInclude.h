#pragma once
#pragma warning(disable: 4996)

#include <assert.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <set>
#include <queue>
#include <list>
#include <limits>
#include <fstream>
#include <sstream>
#include <atlstr.h>
#include <atltypes.h>
#include <omp.h>
#include <strstream>
#include <Windows.h>
#include <ShellAPI.h>
using namespace std;


// OpenCV
#include <opencv.hpp>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "opencv2/core/version.hpp"

#define CV_VERSION_ID  CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif


#pragma comment(lib, cvLIB("core"))
#pragma comment(lib, cvLIB("features2d"))
#pragma comment(lib, cvLIB("highgui"))
#pragma comment(lib, cvLIB("imgproc"))
#pragma comment(lib, cvLIB("flann"))
#pragma comment(lib, cvLIB("nonfree"))
#pragma comment(lib, cvLIB("objdetect"))
using namespace cv;
// #include <cvaux.h>
// #include <highgui.h>
// #ifdef _DEBUG
// #pragma comment(lib, "cxcore210d.lib")
// #pragma comment(lib, "cv210d.lib")
// #pragma comment(lib, "highgui210d.lib")
// #else
// #pragma comment(lib, "cxcore210.lib")
// #pragma comment(lib, "cv210.lib")
// #pragma comment(lib, "highgui210.lib")
// #endif // _DEBUG
// using namespace cv;


#ifdef _USE_QT
#include "CmQt.h"
#endif // _USE_QT


// CmLib Basic coding help
#include "./Basic/CmDefinition.h"
#include "./Basic/CmLog.h"
#include "./Basic/CmSetting.h"
#include "./Basic/CmTimer.h"
#include "./Basic/CmShow.h"
#include "./Basic/CmCv.h"
#include "./Basic/CmFile.h"
#include "./Basic/CmComand.h"


//Segmentation algorithms
/*#include "./maxflow-v2.1.src/adjacency_list/graph.h"*/
//#include "./ImageSegmentation/segment-image.h"
//#include "./ImageSegmentation/CmGrabCut.h"


// Saliency detection algorithms
/*#include "./Saliency/CmSaliency.h"*/

//mean-shift segmentation
//#include "./MeanshiftSegmentation/allsegs.h"


extern CmSetting* gpSet;
#define gCmSet (*gpSet)
#define ToDo CmLog::LogLine("To be implemented, %d:%s\n", __LINE__, __FILE__)
extern bool dbgStop;
#define DBG_POINT if (dbgStop) CmLog::LogLine("%d:%s\n", __LINE__, __FILE__);


#define SHOW_IMG_TITLE(title,img,wait){\
	imshow(title, img); \
	if(wait == 1) waitKey(); \
}
#ifndef  SHOW_IMG
#define SHOW_IMG(img,wait){\
	imshow(#img, img); \
	if(wait==1) waitKey(); \
}
#endif
#ifndef SET_CONSOLE
#define SET_CONSOLE {\
	system("mode con: cols=130");\
	system("title WMLib");\
}
#endif

#define SEG_LINE {\
	SET_FONT_CYAN;\
	cout<<"--------------------------------------------------------------------------"<<endl;\
	SET_FONT_WHITE;\
}

