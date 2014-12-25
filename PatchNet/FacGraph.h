#pragma once
#include "FacEdge.h"
class FacGraph
{
public:

	

	vector<FacNode> _nodes;
	vector<FacEdge> _edges;

private:
	FacNode _node;//drawGraph做对象调用方法
	FacEdge _edge;

public:

	void drawGraph(Mat& src,Scalar nclolor1, Scalar ncolor2, Scalar ecolor);

	FacGraph(void);
	~FacGraph(void);


	//void getEstDepth(vector<SecInf>& secs, string depthDir, int infDep);
};

