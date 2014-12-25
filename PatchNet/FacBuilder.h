#pragma once
#include "FacGraph.h"
#include "SubGraph.h"
#include <queue>


class FacBuilder
{
public:
	FacGraph _facGraph;
	SubGraph _subGraph;
public:

	void buildGraph(Mat& img, Mat& region);
	void buildSubGraph(FacNode& node, FacGraph& facGraph);
	Mat getRegionFromImg(Mat& img);
	vector<FacNode> getNodesFromImg(Mat& img, Mat& region);
	vector<FacEdge> getEdgesFromNodes(Mat& img, vector<FacNode>& nodes);

	FacBuilder(void);
	FacBuilder(FacGraph g);
	~FacBuilder(void);
};

