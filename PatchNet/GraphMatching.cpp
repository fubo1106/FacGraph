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

bool betterThan(OneMatch& m1, OneMatch& m2){
	return (m1.matchScore < m2.matchScore);
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

double GraphMatching::disOfTwoNodes(FacNode& node1, FacNode& node2){

	double weight_areaRatio = 1;
	double weight_posiotn = 1;
	double weight_WslashH = 1;
	double weight_avggray = 1;

	double areaRatio1 = node1._AreaRatio,	areaRatio2 = node2._AreaRatio;
	Point position1 = node1._position,		position2 = node2._position;
	double offsetX1 = node1._offsetX,		offsetX2 = node2._offsetX;
	double offsetY1 = node1._offsetY,		offsetY2 = node2._offsetY;
	Rect box1 = node1._box,					box2 = node2._box;	//region的box
	double WslashH1 = node1._WslashH,		WslashH2 = node2._WslashH; //BOX的宽高比
	Point rel_Wall1,						rel_Wall2;//relation with the wall 
	Vec3b avgcolor1,	avgcolor2;
	int avggray1 = node1._avggray,			avggray2 = node2._avggray;

	//normalize to 0-1
	double dis_areaRatio = abs(areaRatio1-areaRatio2); 
	//double dis_position = distanceP2P(position1,position2)/distanceP2P(Point(0,0),Point());
	double dis_WslashH = abs(WslashH1-WslashH2);
	double dis_avggray = abs(node1._avggray-node2._avggray)/255;
	double dis_offsetX = abs(offsetX1 - offsetX2);
	double dis_offsetY = abs(offsetY1 - offsetY2);

	double distance = weight_areaRatio*dis_areaRatio + weight_posiotn*(dis_offsetX+dis_offsetY) + weight_WslashH*dis_WslashH + weight_avggray*dis_avggray;
	
	return distance;;
	
}