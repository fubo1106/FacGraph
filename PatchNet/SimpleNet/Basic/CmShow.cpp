#include "../stdafx2.h"
#include "CmShow.h"

const Vec3b CmShow::gColors[CmShow::COLOR_NUM] = 
{
	Vec3b(153, 0, 48),	 Vec3b(237, 28, 36),   Vec3b(255, 126, 0),   Vec3b(255, 194, 14),  Vec3b(255, 255, 0),  Vec3b(168, 230, 29),
	Vec3b(0, 183, 239),   Vec3b(77, 109, 243),  Vec3b(47, 54, 153),   Vec3b(111, 49, 152),  Vec3b(156, 90, 60),
	Vec3b(255, 163, 177), Vec3b(229, 170, 122), Vec3b(245, 228, 156), Vec3b(255, 249, 189), Vec3b(211, 249, 188),
	Vec3b(157, 187, 97),  Vec3b(153, 217, 234), Vec3b(112, 154, 209), Vec3b(84, 109, 142),  Vec3b(181, 165, 213),
	Vec3b(40, 40, 40),	 Vec3b(70, 70, 70),	  Vec3b(120, 120, 120), Vec3b(180, 180, 180), Vec3b(220, 220, 220)
};

const Scalar CmShow::RED(0, 0, 255);
const Scalar CmShow::GREEN(0, 255, 0);
const Scalar CmShow::BLUE(255, 0, 0);

const Scalar CmShow::WHITE(255, 255, 255);
const Scalar CmShow::GRAY(128, 128, 128);
const Scalar CmShow::BLACK(0, 0, 0);

const Scalar CmShow::CYAN(255, 255, 0);
const Scalar CmShow::MAGENTA(255, 0, 255);
const Scalar CmShow::YELLOW(0, 255, 255);


/************************************************************************/
/* Show an complex array: Mag, Angle, FFT shifted log mag               */
/************************************************************************/
Mat CmShow::Complex(const Mat& _cmplx, const string& title, float minMagShowAng, int flag)
{
	CV_Assert(_cmplx.channels() == 2 && _cmplx.data != NULL);
	Mat ang(_cmplx.size(), CV_32FC1), mag(_cmplx.size(), CV_32FC1), cmplx;
	_cmplx.convertTo(cmplx, CV_32F);

	for (int y = 0; y < cmplx.rows; y++)
	{
		float* cpV = cmplx.ptr<float>(y);
		float* angV = ang.ptr<float>(y);
		float* magV = mag.ptr<float>(y);
		for (int x = 0; x < cmplx.cols; x++, cpV+=2)
		{
			magV[x] = sqrt(cpV[0] * cpV[0] + cpV[1] * cpV[1]);
			angV[x] = cvFastArctan(cpV[1], cpV[0]);
		}
	}
	return Complex(ang, mag, title, minMagShowAng, flag);
}

Mat CmShow::ComplexD(const Mat& dx, const Mat& dy, const string& tile, float minMagShowAng, int flag)
{
	vector<Mat> chanals(2);
	chanals[0] = dx, chanals[1] = dy;
	Mat cmplx;
	merge(chanals, cmplx);
	return Complex(cmplx, tile, minMagShowAng, flag);
}


/************************************************************************/
/* Show an complex array: Mag, Angle, FFT shifted log mag               */
/*            ang(in degree) and mag should be 64F or 32F				*/
/************************************************************************/
Mat CmShow::Complex(const Mat& _ang, const Mat& _mag, const string& title, float minMagShowAng, int flag)
{
	CV_Assert(_ang.size() == _mag.size() && _ang.channels() == 1 && _mag.channels() == 1);
	Mat ang, mag;
	_ang.convertTo(ang, CV_32F);
	_mag.convertTo(mag, CV_32F);
	if (flag & SHOW_MAG_LOG)
	{
		cv::log(mag + 1, mag);
		CmCv::FFTShift(mag);
	}
	normalize(mag, mag, 0, 255, NORM_MINMAX, CV_8U);
	minMagShowAng *= 255;
	int rows = ang.rows, cols = ang.cols;
	Mat img8U3C(rows, flag & MAG_AS_SAT ? cols : cols * 2, CV_8UC3);
	if (!(flag & MAG_AS_SAT))
		cvtColor(mag, img8U3C(Rect(cols, 0, cols, rows)), CV_GRAY2BGR);

	Mat showAng = img8U3C(Rect(0, 0, cols, rows));
	Ornt2HueFunc ornt2HueFunc = flag & ORNT2HUE_SYM4 ? Ornt2HueSym4 : Ornt2Hue;
	for (int y = 0; y < rows; y++)
	{
		float* angV = ang.ptr<float>(y);
		byte* magV = mag.ptr<byte>(y);
		byte* angShow = showAng.ptr<byte>(y);
		for (int x = 0; x < cols; x++, angShow += 3)
		{
			if (magV[x] < minMagShowAng)
				continue;
			angShow[0] = ornt2HueFunc(angV[x]); 
			angShow[1] = flag & MAG_AS_SAT ? magV[x] : 255;
			angShow[2] = 255;
		}
	}
	//Mat showMag = img8U3C(Rect(0, 0, cols, rows));
	//if (flag & SHOW_MAG_LOG) //Show FFT shifted log magnitude
	//{
	//	Mat magB;
	//	cv::log(mag + 1, mag);
	//	normalize(mag, magB, 0, 255, NORM_MINMAX, CV_8U);
	//	CmCv::FFTShift(magB);
	//	cvtColor(magB, showMag, CV_GRAY2BGR);
	//}
	//else // Show Normal magnitude
	//{
	//	normalize(mag, mag, 0, 255, NORM_MINMAX, CV_8U);
	//	cvtColor(mag, showMag, CV_GRAY2BGR);
	//}

	//// Show angle
	//Ornt2HueFunc ornt2HueFunc = flag & ORNT2HUE_SYM4 ? Ornt2HueSym4 : Ornt2Hue;
	//Mat showAng = img8U3C(Rect(cols, 0, cols, rows));
	//showAng = 0;
	//normalize(_mag, mag, 0, 1, NORM_MINMAX, CV_32F);
	//for (int y = 0; y < rows; y++)
	//{
	//	float* angV = ang.ptr<float>(y);
	//	float* magV = mag.ptr<float>(y);
	//	byte* angShow = showAng.ptr<byte>(y);
	//	for (int x = 0; x < cols; x++, angShow += 3)
	//	{
	//		if (magV[x] < minMagShowAng)
	//			continue;
	//		angShow[0] = ornt2HueFunc(angV[x]); 
	//		angShow[1] = angShow[2] = 255;
	//	}
	//}
	cvtColor(showAng, showAng, CV_HSV2BGR);
	SaveShow(img8U3C, title);
	return img8U3C;
}

