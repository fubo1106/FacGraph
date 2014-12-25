#ifndef PATCH_GROUP_H
#define PATCH_GROUP_H

#include "patch_pool.h"
namespace patch
{

	class PatchGroup :public PatchPool
	{
	public:
		PatchGroup();
		virtual ~PatchGroup();
	private:
		int cluster_label_;
		float *cluster_feature_;
		int cluster_feature_len_;
	};
}


#endif