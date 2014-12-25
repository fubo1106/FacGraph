#ifndef IMAGE_KIT_H_
#define IMAGE_KIT_H_

#include "CmInclude.h"

//namespace image_kit
// in this namespace, all the interfaces will be named as prefix "ik_" or "IK_"
// standing for "Image Kit"
namespace image_kit{

	//Interpolate method
	//nearest or bilinear
	enum IK_METHOD_INTERP
	{
		IK_NEAREST=0,
		IK_BILINEAR
	};
	
	//Bilinear interpolate a point from an image32FC1
	//PARAM INPUT
	//	@ img32FC1	:	image with float type
	//  @ x		:   x-coordinate
	//	@ y		:	y-coordinate
	//PARAM OUTPUT NULL 
	//RETURN
	//	float	-	the interpolation value
	inline float ik_bilinearInterp(const Mat & img32FC1, float x, float y)
	{
		int x1 = int(x), x2 = x1+1, y1=int(y), y2=y1+1;
		float fx1y1 = img32FC1.at<float>(y1,x1);
		float fx1y2 = img32FC1.at<float>(y2,x1);
		float fx1 = fx1y1 + (fx1y2-fx1y1)*(y-y1)/(y2-y1);

		float fx2y1 = img32FC1.at<float>(y1,x2);
		float fx2y2 = img32FC1.at<float>(y2,x2);
		float fx2 = fx2y1 + (fx2y2-fx2y1)*(y-y1)/(y2-y1);

		return fx1 + (fx2-fx1)*(x-x1)/(x2-x1);
		
	}

	//Rotate Point(x,y) by angle theta
	//PARAM	 INPUT
	//	@ x		:		input x-coordinate
	//	@ y		:		input y-coordinate
	//	@ theta	:		rotate angle  theta [0,2*PI)
	//PARAM  OUTPUT
	//	@ ox	:		output x-coordinate after rotation
	//	@ oy	:		output y-coordinate after rotation
	//RETURN VOID
	inline void ik_rotate_coordinate(	int x, 
										int y,
										double theta,
										OUT int &ox, 
										OUT int &oy
									)
	{
		 ox = (int)(x*cos(theta)-y*sin(theta)+0.5);
		 oy = (int)(x*sin(theta)+y*cos(theta)+0.5);
	}

	//Rotate image at (0,0) by angle theta ANTI-CLOCKWISELY, if want to rotate 
	//CLOCKWISELY, the theta should be negative	 the input type MUST be float
	//PARAM  INPUT
	//	@ img_in32F		:		input image	float
	//PARAM  OUTPUT
	//	@ theta			:		rotate angle  theta [0,2*PI)
	//  @ bComplete		:		show image completely or partly 
	//	@ img_out32F	:		output image float
	//RETURN VOID
	void ik_rotate(		const Mat& img_in32F, 
						double theta, 
						bool bComplete=true, 
						IK_METHOD_INTERP method = IK_BILINEAR,
						OUT Mat& img_out32F=Mat()
				);

	//Gradient of image32FC3(Forward u(x,y) = I(x+1,y)-I(x,y) v(x,y) = I(x,y+1)-I(x,y))
	//PARAM  INPUT
	//	@ img_in32FC3		:		input image	float 3 channel
	//PARAM  OUTPUT
	//	@ u					:		x direction difference
	//  @ v					:		y direction difference
	//	@ mag				:		magnitude
	//  @ ori				:       orientation
	//RETURN VOID
	void ik_grad(	const Mat& img_in32FC3, 
					OUT Mat& u, 
					OUT Mat &v,
					OUT Mat & mag=Mat(), 
					OUT Mat &ori=Mat()
				);


	//Divergence  D(x,y) = I.u(x,y)-I.u(x-1,y) + I.v(x,y)-I.v(x,y-1)
	//PARAM  INPUT
	//	@ img_in32FC3		:		input image	float 3 channel
	//PARAM  OUTPUT
	//	@ div				:		divergence
	//RETURN VOID
	void ik_div(const Mat& img_in32FC3, OUT Mat& div);

	//resize by fixed longer size with ratio unchanged
	void ik_resizeFixedLong(const Mat & img_in, Mat & img_out, double length, int method =1);

	void ik_resizeFixedShort(const Mat & img_in, Mat & img_out, double width, int method =1);


}	  //end namespace image_kit
#endif