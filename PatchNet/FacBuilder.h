#pragma once
#include "FacGraph.h"
#include "SubGraph.h"
#include <queue>


class FacBuilder
{
public:
	FacGraph _facGraph;
	SubGraph _subGraph;//Ϊĳһ��node builder subgraph
	vector<SubGraph> _allSubGraphes;//ÿһ��node����Subgraph��vector
public:
	
	void buildGraph(string section,string imgDir, string regDir);
	void buildSubGraph(FacNode& node, FacGraph& facGraph);
	void buildAllSubGraphes(FacGraph& facGraph);
	Mat getRegionFromImg(Mat& img);
	
	FacBuilder(void);
	FacBuilder(FacGraph g);
	~FacBuilder(void);
private:
	void build(string srcDir, string regionDir);
	void buildGraph(string section, Mat& img, Mat& region);
	vector<FacNode> getNodesFromImg(string section,Mat& img, Mat& region);
	vector<FacEdge> getEdgesFromNodes(Mat& img, vector<FacNode>& nodes);

	
};

