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
using namespace cv;

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