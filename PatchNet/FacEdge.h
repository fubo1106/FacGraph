#pragma once
#include "FacNode.h"

class FacEdge
{
public:
	FacNode _node1;
	FacNode _node2;
	Point _rel_direction;
public:

	void drawEdge(FacEdge& edge, Mat& src, Scalar scl);

	FacEdge(void);
	FacEdge(FacNode& n1, FacNode& n2);
	~FacEdge(void);
};

