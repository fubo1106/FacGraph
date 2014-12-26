#include "SubGraph.h"


SubGraph::SubGraph(void)
{
}


SubGraph::~SubGraph(void)
{
	_edges.clear();
}

void SubGraph::drawSubGraph(Mat& src){
	Mat show = src.clone();
	for(int i=0;i<_edges.size();i++){
		_node.drawNode(_edges[i]._node1,show,Scalar(255,0,0));
		_node.drawNode(_edges[i]._node2,show,randColor());
		//namedWindow("graph",0);imshow("graph",show);waitKey(0);
		//show = src.clone();
	}
	for(int i=0;i<_edges.size();i++){
		_edge.drawEdge(_edges[i],show,Scalar(0,0,255));
		namedWindow("graph",0);imshow("graph",show);waitKey(0);
	}
	
}
