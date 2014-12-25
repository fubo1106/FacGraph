#pragma once
#include "stdafx2.h"
#include "SingleNet.h"

typedef struct  
{
	int regionID;
	int regionLevel;
	float matchValue;
	bool isCompo;
	float degree;
}ValidRegionInf;

typedef struct  
{
	float dist;
	int ID;
	bool valid; //Record whether the current is valid
	float coverN;
} Dist_ID;

typedef struct  
{
	string _secN;
	vector<sPatch> _sps;
	vector<vector<float>> _dists; // for each relative patch, a list for similarity 
	
	vector<vector<int>> _simPatchIDs;

	vector<Dist_ID> _bestPatDis; //for each user rel patch, have one best matching patch
	int regionID;
	int regionLevel;
	float matchValue;
	bool isCompo;
	float degree;
	bool isValid;
	vector<ValidRegionInf> validComRegions;
	vector<ValidRegionInf> validBaseRegions;

	//my code
	vector<vector<vector<Point>>> matchedRegions;
	
} SecInf;



class NetExplorer
{
public:
	string _ne_section;
	string _ne_outdir;//src outdir
	string _reg_outdir;//region outdir --mine
	string _ne_new_outdir;
	bool _ifUserOnlyBase;
	float _userRelNumThr; 
	float _userRelNumThr2; 
	float _userdegThr;
	float _patchSimThres;
	SingleNet _userNet;
	SingleNet _regionNet;//mine
	string _resSaveDirS;
	vector<SecInf> _infsWithSecs;
	vector<cv::Mat> _userRelPatches;
	vector<sPatch> _userRelsPatches;
	vector<cv::Mat> _userValidRels;
	
	/*vector<vector<SecInf>> _
	vector<*/

	float _thresForMinDist;
public:
	void getUserNet(const char* userMaskN);

	void compNodeInFolder(string& resSaveDirS);
	
	void compNodeInFolder_file(string& resSaveDirS);

	void compNodeInFolder_file_mine(string& resSaveDirS,vector<SecInf>& matchedSecs);//...mine

	void oneCompPatRes(string& sectionS, string& outdirS, string& sourcePath);

	void oneFindRegionRes(string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath);

	void oneFindRegRes_file(string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath);

	void oneFindRegRes_file(SecInf& inf, string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath);

	void oneFindRegRes_file_mine(SecInf& inf, string& sectionS, string& outdirS, string& outDirR, string& resSaveDirS, string& sourcePath);//...mine

	void getRegionFromMap(cv::Mat& outside_Palatte, cv::Mat& Map, int id, vector<vector<cv::Point>>& out_Regions);

	bool isCntxCovered(cv::Mat& cntx1, cv::Mat& cntx2, float& coverN); //1 is covered by 2

	bool NetExplorer::isCntxCovered_f_b( cv::Mat& cntx1, cv::Mat& cntx2, float& coverScore );
	
	void getCandidateRegion(SecInf& inf, SingleNet& net1, vector<vector<Dist_ID>>& tDist_IDs);
	
//	void getCandidateRegion_file(SecInf& inf, SingleNet& net1,  vector<Dist_ID>& MatchP_IDs);


	void drawResForUI(SecInf& inf, SingleNet& net1, string& sectionS);
	NetExplorer(void);
	NetExplorer(Summrarizer* user_summer); //Initialize it to explore the net library by user mask
	NetExplorer(Summrarizer* user_summer,Summrarizer* region_summer);
	~NetExplorer(void); 

	
};
