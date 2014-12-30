#include "SubGraph.h"


SubGraph::SubGraph(void)
{
}


SubGraph::~SubGraph(void)
{
	_edges.clear();
}

bool lessThan(FacEdge& e1,FacEdge& e2){
	FacNode center = e1._node1;
	FacNode node1 = e1._node2;
	FacNode node2 = e2._node2;

	Point v1 = node1._position - center._position;
	Point v2 = node2._position - center._position;

	return acos(calcAngleOf2Vec(v1,Point(1,0))) < acos(calcAngleOf2Vec(v2,Point(1,0)));
}

void SubGraph::drawSubGraph(Mat& src,string title){
	Mat show = src.clone();
	for(int i=0;i<_edges.size();i++){
		_node.drawNode(_edges[i]._node1,show,Scalar(255,0,0));
		_node.drawNode(_edges[i]._node2,show,randColor());
		//namedWindow("graph",0);imshow("graph",show);waitKey(0);
		//show = src.clone();
	}
	for(int i=0;i<_edges.size();i++){
		_edge.drawEdge(_edges[i],show,Scalar(0,0,255));
		namedWindow(title,0);imshow(title,show);//waitKey(0);
	}
	
}

void SubGraph::reorganizeGraph(){
	FacNode centerNode = _centerNode;

	sort(_edges,lessThan);


}
