#pragma warning(disable : 4996)
#include "stdafx2.h"
#include "union_find_set.h"


void UnionFindSet::init( int n )
{
	 ori_num_ = n_	= n;
	 root_ = new int[n];
	 rank_ = new int[n];
	 fill_n(rank_,n,0);
	 for(int i=0; i< n; ++i)
		 root_[i] = i;
}

int UnionFindSet::find( int x )
{
	int y = x;  
	while (root_[y] != y) {  
		y = root_[y];      
	}  

	int temp, head = y;  
	y = x;  
	while (root_[y] != y) {  
		temp = root_[y];  
		root_[y] = head;  
		y = temp;     
	}  

	return head;  
}

void UnionFindSet::uni( int x, int y )
{
	int f1 = find(x);  
	int f2 = find(y);  

	if (rank_[f1] <= rank_[f2]) {  
		root_[f1] = f2;  
		if (rank_[f1] == rank_[f2]) {  
			rank_[f2] ++;      
		}     
	} else {  
		root_[f2] = f1;  
	}  
	n_--;
}

UnionFindSet::~UnionFindSet()
{
	if(root_ != NULL)
		delete [] root_;
	if(rank_ != NULL)
	delete [] rank_;
	cout << "decompose unionfindset" << endl;
}



