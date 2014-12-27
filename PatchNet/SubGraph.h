#pragma once
#include "FacEdge.h"
class SubGraph
{
public:
	FacNode _centerNode;
	vector<FacEdge> _edges;//node1 is centerNode,node2 2 is the adjacent node

private:
	FacNode _node; //drawSubGraph 方法做对象的
	FacEdge _edge;

public:

	void drawSubGraph(Mat& src);
	void reorganizeGraph();//organize subgraph according to the angle to centerNode

	SubGraph(void);
	~SubGraph(void);
};

