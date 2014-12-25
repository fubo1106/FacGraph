#pragma once

#ifndef PAD_IMG_SYMMETRIC
#define PAD_IMG_SYMMETRIC 0   
#define PAD_IMG_ZERO      1
#endif


#define CV_Assert_(expr, args) \
{\
	if(!(expr)) {\
	string msg = cv::format args; \
	CmLog::LogError("%s in %s:%d\n", msg.c_str(), __FILE__, __LINE__); \
	cv::error(cv::Exception(CV_StsAssert, msg, __FUNCTION__, __FILE__, __LINE__) ); }\
}

class CmCv
{
public:
	// AbsAngle: Calculate magnitude and angle of vectors.
	static void AbsAngle(const Mat& cmplx32FC2, Mat& mag32FC1, Mat& ang32FC1);

	// GetCmplx: Get a complex value image from it's magnitude and angle.
	static void GetCmplx(const Mat& mag32F, const Mat& ang32F, Mat& cmplx32FC2);

	// Mat2GrayLog: Convert and arbitrary mat to [0, 1] for display.
	// The result image is in 32FCn format and range [0, 1.0].
	// Mat2GrayLinear(log(img+1), newImg). In place operation is supported.
	static void Mat2GrayLog(const Mat& img, Mat& newImg);

	// Low frequency part is always been move to the central part:
	//				 -------                          -------	
	//				| 1 | 2 |                        | 3 | 4 |	
	//				 -------            -->           -------	
	//				| 4 | 3 |                        | 2 | 1 |	
	//				 -------                          -------	
	static void FFTShift(Mat& img);

	// Swap the content of two Mat with same type and size
	static inline void Swap(Mat& a, Mat& b);

	// Normalize size/image to min(width, height) = shortLen and use width 
	// and height to be multiples of unitLen while keeping its aspect ratio 
	// as much as possible. unitLen must not be 0.
	static inline Size NormalizeSize(const Size& sz, int shortLen, int unitLen = 1);
	static inline void NormalizeImg(const Mat&img, Mat& dstImg, int shortLen = 256, int unitLen = 8);

	// Get BITMAPINFO for drawing
	static BITMAPINFO* GetBitmapInfo(const Mat& pImg);
	static BITMAPINFO* GetBitmapInfo(int width, int height, int bpp, int origin); 

	// Get image region by two corner point.
	static inline Rect GetImgRegion(Point p1, Point p2, Size imgSz);

	// Check an image (with size imgSz) point and correct it if necessary
	static inline void CheckPoint(Point &p, Size imgSz);

	static inline void Merge(const Mat &m0, const Mat &m1, const Mat &m2, Mat &m);


	// Get mask region. 
	static Rect GetMaskRegion(const Mat &mask1u, int ext = 0);

public:
	static void Demo(const char* fileName = "H:\\Resize\\cd3.avi");

	/************************************************************************/
	/* Generate a meshgrid. Result meshgrid should be released outside      */
	/* Input:																*/
	/*   startX, startY, endX, endY: interval of meshgrid value.			*/
	/* Output:																*/
	/*   fx32FC1, fy32FC1: a matrix in IplImage 32FC1 format.				*/
	/* For example, startX = -2, endX = 2, startY = -1, endY = 2:			*/
	/*   fx32FC1 = -2 -1  0  1												*/
	/*			   -2 -1  0  1												*/
	/*			   -2 -1  0  1												*/
	/*   fy32FC1 = -1 -1 -1 -1												*/
	/*				0  0  0  0												*/
	/*				1  1  1  1												*/
	/************************************************************************/ 
	//static void MeshGrid(IplImage*& fx32FC1, IplImage*& fy32FC1, int startX, int endX, int startY, int endY);
	//static void MeshGrid(IplImage*& fx64FC1, IplImage*& fy64FC1, 
	//	double startX, double endX, double startY, double endY, double stepX, double stepY);

	//// Load mask image and threshold thus noisy by compression can be removed
	//static IplImage* LoadMask(const char* fileName);
	
	//// Adding alpha value to img to show. img: 8U3C, alpha 8U1C
	//static void AddAlpha(CvMat *img, CvMat *alpha);
	//static void AddAlpha(CvMat *img, CvMat *alpha, CvScalar bgColor);

	//// Rotate and scale an image
	//static CvMat* RotateScale(IN CvMat *src, IN double angle = 1.0, double scale = 1.0);
	//static void RotateScale(IN CvMat *src, OUT CvMat *dst, double angle = 1.0, double factor = 1.0);

	// PadImage: Pad image usually used for reducing the boundary effect
	// nRow(nCol): How many rows(columns) will be padded in both sides.
	//static IplImage* PadImage(const IplImage*src, IplImage*& buffer, int nRow, int nCol, int type = PAD_IMG_SYMMETRIC);
};

// Normalize size/image to min(width, height) = shortLen and use width 
// and height to be multiples of unitLen while keeping its aspect ratio 
// as much as possible. unitLen must not be 0.

Size CmCv::NormalizeSize(const Size& sz, int shortLen, int unitLen)
{
	double ratio = double(shortLen) / min(sz.width, sz.height);
	return Size(cvRound(sz.width * ratio / unitLen) * unitLen, cvRound(sz.height * ratio /unitLen) * unitLen);
}

void CmCv::NormalizeImg(const Mat&img, Mat& dstImg, int shortLen, int unitLen)
{
	resize(img, dstImg, NormalizeSize(img.size(), shortLen, unitLen));
}

void CmCv::CheckPoint(Point &p, Size imgSz)
{
	p.x = max(0, p.x), p.y = max(0, p.y);
	p.x = min(imgSz.width - 1, p.x);
	p.y = min(imgSz.height - 1, p.y);
}

Rect CmCv::GetImgRegion(Point p1, Point p2, Size imgSz)
{
	CheckPoint(p1, imgSz);
	CheckPoint(p2, imgSz); 
	return Rect(min(p1.x, p2.x), min(p1.y, p2.y), abs(p1.x - p2.x), abs(p1.y - p2.y));
}

void CmCv::Merge(const Mat &m0, const Mat &m1, const Mat &m2, Mat &m)
{
	const Mat ms[3] = {m0, m1, m2};
	Mat rMat;
	cv::merge(ms, 3, rMat);
	m = rMat;
}