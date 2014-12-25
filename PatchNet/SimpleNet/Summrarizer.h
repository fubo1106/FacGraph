#pragma once
#include "stdafx2.h"
#include "Shared.h"

typedef vector<vector<Point>> CContour;

struct BoundGrad
{
	vector<float> rads;
	vector<int> grads;
	int tanMagHist[2][8];
};


class Summrarizer
{
public:

	double _paraTol;

	Summrarizer(void);
	~Summrarizer(void);
	string _sm_sec;
	string _sm_out;
	string secName;
	vector<sPatch> _summaryPatches;
	vector<vector<Point>> _loc_per_patch;
	cv::Mat _overallMask;
	vector<cv::Mat> _spMasks;
	vector<cv::Mat> _centersMasks;
	vector<cv::Mat> _similaritys;
	vector<CContour> _allOverContours;
	cv::Mat _relGroups; //size * size , float

	cv::Mat _imgGrad;
	cv::Mat _aveGrad;
	cv::Mat _nonBlured;
	cv::Mat _averImg;
	cv::Mat _inverseGrad;
	cv::Mat _thumbLab;
	int _width;
	int _height;
	int _gradStep;
	int _paraAveGradTol;
	

	vector<sPatch> _finalPatches;
	vector<cv::Mat> _finalRegions;

	cv::Rect insideRect;

	cv::Mat _srcImg;
	float _scaleToRSeedX, _scaleToRSeedY;
	int _patchRadius;
	void initialRandPick(string section, string outdir, cv::Mat& img, int patchRad);
	bool pickOnePatch(cv::Mat& in_ValidPosMat, sPatch& out_sPatch, cv::Mat& out_CoveredMask, cv::Mat& out_CenterMask, int ID, float curG = 3); //Randomly pick one patch, and get region it covered; 
	void pickNewSPatch(int ID);
	void refineOveredRegion(int ID);
	void refineBeforeCluster();
	void getSimMap(cv::Mat& src, sPatch& spat, cv::Mat& res);
	void getGradSimMap(cv::Mat& srcGrad, sPatch& spat, cv::Mat& res);
public:
	//MSER-----------------------------------
	cv::Point getCenter(vector<cv::Point> points);
	cv::Point getFarest(vector<cv::Point> points, cv::Point& c, cv::Point& dir);
	//dir means the farest point in what phase, - -, - +, + -, ++
	void getMSER(cv::Mat& img, cv::Mat& msk);

	void getBoundGrads(cv::Point dir, sPatch& inputPatch, cv::Mat& atanMap, 
		cv::Mat& gradMap, BoundGrad& out_BoundGrad); //dir indicates which bound it is, -1 0, +1 0, 0 -1, 0 +1;
	
	//END MSER---------------------------------------
	int compareHist(BoundGrad& b1, BoundGrad& b2);
	void getPatchFromImg(cv::Mat& src, cv::Mat& pat, sPatch& pt);
	void getOverlapRegion(cv::Mat& img, cv::Mat& msk);

	void RandPatchCluster();
	void RandPatchCluster( cv::Mat& img );
	double getTolerance(int src_x, int src_y, int tgt_x, int tgt_y, int curGrad); //get tolerance of the current pair
	double getToleranceTemp(int src_x, int src_y, int tgt_x, int tgt_y, int curGrad); //get tolerance of the current pair

};

