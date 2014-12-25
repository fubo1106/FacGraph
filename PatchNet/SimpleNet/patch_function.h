#ifndef PATCH_FUNCTION_H_
#define PATCH_FUNCITON_H_
#include "patch.h"
#include "patch_pool.h"
namespace patch{
		 
	void selectPatch(Mat img, Mat img_idx, patch::PatchPool &pool);

}
#endif