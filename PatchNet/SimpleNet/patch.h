#ifndef PATCH_H_
#define PATCH_H_

namespace patch{

	class Patch
	{
	public:
		//Patch(int _x =0, int _y =0, int _patch_w = 7, int _img_idx = 0, int _region_idx =0);
		Patch();
		Patch(const Mat& img3f, int _x, int _y, int _patch_w = 7, float weight_grad_ = 1.0);
		Patch(const Patch& rhs);
		Patch& operator=(const Patch& rhs) ;
		virtual ~Patch();
		inline const int x()const{ return x_;}
		inline const int y()const{ return y_;}
		inline const int patch_w()const {return patch_w_;}
		inline const float* feature()const{ return feature_;}
// 		inline const int region_idx()const{ return region_idx_;}
// 		inline const int img_idx()const { return img_idx_;}
		inline const int feature_len()const{ return feature_len_;}
		void set_feature_len(int len){feature_len_ = len;}
		void set_x(int _x){ x_ = _x;}
		void set_y(int _y){ y_ = _y;}
		void set_patch_w(int w){ patch_w_ = w;}
		void set_weight_grad(float w){weight_grad_ = w;}
// 		void set_img_idx(int idx){img_idx_ = idx;}
// 		void set_region_idx(int idx){region_idx_ = idx;}

		void calcFeature();
		void calcGradientBins( int step);
		void init(const Mat& img3f, int _x, int _y, int _patch_w = 7, float weight_grad_ = 1.0);
		float dist(const Patch & rhs);

	private:
		int patch_w_;
		int x_,y_;
		float *feature_;
		int feature_len_;
		Mat patch_;
		float weight_grad_;
// 		int region_idx_;
// 		int img_idx_;
	};


}//namespace patch
#endif