/************************************************************************/
/* Show color circle                                                    */
/************************************************************************/
void CmShow::ColorCircle(const string &tile, int radius, int flag)
{
	int diameter = radius * 2;
	float maxDist = (float)(radius * radius);
	Mat showImg = Mat::zeros(diameter, diameter, CV_8UC3);
	Ornt2HueFunc ornt2HueFunc = flag & ORNT2HUE_SYM4 ? Ornt2HueSym4 : Ornt2Hue;
	for (int r = 0; r < diameter; r++)
	{
		byte* showV = showImg.ptr<byte>(r);
		for (int c = 0; c < diameter; c++, showV += 3)
		{
			float x = (float)(c-radius), y = (float)(r-radius);
			if (x*x + y*y > maxDist)
				continue;
			showV[0] = ornt2HueFunc(cvFastArctan(y, x));
			showV[1] = showV[2] = 255;
		}
	}
	cvtColor(showImg, showImg, CV_HSV2BGR);
	SaveShow(showImg, tile);
}

// Show a label map. labelNum: how many number of random colors used for show, use default colors if is -1
void CmShow::Label(const Mat& label1i, const string& title, int labelNum)
{
	bool useRandom = labelNum > 0;
	labelNum = useRandom ? labelNum : COLOR_NUM_NO_GRAY;
	vector<Vec3b> colors(labelNum);
	if (useRandom)
		for (size_t i = 0; i < colors.size(); i++)
			colors[i] = RandomColor();
	else
		for (size_t i = 0; i < colors.size(); i++)
			colors[i] = gColors[i];

	Mat showImg = Mat::zeros(label1i.size(), CV_8UC3);
	for (int y = 0; y < label1i.rows; y++)
	{
		Vec3b* showD = showImg.ptr<Vec3b>(y);
		const int* label = label1i.ptr<int>(y);
		for (int x = 0; x < label1i.cols; x++)
			if (label[x] >= 0)
				showD[x] = colors[label[x] % labelNum];
	}
	SaveShow(showImg, title);
}

Mat CmShow::HistBins(const Mat& binColor3f, const Mat& _binVal, const string& title, bool descendShow)
{
	int H = 300, bW = 10, spaceH = 6, barH = 10, n = binColor3f.cols;
	CV_Assert(binColor3f.size() == _binVal.size() && binColor3f.rows == 1);
	Size szShow(bW * n, H + spaceH + barH);
	Mat binVal1d, binColor3b, showImg3b(szShow, CV_8UC3);
	normalize(_binVal, binVal1d, 0, 1, NORM_MINMAX, CV_64FC1);
	binColor3f.convertTo(binColor3b, CV_8UC3, 255);
	double* binVal = (double*)(binVal1d.data);
	Vec3b* binColor = (Vec3b*)(binColor3b.data);
	showImg3b = WHITE;

	vector<CostIdx> costIdx(n);
	if (descendShow)
	{
		for (int i = 0; i < n; i++)
			costIdx[i] = make_pair(binVal[i], i);
		sort(costIdx.begin(), costIdx.end(), std::greater<CostIdx>());
	}

	// Show image
	for (int i = 0; i < n; i++)
	{
		int idx = descendShow ? costIdx[i].second : i;
		int h = max(cvRound(H * binVal[idx]), 1);
		Rect reg(i*bW, H - h, bW, h);
		Scalar color(binColor[idx]);
		showImg3b(reg) = color;
		rectangle(showImg3b, reg, BLACK);
		reg.y = H + spaceH;
		reg.height = barH;
		showImg3b(reg) = color;
		rectangle(showImg3b, reg, BLACK);
	}
	SaveShow(showImg3b, title);
	return showImg3b;
}

