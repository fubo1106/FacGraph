#include "../stdafx2.h"
#include "CmCv.h"


/************************************************************************/
/* AbsAngle: Calculate magnitude and angle of vectors.					*/
/************************************************************************/
void CmCv::AbsAngle(const Mat& cmplx32FC2, Mat& mag32FC1, Mat& ang32FC1)
{
	CV_Assert(cmplx32FC2.type() == CV_32FC2);
	mag32FC1.create(cmplx32FC2.size(), CV_32FC1);
	ang32FC1.create(cmplx32FC2.size(), CV_32FC1);

	for (int y = 0; y < cmplx32FC2.rows; y++)
	{
		const float* cmpD = cmplx32FC2.ptr<float>(y);
		float* dataA = ang32FC1.ptr<float>(y);
		float* dataM = mag32FC1.ptr<float>(y);
		for (int x = 0; x < cmplx32FC2.cols; x++, cmpD += 2)
		{
			dataA[x] = atan2(cmpD[1], cmpD[0]);
			dataM[x] = sqrt(cmpD[0] * cmpD[0] + cmpD[1] * cmpD[1]);
		}
	}
}

/************************************************************************/
/* GetCmplx: Get a complex value image from it's magnitude and angle    */
/************************************************************************/
void CmCv::GetCmplx(const Mat& mag32F, const Mat& ang32F, Mat& cmplx32FC2)
{
	CV_Assert(mag32F.type() == CV_32FC1 && ang32F.type() == CV_32FC1 && mag32F.size() == ang32F.size());
	cmplx32FC2.create(mag32F.size(), CV_32FC2);
	for (int y = 0; y < mag32F.rows; y++)
	{
		float* cmpD = cmplx32FC2.ptr<float>(y);
		const float* dataA = ang32F.ptr<float>(y);
		const float* dataM = mag32F.ptr<float>(y);
		for (int x = 0; x < mag32F.cols; x++, cmpD += 2)
		{
			cmpD[0] = dataM[x] * cos(dataA[x]);
			cmpD[1] = dataM[x] * sin(dataA[x]);
		}
	}
}

// Mat2GrayLog: Convert and arbitrary mat to [0, 1] for display.
// The result image is in 32FCn format and range [0, 1.0].
// Mat2GrayLinear(log(img+1), newImg). In place operation is supported.
void CmCv::Mat2GrayLog(const Mat& img, Mat& newImg)
{
	img.convertTo(newImg, CV_32F);
	newImg += 1;
	cv::log(newImg, newImg);
	cv::normalize(newImg, newImg, 0, 1, NORM_MINMAX);
}

// Low frequency part is always been move to the central part:
//				 -------                          -------	
//				| 1 | 2 |                        | 3 | 4 |	
//				 -------            -->           -------	
//				| 4 | 3 |                        | 2 | 1 |	
//				 -------                          -------	
void CmCv::FFTShift(Mat& img)
{
	int w = img.cols / 2, h = img.rows / 2;
	int cx2 = img.cols - w, cy2 = img.rows - h;
	Swap(img(Rect(0, 0, w, h)), img(Rect(cx2, cy2, w, h)));  // swap 1, 3
	Swap(img(Rect(cx2, 0, w, h)), img(Rect(0, cy2, w, h)));  // swap 2, 4
}

/************************************************************************/
/* Swap the content of two Mat with same type and size                  */
/************************************************************************/
void CmCv::Swap(Mat& a, Mat& b)
{
	CV_Assert(a.type() == b.type() && a.size() == b.size());
	Mat t;
	a.copyTo(t);
	b.copyTo(a);
	t.copyTo(b);
}

//Get BITMAPINFO structure
char* bmiBuffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
BITMAPINFO* CmCv::GetBitmapInfo(const Mat& pImg)
{
	CV_Assert(pImg.type() == CV_8UC1 || pImg.type() == CV_8UC3);
	return GetBitmapInfo(pImg.cols, pImg.rows, pImg.channels() * 8, 1);
}

