#pragma once

struct CmShow
{
	// Default colors
	static const Scalar RED, GREEN, BLUE, WHITE, BLACK, GRAY, YELLOW, CYAN, MAGENTA;
	static const int COLOR_NUM = 26, COLOR_NUM_NO_GRAY = 21;
	static const Vec3b gColors[COLOR_NUM];

	// Flags controlling how to visualize complex data
	enum {SHOW_MAG_LOG = 1, // Whether using log value when showing magnitude. Default: using linear value
		ORNT2HUE_SYM4 = 2,	// Whether using 4 symmetrical color when showing angle. Default: show all degree
		MAG_AS_SAT = 4,		// Whether visualize magnitude as saturation. Default: show magnitude and orientation separately
	};
	// Show a single channel complex mat. Only show angle for those with magnitude > minMagShowAng.
	// Angles are measured by degree. flags should refer to enum values of this class.
	static Mat Complex(const Mat& cmplx, const string& title, float minMagShowAng = 0.02f, int flag = 0 /*ORNT2HUE_SYM4*/);
	static Mat ComplexD(const Mat& dx, const Mat& dy, const string& tile, float minMagShowAng = 0.02f, int flag = 0);
	static Mat Complex(const Mat& ang, const Mat& mag, const string& title, float minMagShowAng = 0.02f, int flag = 0);

	static void ColorCircle(const string &tile, int radius = 200, int flag = 0); // Show color circle

	template<class T> static void Normalize(const vector<Point_<T>> &inPs, vector<Point2i> &outPs, int size = 600, int bound = 5);

	template<class T> static void PntSet(const vector<Point_<T>>& pntSet, const Scalar &color, const string &title, Mat &showImg=Mat());

	// Show a label map. labelNum: how many number of random colors used for show, use default colors if is -1
	static void Label(const Mat& label1i, const string& title, int labelNum = 0);

	static Vec3b RandomColor() {return Vec3b((uchar)(rand() % 200 + 25), (uchar)(rand() % 200 + 25), (uchar)(rand() % 200 + 25)); };

	// Show histogram bins. binColor3f and binVal (any single channel type) is 1xn matrix represent bin color and value
	static Mat HistBins(const Mat& binColor3f, const Mat& binVal, const string& title, bool descendShow = false);

	static inline void SaveShow(const Mat& img, const string& title);


	static void Demo(void);

private: // Information about how to visualize an orientation by a color
	// Converting an orientation to Hue which is corresponding to OpenCV HSV color. `degree' in [0, 360)
	static inline byte Ornt2Hue(float degree){ return (byte)(degree / 2); }
	static inline byte Ornt2HueSym4(float degree); 
	typedef byte (*Ornt2HueFunc)(float degree);
};

template<class T> void CmShow::Normalize(const vector<Point_<T>> &inPs, vector<Point2i> &outPs, int size, int bound)
{
	int pntNum = (int)inPs.size();
	CV_Assert(pntNum > 1);
	T minX = inPs[0].x, minY = inPs[0].y;
	T maxX = minX, maxY = minY;
	for (int i = 1; i < pntNum; i++)
	{
		minX = min(minX, inPs[i].x);
		minY = min(minY, inPs[i].y);
		maxX = max(maxX, inPs[i].x);
		maxY = max(maxY, inPs[i].y);
	}

	double scale = (size - 2.0 * bound) / (max((maxX - minX), (maxY - minY)) + 1);
	Point_<T> shift((T)(-(maxX + minX) * scale / 2 + size / 2), (T)(-(maxY + minY) * scale / 2 + size/2));

	outPs.resize(pntNum);
	for (int i = 0; i < pntNum; i++)
		outPs[i] = inPs[i] * scale + shift;
}

template<class T> void CmShow::PntSet(const vector<Point_<T>>& pntSet, const Scalar &color, const string &title, Mat &showImg)
{
	PointSeti pnts;
	const int dSize = 600; // Display size
	Normalize(pntSet, pnts, dSize, 10);
	
	if (showImg.cols == 0 || showImg.rows == 0)
		showImg = Mat::zeros(dSize, dSize, CV_8UC3);
	else
		showImg.create(dSize, dSize, CV_8UC3);

	int pntNum = (int)pnts.size();
	//circle(showImg, pnts[0], 5, CmShow::YELLOW);
	for (int i = 0; i < pntNum; i++)
		circle(showImg, pnts[i], 3, color);

	SaveShow(showImg, title);	
}

void CmShow::SaveShow(const Mat& img, const string& title)
{
	if (title.size() == 0)
		return;

	int mDepth = CV_MAT_DEPTH(img.type());
	double scale = (mDepth == CV_32F || mDepth == CV_64F ? 255 : 1);
	if (title.size() > 4 && title[title.size() - 4] == '.')
		imwrite(title, img*scale);
	else if (title.size())
		imshow(title, img);		
}

byte CmShow::Ornt2HueSym4(float degree)
{
	if (degree > 180.f)
		degree -= 180.f;
	if (degree > 90.f)
		degree = 180.f - degree;
	return byte(degree * (4.f / 3.f));
}
