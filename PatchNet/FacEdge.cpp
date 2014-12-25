#include "FacEdge.h"


FacEdge::FacEdge(void)
{
}

FacEdge::FacEdge(FacNode& node1, FacNode& node2){
	 _node1 = node1;
	 _node2 = node2;
	 _rel_direction = node1._position - node2._position;
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