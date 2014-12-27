#pragma once

#ifndef INF_EDGE
#define INF_EDGE 1000000
#endif

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
//namespace DP{
	class DP
	{
	public:
		int _matrix[100][100];
		int _matrix_min[100][100];
		int _matrix_row[100][100]; //row of all
		int _matrix_col[100][100]; //row of all

		int _cols;//number of column or row
		int _rows;;

		void initilize();
		void printMatrix(int rows,int cols);
		int MinMatrix(int i,int j);
		int MaxMatrix(int i,int j);
		void dp();
	public:



		DP(void);
		~DP(void);
	};
//}

