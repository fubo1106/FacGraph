#pragma once
#include "FacEdge.h"
class FacGraph
{
public:

	

	vector<FacNode> _nodes;
	vector<FacEdge> _edges;

private:
	FacNode _node;//drawGraph��������÷���
	FacEdge _edge;

public:

	void drawGraph(Mat& src,Scalar nclolor1, Scalar ncolor2, Scalar ecolor);

	FacGraph(void);
	~FacGraph(void);


	//void getEstDepth(vector<SecInf>& secs, string depthDir, int infDep);
};

