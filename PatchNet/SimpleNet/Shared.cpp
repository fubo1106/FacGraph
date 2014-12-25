#include "stdafx2.h"
#include "Shared.h"
#include <math.h>

void Shared::getGradMap( cv::Mat& inImg8U3C, cv::Mat& outMap8U, cv::Mat& outAtan32F )
{
	
		Mat& src = inImg8U3C;
		Mat src_gray;
		int scale = 1;
		int delta = 0;
		int ddepth = CV_16S;

		int c;

		GaussianBlur( src, src, Size(5,5), 0, 0, BORDER_DEFAULT );

		/// 转换为灰度图
		cvtColor( src, src_gray, CV_BGR2GRAY);

		/// 创建 grad_x 和 grad_y 矩阵
		Mat grad_x, grad_y;
		Mat abs_grad_x, abs_grad_y;

		/// 求 X方向梯度
		//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
		Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
		convertScaleAbs( grad_x, abs_grad_x );

		/// 求Y方向梯度
		//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
		Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
		convertScaleAbs( grad_y, abs_grad_y );

		/// 合并梯度(近似)
		addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, outMap8U);

		outAtan32F.create(outMap8U.size(), CV_32FC1);
		for (int i = 0; i < outAtan32F.rows; i++)
		{
			float* p = outAtan32F.ptr<float>(i);
			short* pgx = grad_x.ptr<short>(i);
			short* pgy = grad_y.ptr<short>(i);
			for (int j = 0; j < outAtan32F.cols; j++)
			{
				if(abs(pgx[j]) < 0.0001)
					p[j] = 3.14 / 2;
				else
					p[j] = atan(float(pgy[j]) / float(pgx[j]));
			}
		}
		cv::GaussianBlur(outMap8U, outMap8U, cv::Size(3,3),1);
		cv::GaussianBlur(outAtan32F, outAtan32F, cv::Size(3,3),1);
}



void Shared::drawPatch( sPatch& sp, cv::Mat& img, string& name )
{
	cv::Rect rec;
	int radius = sp.width / 2;
	rec.x = sp.center.x -radius;
	rec.y = sp.center.y -radius;
	rec.width = sp.width;
	rec.height = sp.width;
	cv::imwrite(name, img(rec));
}

void Shared::drawPatchOnImg( sPatch& sp, cv::Mat& img, cv::Scalar clr)
{
	cv::Rect rec;
	int radius = sp.width / 2;
	rec.x = sp.center.x -radius;
	rec.y = sp.center.y -radius;
	rec.width = sp.width;
	rec.height = sp.width;

	cv::rectangle(img, rec, clr);
}

void Shared::drawPatchFontOnImg( sPatch& sp, string& ID, cv::Mat& img, cv::Scalar clr )
{
	cv::Rect rec;
	int radius = sp.width / 2;
	rec.x = sp.center.x -radius;
	rec.y = sp.center.y -radius;
	rec.width = sp.width;
	rec.height = sp.width;

	cv::rectangle(img, rec, clr);	
#ifdef _DEBUG
#else
	cv::putText(img, ID, cv::Point(rec.x, rec.y + 20), FONT_HERSHEY_SIMPLEX, 0.35, clr,1);
#endif
}

void Shared::myXor( cv::Mat& src1, cv::Mat& src2, cv::Mat& dst )
{
	src1.copyTo(dst);
	for (int i = 0; i < src1.rows; i++)
	{
		uchar* p1 = src1.ptr<uchar>(i);
		uchar* p2 = src2.ptr<uchar>(i);
		uchar* pd = dst.ptr<uchar>(i);
		for (int j = 0; j < src1.cols; j++, p1++, p2++, pd++)
		{
			if(0 < p2[0] && 0 < p1[0])
				pd[0] = 0;
		}
	}

}

void Shared::myAnd( cv::Mat& src1_32F, cv::Mat& src2_32F, cv::Mat& dst_32F, float thre1, float thre2 )
{
	for (int i = 0; i < src1_32F.rows; i++)
	{
		float* p1 = src1_32F.ptr<float>(i);
		float* p2 = src2_32F.ptr<float>(i);
		float* pd = dst_32F.ptr<float>(i);
		for (int j = 0; j < src1_32F.cols; j++, p1++, p2++, pd++)
		{
			if (p1[0] > thre1 && p2[0] > thre2)
				pd[0] = 255;
			else
				pd[0] = 0;
		} 
	}
}

