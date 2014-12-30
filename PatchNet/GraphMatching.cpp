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

void GraphMatching::subGraphMatching(vector<OneMatch>& matches, SubGraph& target, vector<SubGraph>& candidates){

	matches.clear();
	matches.resize(candidates.size());
	double matchScore;
	for(int k=0;k<candidates.size();k++){

		oneSubGraphMatching(target,candidates[k]);

		matches[k].targ = target;
		matches[k].cadi = candidates[k];
		matches[k].matchScore = _oneMatch.matchScore;
		////calc matchScore between target and candidat[i]
		//double dis_AreaRatio;
		//double dis_Avggray;
		//double dis_WslashH;
		//
		//_matches[k].targ = target;
		//_matches[k].cadi = candidates[k];

		///*dis_Avggray = abs(target._centerNode._avggray - candidates[i]._centerNode._avggray);
		//dis_WslashH = abs( (target._centerNode._WslashH - candidates[i]._centerNode._WslashH) / target._centerNode._WslashH );
		//dis_AreaRatio = target._edges.size();

		//matchScore = dis_AreaRatio + dis_Avggray + dis_WslashH;*/
		//target.reorganizeGraph();
		//candidates[k].reorganizeGraph();

		//double** distance;int rows = target._edges.size(),cols = candidates[k]._edges.size();
		//distance = new double*[rows];
		//for(int i=0;i<rows;i++)
		//	distance[i] = new double[cols];

		////calculate the distance of every 2 nodes from Subgraph#1 and SubGrapg#2
		//for(int i=0;i<rows;i++)
		//	for(int j=0;j<cols;j++)
		//		distance[i][j] = disOfTwoNodes(target._edges[i]._node2,candidates[k]._edges[j]._node2);
		//
		//DP dp(distance,rows,cols);
		//dp.performDP();

		//_matches[k].matchScore = dp._mindistance;
		//delete distance;
		//dp.~DP();
	}//end for

	sort(matches,betterThan);

	//find the best k matches:default k = 5
	/*_threK = matches.size()>5?5:matches.size();
	for(int i=0;i<1;i++){
		matches[i].cadi.drawSubGraph(matches[i].cadi._centerNode._srcImg.clone(),"best candidate#");
	}*/
}

void GraphMatching::doMatchingOfAfolder(string section, string srcDir, string regionDir){
	string testPath = srcDir+"\\"+section+".jpg";
	string testReg = regionDir+"\\"+section+"-label.txt";
	FacBuilder builder1;
	vector<OneMatch> matches;//每一个region 和一个training image里所有region的subgraph distance按从小到大排序
	vector<OneMatch> bestmatches;//每个traning里的best match region
	//build for test 
	builder1.buildGraph(testPath,testReg);
	builder1.buildAllSubGraphes(builder1._facGraph);

	if(!boost::filesystem::exists(srcDir) || !boost::filesystem::exists(regionDir)){
		cout<<"src or region directory not exists";
		return;
	}
	
	fs::path fullpath(srcDir, fs::native);
	cout << "=======> Start comp node in folder: " << srcDir <<endl;
	if(!fs::exists(fullpath)) return; 

	fs::recursive_directory_iterator end_iter;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			cout << "current image" << iter->path().string() << endl;
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			string currentImageBase = currentImageS.substr(0, currentImageS.find_first_of('.'));

			string currentRegPath = regionDir+"\\"+currentImageBase+"-label.txt";

			if(currentImageBase == string(section))
				continue;
			FacBuilder builder2;
			builder2.buildGraph(currentImagePath,currentRegPath);
			builder2.buildAllSubGraphes(builder2._facGraph);
			
			builder1._allSubGraphes[2].drawSubGraph(builder1._allSubGraphes[2]._centerNode._srcImg.clone(),"sub#1");
			//builder2._allSubGraphes[0].drawSubGraph(builder2._allSubGraphes[0]._centerNode._srcImg.clone(),"sub#2");

			subGraphMatching(matches, builder1._allSubGraphes[0],builder2._allSubGraphes);
			bestmatches.push_back(matches[0]);
			builder2.~FacBuilder();
			
			cout << "end of one image" << endl;
		}
	}
	sort(bestmatches,betterThan);
	for(int i=0;i<bestmatches.size();i++){
		bestmatches[i].cadi.drawSubGraph(bestmatches[i].cadi._centerNode._srcImg.clone(),"best candidate#");
		waitKey(0);
	}

	cout << "##############Run here###################" << endl;
}

void GraphMatching::oneSubGraphMatching(SubGraph& target, SubGraph& candidate){

	//target.drawSubGraph(target._centerNode._srcImg.clone(),"target");
	//candidate.drawSubGraph(candidate._centerNode._srcImg.clone(),"candidate");
	
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
	double weight_avgcolor = 1;

	double areaRatio1 = node1._AreaRatio,	areaRatio2 = node2._AreaRatio;
	Point position1 = node1._position,		position2 = node2._position;//用偏移代替
	double offsetX1 = node1._offsetX,		offsetX2 = node2._offsetX;
	double offsetY1 = node1._offsetY,		offsetY2 = node2._offsetY;
	Rect box1 = node1._box,					box2 = node2._box;	//region的box
	double WslashH1 = node1._WslashH,		WslashH2 = node2._WslashH; //BOX的宽高比
	Point rel_Wall1,						rel_Wall2;//relation with the wall 
	int avggray1 = node1._avggray,			avggray2 = node2._avggray;
	Vec3b avgcolor1 = node1._avgcolor,		avgcolor2 = node2._avgcolor;
	//normalize to 0-1
	double dis_areaRatio = abs(areaRatio1-areaRatio2); 
	//double dis_position = distanceP2P(position1,position2)/distanceP2P(Point(0,0),Point());
	double dis_WslashH = abs(WslashH1-WslashH2);
	double dis_avggray = abs(node1._avggray-node2._avggray)/255;
	double dis_offsetX = abs(offsetX1 - offsetX2);
	double dis_offsetY = abs(offsetY1 - offsetY2);
	double dis_avgcolor = ( abs(avgcolor1[0]-avgcolor2[0]) + abs(avgcolor1[1]-avgcolor2[1]) + abs(avgcolor1[2]-avgcolor2[2]) ) / 3;

	double distance = weight_areaRatio*dis_areaRatio + weight_posiotn*(dis_offsetX+dis_offsetY)
		+ weight_WslashH*dis_WslashH + weight_avggray*dis_avggray + weight_avgcolor * dis_avgcolor;
	
	return distance;;
	
}