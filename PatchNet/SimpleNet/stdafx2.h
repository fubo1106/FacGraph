// stdafx2.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define OUTDIR
//#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>



// TODO: reference additional headers your program requires here
#include "CmInclude.h"

// #ifdef OUTDIR
// const string outdir = "D:\\Patch\\data\\Pick\\";
// #else
// extern string outdir;
// #endif

// #ifdef _DEBUG
// #pragma comment(lib, "opencv_core231d.lib")
// #pragma comment(lib, "opencv_highgui231d.lib")
// #pragma comment(lib, "opencv_imgproc231d.lib")
// #pragma comment(lib, "opencv_video231d.lib")
// #pragma comment(lib, "opencv_features2d231d.lib")
// #else
// #pragma comment(lib, "opencv_core231.lib")
// #pragma comment(lib, "opencv_highgui231.lib")
// #pragma comment(lib, "opencv_imgproc231.lib")
// #pragma comment(lib, "opencv_video231.lib")
// #pragma comment(lib, "opencv_features2d231.lib")
// #endif // _DEBUG
// #pragma   comment(lib, "ws2_32.lib ") 

#define  CM_INITIAL(logName) CV_Assert(argc == 3); \
	CmInitial(argv[1], CmSprintf("%s.log", logName), false, false, argv[2]);

extern const char *section, *srcDir, *outdir, *new_outdir, *regionSrcDir, *regionOutDir;