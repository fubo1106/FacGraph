#pragma once
#include "stdafx2.h"
#include "LabelName.h"
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Not Use!!!
using namespace std;
#define REL_WIDTH 2

extern "C" {

//#include "kmeans.h"
}
//#include "patch.h"

struct PatchNetNode
{
	LabelType label;
	vector<cv::Mat> sumPatches;
};

class LocalNet
{
public:

	LocalNet(void);
	~LocalNet(void);
	
	int sumPatchWidth;
	vector<PatchNetNode> nodes;
	vector<vector<cv::Mat>> relationMats;
	vector<vector<float>> relMaxvalues;
	cv::Mat labelMap;
	cv::Mat srcImg;

	int getLabelNum(Vec3b& label);

	void Load(string& labelName, string& srcName);

	float drawMats(cv::Mat& relMat, string& saveName);

	//get information from a hole on this image, get the holes context
	vector<cv::Mat> relMatsTohole;
	vector<float> holeRelMaxvalues;
	void fromHole(string& holeMaskName);

	void loadSumPatch(int x, int y);
};

