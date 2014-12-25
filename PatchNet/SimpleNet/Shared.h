#pragma once
#include "stdafx2.h"
#define FLIP_VERTI 1
#define FLIP_HORIZ 2
#define FLIP_NONE 0
namespace fs = boost::filesystem;
struct sPatch{
	cv::Point center;
	int width; 
	cv::Vec3b aveClr;
	uchar aveGrad;
};

namespace Shared{
	float myArcTan(float x1, float y1, float x2, float y2);
	void drawPoly(cv::Mat& source, vector<cv::Point>& points, cv::Scalar& clr, int thickness, bool fill = false);
	//source created outside
	void getGradMap(cv::Mat& inImg8U3C, cv::Mat& outMap8U, cv::Mat& outAtan32F);
	void drawPatch(sPatch& sp, cv::Mat& img, string& name);
	void drawPatchOnImg(sPatch& sp, cv::Mat& img, cv::Scalar clr);
	void drawPatchFontOnImg(sPatch& sp, string& ID, cv::Mat& img, cv::Scalar clr);
	void myXor(cv::Mat& src1, cv::Mat& src2, cv::Mat& dst); //sub 255 element in src2 from src1, save in dst
	void myAnd(cv::Mat& src1_32F, cv::Mat& src2_32F, cv::Mat& dst_32F, float thre1, float thre2); //if not 0, then as true
	float distForTwoPat(cv::Mat& pat1, cv::Mat& pat2, int& flipType);
	float distForTwoPat(cv::Mat& pat1, cv::Mat& pat2, cv::Vec3b& aveClr1,
		                        cv::Vec3b& aveClr2, uchar aveGra1, uchar aveGra2, int& flipType);
	void showMat(cv::Mat& relMat);
	// 	const vector<string>& scanFilesUseRecursive(const string& rootPath,vector<string>& container=*(new vector<string>())){
// 
// 		fs::path fullpath (rootPath, fs::native);
// 		vector<string> &ret = container;
// 
// 		if(!fs::exists(fullpath)){return ret;}
// 		fs::recursive_directory_iterator end_iter;
// 		for(fs::recursive_directory_iterator iter(fullpath);iter!=end_iter;iter++){
// 			try{
// 				if (fs::is_directory( *iter ) ){
// 					std::cout<<*iter << "is dir" << std::endl;
// 					ret.push_back(iter->path().string());
// 					//ScanAllFiles::scanFiles(iter->path().string(),ret);
// 				}else{
// 					ret.push_back(iter->path().string());
// 					std::cout << *iter << " is a file" << std::endl;
// 				}
// 			} catch ( const std::exception & ex ){
// 				std::cerr << ex.what() << std::endl;
// 				continue;
// 			}
// 		}
// 		return ret;
// 	}
}