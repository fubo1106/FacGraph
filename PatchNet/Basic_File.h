#pragma once

#include "Fac_Include.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

using namespace std;

class Basic_File
{
public:

	int LoadData(string fileName, cv::Mat& matData, int matRows = 0, int matCols = 0);
	int WriteData(string fileName, cv::Mat& matData);

	//void showFacNode(FacNode& node, Mat& src, Scalar scl);

	Basic_File(void);
	~Basic_File(void);
};

