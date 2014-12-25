#pragma once
#ifndef CM_DEFINITION_H
#define  CM_DEFINITION_H


#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

extern Point const DIRECTION4[4];
extern Point const DIRECTION8[8];
extern Point const DIRECTION16[16];
extern float const DRT_ANGLE[8];
extern float const PI_FLOAT;
extern float const PI2;
extern float const PI_HALF;
extern double const SQRT2;

typedef vector<Point2d> PointSetd;
typedef vector<Point2i> PointSeti;
typedef vector<string> vecS;
typedef vector<int> vecI;
typedef vector<float> vecF;
typedef vector<double> vecD;
typedef pair<double, int> CostIdx;
typedef pair<float, int> CostfIdx;


/************************************************************************/
/* Define Macros & Global functions                                     */
/************************************************************************/
const double EPS = 1e-8;		// Epsilon (zero value)
const double INFINITY = 1e20;
#define _S(str) ((str).c_str())

#define CHECK_IND(c, r) (c >= 0 && c < m_w && r >= 0 && r < m_h)
#define CHK_IND(p) ((p).x >= 0 && (p).x < m_w && (p).y >= 0 && (p).y < m_h)

#define ForPointsD(pnt, pntS, pntE, d) 	for (Point pnt = (pntS); pnt.y != (pntE).y; pnt.y += (d)) for (pnt.x = (pntS).x; pnt.x != (pntE).x; pnt.x += (d))
#define ForPoints(pnt, pntS, pntE) 		for (Point pnt = (pntS); pnt.y != (pntE).y; pnt.y++) for (pnt.x = (pntS).x; pnt.x != (pntE).x; pnt.x++)
#define ForPoints2(pnt, xS, yS, xE, yE)	for (Point pnt(0, (yS)); pnt.y != (yE); pnt.y++) for (pnt.x = (xS); pnt.x != (xE); pnt.x++)

/************************************************************************/
/* Useful template                                                      */
/************************************************************************/

template<typename T> inline T sqr(T x) { return x * x; } // out of range risk for T = byte, ...
template<typename T> inline int CmSgn(T number) {if(abs(number) < EPS) return 0; return number > 0 ? 1 : -1; }
template<class T> inline T pntSqrDist(const Point_<T> &p1, const Point_<T> &p2) {return sqr(p1.x - p2.x) + sqr(p1.y - p2.y);} // out of range risk for T = byte, ...
template<class T> inline T pntDist(const Point_<T> &p1, const Point_<T> &p2) {return sqrt(pntSqrDist(p1, p2));} // out of range risk for T = byte, ...
template<class T> inline T vecSqrDist3(const Vec<T, 3> &v1, const Vec<T, 3> &v2) {return sqr(v1[0] - v2[0])+sqr(v1[1] - v2[1])+sqr(v1[2] - v2[2]);} // out of range risk for T = byte, ...
template<class T> inline T vecDist3(const Vec<T, 3> &v1, const Vec<T, 3> &v2) {return sqrt(vecSqrDist3(v1, v2));} // out of range risk for T = byte, ...
template<class T1, class T2> inline void operator /= (Vec<T1, 3> &v1, const T2 v2) { v1[0] /= v2; v1[1] /= v2; v1[2] /= v2; } // out of range risk for T = byte, ...

inline Point round(const Point2d &p) { return Point(cvRound(p.x), cvRound(p.y));}


template<typename T> vector<T> operator +(const vector<T>& v1, const vector<T> &v2)
{
	vector<T> result(v1);
	for (size_t i = 0, iEnd = v1.size(); i != iEnd; i++)
		result[i] = v1[i] + v2[i];
	return result;
}

template<typename T> vector<T> operator -(const vector<T>& v1, const vector<T> &v2)
{
	vector<T> result(v1);
	for (size_t i = 0, iEnd = v1.size(); i != iEnd; i++)
		result[i] = v1[i] - v2[i];
	return result;
}

template<typename T> vector<T> operator *(const vector<T>& v1, const vector<T> &v2)
{
	vector<T> result(v1);
	for (size_t i = 0, iEnd = v1.size(); i != iEnd; i++)
		result[i] = v1[i] * v2[i];
	return result;
}

template<typename T> vector<T> operator /(const vector<T>& v1, const vector<T> &v2)
{
	vector<T> result(v1);
	for (size_t i = 0, iEnd = v1.size(); i != iEnd; i++)
		result[i] = v1[i] * v2[i];
	return result;
}

template<class T> void maxSize(const vector<Point_<T>> &pnts, T &minS, T &maxS)
{
	CV_Assert(pnts.size() > 0);
	minS = maxS = pnts[0].x;
	for (int i = 0, iEnd = (int)pnts.size(); i < iEnd; i++)
	{
		minS = min(minS, pnts[i].x);
		minS = min(minS, pnts[i].y);
		maxS = max(maxS, pnts[i].x);
		maxS = max(maxS, pnts[i].y);
	}
}

#endif