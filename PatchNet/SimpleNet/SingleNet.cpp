#include "stdafx2.h"
#include "SingleNet.h"

int SingleNet::_relMatWidth = 5;
SingleNet::SingleNet(void)
{
}

SingleNet::SingleNet( Summrarizer* s )
{
	_summer = s;
}


SingleNet::~SingleNet(void)
{
}

void SingleNet::initNet( string tsection, string toutdir, string tnewoutdir )
{
//	CmInitial("PatchNetFolder.ini", "Log.txt", false, false, tsection.c_str());
	_sn_new_out = tnewoutdir;
	_sn_out = toutdir;
	_sn_sec = tsection;
	_relRange = 2;
	_relMatWidth = _relRange * 2 + 1;
	_nodeNum = _summer->_finalPatches.size();
	cout << "========== > Init Net";
	_allRels.resize(_nodeNum);
	_relDegree.resize(_nodeNum);
	_relSignif.resize(_nodeNum);
	_isBaseNodesVir.resize(_nodeNum);
	for (int i = 0; i < _nodeNum; i++)
	{
		_relSignif[i].resize(_nodeNum);
		_allRels[i].resize(_nodeNum);
		_relDegree[i].resize(_nodeNum);
		for (int j = 0; j < _nodeNum; j++)
		{
			_allRels[i][j].create(_relMatWidth, _relMatWidth, CV_32FC1);
			_allRels[i][j].setTo(0);		
			_relDegree[i][j] = 0;
			_relSignif[i][j] = 0;
		}
	}
	cout << "=========> End Init" << endl;
}

void SingleNet::collectRel()
{
	cout << "========== > collect Relation" << endl;
	cv::Mat tempR;
	int curX, curY;
	for (int i = 0; i <_allRels.size(); i++)
	{
			cout << "=================== > deal  " << i << endl;
		for (int j = 0; j < _allRels[i].size(); j++)
		{
			if(i == j)
				continue;
	
			cv::Mat& regI = _summer->_finalRegions[i];
			cv::Mat& regJ = _summer->_finalRegions[j];
			cv::bitwise_and(regI, regJ, tempR);
//			imwrite(s, tempR);
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
							if(regJ.ptr<uchar>(curY)[curX] == 255)
							{
								_allRels[i][j].ptr<float>(m + _relRange)[n + _relRange] += 1;
								_relSignif[i][j] += 1;
							}
						}
					}
				}
			}

			//end for each pair
		}
	}

	for (int i = 0; i < _allRels.size(); i++)
	{
		double SigSumI = 0;
		for (int j = 0; j < _allRels[i].size(); j++)
		{
			string s = string(_sn_out) + "z_rel_" + boost::lexical_cast<string>(i)
				+ "_" + boost::lexical_cast<string>(j) + ".png";
			if(cv::countNonZero(_allRels[i][j]) > 1)
	//			_relDegree[i][j] = drawMats(_allRels[i][j], s);
			SigSumI += _relSignif[i][j];
		}
		for (int j = 0; j < _relSignif[i].size(); j++)
			_relSignif[i][j] = _relSignif[i][j] / SigSumI;
	}
}

float SingleNet::drawMats( cv::Mat& relMat, string& saveName, bool ifDraw/*=false*/ )
{
	cv::Mat srcRelMat;
	cv::Scalar sum = cv::sum(relMat);

	relMat.convertTo(srcRelMat, relMat.type());
		//	cv::normalize(relMat, relMat);
		//	relMat = relMat * 1800;
		double minV, maxV;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(srcRelMat, &minV, &maxV, &minLoc, &maxLoc, cv::Mat());
		if(maxV < 20)
			srcRelMat.setTo(0);
		else if (maxV - minV == 0)
			srcRelMat.setTo(255);
		else{
			srcRelMat = srcRelMat - minV;
			srcRelMat = 255.0 / (maxV - minV) * srcRelMat; 
			relMat = srcRelMat;
		}
		
		//	temp.convertTo(temp, CV_8UC1);
	if(ifDraw)
	{		
		int show_col = srcRelMat.cols * 10;
		int show_row = srcRelMat.rows * 10;
		cv::Mat img_show;
		img_show.create(show_row, show_col, CV_8UC1);
		cv::resize(srcRelMat, img_show, img_show.size());
		imwrite(saveName, img_show);
	}
	
//	cout << "User Degree: " << sum.val[0] << endl;
	return sum.val[0];
	//	return maxV;
}

