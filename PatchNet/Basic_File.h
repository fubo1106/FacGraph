#pragma once

#include "Fac_Include.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#ifndef PI
#define PI 3.1416
#endif

using namespace std;

class Basic_File
{
public:

	int LoadData(string fileName, cv::Mat& matData, int matRows = 0, int matCols = 0);
	int WriteData(string fileName, cv::Mat& matData);

	//void showFacNode(FacNode& node, Mat& src, Scalar scl);

	void readMat2LabelFile(Mat& src, string file);

	Basic_File(void);
	~Basic_File(void);
};


