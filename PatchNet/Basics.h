#ifndef _BASIC_H
#define _BASIC_H
#endif

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "Fac_Include.h"
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

Point calcCenterPoint(vector<Point>& points);

int calcAvgGray(vector<Point>& contour, Mat& grayImg);
