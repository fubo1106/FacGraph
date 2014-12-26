#pragma once
#include "SubGraph.h"

struct OneMatch
{
	SubGraph targ;//target subgraph
	SubGraph cadi;//matched candidate;

	double matchScore;
};

class GraphMatching
{
public:

	double _thrsAreaRatio;//threshold for ..
	double _threPosition;
	double _threAvggray;
	double _threWslashH;
	int _threK;//candidate num

	SubGraph _target;
	vector<SubGraph> _candidates;

	vector<OneMatch> _matches;
	
public:

	void initMatching(SubGraph& target,vector<SubGraph>& candidates);//初始化匹配参数 匹配精度
	void subGraphMatching(vector<OneMatch>& matches, SubGraph& target, vector<SubGraph>& candidates);
	double disOfTwoNodes(FacNode& node1, FacNode& node2);
	GraphMatching(void);
	~GraphMatching(void);

private:
	bool betterThan(OneMatch& m1,OneMatch& m2);
};

