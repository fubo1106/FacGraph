#pragma once
#include "FacEdge.h"
class SubGraph
{
public:
	FacNode _centerNode;
	vector<FacEdge> _edges;

private:
	FacNode _node; //drawSubGraph 方法做对象的
	FacEdge _edge;

public:

	void drawSubGraph(Mat& src,Scalar nclolor1, Scalar ncolor2, Scalar ecolor);

	SubGraph(void);
	~SubGraph(void);
};

