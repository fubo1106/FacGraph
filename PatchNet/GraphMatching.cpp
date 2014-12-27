#include "GraphMatching.h"


GraphMatching::GraphMatching(void)
{
}


GraphMatching::~GraphMatching(void)
{
}

void GraphMatching::initMatching(SubGraph& target, SubGraph& candidate){
	_thrsAreaRatio = 0.05;//threshold for ..
	_threPosition = 0.1;
	_threAvggray = 0.2;
	_threWslashH = 0.2;
	_threK = 5;//candidate num

	_target = target;
	_candidate = candidate;
}

void GraphMatching::initMatching(SubGraph& target, vector<SubGraph>& candidates){
	_thrsAreaRatio = 0.05;//threshold for ..
	_threPosition = 0.1;
	_threAvggray = 0.2;
	_threWslashH = 0.2;
	_threK = 5;//candidate num

	_target = target;
	_candidates = candidates;
	_matches.resize(candidates.size());

}

bool betterThan(OneMatch& m1, OneMatch& m2){
	return (m1.matchScore < m2.matchScore);
}

void GraphMatching::subGraphMatching(SubGraph& target, vector<SubGraph>& candidates){
	
	_matches.resize(candidates.size());
	double matchScore;
	for(int k=0;k<candidates.size();k++){
		//calc matchScore between target and candidat[i]
		double dis_AreaRatio;
		double dis_Avggray;
		double dis_WslashH;
		
		_matches[k].targ = target;
		_matches[k].cadi = candidates[k];

		/*dis_Avggray = abs(target._centerNode._avggray - candidates[i]._centerNode._avggray);
		dis_WslashH = abs( (target._centerNode._WslashH - candidates[i]._centerNode._WslashH) / target._centerNode._WslashH );
		dis_AreaRatio = target._edges.size();

		matchScore = dis_AreaRatio + dis_Avggray + dis_WslashH;*/
		target.reorganizeGraph();
		candidates[k].reorganizeGraph();

		double** distance;int rows = target._edges.size(),cols = candidates[k]._edges.size();
		distance = new double*[rows];
		for(int i=0;i<rows;i++)
			distance[i] = new double[cols];

		//calculate the distance of every 2 nodes from Subgraph#1 and SubGrapg#2
		for(int i=0;i<rows;i++)
			for(int j=0;j<cols;j++)
				distance[i][j] = disOfTwoNodes(target._edges[i]._node2,candidates[k]._edges[j]._node2);
		
		DP dp(distance,rows,cols);
		dp.performDP();

		_matches[k].matchScore = dp._mindistance;
		delete distance;
		dp.~DP();
	}//end for

	sort(_matches,betterThan);
}

void GraphMatching::oneSubGraphMatching(SubGraph& target, SubGraph& candidate){
	
	_oneMatch.targ = target;
	_oneMatch.cadi = candidate;

	double matchScore;

	target.reorganizeGraph();
	candidate.reorganizeGraph();

	double** distance;int rows = target._edges.size(),cols = candidate._edges.size();
	distance = new double*[rows];
	for(int i=0;i<rows;i++)
		distance[i] = new double[cols];

	//calculate the distance of every 2 nodes from Subgraph#1 and SubGrapg#2
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
			distance[i][j] = disOfTwoNodes(target._edges[i]._node2,candidate._edges[j]._node2);
		
	DP dp(distance,rows,cols);
	dp.performDP();

	_oneMatch.matchScore = dp._mindistance;
	delete distance;
	dp.~DP();
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