void SingleNet::userRelGet( cv::Mat& userMask )
{
	cout << "*********User mask with: " << _allRels.size() << endl;
	_userRelDegree.resize(_allRels.size());
	_userRels.resize(_allRels.size());
	cv::Mat tempR;
	int curX, curY;
	cv::dilate(userMask, userMask, cv::Mat());
	for (int i = 0; i < _userRels.size(); i++)
	{
//		cout << "Start " << i << endl;
		_userRels[i].create(_relMatWidth, _relMatWidth, CV_32FC1);
		_userRels[i].setTo(0);
		_userRelDegree[i] = 0;
		cv::Mat& regI = _summer->_finalRegions[i];
		cv::bitwise_and(regI, userMask, tempR);
//		cout << "User rel " << i << endl;
		if(cv::countNonZero(tempR) < 5)
			continue; 
		cout << "**** a valid user related node" << i << endl;
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
	cout << "********* output user rel : " << _sn_out << endl;
	double sum = 0.0;
	for (int j = 0; j < _allRels.size(); j++)
	{
		string s = string(_sn_out) + "user_rel_" + "_" + boost::lexical_cast<string>(j) + ".png";
		if(cv::countNonZero(_userRels[j]) > 1)
		{	
			_userRelDegree[j] = drawMats(_userRels[j], s, true);
			sum += _userRelDegree[j];		
		}
	}
	//Normalize the Degree
	cout << "*******User degree: ";
	for (int i = 0; i < _userRels.size(); i++)
	{
		_userRelDegree[i] = _userRelDegree[i] / sum;
		cout << _userRelDegree[i] << ",";
	}
	cout << endl;
}

int SingleNet::getSimUserRel()
{
	int res = 0;
	float minDis = 1E20;
	for (int i = 0; i < 10; i++)
	{
		vector<cv::Mat>& curNodeRels = _allRels[i];
		float curDis = 0.0;
		for (int j = 0; j < curNodeRels.size() / 2; j++)
		{
			if(cv::countNonZero(_userRels[j]) <= 1)
				continue;
			curDis += cv::norm(curNodeRels[j], _userRels[j], CV_L2);
		}
		cout << curDis << " - " << i << endl;
		if(curDis < minDis)
		{
			minDis = curDis;
			res = i;
		}
	}
	return res;
}

void SingleNet::getSubRegions()
{
	cout << "$$$$$$$$--Get Sub Regions";
	for (int i = 0; i < _summer->_finalRegions.size(); i++)
	{
		cv::Mat curFinalRegion;
		_summer->_finalRegions[i].convertTo(curFinalRegion, CV_8UC1);

		vector<vector<Point>> contours;
		cv::findContours(curFinalRegion, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
		_finalContours.push_back(contours);
	}
	
}

void SingleNet::getAtoms()
{
	cout << "$$$$$$$$$--Get Atoms" << endl;
	cv::Mat tempDraw; string nameS = string(_sn_out)+"structPat.txt";
	ofstream ofs(nameS.c_str());
//	if(boost::filesystem::exists(nameS))
//		boost::filesystem::remove(nameS);
	double sum = 0.0, maxArea = 0.0, minArea = 1E10;
	bool isBase = false;
	
	
	// = max(_imgW, _imgH) * largeLengthRateThr;
	_imgH = _summer->_finalRegions[0].rows;
	_imgW = _summer->_finalRegions[0].cols;
	tempDraw.create(_imgH, _imgW, CV_8UC1);
	_baseVirNum = 0;

	//int largeRegionThr = (*gpSet)("SNlarge");
	int largeRegionThr = (_imgH*_imgW)/10;
	//int largeLengthThr = (*gpSet)("SNlength");
	int largeLengthThr = _imgH/10;
	CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY, "Thr large reg = %d\n", largeRegionThr);

	for (int i = 0; i < _finalContours.size(); i++)
	{
		isBase = false;
		sum = 0.0, maxArea = 0.0, minArea = 1E10;
		CContour& curCont = _finalContours[i];
		int length = 0;
		for (int j = 0; j < curCont.size(); j++)
		{
			tempDraw.setTo(0);
			int curArea = cv::contourArea(curCont[j]);
			length = cv::boundingRect(curCont[j]).width + cv::boundingRect(curCont[j]).height;
			maxArea = (curArea > maxArea) ? curArea : maxArea;
			minArea = (curArea < minArea) ? curArea : minArea;

		}
		if(maxArea > largeRegionThr && curCont.size() <= 8)
			isBase = true;
		if(maxArea > largeRegionThr && curCont.size() > 8 && maxArea / minArea > 5)
			isBase = true;
		if(length > largeLengthThr && maxArea < largeRegionThr / 5)
			isBase = true;
		for (int j = 0; j < curCont.size(); j++)
		{
			RealNode curNode;
			curNode._RegContour = curCont[j];
			curNode._virtualID = i;
			curNode._nodeDepth = -1; // nodeDepth -1 means didn't give it
			curNode._isBase = isBase;
			_allNodes.push_back(curNode);
			if(isBase)
				_basicNodes.push_back(curNode);
		}
		_isBaseNodesVir[i] = isBase;
		if(isBase)
		{
			ofs << _summer->_finalPatches[i].center.x << " " << _summer->_finalPatches[i].center.y << endl;
			_baseVirNum ++;
		}
	}
	ofs.flush(); ofs.close();
	cout << "=============> End get atoms, basic nodes: " << _basicNodes.size()  << endl;
	_baseMap.create(_summer->_srcImg.rows, _summer->_srcImg.cols, CV_8UC1);
	_baseMap.setTo(0);
//	for (int i = 0; i < _basicNodes.size(); i++)
//		Shared::drawPoly(_baseMap, _basicNodes[i]._RegContour, cv::Scalar(255), 1, true);
	for (int i = 0; i < _baseVirNum; i++)
	{
		CmLog::LogLine(FOREGROUND_RED, "current region %d\n", i);
		cv::bitwise_or(_baseMap, _summer->_finalRegions[i], _baseMap);
	}
	cv::dilate(_baseMap, _baseMap, cv::Mat(), cv::Point(-1,-1), 2);
	cv::erode(_baseMap, _baseMap, cv::Mat(), cv::Point(-1,-1), 7);
}

bool testCross(cv::Mat& mask8U, vector<cv::Point>& contour)
{
	bool res = false;
	for (int i = 0; i < contour.size(); i++)
	{
		if(mask8U.ptr<uchar>(contour[i].y)[contour[i].x] != 0)
		{
			res = true;
			break;
		}
	}
	return res;
}


void SingleNet::buildHierGraph()
{
	CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Start building Hier graph");
	cv::Mat tempDraw, tempInside;
	tempDraw.create(_imgH,_imgW, CV_8UC1);
	tempDraw.setTo(0);
	tempInside.create(_imgH,_imgW, CV_8UC1);
	tempInside.setTo(0);

 	for (int i = 0; i < _summer->_finalRegions.size(); i++)
 	{
 		if(_isBaseNodesVir[i])
 			cv::bitwise_or(tempDraw, _summer->_finalRegions[i], tempDraw);
 	}
 	cv::bitwise_and(tempDraw, _baseMap, _baseMap);
// 	SHOW_IMG(_baseMap, 1);
	tempDraw.setTo(0);
	for (int i = 0; i < _summer->_finalRegions.size(); i++)
	{
		if(!_isBaseNodesVir[i])
			cv::bitwise_or(tempDraw, _summer->_finalRegions[i], tempDraw);
	}
//	Shared::myXor(tempDraw, _baseMap, tempDraw);
//	SHOW_IMG(tempDraw, 1);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(tempDraw, contours, CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_NONE,cv::Point(0, 0));	
	tempDraw.setTo(0);
	for (int i = 0; i < _summer->_finalRegions.size(); i++)
	{
		if(_isBaseNodesVir[i])
			cv::bitwise_or(tempDraw, _summer->_finalRegions[i], tempDraw);
	}
	_levelMaps.push_back(tempDraw.clone());
//	SHOW_IMG(tempDraw, 1);
	vector<RealNode> curLevelNodes;
	_comNodes.push_back(curLevelNodes);

	for (int i = 0; i < contours.size(); i++)
	{
		vector<cv::Point>& curCont = contours[i];
		if(cv::contourArea(cv::Mat(curCont)) < 200)
			continue;
		tempInside.setTo(0);
		RealNode rn;
		rn._isBase = false;
		rn._nodeDepth = 1;
		rn._virtualID = -1; // -1 represent it is a combination
		for (int k = 0; k < _allNodes.size(); k++)
		{
			if(_allNodes[k]._isBase)
				continue;
			if (cv::pointPolygonTest(cv::Mat(curCont), _allNodes[k]._RegContour[0], true) >= 0)
			{
				Shared::drawPoly(tempInside, _allNodes[k]._RegContour, cv::Scalar(255), 1, true);
		//The father!
				_allNodes[k]._father = i;
				rn._pSons.push_back(&_allNodes[k]);
			}
		}
		rn._RegContour = curCont;
		_comNodes[0].push_back(rn);
		cv::imwrite(string(_sn_out) + "_comL0_" + boost::lexical_cast<string>(_comNodes[0].size() - 1) + ".png", tempInside);
	//		SHOW_IMG(tempInside, 1);
	}

	bool ntravelAll = true;
	int curL = 0;
	while (ntravelAll && curL < 4)
	{
		Mat& curLMap = _levelMaps[curL];
//		SHOW_IMG(curLMap, 1);
		cout << "XXXXXXXXXXXXXXXXXXXXXXXX\n Current level is" << curL << endl; 
		Mat nextLMap;
		nextLMap.create(curLMap.size(), CV_8UC1);
		nextLMap.setTo(0);
		int unTouch = 0;
		for (int i = 0; i < _comNodes[curL].size(); i++)
		{
			tempInside.setTo(0);
			RealNode& curNode = _comNodes[curL][i];
			int countT = 0, countUnT = 0;
			for (int k = 0; k < curNode._pSons.size(); k++)
			{
				RealNode* cn = curNode._pSons[k];
				if(testCross(curLMap, cn->_RegContour))
				{
					Shared::drawPoly(nextLMap, cn->_RegContour, Scalar(255), 1, true);
					cn->_tempMark = 1;
					countT ++;
				}
				else
				{
					unTouch ++;
					Shared::drawPoly(tempInside, cn->_RegContour, Scalar(255), 1, true);
					cn->_tempMark = 0;
					countUnT++;
				}
			}
//			SHOW_IMG(nextLMap, 1);
			std::vector<std::vector<cv::Point> > tcontours;
			cv::findContours(tempInside, tcontours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,cv::Point(0, 0));	
			//Replace the son nodes of current comNode, 
			//Put the next level comNode
			cv::drawContours(tempInside, tcontours, -1, cv::Scalar(100));
			cout << "Show contours" << endl;
//			SHOW_IMG(tempInside, 1);
			vector<struct Node*> tPSons;
			cout << "In this node, Touched = " << countT << "," << countUnT << endl;
			for (int n = 0; n < curNode._pSons.size(); n++)
				if(curNode._pSons[n]->_tempMark == 1)
					tPSons.push_back(curNode._pSons[n]);
			cout << "In this node, tPsons = " << tPSons.size() << endl;
			vector<RealNode> nexLCom;
			_comNodes.push_back(nexLCom);
			
			cv::Mat tempShow1; tempInside.convertTo(tempShow1, CV_8UC1);
			cv::Mat tempShow2; tempInside.convertTo(tempShow2, CV_8UC1);

			for (int n = 0; n < tcontours.size(); n++)
			{
				cout << "---> Current son comnode is" << n << endl;
				vector<cv::Point>& curCont = tcontours[n];
				tempInside.setTo(0);
				RealNode tRN;
				int comSonsN = 0; int onlyID = -1;
				tRN._isBase = false;
				tRN._nodeDepth = curL + 2;
				tRN._virtualID = -1; // -1 represent it is a combination
				for (int k = 0; k < curNode._pSons.size(); k++)
				{
					if(curNode._pSons[k]->_tempMark == 1)
						continue;
					if (cv::pointPolygonTest(cv::Mat(curCont), curNode._pSons[k]->_RegContour[0], true) >= 0)
					{
			//			Shared::drawPoly(tempInside, curNode._pSons[k]->_RegContour, cv::Scalar(255), 1, true);
						//The father
						curNode._pSons[k]->_father = n;
						tRN._pSons.push_back(curNode._pSons[k]);
						
						comSonsN++;
						onlyID = k;
					}
				}						
	//			tempShow1.setTo(0);
	//			Shared::drawPoly(tempShow1, curCont, cv::Scalar(255), 1, true);
	//			cv::imwrite(string(_sn_out) + "_comL0_" + boost::lexical_cast<string>(curL + 1) + ".png", tempInside);
// 				for (int tt = 0; tt < curCont.size(); tt++)
// 				{
// 					tRN._RegContour.push_back(curCont[tt]);
// 				}
				tRN._RegContour = curCont;  
				if(comSonsN <= 1 && tPSons.size() == 0){
					tPSons.push_back(curNode._pSons[onlyID]);
					continue;
				}

				_comNodes[curL + 1].push_back(tRN);
			//	tPSons.push_back(&_comNodes[curL + 1][_comNodes[curL + 1].size() - 1]);
				tRN._father = i;
				curNode._ComSons.push_back(tRN);
				
				if( n >= 1)
				{	
					cout << "Draw" << n << endl;
					Shared::drawPoly(tempInside, curCont, cv::Scalar(60), 1, true);
					Shared::drawPoly(tempInside, _comNodes[curL + 1][_comNodes[curL + 1].size() - 1]._RegContour, cv::Scalar(150), 1, true);
		//			cout << "TpSons " << tPSons.size() - 1 << tPSons[tPSons.size() - 1]->_virtualID<< endl;
					cout << "tRNTpSons " << curNode._ComSons.size() << curNode._ComSons[curNode._ComSons.size() - 2]._virtualID<< endl;
					Shared::drawPoly(tempInside, curNode._ComSons[curNode._ComSons.size() - 1]._RegContour, cv::Scalar(250), 1, true);
					Shared::drawPoly(tempInside, curNode._ComSons[curNode._ComSons.size() - 2]._RegContour, cv::Scalar(250), 1, true);
			//		Shared::drawPoly(tempInside, _comNodes[curL + 1][_comNodes[curL + 1].size() - 1]._RegContour, cv::Scalar(80), 1, true);
			//		Shared::drawPoly(tempInside, _comNodes[curL + 1][_comNodes[curL + 1].size() - 2]._RegContour, cv::Scalar(80), 1, true);
		//			Shared::drawPoly(tempInside, _comNodes[curL + 1][_comNodes[curL + 1].size() - 2]._RegContour, cv::Scalar(250), 1, true);

		//			SHOW_IMG(tempInside, 1);
				}
//				SHOW_IMG(tempInside, 1);
//				cout << "A combo node" << n << endl;
//				tempInside.setTo(0);
//				Shared::drawPoly(tempInside, _comNodes[curL + 1][n]._RegContour, cv::Scalar(255), 1, true);
//				SHOW_IMG(tempInside, 1);

			}
			
			tempDraw.setTo(0);
			cout << "In this node, tPsons = " << tPSons.size() << endl;
			for (int x = 0; x < tPSons.size(); x++)
			{
					Shared::drawPoly(tempDraw, tPSons[x]->_RegContour, cv::Scalar(255), 1, true);
			}
			for (int y = 0; y < curNode._ComSons.size(); y++)
			{
				Shared::drawPoly(tempDraw, curNode._ComSons[y]._RegContour, cv::Scalar(128), 1, true);
			}
			cout << "Verify the node" << endl;
//			SHOW_IMG(tempDraw, 1);
			curNode._pSons = tPSons;
			//For one comNode.   
		}

		if(unTouch == 0)
			ntravelAll = false;
		_levelMaps.push_back(nextLMap);
		curL ++;
	}

	//Verify !!!!!!!!!!!!
/*	cout << "Start verifying ......" << endl;
	cv::Mat tempReg;
	tempDraw.convertTo(tempReg, CV_8UC1);
	for (int i = 0; i < _comNodes.size(); i++)
	{
		for (int j = 0; j < _comNodes[i].size(); j++)
		{
			RealNode& RN = _comNodes[i][j];
			tempReg.setTo(0);
			Shared::drawPoly(tempReg, RN._RegContour, cv::Scalar(255), 1, true);
			SHOW_IMG(tempReg, 1);


			tempDraw.setTo(0);
			cout << "sons size: " << RN._pSons.size() << endl;
			for (int k = 0; k < RN._pSons.size(); k++)
					Shared::drawPoly(tempDraw, RN._pSons[k]->_RegContour, cv::Scalar(250), 1, true);	
			
			for (int l = 0; l < RN._ComSons.size(); l++)
			{
				Shared::drawPoly(tempDraw, RN._ComSons[l]._RegContour, cv::Scalar(100), 1, true);
			}
			cout << endl;

			SHOW_IMG(tempDraw, 1);
			
		}
	}
*/
}

void SingleNet::getRealContext()
{

	//First the real structure node
	/*Mat a;a.create(_summer->_finalRegions[0].size(),CV_8UC1);
	Mat s = a.clone();
	for(int i=0;i<_summer->_finalRegions.size();i++){
		imshow("ss",_summer->_finalRegions[i]);
		waitKey(0);
		s= a.clone();
	}*/
	CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Start collecting the contextual feature of real nodes");
//	cout << "Start collecting the contextual feature of real nodes" << endl;
	
	Mat tempDraw, tempR;
	tempDraw.create(_imgH, _imgW, CV_8UC1); tempDraw.setTo(0);

	_baseCntxs.resize(_basicNodes.size());
	_baseCntxsDegrees.resize(_basicNodes.size());

	for (int i = 0; i < _basicNodes.size(); i++)
	{
		RealNode& curnode = _basicNodes[i];
		tempDraw.setTo(0);
		Shared::drawPoly(tempDraw, curnode._RegContour, cv::Scalar(255), 1, true);
		cv::bitwise_and(tempDraw, _summer->_finalRegions[curnode._virtualID], tempDraw); // In case for holes
		_baseCntxs[i].resize(_summer->_finalRegions.size());
		_baseCntxsDegrees[i].resize(_summer->_finalRegions.size());
		int curX, curY;
		for (int j = 0; j < _summer->_finalRegions.size(); j++)
		{
			cv::Mat& curCnxt = _baseCntxs[i][j];
			curCnxt.create(_relMatWidth, _relMatWidth, CV_32FC1); curCnxt.setTo(0);
			cv::Mat& regJ = _summer->_finalRegions[j];
			cv::bitwise_and(tempDraw, regJ, tempR);
			if(cv::countNonZero(tempR) < 5)
				continue;
			if(j == curnode._virtualID)
				continue;
			for (int r = 0; r < regJ.rows; r++)
			{
				uchar* pI = regJ.ptr<uchar>(r);
				for (int c = 0; c < regJ.cols; c++)
				{
					if(pI[c] == 0) continue;
					for (int m = -_relRange; m<=_relRange; m++)
					{
						for (int n = -_relRange; n <= _relRange; n++)
						{
							curX = c + n; curY = r + m;
							if(curX < 0 || curX >= regJ.cols || curY < 0 || curY >= regJ.rows)
								continue;
							if(tempR.ptr<uchar>(r)[c]==255 && tempR.ptr<uchar>(curY)[curX]==255)
								continue;
							if(tempDraw.ptr<uchar>(curY)[curX]==255)
								curCnxt.ptr<float>(m+_relRange)[n+_relRange]+=1;
						}
					}
				}
			}//end go through one mask
			
		}//end for one basic node

		double sum = 0.0;
		for (int j = 0; j < _baseCntxs[i].size(); j++)
		{
			string s = string(_sn_out) + "base_rel_" + boost::lexical_cast<string>(i) + "_"
				+ boost::lexical_cast<string>(j) + ".png";
	//!
			if(cv::countNonZero(_baseCntxs[i][j]) > 1)
			{
				_baseCntxsDegrees[i][j] = drawMats(_baseCntxs[i][j], s);
				sum += _baseCntxsDegrees[i][j];
			}
		}
		for (int j = 0; j < _baseCntxsDegrees[i].size(); j++)
			_baseCntxsDegrees[i][j] = _baseCntxsDegrees[i][j] / sum;
	}
	//Then all the compound node
	_comCntxs.resize(_comNodes.size());
	_comCntxDegrees.resize(_comNodes.size());
	for (int level = 0; level < _comNodes.size(); level++)
	{
		_comCntxs[level].resize(_comNodes[level].size());
		_comCntxDegrees[level].resize(_comNodes[level].size());
		for (int i = 0; i < _comNodes[level].size(); i++)
		{
			_comCntxs[level][i].resize(_summer->_finalRegions.size());
			_comCntxDegrees[level][i].resize(_summer->_finalRegions.size());
			RealNode& curnode = _comNodes[level][i];
			tempDraw.setTo(0);
			Shared::drawPoly(tempDraw, curnode._RegContour, cv::Scalar(255), 1, true);
			if(level == 1 && i == 0)
			{
				cout << "LLLLLLLLL" << endl;
//				SHOW_IMG(tempDraw, 1);
			}
			for (int j = 0; j < _summer->_finalRegions.size(); j++)
			{
				cv::Mat& curCnxt = _comCntxs[level][i][j];
				curCnxt.create(_relMatWidth, _relMatWidth, CV_32FC1); curCnxt.setTo(0);
				cv::Mat& regJ = _summer->_finalRegions[j];
				cv::bitwise_and(tempDraw, regJ, tempR);
				getOnePairCntx(tempDraw, regJ, curCnxt, tempR);
			}// end for one compound node
			double sum = 0.0;
			for (int j = 0; j <_comCntxs[level][i].size(); j++)
			{
				string s = string(_sn_out) + "com_rel_L" + boost::lexical_cast<string>(level) + "_" 
					+ boost::lexical_cast<string>(i) + "_" + boost::lexical_cast<string>(j) + ".png";
	//!
				if(cv::countNonZero(_comCntxs[level][i][j]) > 1)
				{  
					_comCntxDegrees[level][i][j] = drawMats(_comCntxs[level][i][j], s);
					sum+=_comCntxDegrees[level][i][j];
				}
			}
			for (int j = 0; j < _comCntxDegrees[level][i].size(); j++)
				_comCntxDegrees[level][i][j] = _comCntxDegrees[level][i][j] / sum;
		}//end for each level
	}//

}

void SingleNet::getOnePairCntx( cv::Mat& src, cv::Mat& tgt, cv::Mat& res, cv::Mat& tempR)
{
	cv::Mat& curCnxt = res;
	curCnxt.create(_relMatWidth, _relMatWidth, CV_32FC1); curCnxt.setTo(0);
	cv::Mat& regJ =tgt;
	cv::bitwise_and(src, regJ, tempR);
	if(cv::countNonZero(tempR) < 5)
		return;
	int curX, curY;
	for (int r = 0; r < regJ.rows; r++)
	{
		uchar* pI = regJ.ptr<uchar>(r);
		for (int c = 0; c < regJ.cols; c++)
		{
			if(pI[c] == 0) continue;
			for (int m = -_relRange; m<=_relRange; m++)
			{
				for (int n = -_relRange; n <= _relRange; n++)
				{
					curX = c + n; curY = r + m;
					if(curX < 0 || curX >= regJ.cols || curY < 0 || curY >= regJ.rows)
						continue;
					if(tempR.ptr<uchar>(r)[c]==255 && tempR.ptr<uchar>(curY)[curX]==255)
						continue;
					if(src.ptr<uchar>(curY)[curX]==255)
						curCnxt.ptr<float>(m+_relRange)[n+_relRange]+=1;
				}
			}
		}
	}//end go through one mask
}

void SingleNet::fwriteOneCtx(cv::Mat& cntx, cv::Mat& bigCntxMap, int level, int toWhich)
{
	cv::Rect roi = cv::Rect(toWhich * _relMatWidth, level * _relMatWidth, _relMatWidth, _relMatWidth);
	for (int i = roi.y, is = 0; is < _relMatWidth; i++, is++)
	{
		for (int j = roi.x, js = 0; js < _relMatWidth; j++, js++)
		{
			bigCntxMap.ptr<uchar>(i)[j] = cntx.ptr<float>(is)[js];
		}
	}
//	cntx.copyTo(bigCntxMap(roi));
//	Shared::showMat(cntx);
//	Shared::showMat(bigCntxMap(roi));
}

void SingleNet::saveToFiles()
{
	string SectionS = this->_summer->secName;
	CmLog::LogLine(FOREGROUND_INTENSITY|FOREGROUND_RED, "Start draw to files! \n");
	//Save contexts into matrice according to levels (for compound node, one map, row-levels, col-to different real);
	//for base nodes, row-bases, col-realbases.
	vector<string> fnComContxs;
	int levelsN = _comNodes.size();	
	int realTypesN = _summer->_finalRegions.size();
	fnComContxs.resize(_comNodes.size());
	for (int i = 0; i < _comNodes.size(); i++)
	{
		fnComContxs[i] = string(_sn_new_out) + SectionS + "_ccxt"
			+ boost::lexical_cast<string>(i) + ".png";
	}
	string fnBaseContxs = string(_sn_new_out) + SectionS + "_bcxt.png";
	cv::Mat baseContxsMap;
	baseContxsMap.create(1+_basicNodes.size() * _relMatWidth, realTypesN * _relMatWidth, CV_8UC1);
	baseContxsMap.setTo(0);
	for (int i = 0; i < _baseCntxs.size(); i++) // 非正方形，每个都是对real的关系，不是region间的
	{
		for (int j = 0; j < _baseCntxs[i].size(); j++)
		{
//			Shared::showMat(_baseCntxs[i][j]);
			fwriteOneCtx(_baseCntxs[i][j], baseContxsMap, i, j);
		}
	}

	vector<cv::Mat> bigCntxMaps;
	bigCntxMaps.resize(levelsN); //level
	//Before write, the cntx map should be normalized to 0-255
	for (int i = 0; i < _levelMaps.size() - 1; i++)
	{
		bigCntxMaps[i].create(1+_comNodes[i].size() * _relMatWidth, realTypesN * _relMatWidth, CV_8UC1);
		bigCntxMaps[i].setTo(0);
		for (int j = 0; j < _comCntxs[i].size(); j++)
			for (int n = 0; n < _comCntxs[i][j].size(); n++)
				fwriteOneCtx(_comCntxs[i][j][n], bigCntxMaps[i], j, n);
	}
	
	///////////////////////////////////////////////////////////////////
	//Save real centers;
	//this->_summer->_centersMasks
//	cv::Mat allCentersSave1U;
//	allCentersSave1U.create(_imgH, _imgW, CV_8UC1);
//	allCentersSave1U.setTo(0);
//	for (int i = 0; i < realTypesN && i <= 255; i++)
//		allCentersSave1U.setTo(i+1, this->_summer->_centersMasks[i]);
	
	/////////////////////////////////////////////////////////////////////
	//Save the regions they cover
	cv::Mat baseNodesMap;
	vector<cv::Mat> comNodesMaps;
	comNodesMaps.resize(levelsN);
	baseNodesMap.create(_imgH, _imgW, CV_8UC3);
	baseNodesMap.setTo(0);

	for (int i = 0; i < _basicNodes.size(); i++)
	{
		int tArea = cv::contourArea(cv::Mat(_basicNodes[i]._RegContour)) / 500;
		Shared::drawPoly(baseNodesMap, _basicNodes[i]._RegContour, 
			cv::Scalar(255-i, _basicNodes[i]._virtualID, tArea), 1, true);		
		if(i < baseContxsMap.cols)
			baseContxsMap.ptr<uchar>(baseContxsMap.rows - 1)[i] = tArea > 255 ? 255:tArea;
	}
	//Save the area inf into 
	cv::imwrite(fnBaseContxs, baseContxsMap);
//	baseNodesMap.copyTo(baseNodesMap, _baseMap);

	for (int i = 0; i < baseNodesMap.rows; i++)
	{
		for (int j = 0; j < baseNodesMap.cols; j++)
		{
			if(_baseMap.ptr<uchar>(i)[j] == 0)
				baseNodesMap.ptr<Vec3b>(i)[j] = Vec3b(0,0,0);
		}
	}

	string fnBaseMap = string(_sn_new_out) + SectionS + "_bmap.png";
	cv::imwrite(fnBaseMap, baseNodesMap);

	for (int i = 0; i < _levelMaps.size() - 1; i++)
	{
		comNodesMaps[i].create(_imgH, _imgW, CV_8UC3);
		comNodesMaps[i].setTo(0);
		for (int j = 0; j < _comNodes[i].size(); j++)
		{
			int tArea = cv::contourArea(cv::Mat(_comNodes[i][j]._RegContour)) / 500;
			Shared::drawPoly(comNodesMaps[i], _comNodes[i][j]._RegContour,
				cv::Scalar(255-j, _comNodes[i][j]._father, tArea), 1, true);
			if(j < bigCntxMaps[i].cols)
				bigCntxMaps[i].ptr<uchar>(bigCntxMaps[i].rows - 1)[j] = tArea > 255 ? 255:tArea;
		}

		if(bigCntxMaps[i].rows>0)
			cv::imwrite(fnComContxs[i], bigCntxMaps[i]);

		string fnCntxMap = string(_sn_new_out) + SectionS + "_cmap" + boost::lexical_cast<string>(i) + ".png";
		for (int ii = 0; ii < comNodesMaps[i].rows; ii++)
		{
			for (int j = 0; j < comNodesMaps[i].cols; j++)
			{
				if(_levelMaps[i].ptr<uchar>(ii)[j] == 255)
					comNodesMaps[i].ptr<Vec3b>(ii)[j] = Vec3b(0,0,0);
			}
		}
		cv::imwrite(fnCntxMap, comNodesMaps[i]);
	}
}

void SingleNet::getCntxFromMap( cv::Mat& out_cntx, cv::Mat& cntxMap, int level, int toWhich )
{
	cv::Rect roi = cv::Rect(toWhich * _relMatWidth, level * _relMatWidth, _relMatWidth, _relMatWidth);
#ifdef _DEBUG
	out_cntx.create(_relMatWidth, _relMatWidth, CV_8UC1);
	for (int i = 0; i < _relMatWidth; i++)
	{
		for (int j = 0; j < _relMatWidth; j++)
		{
			out_cntx.ptr<uchar>(i)[j] = cntxMap.ptr<uchar>(i + roi.y)[j + roi.x];
		}
	}
#else
	cntxMap(roi).copyTo(out_cntx);
#endif
}


