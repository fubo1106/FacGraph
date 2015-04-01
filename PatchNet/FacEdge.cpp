#include "FacEdge.h"


FacEdge::FacEdge(void)
{
}

FacEdge::FacEdge(FacNode& node1, FacNode& node2){
	 _node1 = node1;
	 _node2 = node2;
	 _rel_direction = node1._position - node2._position;
	 calcSpatialFeature();
}

FacEdge::~FacEdge(void)
{
	
}

void FacEdge::drawEdge(FacEdge& edge, Mat& src, Scalar scl){
	int b = scl[0],g = scl[1],r = scl[2];
	FacNode n1 = edge._node1;
	FacNode n2 = edge._node2;

	if(src.channels() == 1)
		cvtColor(src,src,CV_GRAY2BGR);

	circle(src,Point(n1._position.y,n1._position.x),4,scl,2);
	circle(src,Point(n2._position.y,n2._position.x),4,scl,2);
	line(src,Point(n1._position.y,n1._position.x),Point(n2._position.y,n2._position.x),scl,2,CV_AA);
}

vector<double> FacEdge::calcSpatialFeature(){
	vector<double> feature;
	feature.resize(5);
	feature[0] = _node1._box.tl().x - _node2._box.tl().x;
	feature[1] = _node1._box.br().x - _node2._box.br().x;
	feature[2] = distanceP2P(_node1._position,_node2._position);
	double unionNN = countUnion(_node1,_node2);
	feature[3] = unionNN/contourArea(_node1._contour);
	feature[4] = unionNN/contourArea(_node2._contour);
	return feature;
}

int FacEdge::countUnion(FacNode& node1,FacNode& node2){
	int r1 = node1._srcImg.rows;
	int c1 = node1._srcImg.cols;
	int r2 = node2._srcImg.rows;
	int c2 = node2._srcImg.cols;
	int r = max(r1,r2);
	int c = max(c1,c2);

	Mat mask1 = Mat::zeros(r,c,CV_8UC1);
	Mat mask2 = Mat::zeros(r,c,CV_8UC1);
	Mat mask = Mat::zeros(r,c,CV_8UC1);

	vector<Point> contour1 = node1._contour;
	vector<Point> contour2 = node2._contour;

	for(int i=0;i<contour1.size();i++){
		mask1.at<uchar>(contour1[i].x,contour1[i].y) = 1;
	}
	for(int i=0;i<contour2.size();i++){
		mask2.at<uchar>(contour2[i].x,contour2[i].y) = 1;
	}
	bitwise_and(mask1,mask2,mask);
	
	return countNonZero(mask); 
}