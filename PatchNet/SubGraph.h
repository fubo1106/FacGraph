#pragma once
#include "FacEdge.h"
class SubGraph
{
public:
	FacNode _centerNode;
	vector<FacEdge> _edges;

private:
	FacNode _node; //drawSubGraph �����������
	FacEdge _edge;

public:

	void drawSubGraph(Mat& src);

	SubGraph(void);
	~SubGraph(void);
};

