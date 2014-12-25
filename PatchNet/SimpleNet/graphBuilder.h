#pragma once
#include "stdafx2.h"
#include "LabelName.h"

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Not Use!!!
using namespace std;
extern "C" {

//#include "kmeans.h"
}
//#include "patch.h"

class graphBuilder
{
public:
	int fixed_ClassNum;
	int patch_width;
	vector<vector<cv::Mat>> relationMat;
	vector<vector<int>> relationStrength;

public:
	graphBuilder(int classNum, int pw);
	graphBuilder(void);
	~graphBuilder(void);

    void collectDirs(cv::Mat& img, int* labels);
	int drawDir(cv::Mat& relMat, string& saveName);

};

