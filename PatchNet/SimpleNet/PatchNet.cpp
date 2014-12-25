// PatchNet.cpp : Defines the entry point for the console application.
//
#include "stdafx2.h"
#include "patch.h"
#include "patch_pool.h"
#include "image_kit.h"

int patch_w = 10;
void onMouse(int mouseEvent,int x,int y,int flags,void* param);

struct patch_show{
	Mat imgA;
	Rect roi;
	Point center;
	bool left_button_down ;

};

patch::Patch global_patch;
vector<patch::Patch> vector_patch;
const int gPatch_w = 10;

bool cmp(const patch::Patch& A, const patch::Patch& B)
{
	float ans1 = global_patch.dist(A);
	float ans2 = global_patch.dist(B);
	return ans1< ans2;
}

void gFoo(const Mat &imgB)
{
	   vector_patch.clear();
	   for(int y = 0; y + gPatch_w < imgB.rows; y+=1)
	   {
		   for(int x = 0; x + gPatch_w < imgB.cols; x+=1)
		   {
				patch::Patch patch_B;
				patch_B.init(imgB, x, y, gPatch_w);
				vector_patch.push_back(patch_B);
				cout<<global_patch.dist(patch_B)<<endl;
		   }
	   }
	   sort(vector_patch.begin(),vector_patch.end(),cmp);
}
										   
/*
int _tmainxxx(int argc, _TCHAR* argv[])
{
	//cout<<atan2(10.,0.)<<endl;
	const string input_str = "E:\\WANGMIAO\\crawer_test\\palace\\";
	const string output_str = "E:\\WANGMIAO\\crawer_test\\palaceSeg\\";
	//Mat input_img =  imread("E:\\WANGMIAO\\crawer_test\\Yosemite\\2Yosemite.jpg");
	//input_img.convertTo(input_img,CV_32F,1/255.0);
// 	CAllSegs s;
// 	s.InitSegs(input_img, s.MS_MODE);
// 	s.RunSeg();
// 	s.GetSegs();
// 	imshow("a",s.segs);
// 	waitKey();
	//SegmentImageDir(input_str, output_str);
  	//patch::PatchPool patch_pool;

	const string imgA_path = "E:\\WANGMIAO\\crawer_test\\palace\\imageA.jpg";
	const string imgB_path = "E:\\WANGMIAO\\crawer_test\\palace\\imageB.jpg";

	Mat imgA = imread(imgA_path);
	Mat imgB = imread(imgB_path);
	imgA.convertTo(imgA,CV_32F,1.0/255.0);
	imgB.convertTo(imgB,CV_32F,1.0/255.0);

	image_kit::ik_resizeFixedLong(imgA,imgA,256);
	image_kit::ik_resizeFixedShort(imgB,imgB,256);
	


	patch_show patchA;
	patchA.imgA = imgA.clone();
	patchA.left_button_down = false;
	patchA.center =Point(0,0);

	namedWindow("imgA");
	namedWindow("imgB");
	setMouseCallback("imgA",onMouse, &patchA);
	

	while(waitKey(100)!=27)
	{
	   imshow("imgA", imgA);
	   if(patchA.left_button_down == true)
			 break;
	}

	global_patch.init(imgA, patchA.center.x, patchA.center.y, gPatch_w);
	const float* fea = global_patch.feature();
	for(int i=0;i<global_patch.feature_len();++i)
	{
		cout<< fea[i]<<"	";
	}

	line(imgA, patchA.center, Point(patchA.center.x ,patchA.center.y + gPatch_w), Scalar(0,0,255));
	line(imgA, patchA.center, Point(patchA.center.x + gPatch_w , patchA.center.y),Scalar(0,0,255));
	line(imgA, Point(patchA.center.x+gPatch_w ,patchA.center.y + gPatch_w),Point(patchA.center.x ,patchA.center.y + gPatch_w),Scalar(0,0,255));
	line(imgA, Point(patchA.center.x+gPatch_w ,patchA.center.y + gPatch_w),Point(patchA.center.x+gPatch_w ,patchA.center.y),Scalar(0,0,255));
	
	imshow("imgA",imgA);
	waitKey();



	gFoo(imgB);
	for(int i = 0; i<200 ; ++i )
	{
		Point pt = Point(vector_patch[i].x(), vector_patch[i].y());
		line(imgB, pt, Point(pt.x,pt.y + gPatch_w), Scalar(0,0,255));
		line(imgB, pt, Point(pt.x + gPatch_w , pt.y),Scalar(0,0,255));
		line(imgB, Point(pt.x+gPatch_w ,pt.y + gPatch_w),Point(pt.x ,pt.y + gPatch_w),Scalar(0,0,255));
		line(imgB, Point(pt.x+gPatch_w ,pt.y + gPatch_w),Point(pt.x+gPatch_w ,pt.y),Scalar(0,0,255));
	}
	imshow("imgB",imgB);
	waitKey();



	return 0;
}*/

void onMouse( int mouseEvent,int x,int y,int flags,void* param )
{
	patch_show *show = (patch_show *)param;  
	switch(mouseEvent)  
	{  
	case CV_EVENT_LBUTTONDOWN:  
		show->center = Point(x,y);  
		show->left_button_down = true;  
		break; 
	}  
	return;  
}

