#pragma once
#include "stdafx2.h"
#include "assert.h"
class UnionFindSet{
	
public:
	UnionFindSet():ori_num_(0),n_(0),root_(NULL),rank_(NULL){}
	UnionFindSet(int n){ init(n);}
	~UnionFindSet();
	void init(int n);
	int find( int x);
	void uni( int x, int y);
	inline int n()const{return n_;}
	void setRank(int idx,int val){
		if(idx >= n_)
			cout << "n_: " << n_ << "--idx:" << idx << endl;
		else
		/* assert(idx<n_);*/rank_[idx] = val;}


private:
	int ori_num_;
	int n_;
	int *root_;
	int *rank_;
};