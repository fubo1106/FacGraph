#include "GraphMatching.h"


GraphMatching::GraphMatching(void)
{
}


GraphMatching::~GraphMatching(void)
{
}

void GraphMatching::initMatching(SubGraph& target, vector<SubGraph>& candidates){
	_thrsAreaRatio = 0.05;//threshold for ..
	_threPosition = 0.1;
	_threAvggray = 0.2;
	_threWslashH = 0.2;
	_threK = 5;//candidate num

	_target = target;
	_candidates = candidates;

}

void GraphMatching::subGraphMatching(vector<OneMatch>& matches, SubGraph& target, vector<SubGraph>& candidates){
	target = _target;
	candidates = _candidates;
	
	matches.resize(candidates.size());
	double matchScore;
	for(int i=0;i<candidates.size();i++){
		//calc matchScore between target and candidat[i]
		double dis_AreaRatio;
		double dis_Avggray;
		double dis_WslashH;
		
		matches[i].targ = target;
		matches[i].cadi = candidates[i];

		dis_Avggray = abs(target._centerNode._avggray - candidates[i]._centerNode._avggray);
		dis_WslashH = abs( (target._centerNode._WslashH - candidates[i]._centerNode._WslashH) / target._centerNode._WslashH );
		dis_AreaRatio = target._edges.size();

		matchScore = dis_AreaRatio + dis_Avggray + dis_WslashH;
		matches[i].matchScore = matchScore;
	}
	sort(matches,betterThan);
}

bool GraphMatching::betterThan(OneMatch& m1, OneMatch& m2){
	return (m1.matchScore < m2.matchScore);
}