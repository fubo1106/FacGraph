#pragma once
#include "SubGraph.h"
#include "FacBuilder.h"
#include "DP.h"
struct OneMatch
{
	SubGraph targ;//target subgraph
	SubGraph cadi;//matched candidate;

	pair<int,int> matchedPair;//对应匹配的node id pair

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
	SubGraph _candidate;
	vector<SubGraph> _candidates;

	OneMatch _oneMatch;		//一对subgraph的match纪录
	vector<OneMatch> _matches;//所有的match对
	
public:

	void initMatching(SubGraph& target,vector<SubGraph>& candidates);//初始化匹配参数 匹配精度
	void initMatching(SubGraph& target,SubGraph& candidates);
	void doMatching(string section, string srcDir, string regionDir,string depthDir);
	void doMatchingOfARegion(vector<OneMatch>& matches, string section, SubGraph& sub, string srcDir, string regionDir);
	void subGraphMatching(vector<OneMatch>& matches, SubGraph& target, vector<SubGraph>& candidates);
	void oneSubGraphMatching(SubGraph& target, SubGraph& candidate);
	double disOfTwoNodes(FacNode& node1, FacNode& node2);
	void depthTransfer(Mat& result, OneMatch& oneMatch, string depthDir);
	GraphMatching(void);
	~GraphMatching(void);

private:
	
};

