#include "stdafx2.h"
#include "patch.h"
#include "image_kit.h"
patch::Patch::Patch( const Mat& img3f, int _x, int _y ,int _patch_w, float _weight_grad):x_(_x),y_(_y),patch_w_(_patch_w),weight_grad_(_weight_grad)
{
	Rect roi = Rect(_x,_y,_patch_w,_patch_w);
	patch_ = img3f(roi).clone();
	calcFeature();
	//calcGradientBins(4);
}

patch::Patch::Patch( const Patch& rhs)
{
	 this->x_ = rhs.x();
	 this->y_ = rhs.y();
	 this->patch_w_ = rhs.patch_w();

	 this->feature_len_ = rhs.feature_len();
	 this->feature_ = new float[feature_len_];
	 memcpy(feature_,rhs.feature_,sizeof(float)*feature_len_);
}

patch::Patch::Patch()
{
	feature_ = NULL;
}



patch::Patch::~Patch()
{
	  if(feature_!=NULL)
	  {
		  delete []feature_;
		  feature_ = NULL;
	  }
}

void patch::Patch::calcFeature()
{
   
   set_feature_len(patch_w_*patch_w_*3 + 8);
   feature_ = new float[feature_len_];
   fill_n(feature_, feature_len_ , 0);
   //grad
   Mat u, v, mag, ori;
   image_kit::ik_grad(patch_, u, v, mag, ori);
   float per_angle = CV_PI / 4.0;
   float mag_total = cv::sum(mag)[0];
//    for(int y = 0; y< mag.rows; ++y )
//    {
// 	   for( int x =0; x< mag.cols; ++x)
// 	   {
// 		   mag_total += mag.at<float>(y,x);
// 	   }
//    }
   //SHOW_IMG(mag);
  // cout<<mag_total<<endl;
   // color
   for(int y=0; y< patch_w_; ++y)
   {
	   Vec3f* patch_ptr = patch_.ptr<Vec3f>(y);
	   float* mag_ptr = mag.ptr<float>(y);
	   float* ori_ptr = ori.ptr<float>(y);
	   for( int x=0; x < patch_w_; ++x)
	   {
		   for(int i=0; i<3; ++i)
			   feature_ [y * patch_w_ + x + i] = patch_ptr[x][i];

		   int grad_bin = (int)((ori_ptr[x] + CV_PI) / per_angle);
		   if( grad_bin < 0 || grad_bin >7)
		   {
				continue;
		   }//8 bins
		   feature_[ patch_w_*patch_w_*3 + grad_bin ] += weight_grad_*mag_ptr[x]/mag_total; 
	   }
   }

   return ;
}

patch::Patch& patch::Patch::operator=( const Patch& rhs )
{
	this->x_ = rhs.x();
	this->y_ = rhs.y();
	this->patch_w_ = rhs.patch_w();

	this->feature_len_ = rhs.feature_len();
	if(this->feature_len_!=NULL)
	{
		delete[]feature_;
		this->feature_ = new float[feature_len_];
		memcpy(feature_,rhs.feature_,sizeof(float)*feature_len_);
	}
	return *this;
}

void patch::Patch::init( const Mat& img3f, int _x, int _y, int _patch_w /*= 7*/, 
	float _weight_grad /*= 1.0*/ )
{
	x_=_x,y_=_y,patch_w_=_patch_w,weight_grad_=_weight_grad;
	Rect roi = Rect(_x,_y,_patch_w,_patch_w);
	patch_ = img3f(roi).clone();

	calcFeature();
	//calcGradientBins(4);
}

float patch::Patch::dist( const Patch & rhs )
{
	float tol = 0.f;
// 	for(int i=0; i<feature_len_ ; ++i)
// 	{
// 		if(i+8<feature_len_)
// 			tol += sqr(feature_[i] - rhs.feature_[i]);
// 		else tol+= weight_grad_*sqr(feature_[i]-rhs.feature_[i]);
// 	}
	for(int i=0; i<feature_len_ ; ++i)
	{
		if(i+8<feature_len_)
			tol += sqr(feature_[i] - rhs.feature_[i]);
		else {
			tol+= weight_grad_*sqr(feature_[i]-rhs.feature_[i]);
		}
	}
	return sqrt(tol);
}



void patch::Patch::calcGradientBins( int sub_patch_per_dim )
{ 
	set_feature_len(sub_patch_per_dim * sub_patch_per_dim * 8);
	feature_ = new float[feature_len_];
	fill_n(feature_, feature_len_ , 0);
	//grad
	Mat u, v, mag, ori;
	image_kit::ik_grad(patch_, u, v, mag, ori);
	float per_angle = CV_PI / 4.0;
	float mag_total = 1.f;/*cv::sum(mag)[0]*/;
// 	for(int y = 0; y< mag.rows; ++y )
// 	{
// 		for( int x =0; x< mag.cols; ++x)
// 		{
// 			mag_total += mag.at<float>(y,x);
// 		}
// 	}
	//cout<<"mag_total"<<endl;
	int sub_step = patch_w_ / sub_patch_per_dim;
	int sub_patch_idx = 0;
	for(int suby =0; suby < patch_w_; suby+= sub_step)
	{
		for(int subx =0; subx< patch_w_ ; subx+= sub_step)
		{
			Rect roi = Rect(subx,suby,sub_step,sub_step);
			Mat sub_patch = patch_(roi);
			for(int y=0; y< sub_step; ++y)
			{
				Vec3f* patch_ptr = sub_patch.ptr<Vec3f>(y);
				float* mag_ptr = mag.ptr<float>(y);
				float* ori_ptr = ori.ptr<float>(y);
				for( int x=0; x < sub_step; ++x)
				{
					int grad_bin = (int)((ori_ptr[x] + CV_PI) / per_angle);
					if( grad_bin < 0 || grad_bin >7)
					{
						continue;
					}//8 bins
					feature_[sub_patch_idx * 8 + grad_bin ] += mag_ptr[x]/mag_total; 
				}
			}
			sub_patch_idx++;
		}
	}
	
}
