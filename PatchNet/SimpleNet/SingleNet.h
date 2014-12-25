#pragma once
#include "Summrarizer.h"

typedef struct Node{
	uchar _tempMark;

	bool _isBase;
	int _virtualID;  //the virtual ID corresponding to the ID in _summer, 
	                    //use virtual ID can get the summary patch
	int _nodeDepth; //0-Base node
	vector<cv::Point> _RegContour;
	vector<struct Node*> _pSons;
	vector<struct Node> _ComSons;
	int _father;
} RealNode;

class SingleNet
{
public:
	string _sn_sec;
	string _sn_out;
	string _sn_new_out;
	string _sn_src;
	int _imgW, _imgH;
	int _nodeNum;
	Summrarizer* _summer;
	vector<vector<cv::Mat>> _allRels; //Record the relation matrix 
	vector<vector<double>> _relDegree; //Record of the max value of the relation
	vector<vector<double>> _relSignif; // Record the normalized significant
	vector<RealNode> _allNodes;
	vector<RealNode> _basicNodes;
	vector<vector<RealNode>> _comNodes; // with levels!
	vector<bool> _isBaseNodesVir;
	vector<cv::Mat> _levelMaps;

	vector<cv::Mat> _userRels;
	vector<double> _userRelDegree;

	vector<CContour> _finalContours;
	cv::Mat _baseMap;
	static int _relMatWidth;
	int _relRange;
	
	int _baseVirNum;

	void initNet(string tsection, string toutdir, string tnewoutdir);
	void collectRel();
	float drawMats(cv::Mat& relMat, string& saveName, bool ifDraw=false);
	void userRelGet(cv::Mat& userMask);
	int getSimUserRel();
	void getSubRegions();
	void getAtoms();

	//After the graph build, get the contextual relationships of all the compound/base node
	vector<vector<vector<cv::Mat>>> _comCntxs; // Corresponding to the hier compound structure;
	vector<vector<vector<double>>> _comCntxDegrees;
	vector<vector<cv::Mat>> _baseCntxs; //Structure nodes' context
	vector<vector<double>> _baseCntxsDegrees;

	void getOnePairCntx(cv::Mat& src, cv::Mat& tgt, cv::Mat& res, cv::Mat& tempR);
	void getRealContext();

	void buildHierGraph();
	void saveToFiles();
	void fwriteOneCtx(cv::Mat& cntx, cv::Mat& bigCntxMap, int level, int toWhich);

public:
	static void getCntxFromMap(cv::Mat& out_cntx, cv::Mat& cntxMap, int level, int toWhich);
	SingleNet(void);

	SingleNet(Summrarizer* s);
	~SingleNet(void);


};
/*
void SingleNet::userRelGet( cv::Mat& userMask )
{
	cout << _allRels.size() << endl;
	_userRelDegree.resize(_allRels.size());
	_userRels.resize(_allRels.size());
	cv::Mat tempR;
	int curX, curY;
	cv::dilate(userMask, userMask, cv::Mat());
	for (int i = 0; i < _userRels.size(); i++)
	{
		cout << "Start " << i << endl;
		_userRels[i].create(_relMatWidth, _relMatWidth, CV_32FC1);
		_userRels[i].setTo(0);
		_userRelDegree[i] = 0;
		cv::Mat& regI = _summer->_finalRegions[i];
		cv::bitwise_and(regI, userMask, tempR);
		cout << "User rel " << i << endl;
		if(cv::countNonZero(tempR) < 5)
			continue;
		//		SHOW_IMG(userMask, 1);
		//		SHOW_IMG(regI, 1);
		//		SHOW_IMG(tempR,1);
		for (int r = 0; r < regI.rows; r++)
		{
			uchar* pI = regI.ptr<uchar>(r);
			for (int c = 0; c < regI.cols; c++)
			{
				if(pI[c] == 0)
					continue;
				for (int m = -_relRange; m <= _relRange; m++)
				{
					for (int n = -_relRange; n <= _relRange; n++)
					{
						curX = c + n; curY = r + m;
						if(curX < 0 || curX >= regI.cols || curY < 0|| curY >= regI.rows)
							continue;
						if(tempR.ptr<uchar>(r)[c] == 255 && tempR.ptr<uchar>(curY)[curX] == 255)
							continue;
						if(userMask.ptr<uchar>(curY)[curX] == 255)
							_userRels[i].ptr<float>(m + _relRange)[n + _relRange] += 1;
					}
				}
			}
		}
		//end for one mask
	}
	cout << "output user rel" << endl;
	for (int j = 0; j < _allRels.size(); j++)
	{
		string s = string(outdir) + "user_rel_" + "_" + boost::lexical_cast<string>(j) + ".png";
		if(cv::countNonZero(_userRels[j]) > 1)
			_userRels[j] = drawMats(_userRels[j], s);
	}
}
*/