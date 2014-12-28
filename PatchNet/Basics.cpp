#include "Basics.h"

cv::Point calcCenterPoint(vector<cv::Point>& points){
	int sum_x = 0,sum_y = 0;
	for(int i=0;i<points.size();i++){
		sum_x += points[i].x;
		sum_y += points[i].y;
	}
	return cv::Point(sum_x/points.size(),sum_y/points.size());
}

int calcAvgGray(vector<cv::Point>& contour, cv::Mat& grayImg){
	int sumGray=0;
	for(int i=0;i<contour.size();i++){
		sumGray += grayImg.at<uchar>(contour[i].x,contour[i].y);
	}
	return sumGray/contour.size();
}

cv::Vec3b calcAvgColor(vector<cv::Point>& contour, cv::Mat& colorImg){
	if(colorImg.channels() == 1){
		cout<<"calcAvgColor(): input img must be 3 channels image!!"<<endl;
		return 0;
	}

	int sum_b=0,sum_g=0,sum_r=0;
	for(int i=0;i<contour.size();i++){
		sum_b += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[0];
		sum_g += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[1];
		sum_r += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[2];
	}
	return cv::Vec3b(sum_b/contour.size(),sum_g/contour.size(),sum_r/contour.size());
}

cv::Scalar randColor(){
	int x=0,y=255;
	int b = x+rand()%(y-x+1);
	int g = x+rand()%(y-x+1);
	int r = x+rand()%(y-x+1);
	return cv::Vec3b(b,g,r);
}

double distanceP2P(const cv::Point& p1,const cv::Point& p2){
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) );
}

double calcAngleOf2Vec(cv::Point& vec1,cv::Point& vec2){
	double cos_theta = (vec1.x * vec2.x + vec1.y * vec2.y) / (distanceP2P(cv::Point(0,0),vec1) * distanceP2P(cv::Point(0,0),vec2));
	return cos_theta;
}

cv::Point getGravityCenter(vector<cv::Point>& contour){
	Moments mu;
	mu = moments(contour,false);
	return cv::Point( mu.m10/mu.m00 , mu.m01/mu.m00 );
	//return cv::Point( mu.m01/mu.m00 , mu.m10/mu.m00 );//x y相反 代表图像中的位置
}