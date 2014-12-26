#include "Basics.h"

Point calcCenterPoint(vector<Point>& points){
	int sum_x = 0,sum_y = 0;
	for(int i=0;i<points.size();i++){
		sum_x += points[i].x;
		sum_y += points[i].y;
	}
	return Point(sum_x/points.size(),sum_y/points.size());
}

int calcAvgGray(vector<Point>& contour, Mat& grayImg){
	int sumGray=0;
	for(int i=0;i<contour.size();i++){
		sumGray += grayImg.at<uchar>(contour[i].x,contour[i].y);
	}
	return sumGray/contour.size();
}

Scalar randColor(){
	int x=0,y=255;
	int b = x+rand()%(y-x+1);
	int g = x+rand()%(y-x+1);
	int r = x+rand()%(y-x+1);
	return Vec3b(b,g,r);
}