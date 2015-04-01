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

	OneMatch match;
	//matches.resize(candidates.size());
	double matchScore;
	for(int k=0;k<candidates.size();k++){

		oneSubGraphMatching(target,candidates[k]);

		match.targ = target;
		match.cadi = candidates[k];
		match.matchScore = _oneMatch.matchScore;
		matches.push_back(match);
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

void GraphMatching::doMatching(string section, string srcDir, string regionDir, string depthDir){
	
	FacBuilder builder1;
	vector<vector<OneMatch>> bestmatches;//test image的每个reigon对应的best match
	vector<OneMatch> matches; //the best correspondent region from every training image
	OneMatch bestOneMatch;
	builder1.buildGraph(section,srcDir,regionDir);
	builder1.buildAllSubGraphes(builder1._facGraph);

	Mat testReg,matchedReg;
	testReg = builder1._allSubGraphes[0]._centerNode._srcImg.clone();
	Mat depthResult = Mat::zeros(testReg.rows,testReg.cols,CV_8UC1);

	for(int i=0;i<builder1._allSubGraphes.size();i++){//为每一个testImage的subgraph找到最优的subgraph匹配
	
		//show test region
		/*builder1._allSubGraphes[i].drawSubGraph(testReg,"testReg");
		waitKey(0);*/
		//i=9;
		doMatchingOfARegion(matches,section,builder1._allSubGraphes[i],srcDir,regionDir);
		bestmatches.push_back(matches);

		builder1._allSubGraphes[i].drawSubGraph(testReg,"sub");
		imwrite("FacGraph-output\\region-" + boost::lexical_cast<string>(i) + ".jpg",testReg);

		testReg = builder1._allSubGraphes[i]._centerNode._srcImg.clone();
		for(int j=0;j<matches.size();j++){
			if(matches[j].matchScore < INF){
				matchedReg = matches[j].cadi._centerNode._srcImg.clone();
				matches[j].cadi.drawSubGraph(matchedReg,"matched");
				string s =  "score:"+boost::lexical_cast<string>(matches[j].matchScore);
				//putText(matchedReg,boost::lexical_cast<string>(matches[j].matchScore),Point(0,0),font,1,Scalar(0,255,0),1,CV_AA);
				putTextOnImg(matchedReg,s);
				//putText(matchedReg, s, cvPoint(10,50),FONT_HERSHEY_SIMPLEX,0.4,Scalar(0,0,0));
				imwrite("FacGraph-output\\region" + boost::lexical_cast<string>(i)+"-match"+ boost::lexical_cast<string>(j)+ ".jpg",matchedReg);
			}
		}

		depthTransfer(depthResult,matches[0],depthDir);

	}
	namedWindow("final-depResult",0);imshow("final-depResult",depthResult);waitKey(0);
	imwrite("FacGraph-output\\depthtransfer.jpg",depthResult);
	//bestmatches: test image的每个region 去 training set retrieve出的 best matched region

}

void GraphMatching::doMatchingOfARegion(vector<OneMatch>& matchesReg, string section, SubGraph& sub, string srcDir, string regionDir){
	/*string testPath = srcDir+"\\"+section+".jpg";
	string testReg = regionDir+"\\"+section+"-label.txt";*/
	//FacBuilder builder1;
	matchesReg.clear();
	vector<OneMatch> matches;//每一个region 和一个training image里所有region的subgraph distance按从小到大排序
	vector<OneMatch> bestmatches;//每个traning里的best match region
	//build for test 
	/*builder1.buildGraph(section,testPath,testReg);
	builder1.buildAllSubGraphes(builder1._facGraph);*/

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
			builder2.buildGraph(currentImageBase,srcDir,regionDir);
			builder2.buildAllSubGraphes(builder2._facGraph);
			
			/*Mat test = builder1._allSubGraphes[1]._centerNode._srcImg.clone();
			builder1._allSubGraphes[1].drawSubGraph(test,"sub#1");
			imwrite("test-region.jpg",test);*/
			//builder2._allSubGraphes[0].drawSubGraph(builder2._allSubGraphes[0]._centerNode._srcImg.clone(),"sub#2");

			subGraphMatching(matches, sub, builder2._allSubGraphes);
			//if(matches[0].matchScore < 0.5)
				matchesReg.push_back(matches[0]);
			//sub.drawSubGraph(sub._centerNode._srcImg,"testReg");
			//imwrite("Fac-temp\\"+
			builder2.~FacBuilder();
			matches.clear();
			
			cout << "end of one image" << endl;
		}
	}                         
	sort(matchesReg,betterThan);
	//bestmatch = bestmatches[0];
	//for(int i=0;i<bestmatches.size();i++){
	//	Mat bestMatch = bestmatches[i].cadi._centerNode._srcImg.clone();
	//	bestmatches[i].cadi.drawSubGraph(bestMatch,"best candidate#");
	//	imwrite("bestMatch"+boost::lexical_cast<string>(i)+".jpg",bestMatch);
	//	//waitKey(0);
	//}

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
	int rows = target._edges.size(),cols = candidate._edges.size();
	if(rows == cols){//candidate and target subgraph has same nodes
		double** distance;
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
	if(rows == cols){//排列之后
		_oneMatch.matchScore = PosDistOf2Subgraph(target,candidate);
	}
	else{
		_oneMatch.matchScore = INF;
	}
}

