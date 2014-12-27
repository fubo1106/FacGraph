#include "FacNode.h"


FacNode::FacNode(void)
{
}


FacNode::~FacNode(void)
{
}

void FacNode::drawNode(FacNode& node, Mat& src, Scalar scl){ 

	int b = scl[0],g = scl[1],r = scl[2];
	if(src.channels() == 3){
		for(int i=0;i<node._contour.size();i++){
			src.at<Vec3b>(node._contour[i].x,node._contour[i].y) = Vec3b(b,g,r);
		}
	}
	else{
		for(int i=0;i<node._contour.size();i++){
			src.at<uchar>(node._contour[i].x,node._contour[i].y) = 100;
		}
	}

}

void FacNode::compAttributes(Mat& src){
	_srcImg = src;
	_AreaRatio = contourArea(_contour)/(src.rows*src.cols);
	_box = boundingRect(_contour);
	_WslashH = _box.width/_box.height;
	//_position = calcCenterPoint(_contour);
	_position = getGravityCenter(_contour);

	Mat gray;
	if(src.channels() == 3){
		 cvtColor(src,gray,CV_BGR2GRAY);
	}
	_avggray = calcAvgGray(_contour,gray);

	_offsetX = _position.x/_srcImg.rows;//check x y row col's correspondence
	_offsetY = _position.y/_srcImg.cols;

}

bool FacNode::isAdjacentWith(FacNode& node2){

	vector<Point> contour1 = _contour;
	vector<Point> contour2 = node2._contour;

	Mat mask1 = Mat::zeros(_srcImg.rows,_srcImg.cols,CV_8UC1);
	Mat mask2 = Mat::zeros(_srcImg.rows,_srcImg.cols,CV_8UC1);
	Mat mask = Mat::zeros(_srcImg.rows,_srcImg.cols,CV_8UC1);

	for(int i=0;i<contour1.size();i++){

		mask1.at<uchar>(contour1[i].x,contour1[i].y) = 1;

		//四个方向扩展
		if(contour1[i].x-1 >= 0 && mask1.at<uchar>(contour1[i].x-1,contour1[i].y) == 0){
			mask1.at<uchar>(contour1[i].x-1,contour1[i].y) = 1;
		}
		if(contour1[i].x+1 < _srcImg.rows && mask1.at<uchar>(contour1[i].x+1,contour1[i].y) == 0){
			mask1.at<uchar>(contour1[i].x+1,contour1[i].y) = 1;
		}
		if(contour1[i].y-1 >= 0 && mask1.at<uchar>(contour1[i].x,contour1[i].y-1) == 0){
			mask1.at<uchar>(contour1[i].x,contour1[i].y-1) = 1;
		}
		if(contour1[i].y+1 < _srcImg.cols && mask1.at<uchar>(contour1[i].x,contour1[i].y+1) == 0){
			mask1.at<uchar>(contour1[i].x,contour1[i].y+1) = 1;
		}

	}

	for(int i=0;i<contour2.size();i++){

		mask2.at<uchar>(contour2[i].x,contour2[i].y) = 1;

		//四个方向扩展
		if(contour2[i].x-1 >= 0 && mask2.at<uchar>(contour2[i].x-1,contour2[i].y) == 0){
			mask2.at<uchar>(contour2[i].x-1,contour2[i].y) = 1;
		}
		if(contour2[i].x+1 < _srcImg.rows && mask2.at<uchar>(contour2[i].x+1,contour2[i].y) == 0){
			mask2.at<uchar>(contour2[i].x+1,contour2[i].y) = 1;
		}
		if(contour2[i].y-1 >= 0 && mask2.at<uchar>(contour2[i].x,contour2[i].y-1) == 0){
			mask2.at<uchar>(contour2[i].x,contour2[i].y-1) = 1;
		}
		if(contour2[i].y+1 < _srcImg.cols && mask2.at<uchar>(contour2[i].x,contour2[i].y+1) == 0){
			mask2.at<uchar>(contour2[i].x,contour2[i].y+1) = 1;
		}

	}

	bitwise_and(mask1,mask2,mask);
	if(cv::countNonZero(mask) > 10)
		return true;
	else 
		return false;
}