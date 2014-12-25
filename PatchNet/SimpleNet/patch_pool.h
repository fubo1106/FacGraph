#ifndef PATCH_POOL_H
#define PATCH_POOL_H


namespace patch
{
	class PatchPool
	{
	public:
		PatchPool();
		~PatchPool();
		void addPatch(const Patch&);
		inline const Patch& getPatch(int idx)const
		{
			assert(idx<pool.size());
			return pool[idx];
		}
		inline Patch& getPatch(int idx)
		{
			assert(idx<pool.size());
			return pool[idx];
		}
		inline const Patch& operator[](int idx)const
		{
			assert(idx<pool.size());
			return pool[idx];
		}
		inline Patch& operator[](int idx)
		{
			assert(idx<pool.size());
			return pool[idx];
		}
	private:
		vector<Patch> pool;
		
	};


}




#endif