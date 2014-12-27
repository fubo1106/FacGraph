#pragma once
#include "SubGraph.h"

struct OneMatch
{
	SubGraph targ;//target subgraph
	SubGraph cadi;//matched candidate;

	pair<int,int> matchedPair;//��Ӧƥ���node id pair

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

	void initMatching(SubGraph& target,vector<SubGraph>& candidates);//��ʼ��ƥ����� ƥ�侫��
	void subGraphMatching(vector<OneMatch>& matches, SubGraph& target, vector<SubGraph>& candidates);
	double disOfTwoNodes(FacNode& node1, FacNode& node2);
	GraphMatching(void);
	~GraphMatching(void);

private:
	
};

