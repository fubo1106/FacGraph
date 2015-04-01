#include "FacNode.h"


FacNode::FacNode(void)
{
}


FacNode::~FacNode(void)
{
	_feature.clear();
	_contour.clear();
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
	_position = calcCenterPoint(_contour);
	//_position = getGravityCenter(_contour);

	Mat gray;
	if(src.channels() == 3){
		 cvtColor(src,gray,CV_BGR2GRAY);
	}
	_avggray = calcAvgGray(_contour,gray);
	_avgcolor = calcAvgColor(_contour,_srcImg);

	_offsetX = _position.x/_srcImg.rows;//check x y row col's correspondence
	_offsetY = _position.y/_srcImg.cols;

	//calculate geometric feature 14 dimensions
	_feature.resize(16);
	_feature[0] = _box.width;
	_feature[1] = _box.height;
	_feature[2] = _WslashH;

	double v1 = calcVariance(_contour,"x");
	double v2 = calcVariance(_contour,"y");
	double V1 = max(v1,v2),V2 = min(v1,v2);
	_feature[3] = V1;
	_feature[4] = V2;
	_feature[5] = V2/V1;

	vector<double> hx = calcHistogram(_contour,"x");
	vector<double> hy = calcHistogram(_contour,"y");

	_feature[6] = hx[0];
	_feature[7] = hx[1];
	_feature[8] = hx[2];
	_feature[9] = hx[3];
	_feature[10] = hy[0];
	_feature[11] = hy[1];
	_feature[12] = hy[2];
	_feature[13] = hy[3];

	_feature[14] = _position.x;
	_feature[15] = _position.y;
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

double FacNode::calcVariance(vector<Point>& contour, string coordinate){
	vector<double> data;
	double v;
	if(coordinate == "x" || coordinate == "X"){
		for(int i=0;i<contour.size();i++)
			data.push_back(contour[i].x);
		v = variance(data);
	}
	else if(coordinate == "y" || coordinate == "Y"){
		for(int i=0;i<contour.size();i++)
			data.push_back(contour[i].y);
		v = variance(data);
	}
	else{
		cout<<"not exist this coordinate.."<<endl;
		return 0;
	}
	return v;
}

vector<double> FacNode::calcHistogram(vector<Point>& contour, string coordinate){
	vector<double> h;
	h.resize(4); //4-bins
	double h1=0,h2=0,h3=0,h4=0;
	if(coordinate == "x" || coordinate == "X"){
		int step = (_box.br().x-_box.tl().x)/4;
		for(int i=0;i<contour.size();i++){
			if(contour[i].x >= _box.tl().x && contour[i].x <= (_box.tl().x+step)) h1++;
			else if(contour[i].x > (_box.tl().x+step) && contour[i].x <= (_box.tl().x+2*step)) h2++;
			else if(contour[i].x > (_box.tl().x+2*step) && (contour[i].x <= _box.tl().x+3*step)) h3++;
			else h4++;
		}
	}
	else if(coordinate == "y" || coordinate == "Y"){
		int step = (_box.br().y-_box.tl().y)/4;
		for(int i=0;i<contour.size();i++){
			if(contour[i].y >= _box.tl().y && contour[i].y <= (_box.tl().y+step)) h1++;
			else if(contour[i].y > (_box.tl().y+step) && contour[i].y <= (_box.tl().y+2*step)) h2++;
			else if(contour[i].y > (_box.tl().y+2*step) && contour[i].y <= (_box.tl().y+3*step)) h3++;
			else h4++;
		}
	}
	else{
		cout<<"not exist this coordinate.."<<endl;
		return h;
	}
	h[0] = h1/_contour.size();
	h[1] = h2/_contour.size();
	h[2] = h3/_contour.size();
	h[3] = h4/_contour.size();
	return h;
}