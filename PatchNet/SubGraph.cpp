#include "SubGraph.h"


SubGraph::SubGraph(void)
{
}


SubGraph::~SubGraph(void)
{
}

void SubGraph::drawSubGraph(Mat& src,Scalar ncolor1, Scalar ncolor2, Scalar ecolor){
	Mat show = src.clone();
	for(int i=0;i<_edges.size();i++){
		_node.drawNode(_edges[i]._node1,show,ncolor1);
		_node.drawNode(_edges[i]._node2,show,ncolor2);
		//namedWindow("graph",0);imshow("graph",src);waitKey(0);
		_edge.drawEdge(_edges[i],show,ecolor);
		namedWindow("graph",0);imshow("graph",show);waitKey(0);
		show = src.clone();
	}

}