void Shared::drawPoly( cv::Mat& source, vector<cv::Point>& points, cv::Scalar& clr, int thickness, bool fill /*= false*/ )
{
	cv::Point** pp = new cv::Point*[1];
	pp[0] = new cv::Point[points.size()];
	for (int k = 0; k < points.size(); k++)
	{
		if(points.size() == 20)
		cout << "Draw k = " << k << points[k].x << points[k].y << endl;
		pp[0][k] = points[k];
	}
	int* npt = new int[1];
	npt[0] = points.size();
	//	cv::fillPoly(source, (const cv::Point**)pp, npt, 1, cv::Scalar(255));
	if(false == fill)
		cv::polylines(source, (const cv::Point**)pp, npt, 1, true, clr, thickness, 8, 0);
	else
		cv::fillPoly(source, (const cv::Point**)pp, npt, 1, clr);
	delete[] npt;
	delete[] pp[0];
	delete[] pp;
}

float Shared::distForTwoPat( cv::Mat& pat1, cv::Mat& pat2, int& flipType)
{
	float res = 1E20, tRes;
	cv::Mat flipMat;
	res = cv::norm(pat1, pat2, CV_L2);
	cv::flip(pat2, flipMat, 0); //flip-x
	tRes = cv::norm(pat1, flipMat, CV_L2);
	if(tRes < res)
	{
		res = tRes;
		flipType = FLIP_HORIZ;
	}
	cv::flip(pat2, flipMat, 1);
	tRes = cv::norm(pat1, flipMat, CV_L2);
	if(tRes < res)
	{
		res = tRes;
		flipType = FLIP_VERTI;
	}
	return res;
}

float Shared::distForTwoPat( cv::Mat& pat1, cv::Mat& pat2, cv::Vec3b& aveClr1, cv::Vec3b& aveClr2, uchar aveGra1, uchar aveGra2, int& flipType )
{
	
	float res = 1E20, tRes;
	int dist1 = abs(aveGra1 - aveGra2);
	if(dist1 > 40)
		return res;
	int dist2 = abs(aveClr1[0] - aveClr2[0]) + abs(aveClr1[1] - aveClr2[1]) + abs(aveClr1[2] - aveClr2[2]);
	if(dist2 > 110)
		return res;
	cv::Mat flipMat;
	res = cv::norm(pat1, pat2, CV_L2);
	cv::flip(pat2, flipMat, 0); //flip-x
	tRes = cv::norm(pat1, flipMat, CV_L2);
	if(tRes < res)
	{
		res = tRes;
		flipType = FLIP_HORIZ;
	}
	cv::flip(pat2, flipMat, 1);
	tRes = cv::norm(pat1, flipMat, CV_L2);
	if(tRes < res)
	{
		res = tRes;
		flipType = FLIP_VERTI;
	}
	return res;
}

void Shared::showMat( cv::Mat& relMat )
{
	cv::Mat srcRelMat;
		relMat.convertTo(srcRelMat, relMat.type());
		int show_col = srcRelMat.cols * 10;
		int show_row = srcRelMat.rows * 10;
		cv::Mat img_show;
		img_show.create(show_row, show_col, CV_8UC1);
		//	cv::normalize(relMat, relMat);
		//	relMat = relMat * 1800;
		double minV, maxV;
		cv::Point minLoc, maxLoc;
/*
		for (int i = 0; i < relMat.rows; i++)
		{
			for (int j = 0; j < relMat.cols; j++)
			{
				uchar vvv = relMat.ptr<>(i)[j];
				if(vvv > maxV)
				{
					maxV = vvv;
					maxLoc.x = j; maxLoc.y = i;
				}
				if(vvv < minV)
				{
					minV = vvv;
					minLoc.x = j; minLoc.y = i;
				}
			}
		}*/
		cv::minMaxLoc(srcRelMat, &minV, &maxV, &minLoc, &maxLoc);
		srcRelMat = srcRelMat - minV;
		srcRelMat = 255.0 / (maxV - minV) * srcRelMat; 
		//	temp.convertTo(temp, CV_8UC1);
		cv::resize(srcRelMat, img_show, img_show.size());
		cout << minV << "--" << maxV << endl;
		img_show.convertTo(img_show, CV_8UC1);
		SHOW_IMG(img_show, 1);
}

float Shared::myArcTan( float x1, float y1, float x2, float y2 )
{
	if(x1!=x2)
		return atan((y2-y1)/(x2-x1));
	else
		return 3.1416 / 2;
}
