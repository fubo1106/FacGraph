#ifndef _BASIC_H
#define _BASIC_H
#endif

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "Fac_Include.h"
#include "Basic_File.h"
using namespace std;

//
//string num2str(double i)
//{
//        stringstream ss;
//        ss<<i;
//        return ss.str();
//}
//
//int str2num(string s)
// {   
//        int num;
//        stringstream ss(s);
//        ss>>num;
//        return num;
//}

//variables definition
//for Mean shift
//end for meanshift

cv::Point calcCenterPoint(vector<cv::Point>& points);

int calcAvgGray(vector<cv::Point>& contour,cv:: Mat& grayImg);

cv::Vec3b calcAvgColor(vector<cv::Point>& contour, cv::Mat& coolorImg);

cv::Scalar randColor();

double distanceP2P(const cv::Point& p1,const cv::Point& p2);

double calcAngleOf2Vec(cv::Point& vec1,cv::Point& vec2);

cv::Point getGravityCenter(vector<cv::Point>& contour);

void ImageSegmentByKMeans2(cv::Mat& src,cv::Mat& dst,int nClusters, int sortFlag);

void maskProcess(Mat& src,Mat& mask);

void ImageSefmentByMeanshift(cv::Mat& src,cv::Mat& dst);