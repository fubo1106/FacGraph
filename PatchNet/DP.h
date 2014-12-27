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
	int _matrix[1000][1000];
	int _matrix_min[1000][1000];
	int _matrix_row[1000][1000]; //row of all
	int _matrix_col[1000][1000]; //row of all

	int _cols;//number of column or row
	int _rows;;

	int _mindistance;
	int _maxdiatance;
	vector<pair<int,int>> _path;

	void initilize();
	void printMatrix(int rows,int cols);
	int MinMatrix(int i,int j);
	int MaxMatrix(int i,int j);
	void performDP();

public:
	DP(int** distance,int rows,int cols);
	DP(void);
	~DP(void);
};
//}