void GraphMatching::depthTransfer(Mat& result, OneMatch& match, string depthDir){
	SubGraph targ = match.targ;
	SubGraph cadi = match.cadi;

	//targ's srcImg have the same size of result
	string section = match.cadi._centerNode._section;
	string depthPath = depthDir + "\\" + section + ".png";
	if(!fs::exists(depthPath)) depthPath = depthDir + "\\" + section + ".jpg";

	int inferredDepth = cadi._centerNode._depth;
	Mat depthMap = imread(depthPath,0);

	for(int i=0;i<cadi._centerNode._contour.size();i++){
			inferredDepth = depthMap.at<uchar>(cadi._centerNode._contour[i].x,cadi._centerNode._contour[i].y);
	}

	for(int i=0;i<targ._centerNode._contour.size();i++){
		if(result.channels() == 1)
			result.at<uchar>(targ._centerNode._contour[i].x,targ._centerNode._contour[i].y) = inferredDepth;
		else
			result.at<Vec3b>(targ._centerNode._contour[i].x,targ._centerNode._contour[i].y) = Vec3b(inferredDepth,inferredDepth,inferredDepth);
	}

}

double GraphMatching::disOfTwoNodes(FacNode& node1, FacNode& node2){
	double distance;
#if 0 //previous measure
	double weight_areaRatio = 1;
	double weight_posiotn = 1;
	double weight_WslashH = 1;
	double weight_avggray = 0;
	double weight_avgcolor = 0;

	/*Mat src1 = imread("FacGraph-src\\img-32-depth.png");
	node1.drawNode(node1,src1,Scalar(255,0,0));
	imshow("src",src1);waitKey(0);

	Mat src2 = imread("FacGraph-src\\14.png");
	node2.drawNode(node2,src2,Scalar(255,0,0));
	imshow("src2",src2);waitKey(0);*/

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

	distance = weight_areaRatio*dis_areaRatio + weight_posiotn*(dis_offsetX+dis_offsetY)
		+ weight_WslashH*dis_WslashH + weight_avggray*dis_avggray + weight_avgcolor * dis_avgcolor;
#endif 

#if 1

	vector<double>weight;
	distance = EulerDistance(node1._feature,node2._feature, weight);
#endif
	return distance;
	
}

double GraphMatching::PosDistOf2Subgraph(SubGraph& sub1, SubGraph& sub2){

	sub1.reorganizeGraph();
	sub2.reorganizeGraph();

	int num1 = sub1._edges.size();
	int num2 = sub2._edges.size();
	double dist = 0;
	vector<double> vec_sub1,vec_sub2;//记录中心node与adjacent nodes之间的方向vector cos thea
	FacNode node1 = sub1._centerNode;
	FacNode node2 = sub2._centerNode;
#if 0 //previous diatance measure
	for(int i=0;i<sub1._edges.size();i++){
		Point vec1 = sub1._edges[i]._node2._position - node1._position;
		Point vec2 = sub2._edges[i]._node2._position - node1._position;

		//夹角差
		double thea1 = acos (calcAngleOf2Vec(vec1,Point(1,0)))/PI;
		double thea2 = acos (calcAngleOf2Vec(vec2,Point(1,0)))/PI;

		double posdist = distanceP2P(sub1._edges[i]._node2._position,sub2._edges[i]._node2._position)
			/sqrt(pow(sub1._centerNode._srcImg.rows,2)+pow(sub1._centerNode._srcImg.cols,2));

		dist += abs(thea1 - thea2) + posdist;

		//bounding box 
		double thresh_neighbor = 0.4;
		dist += thresh_neighbor*abs(sub1._edges[i]._node2._WslashH - sub2._edges[i]._node2._WslashH);
		dist += thresh_neighbor*(abs(boundingRect(sub1._edges[i]._node2._contour).width - boundingRect(sub2._edges[i]._node2._contour).width)
				+abs(boundingRect(sub1._edges[i]._node2._contour).height - boundingRect(sub2._edges[i]._node2._contour).height))/2;
	}
	//distance of 2 center node
	dist += distanceP2P(sub1._centerNode._position,sub2._centerNode._position);
		///sqrt(pow(sub1._centerNode._srcImg.rows,2)+pow(sub1._centerNode._srcImg.cols,2));

	//distance of geometry W/H
	dist += abs(sub1._centerNode._WslashH - sub2._centerNode._WslashH);
	//distance of geometry width&height of boundingbox
	dist += (abs(boundingRect(sub1._centerNode._contour).width - boundingRect(sub2._centerNode._contour).width)
			+abs(boundingRect(sub1._centerNode._contour).height - boundingRect(sub2._centerNode._contour).height))/2;
#endif

#if 1
	if(num1 != num2){
		std::cout<<"PosDistOf2Subgraph(): 2 subgraph does not have same size"<<endl;
		return INF;
	}
	for(int i=0;i<sub1._edges.size();i++){
		dist += disOfTwoNodes(sub1._edges[i]._node2,sub2._edges[i]._node2);
	}
	dist += disOfTwoNodes(sub1._centerNode,sub2._centerNode);
#endif
	return dist;
}