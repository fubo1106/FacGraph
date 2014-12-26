#pragma once
#include "Basics.h"

class FacNode
{
public:
	
	int _ID;
	int _cluster;//
	double _AreaRatio;
	Point _position;//中心位置
	Rect _box;	//region的box
	double _WslashH; //BOX的宽高比
	Point _rel_Wall;//relation with the wall 
	Vec3b _avgcolor;
	int _avggray;
	int _depth;

	double _offsetX,_offsetY;//x y方向的偏移比例

	Mat _srcImg;

	vector<Point> _contour;

public:
	FacNode(void);
	~FacNode(void);
	
	void drawNode(FacNode& node, Mat& src, Scalar scl);
	void compAttributes(Mat& src);
	bool isAdjacentWith(FacNode& node2);
};

