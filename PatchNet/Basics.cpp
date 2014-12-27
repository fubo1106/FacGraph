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

double distanceP2P(const Point& p1,const Point& p2){
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) );
}

double calcAngleOf2Vec(Point& vec1,Point& vec2){
	double cos_theta = (vec1.x * vec2.x + vec1.y * vec2.y) / (distanceP2P(Point(0,0),vec1) * distanceP2P(Point(0,0),vec2));
	return cos_theta;
}

Point getGravityCenter(vector<Point>& contour){
	return calcCenterPoint(contour);
}