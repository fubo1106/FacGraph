#include "stdafx2.h"
#include "patch.h"
#include "patch_pool.h"


void patch::PatchPool::addPatch( const Patch& rhs)
{
	pool.push_back(rhs);
}

patch::PatchPool::~PatchPool()
{
	pool.clear();
}

patch::PatchPool::PatchPool()
{

}