BITMAPINFO* CmCv::GetBitmapInfo(int width, int height, int bpp, int origin)
{
	BITMAPINFO* bmi = (BITMAPINFO*)bmiBuffer;
	assert(width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset( bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	static bool notFilled = true;
	if(bpp == 8 && notFilled)
	{
		RGBQUAD* palette = bmi->bmiColors;
		for(int i = 0; i < 256; i++ )
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
		notFilled = false;
	}
	return bmi;
}

Rect CmCv::GetMaskRegion(const Mat &mask1u, int ext)
{
	int maxX = INT_MIN, maxY = INT_MIN, minX = INT_MAX, minY = INT_MAX, rows = mask1u.rows, cols = mask1u.cols;
	for (int r = 0; r < rows; r++)
	{
		const byte* data = mask1u.ptr<byte>(r);
		for (int c = 0; c < cols; c++)
			if (data[c] > 10)
			{
				maxX = max(maxX, c);
				minX = min(minX, c);
				maxY = max(maxY, r);
				minY = min(minY, r);
			}
	}

	maxX = maxX + ext + 1 < cols ? maxX + ext + 1 : cols;
	maxY = maxY + ext + 1 < rows ? maxY + ext + 1 : rows;
	minX = minX - ext > 0 ? minX - ext : 0;
	minY = minY - ext > 0 ? minY - ext : 0;

	return Rect(minX, minY, maxX - minX, maxY - minY);
}





void CmCv::Demo(const char* fileName/* = "H:\\Resize\\cd3.avi"*/)
{

}

/************************************************************************/
/* CmmPadImage: Pad image usually used for reducing the boundary effect.*/
/*   nRow(nCol): How many rows(columns) will be padded in both sides.	*/
/************************************************************************/
/*IplImage* CmCv::PadImage(const IplImage*src, IplImage*& buffer, int nRow, int nCol, int type)
{
	// Manage memory 
	CvSize newSize = cvSize(src->width + nCol * 2, src->height + nRow * 2);
	if (buffer == NULL)
		buffer = cvCreateImage(newSize, src->depth, src->nChannels);
	else
	{
#ifdef _DEBUG
		IplImage* tmp = cvCreateImageHeader(newSize, src->depth, src->nChannels);
		CmAssertImgFormatSame(tmp, buffer);
		cvReleaseImageHeader(&tmp);
#endif // _DEBUG
	}
	assert(src->width >= nCol && src->height >= nRow);


	// copy original data
	CvMat padded;
	cvGetSubRect(buffer, &padded, cvRect(nCol, nRow, src->width, src->height));

	// Padding images
	if (type == PAD_IMG_SYMMETRIC)
	{
		// Copy center part
		cvCopy(src, &padded);

		CvMat sub; 
		// Set left part of image
		cvGetSubRect(buffer, &sub, cvRect(nCol, nRow, nCol, src->height));
		cvGetSubRect(buffer, &padded, cvRect(0, nRow, nCol, src->height));
		cvCopy(&sub, &padded);
		cvFlip(&padded, NULL, 1);

		// Set right part of image
		cvGetSubRect(buffer, &sub, cvRect(src->width, nRow, nCol, src->height));
		cvGetSubRect(buffer, &padded, cvRect(nCol + src->width, nRow, nCol, src->height));
		cvCopy(&sub, &padded);
		cvFlip(&padded, NULL, 1);

		// Set up part of image
		cvGetSubRect(buffer, &sub, cvRect(0, nRow, buffer->width, nRow));
		cvGetSubRect(buffer, &padded, cvRect(0, 0, buffer->width, nRow));
		cvCopy(&sub, &padded);
		cvFlip(&padded, NULL, 0);

		// Set bottom part of image
		cvGetSubRect(buffer, &sub, cvRect(0, src->height, buffer->width, nRow));
		cvGetSubRect(buffer, &padded, cvRect(0, nRow + src->height, buffer->width, nRow));
		cvCopy(&sub, &padded);
		cvFlip(&padded, NULL, 0);
	}
	else // PAD_IMG_ZERO
	{
		cvZero(buffer);
		cvCopy(src, &padded);
	}

	return buffer;
}*/




///************************************************************************/
///* Generate a meshgrid. Result meshgrid should be released outside.     */
///* Input:																*/
///*   startX, startY, endX, endY: interval of meshgrid value.			*/
///* Output:																*/
///*   fx32FC1, fy32FC1: a matrix in IplImage 32FC1 format.				*/
///* For example, startX = -2, endX = 2, startY = -1, endY = 2:			*/
///*   fx32FC1 = -2 -1  0  1												*/
///*			   -2 -1  0  1												*/
///*			   -2 -1  0  1												*/
///*   fy32FC1 = -1 -1 -1 -1												*/
///*				0  0  0  0												*/
///*				1  1  1  1												*/
///************************************************************************/
//void CmCv::MeshGrid(IplImage*& fx32FC1, IplImage*& fy32FC1, int startX, int endX, int startY, int endY)
//{
//	int width = endX - startX;
//	int height = endY - startY;
//	fx32FC1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
//	fy32FC1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
//
//	assert((fx32FC1) != NULL);
//	assert((fy32FC1) != NULL);
//
//	for (int y = startY, i = 0; y < endY; y++, i++)
//	{
//		float* dataX = (float*)(fx32FC1->imageData + fx32FC1->widthStep * i);
//		float* dataY = (float*)(fy32FC1->imageData + fy32FC1->widthStep * i);
//		for (int x = startX, j = 0; x < endX; x++, j++)
//		{
//			dataX[j] = (float)x;
//			dataY[j] = (float)y;
//		}
//	}
//}
//
//void CmCv::MeshGrid(IplImage *&fx64FC1, IplImage *&fy64FC1, double startX, double endX, double startY, double endY, double stepX, double stepY)
//{
//	int width = (int)((endX - startX)/stepX) + 1;
//	int heigh = (int)((endY - startY)/stepY) + 1;
//
//	fx64FC1 = cvCreateImage(cvSize(width, heigh), IPL_DEPTH_64F, 1);
//	fy64FC1 = cvCreateImage(cvSize(width, heigh), IPL_DEPTH_64F, 1);
//
//	assert(fx64FC1 != NULL);
//	assert(fy64FC1 != NULL);
//	for (int r = 0; r < heigh; r++)
//	{
//		double y = startY + r * stepY;
//		for (int c = 0; c < width; c++)
//		{
//			double x = startX + c * stepX;
//			CV_IMAGE_ELEM(fx64FC1, double, r, c) = x;
//			CV_IMAGE_ELEM(fy64FC1, double, r, c) = y;
//		}
//	}
//}
//

//
//void CmCv::AddAlpha(CvMat *img, CvMat *alpha)
//{
//#pragma omp parallel for
//	for (int r = 0; r < img->height; r++)
//	{
//		byte* imgD = img->data.ptr + img->step * r;
//		byte* alpD = alpha->data.ptr + alpha->step * r;
//		for (int c = 0; c < img->width; c++, imgD += 3)
//		{
//			double a = alpD[c] / 255.0;
//			imgD[0] = (byte)(imgD[0] * a);
//			imgD[1] = (byte)(imgD[1] * a);
//			imgD[2] = (byte)(imgD[2] * a);
//		}
//	}
//}
//
//void CmCv::AddAlpha(CvMat *img, CvMat *alpha, CvScalar bgColor)
//{
//#pragma omp parallel for
//	for (int r = 0; r < img->height; r++)
//	{
//		byte* imgD = img->data.ptr + img->step * r;
//		byte* alpD = alpha->data.ptr + alpha->step * r;
//		for (int c = 0; c < img->width; c++, imgD += 3)
//		{
//			double a = alpD[c] / 255.0;
//			imgD[0] = (byte)(imgD[0] * a + (1-a) * bgColor.val[0]);
//			imgD[1] = (byte)(imgD[1] * a + (1-a) * bgColor.val[1]);
//			imgD[2] = (byte)(imgD[2] * a + (1-a) * bgColor.val[2]);
//		}
//	}
//}
//
//CvMat* CmCv::RotateScale(IN CvMat *src, IN double angle, double factor )
//{
//	float m[6], mm[4];
//	// Matrix m looks like:
//	//
//	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
//	// [ m3  m4  m5 ]       [ A21  A22   b2 ]
//	//
//	CvMat M = cvMat (2, 3, CV_32F, m);
//	int w = src->width;
//	int h = src->height;
//	m[0] = (float) (1/factor * cos (angle * CV_PI / 180.));
//	m[1] = (float) (1/factor * sin (angle * CV_PI / 180.));
//	m[3] = -m[1];
//	m[4] = m[0];
//
//	mm[0] = (float)(m[0] * factor * factor);
//	mm[1] = (float)(m[1] * factor * factor);
//	mm[2] = -mm[1];
//	mm[3] = mm[0];
//	// 将旋转中心移至图像中间
//	m[2] = (float)(w*0.5);
//	m[5] = (float)(h*0.5);
//	float x[10];
//	float y[10];
//	x[0] = - m[2];
//	y[0] = - m[5];
//	x[1] = w-1-m[2];
//	y[1] = - m[5];
//	x[2] = w-1-m[2];
//	y[2] = h-1-m[5];
//	x[3] = - m[2];
//	y[3] = h-1-m[5];
//	x[4] = 100-m[2];
//	y[4] = 100-m[5];
//	for (int i=0;i<5;i++)
//	{
//		x[i+5] = mm[0]*x[i]-mm[1]*y[i];
//		y[i+5] = -mm[2]*x[i]+mm[3]*y[i];
//	}
//	float minx =min(min(x[8],x[5]),min(x[6],x[7]));
//	float miny =min(min(y[8],y[5]),min(y[6],y[7]));
//	float maxx =max(max(x[8],x[5]),max(x[6],x[7]));
//	float maxy =max(max(y[8],y[5]),max(y[6],y[7]));
//	int neww = round(maxx-minx+1);
//	int newh = round(maxy-miny+1);
//
//	CvMat *dst = cvCreateMat(newh, neww, src->type);
//	//  dst(x,y) = A * src(x,y) + b
//	cvZero (dst);
//	cvGetQuadrangleSubPix (src, dst, &M);
//	return dst;
//}
//
//void CmCv::RotateScale(IN CvMat *src, OUT CvMat *dst, double angle, double factor)
//{
//	// Matrix m looks like:
//	//
//	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
//	// [ m3  m4  m5 ]       [ A21  A22   b2 ]
//	//
//	float m[6];
//	CvMat M = cvMat (2, 3, CV_32F, m);
//	int w = src->width;
//	int h = src->height;
//	m[0] = (float) (1/factor * cos (angle * CV_PI / 180.));
//	m[1] = (float) (1/factor * sin (angle * CV_PI / 180.));
//	m[3] = -m[1];
//	m[4] = m[0];
//
//	// 将旋转中心移至图像中间
//	m[2] = (float)(w*0.5);
//	m[5] = (float)(h*0.5);
//
//	//  dst(x,y) = A * src(x,y) + b
//	cvZero (dst);
//	cvGetQuadrangleSubPix (src, dst, &M);
//}
//
