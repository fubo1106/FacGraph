#include "stdafx2.h"
#include "NetExplorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool lessThan(Dist_ID d1, Dist_ID d2)
{
	return d1.dist<d2.dist;
}

NetExplorer::NetExplorer(void)
{
}

NetExplorer::NetExplorer( Summrarizer* user_summer )
{
	_ifUserOnlyBase = false;
	_thresForMinDist = 980;
	_patchSimThres = 2000;
	//_userRelNumThr = (*gpSet)("userRelNumThr");
	_userRelNumThr = 45;
	//_userRelNumThr2 = (*gpSet)("userRelNumThr2");
	_userRelNumThr2 = 25;
	_userNet._summer = user_summer;
}

NetExplorer::NetExplorer(Summrarizer* user_summer,Summrarizer* region_summer){
	_ifUserOnlyBase = false;
	_thresForMinDist = 980;
	_patchSimThres = 2000;
	//_userRelNumThr = (*gpSet)("userRelNumThr");
	_userRelNumThr = 45;
	//_userRelNumThr2 = (*gpSet)("userRelNumThr2");
	_userRelNumThr2 = 25;
	_userNet._summer = user_summer;
	_regionNet._summer = region_summer;
}

NetExplorer::~NetExplorer(void)
{
}

void NetExplorer::getUserNet( const char* userMaskN)
{
	cv::Mat userM = cv::imread(userMaskN, 0);
	_userNet.initNet(_ne_section, _ne_outdir, _ne_new_outdir);
//	_userNet.collectRel();
	_userNet.userRelGet(userM);
	_userNet.getSubRegions();
	_userNet.getAtoms();
	cv::Mat ttShow;
	_userNet._summer->_srcImg.copyTo(ttShow);
	//_userdegThr = (*gpSet)("userDeg");
	_userdegThr = 0.15;
//	cout << "(((((((getUserNet)))" << _userdegThr; 
	if(_ifUserOnlyBase == false)
		for (int i = 0; i < _userNet._userRels.size(); i++)
		{
			cv::Mat& curUserRel = _userNet._userRels[i];
	//		cout << "(((getUserNet))) "<< _userNet._userRelDegree[i] << endl;
			if(_userNet._userRelDegree[i] > _userdegThr)
			{
				cv::Mat curP;
				_userNet._summer->getPatchFromImg(_userNet._summer->_srcImg, curP, _userNet._summer->_finalPatches[i]);
				_userRelPatches.push_back(curP.clone());
				_userRelsPatches.push_back(_userNet._summer->_finalPatches[i]);
				_userValidRels.push_back(curUserRel.clone());
			
 				string s = string(_ne_outdir) + string("userRelPath")+boost::lexical_cast<string>(i) + string(".png");
				cv::imwrite(s, curP);
				s = boost::lexical_cast<string>(_userValidRels.size());
				Shared::drawPatchFontOnImg(_userNet._summer->_finalPatches[i], s, ttShow, cv::Scalar(0,0,255));
			}
		}
	else // Only use the structure node
		for (int i = 0; i < _userNet._userRels.size(); i++)
		{
			cv::Mat& curUserRel = _userNet._userRels[i];
			//		cout << "(((getUserNet))) "<< _userNet._userRelDegree[i] << endl;
			if(_userNet._isBaseNodesVir[i] == true && _userNet._userRelDegree[i] > 0.05)
			{
				cv::Mat curP;
				_userNet._summer->getPatchFromImg(_userNet._summer->_srcImg, curP, _userNet._summer->_finalPatches[i]);
				_userRelPatches.push_back(curP.clone());
				_userRelsPatches.push_back(_userNet._summer->_finalPatches[i]);
				_userValidRels.push_back(curUserRel.clone());

				string s = string(_ne_outdir) + string("userRelPath")+boost::lexical_cast<string>(i) + string(".png");
				cv::imwrite(s, curP);
				s = boost::lexical_cast<string>(_userValidRels.size());
				Shared::drawPatchFontOnImg(_userNet._summer->_finalPatches[i], s, ttShow, cv::Scalar(0,0,255));
			}
		}
	SHOW_IMG(ttShow, 1);
}

void NetExplorer::oneCompPatRes( string& sectionS, string& outdirS, string& sourcePath ) // Only the saved structure node
{
	this->_ne_section = sectionS.c_str();
	_ne_outdir = outdirS.c_str();
	//	const char* userMaskFN = gCmSet.Val("UserMaskPath");
	string imgA_path = string(sourcePath);
	string tt = string(_ne_outdir);
	_ne_new_outdir = tt.c_str();

	string temp = string(_ne_outdir) + string(_ne_section) + "\\";
	
	_ne_outdir = temp.c_str();
	if(!boost::filesystem::exists(_ne_outdir))
		boost::filesystem::create_directory(_ne_outdir);
	string patchesFN = string(_ne_outdir) + "structPat.txt";
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	//	Mat userM = imread(userMaskFN, 0);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
//	if(imgA.rows >= 600 || imgA.cols >= 600)
//		summer.initialRandPick(imgA, 7);
//	else
		summer.initialRandPick(_ne_section, _ne_outdir, imgA, 6);
	summer.secName = string(_ne_section);

	if(boost::filesystem::exists(patchesFN))
	{
		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
			//only use the patches
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			sp.aveClr = summer._averImg.ptr<Vec3b>(y)[x];
			sp.aveGrad = summer._aveGrad.ptr<uchar>(y)[x];
			summer._finalPatches.push_back(sp);
	//		cv::Mat reg;
	//		string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
	//		reg = imread(fn, 0);
	//		summer._finalRegions.push_back(reg.clone());			
			curCount ++;
		}
	//	SingleNet net1(&summer);
	}
	//Get the structure node similarity to filter
	SecInf inf; inf._dists.resize(_userRelPatches.size()); inf._secN = summer.secName;
	float dist; int flipped;
	cv::Mat tempDraw;
	for (int i = 0; i < _userRelPatches.size(); i++)
	{
		summer._srcImg.copyTo(tempDraw);
		cv::Mat& curUPat = _userRelPatches[i];
		sPatch& curUsP = _userRelsPatches[i];
		inf._dists[i].resize(summer._finalPatches.size());
		for (int j = 0; j < summer._finalPatches.size(); j++)
		{
			cv::Mat curCPat;
			dist = abs(curUsP.aveClr[0] - summer._finalPatches[j].aveClr[0])
				+ abs(curUsP.aveClr[1] - summer._finalPatches[j].aveClr[1])
				+ abs(curUsP.aveClr[2] - summer._finalPatches[j].aveClr[2]);
			if(dist > 120) 
			{	
				inf._dists[i][j] = 1E20;
				continue;
			}
			summer.getPatchFromImg(summer._srcImg, curCPat, summer._finalPatches[j]);
	//		inf._sims[i].push_back(Shared::distForTwoPat(curUPat, curCPat, ))	
			inf._dists[i][j] = Shared::distForTwoPat(curCPat, curUPat, flipped);

			///! for show
			string s = boost::lexical_cast<string>(inf._dists[i][j]);
			Shared::drawPatchFontOnImg(summer._finalPatches[j], s, tempDraw, cv::Scalar(255, 0, 0));
		}
		SHOW_IMG(tempDraw, 1);
	}
	//End the pre filter
}

void NetExplorer::compNodeInFolder(string& resSaveDirS )
{
	_ne_section = gCmSet.Val("section");
	_ne_outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");
	string outdirS(_ne_outdir);

	fs::path fullpath(srcDir, fs::native);
	cout << "=======> Start comp node in folder: " << srcDir << " ### output to" << outdirS << endl;
	if(!fs::exists(fullpath)) return; 

	fs::recursive_directory_iterator end_iter;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			cout << "current image" << iter->path().string() << endl;
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			string currentImageBase = currentImageS.substr(0, currentImageS.find_first_of('.'));
			if(currentImageBase == string(_ne_section))
				continue;
			if(iter->path().extension() == string(".jpg") || 
				iter->path().extension() == string(".png")
				)
				cout << "current _ne_section: " << currentImageBase  << "-" << iter->path().filename().string()<< endl;

	//		oneCompPatRes(currentImageBase, outdirS, currentImagePath);
			oneFindRegionRes(currentImageBase, outdirS, resSaveDirS, currentImagePath);
			cout << "end of one image" << endl;
		}
	}
	cout << "##############Run here###################" << endl;
}

void NetExplorer::getCandidateRegion(SecInf& inf, SingleNet& net1, vector<vector<Dist_ID>>& tDist_IDs)
{
	double time_Start = (double)clock(); 
	float curMinL0Dis = 1E20, curMinID = 0; float curDegree = 1E20;
	if(net1._comNodes.size() == 0 && net1._basicNodes.size() == 0)
		return;
	for (int j = 0; j < _userNet._userRels.size(); j++)
	{
		string s = string(outdir) + "user_rel_" + "_" + boost::lexical_cast<string>(j) + ".png";
		if(cv::countNonZero(_userNet._userRels[j]) > 1)
			_userNet.drawMats(_userNet._userRels[j], s);
	}
	
	for (int i = 0; i < net1._comNodes[0].size(); i++)
	{
		RealNode& curComNode = net1._comNodes[0][i];
		cv::Rect brect = cv::boundingRect(cv::Mat(curComNode._RegContour));
	//For UI
	//	if(cv::contourArea(curComNode._RegContour) < 900 
	//		|| brect.width > net1._summer->_finalRegions[0].cols / 2
	//		|| brect.height > net1._summer->_finalRegions[0].rows / 2)
	//		continue;
		if(brect.width > net1._summer->_finalRegions[0].cols * 2 / 3|| brect.height > net1._summer->_finalRegions[0].rows * 2 / 3)
			continue;
		vector<cv::Mat>& curCntxs = net1._comCntxs[0][i];
		vector<double>& curCntxsDeg = net1._comCntxDegrees[0][i];
		//For each ordered list:
		bool flag = true;
		cv::Mat curNODEShow;
		net1._summer->_srcImg.copyTo(curNODEShow);
		Shared::drawPoly(curNODEShow, curComNode._RegContour, cv::Scalar(0, 255, 0), 1);
		//cout << i << endl;
//		SHOW_IMG(curNODEShow, 1);
		for (int k = 0; k < tDist_IDs.size(); k++)
		{
			bool relToThis = false;
			//cout << "current top 3 ID:";
			//For the top 3 nodes with the current compo node, contextual feature contains XXX
			for (int n = 0; n < tDist_IDs[k].size() && n < 3; n++)
			{
				//cout <<tDist_IDs[k][n].ID << " Dist:" << tDist_IDs[k][n].dist << "; ";
				if(isCntxCovered(_userValidRels[k], curCntxs[tDist_IDs[k][n].ID], tDist_IDs[k][n].coverN))
				{
					//cout << "Is covered";
					tDist_IDs[k][n].valid = true;
					relToThis = true;
				}
			}
			if(!relToThis)
				flag = false;		
			cout << endl;
		}
		//If every user related patch has a valid candidate, (?)then it is a dynamic programming

		if(flag)
		{
			float curRes = 0, curDegSum = 0;
			for (int ii = 0; ii < tDist_IDs.size(); ii++)
			{
				float curMin = 1E20, curMinJ = 0; 
				for (int jj = 0; jj < tDist_IDs[ii].size(); jj++)
					if(true == tDist_IDs[ii][jj].valid)
						if(curMin > tDist_IDs[ii][jj].dist)
						{
							curMin = tDist_IDs[ii][jj].dist;
							curMinJ = jj;
						}					
						curRes += curMin;
						curDegSum += tDist_IDs[ii][curMinJ].coverN * curCntxsDeg[tDist_IDs[ii][curMinJ].ID];
			}
			if(curRes < curMinL0Dis)
			{
				curMinL0Dis = curRes;
				curMinID = i;
				curDegree = curDegSum;
			}
			if(curRes == curMinL0Dis && curDegSum > curDegree)
			{
				curMinL0Dis = curRes;
				curMinID = i;
				curDegree = curDegSum;
			}
			//cout << "i" << " is a valid Level 0 candidate, with the similarity of" << curMinL0Dis << endl;
			//cout << "and significant value of " << curDegSum << endl;
			//cout << "Current most similar node (Comp): " << curMinID << endl;
			ValidRegionInf vri;
			vri.isCompo = true; vri.matchValue = curRes; vri.regionID = i;
			vri.degree = curDegSum; vri.regionLevel = 0;
			inf.validComRegions.push_back(vri);
		}
	}


	float curMinBaseDis = 1E20, curBaseMinID = 0, curBaseDegree = 0;
	for (int i = 0; i < net1._basicNodes.size(); i++)
	{
		RealNode& curComNode = net1._basicNodes[i];
		cv::Rect brect = cv::boundingRect(cv::Mat(curComNode._RegContour));
	//For UI
		//	if(cv::contourArea(curComNode._RegContour) < 900 
	//		|| brect.width > net1._summer->_finalRegions[0].cols / 2
	//		|| brect.height > net1._summer->_finalRegions[0].rows / 2)
	//		continue;
		if(brect.width > net1._summer->_finalRegions[0].cols * 2 / 3|| brect.height > net1._summer->_finalRegions[0].rows * 2 / 3)
			continue;
		vector<cv::Mat>& curCntxs = net1._baseCntxs[i];
		vector<double>& curCntxsDeg = net1._baseCntxsDegrees[i];
		//For each ordered list:
		bool flag = true;
		cv::Mat curNODEShow;
		net1._summer->_srcImg.copyTo(curNODEShow);
		Shared::drawPoly(curNODEShow, curComNode._RegContour, cv::Scalar(0, 255, 2555), 1);
		//cout << i << endl;
//		SHOW_IMG(curNODEShow, 1);
		for (int k = 0; k < tDist_IDs.size(); k++)
		{
			bool relToThis = false;
			//For the top 3 nodes with the current compo node, contextual feature contains XXX
			for (int n = 0; n < tDist_IDs[k].size() && n < 3; n++)
			{
				//cout << "current top 3 ID " << tDist_IDs[k][n].ID << " Dist:" << tDist_IDs[k][n].dist << endl;

				if(isCntxCovered(_userValidRels[k], curCntxs[tDist_IDs[k][n].ID], tDist_IDs[k][n].coverN))
				{
					tDist_IDs[k][n].valid = true;
					relToThis = true;
				}
			}
			if(!relToThis)
				flag = false;			
		}
		//If every user related patch has a valid candidate, (?)then it is a dynamic programming

		if(flag)
		{
			float curRes = 0, curDegSum = 0;
			for (int ii = 0; ii < tDist_IDs.size(); ii++)
			{
				float curMin = 1E20, curMinJ = 0; 
				for (int jj = 0; jj < tDist_IDs[ii].size(); jj++)
					if(true == tDist_IDs[ii][jj].valid)
						if(curMin > tDist_IDs[ii][jj].dist)
						{
							curMin = tDist_IDs[ii][jj].dist;
							curMinJ = jj;
						}					
						curRes += curMin;
						curDegSum += tDist_IDs[ii][curMinJ].coverN * curCntxsDeg[tDist_IDs[ii][curMinJ].ID];
			}
			if(curRes < curMinBaseDis)
			{
				curMinBaseDis = curRes;
				curBaseMinID = i;
				curBaseDegree = curDegSum;
			}
			if(curRes == curMinBaseDis && curDegSum > curBaseDegree)
			{
				curMinBaseDis = curRes;
				curBaseMinID = i;
				curBaseDegree = curDegSum;
			}
//			cout << "i" << " is a valid Level 0 candidate, with the similarity of" << curMinBaseDis << endl;
//			cout << "and significant value of " << curDegSum << endl;
//			cout << "Current most similar node (Comp): " << curBaseMinID << endl;
			ValidRegionInf vri;
			vri.isCompo = false; vri.matchValue = curRes; vri.regionID = i;
			vri.degree = curDegSum; vri.regionLevel = 0;
			inf.validBaseRegions.push_back(vri);
		}
	}

	if(curMinL0Dis < curMinBaseDis || (curMinL0Dis == curMinBaseDis && curDegree > curBaseDegree))
	{
		inf.regionID = curMinID;
		inf.regionLevel = 0;
		inf.matchValue = curMinL0Dis;
		inf.isCompo = true;
	}
	else 
	{
		inf.regionID = curBaseMinID;
		inf.regionLevel = 0;
		inf.matchValue = curMinBaseDis;
		inf.isCompo = false;
	}

	double time_Finish = (double)clock(); 
	cout << " \n =======> operate time:" << (time_Finish-time_Start);
}

void NetExplorer::oneFindRegRes_file( string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath )
{
	string this_ne_outdir = outdirS.c_str();
	_ne_new_outdir = outdirS.c_str();
	_resSaveDirS = resSaveDirS;
	string imgA_path = string(sourcePath);
	string temp = string(this_ne_outdir) + sectionS + "\\";
	this_ne_outdir = temp.c_str();
	if(!boost::filesystem::exists(this_ne_outdir))
		boost::filesystem::create_directory(this_ne_outdir);
	string patchesFN = string(this_ne_outdir) + "structPat.txt";
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
	summer.initialRandPick(sectionS, this_ne_outdir, imgA, 6);
	summer.secName = sectionS;
	_thresForMinDist = _patchSimThres;
	if(boost::filesystem::exists(patchesFN))
	{
		//load 1-baseMap, n-cntMap, 1-baseRel, n-cntRel
		vector<cv::Mat> comMaps, contxRelMaps;
		cv::Mat baseMap, baseRelMap;
		int cntxLevelCount = 0;
		string fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
			+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		string fnBaseMap = string(this_ne_outdir) + sectionS + "_bmap.png";
		string fnBaseContxs = string(this_ne_outdir) + sectionS + "_bcxt.png";
		string	fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		
		baseMap = cv::imread(fnBaseMap, 1);
		baseRelMap = cv::imread(fnBaseContxs,0);
		int baseNodeN = baseRelMap.rows / 5;

		vector<int> comNodeNumbers;
		while(boost::filesystem::exists(fnCntxMap))
		{
			cv::Mat tempCntxMap, tempCntxRelMap;
			tempCntxMap = cv::imread(fnCntxMap, 1);
			tempCntxRelMap = cv::imread(fnComContxs, 0);
			comMaps.push_back(tempCntxMap); contxRelMaps.push_back(tempCntxRelMap);
			int nc = contxRelMaps[cntxLevelCount].rows/5;
			comNodeNumbers.push_back(nc);
			cntxLevelCount ++;

			fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
			fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		}

		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
			//only use the patches
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			sp.aveClr = summer._averImg.ptr<Vec3b>(y)[x];
			sp.aveGrad = summer._aveGrad.ptr<uchar>(y)[x];
			summer._finalPatches.push_back(sp);		
//			cv::Mat reg;
//			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
//			reg = imread(fn, 0);
//			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}
		//每一个user related patch只找最像的那个块，跟它有相邻关系的才予以考虑

 		SecInf inf; inf._dists.resize(_userRelPatches.size()); inf._secN = summer.secName;
		
	//	vector<Dist_ID> inf._bestPatDis;

		int tempD; int flipped = 1; bool validImg = true; float matchValue = 0;
		for (int m = 0; m < _userRelPatches.size(); m++)
		{
			cv::Mat& curUPat = _userRelPatches[m];
			cv::Mat curCPat;
			Dist_ID did;
			int minDis = 1E20, minID = 0;
			for (int n = 0; n < summer._finalPatches.size(); n++)
			{
				summer.getPatchFromImg(summer._srcImg, curCPat, summer._finalPatches[n]);
				tempD = Shared::distForTwoPat(curCPat, curUPat, 
					summer._finalPatches[n].aveClr, _userRelsPatches[m].aveClr,
					summer._finalPatches[n].aveGrad, _userRelsPatches[m].aveGrad,
					flipped);
				if(tempD < minDis)
				{	minDis = tempD; minID = n; }
			}
			CmLog::LogLine(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "min value for %s is %d, ##ID = %d\n",
				summer.secName.c_str(), minDis, minID);

			if(minDis > _thresForMinDist)
			{
				CmLog::LogLine( FOREGROUND_RED | FOREGROUND_INTENSITY, "Too large difference %s, with value %d", summer.secName.c_str(), minDis);
				validImg = false;
				return;
			}
			did.dist = minDis; did.ID = minID;
			matchValue += minDis;
			inf._bestPatDis.push_back(did);
		}

		if(matchValue > 2 * _thresForMinDist - 200)
		{
			validImg = false;
			return;
		}

		//Find in base nodes:
		vector<cv::Mat> tContxtBestPats; tContxtBestPats.resize(inf._bestPatDis.size());
		float coverN;
		vector<pair<float, int>> matchScoresBase; vector<vector<pair<float, int>>> matchScoresCom;
		bool flag = true; bool hasValidRes = false;
		pair<float, int> bestMatchBase; vector<pair<float, int>> bestMatchsCom;
		bestMatchBase.first = 1E20;
		for (int i = 0; i < baseNodeN; i++)
		{
			flag = true;
			pair<float, int> p; p.first = 0; p.second = i;
			for (int j = 0; j < inf._bestPatDis.size(); j++)
			{
				SingleNet::getCntxFromMap(tContxtBestPats[j], baseRelMap, i, inf._bestPatDis[j].ID);
//				CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
//				Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
				if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
				{
					p.first += coverN;
					cout << "Covered!!!!!!!!!" <<endl;
				}
				else
				{	flag = false; break;}
			}
			if(flag)
			{  
				matchScoresBase.push_back(p);	
				if(p.first < bestMatchBase.first)
					bestMatchBase = p;
				hasValidRes = true;
			}
		}
		//Find in com nodes:
 		for (int L = 0; L < cntxLevelCount && L < 1; L++)
		{
			vector<pair<float, int>> matchScoresComL;
			matchScoresCom.push_back(matchScoresComL);
			pair<float, int> bestMatchComL;
			bestMatchComL.first = 1E20;
			for (int i = 0; i < comNodeNumbers[L]; i++)
			{
				flag = true;
				pair<float, int> p; p.first = 0; p.second = i;
				for (int j = 0; j < inf._bestPatDis.size(); j++)
				{
					SingleNet::getCntxFromMap(tContxtBestPats[j], contxRelMaps[L], i, inf._bestPatDis[j].ID);
		//			CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
	//				Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
					if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
					{
						p.first += coverN;
		//				cout << "Covered!!!!!!!!!" <<endl;
					}
					else
					{	flag = false; break;}
				}
				if(flag)
				{	
					matchScoresCom[L].push_back(p);	
					if(p.first < bestMatchComL.first)
					bestMatchComL = p;
					hasValidRes = true;
				}
			}
			bestMatchsCom.push_back(bestMatchComL);
		}		

		//Then get the region of them

		//Show the result:
		if(hasValidRes)
		{		
			cv::Mat tttShow;
			summer._srcImg.copyTo(tttShow);
			cv::Scalar clr(0, 255, 255);
			if(inf.matchValue > 1E10)
				clr = cv::Scalar(0, 0, 255);
			cv::Mat drawMask;
			tttShow.copyTo(drawMask); drawMask.setTo(0);

			cv::Mat Pallete; Pallete.create(summer._srcImg.size(), CV_8UC1);
			vector<vector<Point>> foundRegions;
			if(bestMatchsCom.size() > 0)
				CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY,
				"Min values -- Com: %f, Base: %f",bestMatchsCom[0].first , bestMatchBase.first);

			if(bestMatchsCom[0].first < bestMatchBase.first)
			{
				inf.regionID = bestMatchsCom[0].second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = true;		
				CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Come from compound\n");
				this->getRegionFromMap(Pallete, comMaps[0], bestMatchsCom[0].second, foundRegions);
			}
			else 
			{
				inf.regionID = bestMatchBase.second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = false;
				this->getRegionFromMap(Pallete, baseMap, bestMatchBase.second, foundRegions);
			}
			if(foundRegions.size()>0)
			{
				if(cv::contourArea(foundRegions[0]) < 500)
					return;
				Shared::drawPoly(tttShow, foundRegions[0], clr, 2);
				Shared::drawPoly(drawMask, foundRegions[0], cv::Scalar(255,255,255), 1, true);
				SHOW_IMG(tttShow, 1);
				cv::imwrite(_resSaveDirS + sectionS + ".jpg", summer._srcImg);
				cv::imwrite(_resSaveDirS + sectionS + ".png", drawMask);
				_infsWithSecs.push_back(inf);
			}
		}
		//End if it is valid image
	}
}
//Parrallel version
void NetExplorer::oneFindRegRes_file(SecInf& inf, string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath)
{
	string this_ne_outdir = outdirS.c_str();
	string this_ne_new_outdir = outdirS.c_str();
	string this_resSaveDirS = resSaveDirS;
	string imgA_path = string(sourcePath);
	string temp = string(this_ne_outdir) + sectionS + "\\";
	this_ne_outdir = temp.c_str();
	if(!boost::filesystem::exists(this_ne_outdir))
		boost::filesystem::create_directory(this_ne_outdir);
	string patchesFN = string(this_ne_outdir) + "structPat.txt";
	
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	
	if (imgA.rows >= 800 || imgA.cols >= 800)
		if(sectionS.compare("bag222") != 0)
			cv::resize(imgA,imgA, cv::Size(640, 480));
	summer.initialRandPick(sectionS, this_ne_outdir, imgA, 6);
	summer.secName = sectionS;
	_thresForMinDist = _patchSimThres;

	if(boost::filesystem::exists(patchesFN))
	{
		//load 1-baseMap, n-cntMap, 1-baseRel, n-cntRel
		vector<cv::Mat> comMaps, contxRelMaps;
		cv::Mat baseMap, baseRelMap;
		int cntxLevelCount = 0;
		string fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
			+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		string fnBaseMap = string(this_ne_outdir) + sectionS + "_bmap.png";
		string fnBaseContxs = string(this_ne_outdir) + sectionS + "_bcxt.png";
		string	fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
			+ boost::lexical_cast<string>(cntxLevelCount) + ".png";

		baseMap = cv::imread(fnBaseMap, 1);
		baseRelMap = cv::imread(fnBaseContxs,0);
		int baseNodeN = baseRelMap.rows / 5;

		vector<int> comNodeNumbers;
		while(boost::filesystem::exists(fnCntxMap))
		{
			cv::Mat tempCntxMap, tempCntxRelMap;
			tempCntxMap = cv::imread(fnCntxMap, 1);
			tempCntxRelMap = cv::imread(fnComContxs, 0);
			comMaps.push_back(tempCntxMap); contxRelMaps.push_back(tempCntxRelMap);
			int nc = contxRelMaps[cntxLevelCount].rows/5;
			comNodeNumbers.push_back(nc);
			cntxLevelCount ++;

			fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
			fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		}

		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
			//only use the patches
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			sp.aveClr = summer._averImg.ptr<Vec3b>(y)[x];
			sp.aveGrad = summer._aveGrad.ptr<uchar>(y)[x];
			summer._finalPatches.push_back(sp);		
			//			cv::Mat reg;
			//			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			//			reg = imread(fn, 0);
			//			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}
		//for every user related patch, only find the most similar one

		//inf._dists.resize(_userRelPatches.size()); 
		inf._secN = sectionS;
		inf.matchValue = 1E20;
		inf.isValid = false;
		//	vector<Dist_ID> inf._bestPatDis;

		int tempD; int flipped = 1; bool validImg = true; float matchValue = 0;
		for (int m = 0; m < _userRelPatches.size(); m++)
		{
			cv::Mat& curUPat = _userRelPatches[m];
			cv::Mat curCPat;
			Dist_ID did;
			float minDis = 1E20, minID = 0;
			for (int n = 0; n < summer._finalPatches.size(); n++)
			{
				summer.getPatchFromImg(summer._srcImg, curCPat, summer._finalPatches[n]);			
				tempD = Shared::distForTwoPat(curCPat, curUPat, 
			//		summer._finalPatches[n].aveClr, _userRelsPatches[m].aveClr,
			//		summer._finalPatches[n].aveGrad, _userRelsPatches[m].aveGrad,
					flipped);
				if(tempD < minDis)
				{	minDis = tempD; minID = n; }
			}
			CmLog::LogLine(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "min value for %s is %d, ##ID = %d\n",
				summer.secName.c_str(), minDis, minID);

			if(minDis > _thresForMinDist)
			{
				CmLog::LogLine( FOREGROUND_RED | FOREGROUND_INTENSITY, "Too large difference %s, with value %d", summer.secName.c_str(), minDis);
				validImg = false;
				return;
			}
			did.dist = minDis; did.ID = minID;
			matchValue += minDis;
			inf._bestPatDis.push_back(did);
		}

		if(matchValue > 2 * _thresForMinDist - 250)
		{
			validImg = false;
			return;
		}

		//Find in base nodes:
		vector<cv::Mat> tContxtBestPats; tContxtBestPats.resize(inf._bestPatDis.size());
		float coverN;
		vector<pair<float, int>> matchScoresBase; vector<vector<pair<float, int>>> matchScoresCom;
		bool flag = true; bool hasValidRes = false;
		pair<float, int> bestMatchBase; vector<pair<float, int>> bestMatchsCom;
		bestMatchBase.first = 1E20;
		int areaBaseRes = 0, areaComRes = 0;
		for (int i = 0; i < baseNodeN; i++)
		{
			flag = true;
			pair<float, int> p; p.first = 0; p.second = i;
			int tArea = baseRelMap.ptr<uchar>(baseRelMap.rows - 1)[i];
			if(tArea < 2)
				continue;
			for (int j = 0; j < inf._bestPatDis.size(); j++)
			{
				SingleNet::getCntxFromMap(tContxtBestPats[j], baseRelMap, i, inf._bestPatDis[j].ID);
				//				CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
#ifdef _DEBUG
				//Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
#endif				
				if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
				{
					p.first += coverN;
					cout << "Covered!!!!!!!!!" <<endl;
				}
				else
				{	flag = false; break;}
			}
			if(flag)
			{  
				matchScoresBase.push_back(p);	
				if(p.first < bestMatchBase.first && tArea > areaBaseRes / 25)
				{
					bestMatchBase = p;
					areaBaseRes = tArea;
				}
				hasValidRes = true;
			}
		}
		//Find in com nodes:
		for (int L = 0; L < cntxLevelCount && L < 1; L++)
		{
			vector<pair<float, int>> matchScoresComL;
			matchScoresCom.push_back(matchScoresComL);
			pair<float, int> bestMatchComL;
			bestMatchComL.first = 1E20; 
			float curArea = 1E20;
			areaComRes = 0;
			for (int i = 0; i < comNodeNumbers[L]; i++)
			{
				flag = true;
				pair<float, int> p; p.first = 0; p.second = i;
				int tArea = contxRelMaps[L].ptr<uchar>(contxRelMaps[L].rows-1)[i];
				if(tArea < 2)
					continue;
				for (int j = 0; j < inf._bestPatDis.size(); j++)
				{
					SingleNet::getCntxFromMap(tContxtBestPats[j], contxRelMaps[L], i, inf._bestPatDis[j].ID);
					//			CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
#ifdef _DEBUG
					//Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
#endif
  					if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
					{
						p.first += coverN;
						//				cout << "Covered!!!!!!!!!" <<endl;
					}
					else
					{	flag = false; break;}
				}
				if(flag)
				{	
					matchScoresCom[L].push_back(p);	
 					if(p.first < bestMatchComL.first && tArea > areaComRes / 25)
					{
						bestMatchComL = p;
						areaComRes = tArea;
					}
					hasValidRes = true;
				}
			}
			bestMatchsCom.push_back(bestMatchComL);
		}		

		//Then get the region of them

		//Show the result:
 		if(hasValidRes)
		{		
			cv::Mat tttShow;
			summer._srcImg.copyTo(tttShow);
			cv::Scalar clr(0, 255, 255);
	//		if(inf.matchValue > 1E10)
	//			clr = cv::Scalar(0, 0, 255);
			cv::Mat drawMask;
			tttShow.copyTo(drawMask); drawMask.setTo(0);

			cv::Mat Pallete; Pallete.create(summer._srcImg.size(), CV_8UC1);
			vector<vector<Point>> foundRegions;
			if(bestMatchsCom.size() > 0)
				CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY,
				"Min values -- Com: %f, Base: %f",bestMatchsCom[0].first , bestMatchBase.first);

			if(bestMatchsCom[0].first < bestMatchBase.first && areaComRes > areaBaseRes / 25)
			{
				inf.regionID = bestMatchsCom[0].second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = true;		
				this->getRegionFromMap(Pallete, comMaps[0], bestMatchsCom[0].second, foundRegions);
				inf.matchedRegions.push_back(foundRegions);//record the comp region mine..
				CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Come from compound with region size: %d\n", foundRegions.size());
			}
			else 
			{
				inf.regionID = bestMatchBase.second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = false;
				this->getRegionFromMap(Pallete, baseMap, bestMatchBase.second, foundRegions);		
				inf.matchedRegions.push_back(foundRegions);;//record the comp region mine..
				CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Come from base with region size: %d\n", foundRegions.size());

			}
			if(foundRegions.size()>0)
			{
				bool tFlag = false;
				int maxRID = 0; int maxAArea = 0, tempA = 0;
				for (int tti = 0; tti < foundRegions.size(); tti++)
				{
					tempA = cv::contourArea(foundRegions[tti]);
					if(tempA >= maxAArea)
					{	maxRID = tti; maxAArea = tempA;}
				}
				if(maxAArea > 500)
					for (int ti = 0; ti < foundRegions.size(); ti++)
					{
						tempA = cv::contourArea(foundRegions[ti]);
						if(tempA > 500)
						{	
							if(tempA > 15000 && tempA / cv::arcLength(foundRegions[ti], true) < 20)
							{
								tFlag = false;
								continue;
							}
							Shared::drawPoly(drawMask, foundRegions[ti], cv::Scalar(255,255,255), 1, true);
								//SHOW_IMG(tttShow, 1);
							//imshow("drawMask",drawMask);waitKey(0);
							CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY, 
								"%s, area:%lf, length:%lf\n", this_resSaveDirS.c_str(), cv::contourArea(foundRegions[ti]),
								cv::arcLength(foundRegions[ti], true)
								);
							tFlag = true;
						}
					}
			//	Shared::drawPoly(tttShow, foundRegions[0], clr, 2);
				if(tFlag)
				{
					cv::imwrite(this_resSaveDirS + sectionS + ".jpg", summer._nonBlured);
		//			cv::imwrite(this_resSaveDirS + sectionS + ".jpg", summer._srcImg);
					cv::imwrite(this_resSaveDirS + sectionS + ".png", drawMask);
					inf.isValid = true;
				}
					//	_infsWithSecs.push_back(inf); For openmp, this has already in the vector
			}
		}

		//End if it is valid image
	}
}

void NetExplorer::oneFindRegRes_file_mine(SecInf& inf, string& sectionS, string& outdirS, string& outDirR, string& resSaveDirS, string& sourcePath){
	
	string this_ne_outdir = outdirS.c_str();
	string this_ne_new_outdir = outdirS.c_str();
	string this_resSaveDirS = resSaveDirS;
	string imgA_path = string(sourcePath);
	string temp = string(this_ne_outdir) + sectionS + "\\";
	this_ne_outdir = temp.c_str();

	string this_ne_outdirR = outDirR.c_str();
	string this_ne_new_outdirR = outDirR.c_str();
	string this_resSaveDirR = resSaveDirS;
	string imgA_pathR = string(sourcePath);
	string tempR = string(this_ne_outdirR) + sectionS + "\\";
	this_ne_outdirR = tempR.c_str();

	if(!boost::filesystem::exists(this_ne_outdir))
		boost::filesystem::create_directory(this_ne_outdir);
	if(!boost::filesystem::exists(this_ne_outdirR))
		boost::filesystem::create_directory(this_ne_outdirR);

	string patchesFN = string(this_ne_outdir) + "structPat.txt";
	string patchesFNR = string(this_ne_outdirR) + "structPat.txt";
	
	Summrarizer summer,summerR;
	Mat imgA = imread(imgA_path);
	
	if (imgA.rows >= 800 || imgA.cols >= 800)
		if(sectionS.compare("bag222") != 0)
			cv::resize(imgA,imgA, cv::Size(640, 480));
	summer.initialRandPick(sectionS, this_ne_outdir, imgA, 6);
	summer.secName = sectionS;
	_thresForMinDist = _patchSimThres;

	summerR.initialRandPick(sectionS, this_ne_outdirR, imgA, 6);
	summerR.secName = sectionS;

	if(boost::filesystem::exists(patchesFN) && boost::filesystem::exists(patchesFNR) )
	{
		//load 1-baseMap, n-cntMap, 1-baseRel, n-cntRel 
		//for src image
		vector<cv::Mat> comMaps, contxRelMaps;
		cv::Mat baseMap, baseRelMap;
		int cntxLevelCount = 0;
		string fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
			+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		string fnBaseMap = string(this_ne_outdir) + sectionS + "_bmap.png";
		string fnBaseContxs = string(this_ne_outdir) + sectionS + "_bcxt.png";
		string	fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
			+ boost::lexical_cast<string>(cntxLevelCount) + ".png";

		baseMap = cv::imread(fnBaseMap, 1);
		baseRelMap = cv::imread(fnBaseContxs,0);
		int baseNodeN = baseRelMap.rows / 5;

		vector<int> comNodeNumbers;
		while(boost::filesystem::exists(fnCntxMap))
		{
			cv::Mat tempCntxMap, tempCntxRelMap;
			tempCntxMap = cv::imread(fnCntxMap, 1);
			tempCntxRelMap = cv::imread(fnComContxs, 0);
			comMaps.push_back(tempCntxMap); contxRelMaps.push_back(tempCntxRelMap);
			int nc = contxRelMaps[cntxLevelCount].rows/5;
			comNodeNumbers.push_back(nc);
			cntxLevelCount ++;

			fnCntxMap = string(this_ne_outdir) + sectionS + "_cmap"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
			fnComContxs = string(this_ne_outdir) + sectionS + "_ccxt"
				+ boost::lexical_cast<string>(cntxLevelCount) + ".png";
		}

		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
			//only use the patches
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			sp.aveClr = summer._averImg.ptr<Vec3b>(y)[x];
			sp.aveGrad = summer._aveGrad.ptr<uchar>(y)[x];
			summer._finalPatches.push_back(sp);		
			//			cv::Mat reg;
			//			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			//			reg = imread(fn, 0);
			//			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}//end for src image

		//for region image
		vector<cv::Mat> comMapsR, contxRelMapsR;
		cv::Mat baseMapR, baseRelMapR;
		int cntxLevelCountR = 0;
		string fnCntxMapR = string(this_ne_outdirR) + sectionS + "_cmap"
			+ boost::lexical_cast<string>(cntxLevelCountR) + ".png";
		string fnBaseMapR = string(this_ne_outdirR) + sectionS + "_bmap.png";
		string fnBaseContxsR = string(this_ne_outdirR) + sectionS + "_bcxt.png";
		string	fnComContxsR = string(this_ne_outdirR) + sectionS + "_ccxt"
			+ boost::lexical_cast<string>(cntxLevelCountR) + ".png";

		baseMapR = cv::imread(fnBaseMapR, 1);
		baseRelMapR = cv::imread(fnBaseContxsR,0);
		int baseNodeNR = baseRelMapR.rows / 5;//end for region images

		vector<int> comNodeNumbersR;
		while(boost::filesystem::exists(fnCntxMapR))
		{
			cv::Mat tempCntxMapR, tempCntxRelMapR;
			tempCntxMapR = cv::imread(fnCntxMapR, 1);
			tempCntxRelMapR = cv::imread(fnComContxsR, 0);
			comMapsR.push_back(tempCntxMapR); contxRelMapsR.push_back(tempCntxRelMapR);
			int ncR = contxRelMapsR[cntxLevelCountR].rows/5;
			comNodeNumbersR.push_back(ncR);
			cntxLevelCountR ++;

			fnCntxMapR = string(this_ne_outdirR) + sectionS + "_cmap"
				+ boost::lexical_cast<string>(cntxLevelCountR) + ".png";
			fnComContxsR = string(this_ne_outdirR) + sectionS + "_ccxt"
				+ boost::lexical_cast<string>(cntxLevelCountR) + ".png";
		}

		ifstream ifsR(patchesFNR);
		int xR, yR; int curCountR = 0;
		summerR._finalPatches.clear();
		summerR._finalRegions.clear();
		while(!ifsR.eof())
		{
			//only use the patches
			ifsR >> xR;
			if(ifsR.eof())
				break;
			ifsR >> yR;
			sPatch spR;
			spR.center.x = xR; spR.center.y = yR;
			spR.width = summerR._patchRadius * 2 + 1;
			spR.aveClr = summerR._averImg.ptr<Vec3b>(yR)[xR];
			spR.aveGrad = summerR._aveGrad.ptr<uchar>(yR)[xR];
			summerR._finalPatches.push_back(spR);		
			//			cv::Mat reg;
			//			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			//			reg = imread(fn, 0);
			//			summer._finalRegions.push_back(reg.clone());
			curCountR ++;
		}//end for region image


		//for every user related patch, only find the most similar one

		//inf._dists.resize(_userRelPatches.size()); 
		inf._secN = sectionS;
		inf.matchValue = 1E20;
		inf.isValid = false;
		//	vector<Dist_ID> inf._bestPatDis;

		int tempD; int flipped = 1; bool validImg = true; float matchValue = 0;
		for (int m = 0; m < _userRelPatches.size(); m++)
		{
			cv::Mat& curUPat = _userRelPatches[m];
			cv::Mat curCPat;
			Dist_ID did;
			float minDis = 1E20, minID = 0;
			for (int n = 0; n < summer._finalPatches.size(); n++)
			{
				summer.getPatchFromImg(summer._srcImg, curCPat, summer._finalPatches[n]);			
				tempD = Shared::distForTwoPat(curCPat, curUPat, 
			//		summer._finalPatches[n].aveClr, _userRelsPatches[m].aveClr,
			//		summer._finalPatches[n].aveGrad, _userRelsPatches[m].aveGrad,
					flipped);
				if(tempD < minDis)
				{	minDis = tempD; minID = n; }
			}
			CmLog::LogLine(FOREGROUND_GREEN | FOREGROUND_INTENSITY, "min value for %s is %d, ##ID = %d\n",
				summer.secName.c_str(), minDis, minID);

			if(minDis > _thresForMinDist)
			{
				CmLog::LogLine( FOREGROUND_RED | FOREGROUND_INTENSITY, "Too large difference %s, with value %d", summer.secName.c_str(), minDis);
				validImg = false;
				return;
			}
			did.dist = minDis; did.ID = minID;
			matchValue += minDis;
			inf._bestPatDis.push_back(did);
		}

		if(matchValue > 2 * _thresForMinDist - 250)
		{
			validImg = false;
			return;
		}

		//Find in base nodes:
		vector<cv::Mat> tContxtBestPats; tContxtBestPats.resize(inf._bestPatDis.size());
		float coverN;
		vector<pair<float, int>> matchScoresBase; vector<vector<pair<float, int>>> matchScoresCom;
		bool flag = true; bool hasValidRes = false;
		pair<float, int> bestMatchBase; vector<pair<float, int>> bestMatchsCom;
		bestMatchBase.first = 1E20;
		int areaBaseRes = 0, areaComRes = 0;
		for (int i = 0; i < baseNodeNR; i++)
		{
			flag = true;
			pair<float, int> p; p.first = 0; p.second = i;
			int tArea = baseRelMapR.ptr<uchar>(baseRelMapR.rows - 1)[i];
			if(tArea < 2)
				continue;
			for (int j = 0; j < inf._bestPatDis.size(); j++)
			{
				SingleNet::getCntxFromMap(tContxtBestPats[j], baseRelMapR, i, inf._bestPatDis[j].ID);
				//				CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
#ifdef _DEBUG
				//Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
#endif				
				if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
				{
					p.first += coverN;
					cout << "Covered!!!!!!!!!" <<endl;
				}
				else
				{	flag = false; break;}
			}
			if(flag)
			{  
				matchScoresBase.push_back(p);	
				if(p.first < bestMatchBase.first && tArea > areaBaseRes / 25)
				{
					bestMatchBase = p;
					areaBaseRes = tArea;
				}
				hasValidRes = true;
			}
		}
		//Find in com nodes:
		for (int L = 0; L < cntxLevelCountR && L < 1; L++)
		{
			vector<pair<float, int>> matchScoresComL;
			matchScoresCom.push_back(matchScoresComL);
			pair<float, int> bestMatchComL;
			bestMatchComL.first = 1E20; 
			float curArea = 1E20;
			areaComRes = 0;
			for (int i = 0; i < comNodeNumbersR[L]; i++)
			{
				flag = true;
				pair<float, int> p; p.first = 0; p.second = i;
				int tArea = contxRelMapsR[L].ptr<uchar>(contxRelMapsR[L].rows-1)[i];
				if(tArea < 2)
					continue;
				for (int j = 0; j < inf._bestPatDis.size(); j++)
				{
					SingleNet::getCntxFromMap(tContxtBestPats[j], contxRelMapsR[L], i, inf._bestPatDis[j].ID);
					//			CmLog::LogLine(FOREGROUND_RED, "node num: %d, rels to: %d \n", i, j);
#ifdef _DEBUG
					//Shared::showMat(_userValidRels[j]); Shared::showMat(tContxtBestPats[j]);
#endif
  					if(this->isCntxCovered_f_b(_userValidRels[j], tContxtBestPats[j], coverN))
					{
						p.first += coverN;
						//				cout << "Covered!!!!!!!!!" <<endl;
					}
					else
					{	flag = false; break;}
				}
				if(flag)
				{	
					matchScoresCom[L].push_back(p);	
 					if(p.first < bestMatchComL.first && tArea > areaComRes / 25)
					{
						bestMatchComL = p;
						areaComRes = tArea;
					}
					hasValidRes = true;
				}
			}
			bestMatchsCom.push_back(bestMatchComL);
		}		

		//Then get the region of them

		//Show the result:
 		if(hasValidRes)
		{		
			cv::Mat tttShow;
			summer._srcImg.copyTo(tttShow);
			cv::Scalar clr(0, 255, 255);
	//		if(inf.matchValue > 1E10)
	//			clr = cv::Scalar(0, 0, 255);
			cv::Mat drawMask;
			tttShow.copyTo(drawMask); drawMask.setTo(0);

			cv::Mat Pallete; Pallete.create(summer._srcImg.size(), CV_8UC1);
			vector<vector<Point>> foundRegions;
			if(bestMatchsCom.size() > 0)
				CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY,
				"Min values -- Com: %f, Base: %f",bestMatchsCom[0].first , bestMatchBase.first);

			if(bestMatchsCom[0].first < bestMatchBase.first && areaComRes > areaBaseRes / 25)
			{
				inf.regionID = bestMatchsCom[0].second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = true;		
				this->getRegionFromMap(Pallete, comMaps[0], bestMatchsCom[0].second, foundRegions);
				inf.matchedRegions.push_back(foundRegions);//record the comp region mine..
				CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Come from compound with region size: %d\n", foundRegions.size());
			}
			else 
			{
				inf.regionID = bestMatchBase.second;
				inf.regionLevel = 0;
				inf.matchValue = matchValue;
				inf.isCompo = false;
				this->getRegionFromMap(Pallete, baseMap, bestMatchBase.second, foundRegions);		
				inf.matchedRegions.push_back(foundRegions);;//record the comp region mine..
				CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "Come from base with region size: %d\n", foundRegions.size());

			}
			if(foundRegions.size()>0)
			{
				bool tFlag = false;
				int maxRID = 0; int maxAArea = 0, tempA = 0;
				for (int tti = 0; tti < foundRegions.size(); tti++)
				{
					tempA = cv::contourArea(foundRegions[tti]);
					if(tempA >= maxAArea)
					{	maxRID = tti; maxAArea = tempA;}
				}
				if(maxAArea > 500)
					for (int ti = 0; ti < foundRegions.size(); ti++)
					{
						tempA = cv::contourArea(foundRegions[ti]);
						if(tempA > 500)
						{	
							if(tempA > 15000 && tempA / cv::arcLength(foundRegions[ti], true) < 20)
							{
								tFlag = false;
								continue;
							}
							Shared::drawPoly(drawMask, foundRegions[ti], cv::Scalar(255,255,255), 1, true);
								//SHOW_IMG(tttShow, 1);
							//imshow("drawMask",drawMask);waitKey(0);
							CmLog::LogLine(FOREGROUND_BLUE|FOREGROUND_INTENSITY, 
								"%s, area:%lf, length:%lf\n", this_resSaveDirS.c_str(), cv::contourArea(foundRegions[ti]),
								cv::arcLength(foundRegions[ti], true)
								);
							tFlag = true;
						}
					}
			//	Shared::drawPoly(tttShow, foundRegions[0], clr, 2);
				if(tFlag)
				{
					cv::imwrite(this_resSaveDirS + sectionS + ".jpg", summer._nonBlured);
		//			cv::imwrite(this_resSaveDirS + sectionS + ".jpg", summer._srcImg);
					cv::imwrite(this_resSaveDirS + sectionS + ".png", drawMask);
					inf.isValid = true;
				}
					//	_infsWithSecs.push_back(inf); For openmp, this has already in the vector
			}
		}

		//End if it is valid image
	}
}

void NetExplorer::oneFindRegionRes( string& sectionS, string& outdirS, string& resSaveDirS, string& sourcePath )
{
//	section = sectionS.c_str();
	_ne_outdir = outdirS.c_str();
	_resSaveDirS = resSaveDirS;
	string imgA_path = string(sourcePath);
	string temp = string(_ne_outdir) + sectionS + "\\";
	_ne_outdir = temp.c_str();
	if(!boost::filesystem::exists(_ne_outdir))
		boost::filesystem::create_directory(_ne_outdir);
	string patchesFN = string(_ne_outdir) + "rpatches.txt";
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
	summer.initialRandPick(sectionS, _ne_outdir, imgA, 6);
	summer.secName = sectionS;

	if(boost::filesystem::exists(patchesFN))
	{
		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
				//only use the patches
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			sp.aveClr = summer._averImg.ptr<Vec3b>(y)[x];
			sp.aveGrad = summer._aveGrad.ptr<uchar>(y)[x];
			summer._finalPatches.push_back(sp);		

			cv::Mat reg;
			string fn = string(_ne_outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			reg = imread(fn, 0);
			summer._finalRegions.push_back(reg.clone());

			curCount ++;
		}
 		SingleNet net1(&summer);
		net1.initNet(_ne_section, _ne_outdir, _ne_new_outdir);
		//		net1.getSimUserRel();
		net1.getSubRegions();
		net1.getAtoms();
		//Get the structure node similarity to filter
		SecInf inf; inf._dists.resize(_userRelPatches.size()); inf._secN = summer.secName;
		float dist; int flipped;
		cv::Mat tempDraw;
		vector<vector<Dist_ID>> tDist_IDs;
		tDist_IDs.resize(_userRelPatches.size());
		bool flag = true;
		for (int i = 0; i < _userRelPatches.size(); i++)
		{
			summer._srcImg.copyTo(tempDraw);
			cv::Mat& curUPat = _userRelPatches[i];
			sPatch& curUsP = _userRelsPatches[i];
			inf._dists[i].resize(summer._finalPatches.size());
			for (int j = 0; j < summer._finalPatches.size(); j++)
			{
				if(false == net1._isBaseNodesVir[j])
				{
					inf._dists[i][j] = 1E20;
					continue;
				}
				dist = abs(curUsP.aveClr[0] - summer._finalPatches[j].aveClr[0])
					+ abs(curUsP.aveClr[1] - summer._finalPatches[j].aveClr[1])
					+ abs(curUsP.aveClr[2] - summer._finalPatches[j].aveClr[2]);
				if(dist > 120)  
				{	
					inf._dists[i][j] = 1E20;
					continue;
				}
				cv::Mat curCPat;
				summer.getPatchFromImg(summer._srcImg, curCPat, summer._finalPatches[j]);
					//		inf._sims[i].push_back(Shared::distForTwoPat(curUPat, curCPat, ))	
				inf._dists[i][j] = Shared::distForTwoPat(curCPat, curUPat, flipped);
				Dist_ID d_i; d_i.dist = inf._dists[i][j]; d_i.ID = j; d_i.valid = false;
				tDist_IDs[i].push_back(d_i);
					///! for show
				string s = boost::lexical_cast<string>(inf._dists[i][j]);
				Shared::drawPatchFontOnImg(summer._finalPatches[j], s, tempDraw, cv::Scalar(255, 0, 0));
			}
			SHOW_IMG(tempDraw, 1);

			sort(tDist_IDs[i].begin(), tDist_IDs[i].end(), lessThan);
			//Make the top 3 has valid value, others very large
			for(int k = 3; k < tDist_IDs[i].size(); k++)
				inf._dists[i][tDist_IDs[i][k].ID] = 1E20;
			if(tDist_IDs[i].size() > 0)
				if(tDist_IDs[i][0].dist > _patchSimThres)
					flag = false;
		}
	//End the pre filter		
	//Find the best region 
		//In one net, search all the l0-compound node
		if(false == flag)
		{
			cout << "XXXXXXXXXXXXXXXXXX " << inf._secN << "Unvalid candi image!" << endl;
			return;
		}
		cout << "YYYYYYYYYYYYYYYY" << inf._secN <<  "Vaild image" << endl;
		net1.buildHierGraph();
		net1.getRealContext();

		getCandidateRegion(inf, net1, tDist_IDs);
		cout << "YYYY" << inf._secN <<  "Get regions done!" << endl;

		//Show the result:
		cv::Mat tttShow;
		net1._summer->_srcImg.copyTo(tttShow);
		cv::Scalar clr(0, 255, 255);
		if(inf.matchValue > 1E10)
			clr = cv::Scalar(0, 0, 255);
		cv::Mat drawMask;
		tttShow.copyTo(drawMask); drawMask.setTo(0);
		if(inf.isCompo)
		{
			Shared::drawPoly(tttShow, net1._comNodes[0][inf.regionID]._RegContour, clr, 2);
			Shared::drawPoly(drawMask, net1._comNodes[0][inf.regionID]._RegContour, cv::Scalar(255, 255, 255), 1, true);			
		}
		else
		{
			Shared::drawPoly(tttShow, net1._basicNodes[inf.regionID]._RegContour, cv::Scalar(255, 255, 120), 2);
			Shared::drawPoly(drawMask, net1._basicNodes[inf.regionID]._RegContour, cv::Scalar(255, 255, 255), 1, true);			
		}
		SHOW_IMG(tttShow, 1);
		cv::imwrite(_resSaveDirS + sectionS + ".jpg", net1._summer->_nonBlured);
		cv::imwrite(_resSaveDirS + sectionS + ".png", drawMask);
		drawResForUI(inf, net1, sectionS);
		_infsWithSecs.push_back(inf);
	}//End for an image where the patches have already calculated	
}

bool NetExplorer::isCntxCovered( cv::Mat& cntx1, cv::Mat& cntx2, float& coverScore )
{
	bool res = true;
	coverScore = 0;
	for (int i = 0; i < cntx1.rows; i++)
	{
		for (int j = 0; j < cntx1.cols; j++)
		{
			if(cntx1.ptr<float>(i)[j] > _userRelNumThr && cntx2.ptr<float>(i)[j] < _userRelNumThr2)
			{
				res = false;
//				cout << endl << "not covered at i: " << i << " j:" << j << " with value of " << cntx1.ptr<float>(i)[j] << " not coverd by" << cntx2.ptr<float>(i)[j] << endl;
				break;
			}
			if(cntx1.ptr<float>(i)[j] > _userRelNumThr && cntx2.ptr<float>(i)[j] > _userRelNumThr2)
				coverScore += cntx2.ptr<float>(i)[j];
		}
		if(res == false)
			break;
	}
	if(coverScore > 80000)
	{	cout << "Show Contxt" << endl;
//	Shared::showMat(cntx1); 
//	Shared::showMat(cntx2);
	}
	return res;
}

bool NetExplorer::isCntxCovered_f_b( cv::Mat& cntx1, cv::Mat& cntx2, float& coverScore )
{
	bool res = true;
	coverScore = 0;
	for (int i = 0; i < cntx1.rows; i++)
	{
		for (int j = 0; j < cntx1.cols; j++)
		{	
			if(cntx1.ptr<float>(i)[j] > _userRelNumThr && int(cntx2.ptr<uchar>(i)[j]) < _userRelNumThr2)
			{
				res = false;
	//			cout << endl << "not covered at i: " << i << " j:" << j << " with value of " << cntx1.ptr<float>(i)[j] << " not coverd by" << int(cntx2.ptr<uchar>(i)[j]) << endl;
				break;
			}
			if(cntx1.ptr<float>(i)[j] > _userRelNumThr && int(cntx2.ptr<uchar>(i)[j]) >= _userRelNumThr2)
			{
		//		cout << endl << "covered at i: " << i << " j:" << j << " with value of " << cntx1.ptr<float>(i)[j] << "coverd by" << int(cntx2.ptr<uchar>(i)[j]) << endl;
				coverScore += abs(int(cntx2.ptr<uchar>(i)[j])-cntx1.ptr<float>(i)[j]);
			}	
		}
		if(res == false)
			break;
	}

	return res;
}

void NetExplorer::drawResForUI( SecInf& inf, SingleNet& net1, string& sectionS)
{
	cv::Mat drawMask;
	cv::Mat ttShow;
	drawMask.create(net1._summer->_srcImg.rows, net1._summer->_srcImg.cols, CV_8UC1);
	sPatch sp1; sp1.width = 10; sp1.center = cv::Point(20,20);
	sPatch sp2; sp2.width = 10; sp2.center = cv::Point(20,50); 
	for (int i = 0; i < inf.validComRegions.size(); i++)
	{
		ValidRegionInf& curRegInf = inf.validComRegions[i];
		cv::Scalar clr(0, 255, 255);
		drawMask.setTo(0);
		Shared::drawPoly(drawMask, net1._comNodes[0][curRegInf.regionID]._RegContour, cv::Scalar(255), 1, true);			
		net1._summer->_nonBlured.copyTo(ttShow);
		Shared::drawPatchFontOnImg(sp1, boost::lexical_cast<string>(curRegInf.matchValue), ttShow, clr);
		Shared::drawPatchFontOnImg(sp2, boost::lexical_cast<string>(curRegInf.degree), ttShow, clr);
//		SHOW_IMG(drawMask, 1);
//		SHOW_IMG(net1._baseMap, 1);
		Shared::myXor(drawMask, net1._baseMap, drawMask);		
//		SHOW_IMG(drawMask, 1);
		cv::imwrite(_resSaveDirS + sectionS + "_c" + boost::lexical_cast<string>(i) + ".jpg", ttShow);
		cv::imwrite(_resSaveDirS + sectionS + "_c" + boost::lexical_cast<string>(i) + ".png", drawMask);
	}	
	for (int i = 0; i < inf.validBaseRegions.size(); i++)
	{
		ValidRegionInf& curRegInf = inf.validBaseRegions[i];
		drawMask.setTo(0);
		Shared::drawPoly(drawMask, net1._basicNodes[curRegInf.regionID]._RegContour, cv::Scalar(255), 1, true);			
		
		cv::Scalar clr(0, 255, 255);
		net1._summer->_nonBlured.copyTo(ttShow);
		Shared::drawPatchFontOnImg(sp1, boost::lexical_cast<string>(curRegInf.matchValue), ttShow, clr);
		Shared::drawPatchFontOnImg(sp2, boost::lexical_cast<string>(curRegInf.degree), ttShow, clr);
		cv::imwrite(_resSaveDirS + sectionS + "_b" + boost::lexical_cast<string>(i) + ".jpg", ttShow);
		cv::imwrite(_resSaveDirS + sectionS + "_b" + boost::lexical_cast<string>(i) + ".png", drawMask);
	}
}

void NetExplorer::getRegionFromMap( cv::Mat& outside_Palatte, cv::Mat& Map, int id, vector<vector<cv::Point>>& out_Regions )
{
//	vector<vector<Point>> contours;
	outside_Palatte.setTo(0);
	for (int i = 0; i < Map.rows; i++)
	{
		uchar* pP = outside_Palatte.ptr<uchar>(i);
		uchar* pM = Map.ptr<uchar>(i);
		for (int j = 0; j < Map.cols; j++, pP++, pM+=3)
		{
			if(pM[0] == 255 - id)
				pP[0] = 255;
		}
	}
	cv::dilate(outside_Palatte, outside_Palatte, cv::Mat(), cv::Point(-1,-1),2);
	cv::erode(outside_Palatte, outside_Palatte, cv::Mat(), cv::Point(-1,-1),5);
	cv::dilate(outside_Palatte, outside_Palatte, cv::Mat(), cv::Point(-1,-1),3);
 	cv::findContours(outside_Palatte, out_Regions, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
}

bool betterThan(SecInf d1, SecInf d2)
{
	return d1.matchValue < d2.matchValue;
}

void NetExplorer::compNodeInFolder_file( string& resSaveDirS )
{
	/*_ne_section = gCmSet.Val("section");
	_ne_outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");*/
	_ne_section = section;
	_ne_outdir = outdir;//src outdir
	_reg_outdir = regionOutDir;  // for regions outdir   =---mine

	srcDir = srcDir;

	string outdirS(_ne_outdir);

	fs::path fullpath(srcDir, fs::native);
	cout << "=======> Start comp node in folder: " << srcDir << " ### output to" << outdirS << endl;
	if(!fs::exists(fullpath)) return; 

	fs::recursive_directory_iterator end_iter;
	vector<string> imagePaths;
	vector<string> imageSs;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")||
				iter->path().extension() == string(".png")){
				imagePaths.push_back(currentImagePath);
				imageSs.push_back(currentImageS);
			}
		}
	}

	_infsWithSecs.resize(imagePaths.size());

#pragma omp parallel for
	for (int i = 0; i < imagePaths.size(); i++)
	{
		cout << "current image" << imagePaths[i] << endl;
		string currentImageBase = imageSs[i].substr(0, imageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;
		if(currentImageBase == string(_ne_section))
			continue;

		oneFindRegRes_file(_infsWithSecs[i], currentImageBase, outdirS, resSaveDirS, imagePaths[i]);
		cout << "end of one image" << endl;
	}

// 	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
// 	{
// 		if(!fs::is_directory(*iter)){
// 			cout << "current image" << iter->path().string() << endl;
// 			string currentImagePath = iter->path().string();
// 			string currentImageS = iter->path().filename();
// 			string currentImageBase = currentImageS.substr(0, currentImageS.find_first_of('.'));
// 			if(currentImageBase == string(_ne_section))
// 				continue;
// 			if(iter->path().extension() == string(".jpg"))
// 				cout << "current _ne_section: " << currentImageBase  << "-" << iter->path().native_file_string()<< endl;
// 
// 			//		oneCompPatRes(currentImageBase, outdirS, currentImagePath);
// 			oneFindRegRes_file(currentImageBase, outdirS, resSaveDirS, currentImagePath);
// 			cout << "end of one image" << endl;
// 		}
// 	}

	/*oneFindRegRes_file(_infsWithSecs[i], currentImageBase, outdirS, resSaveDirS, imagePaths[i]);
	_infsWithSecs记录结果
	_ne_section对应匹配的图像
	regionID匹配match的id
	可以从this->getRegionFromMap(Pallete, comMaps[0], bestMatchsCom[0].second, foundRegions);或
	comMaps是_ccx0....组成的vector<Mat>
	this->getRegionFromMap(Pallete, baseMap, bestMatchBase.second, foundRegions);	得到
	baseMap: _bmap Mat
	matchedRegions:的记录匹配的region  多个contour
	_bestpatdis:与userpatch对应的最近的patch  id:  _finalPatch[i];
	*/
	
	sort(_infsWithSecs, betterThan);

	for (int i = 0; i < _infsWithSecs.size(); i++)
	{
		if(_infsWithSecs[i].isValid == false)
			continue;
		CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "%s\n", _infsWithSecs[i]._secN.c_str());
	}

	/*Mat
	getRegionFromMap(*/

	cout << "##############File Run here###################" << endl;

}

void NetExplorer::compNodeInFolder_file_mine(string& resSaveDirS,vector<SecInf>& matchedSecs){

	/*_ne_section = gCmSet.Val("section");
	_ne_outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");*/
	_ne_section = section;
	_ne_outdir = outdir;//src outdir
	_ne_outdir = _userNet._summer->_sm_out;
	_reg_outdir = regionOutDir;  // for regions outdir   =---mine
	_reg_outdir = _regionNet._summer->_sm_out;
	srcDir = srcDir;

	string outDirR(_reg_outdir);
	//for src images
	string outdirS(_ne_outdir);

	fs::path fullpath(srcDir, fs::native);
	cout << "=======> Start comp node in folder: " << srcDir << " ### output to" << outdirS << endl;
	if(!fs::exists(fullpath)) return; 

	fs::recursive_directory_iterator end_iter;
	vector<string> imagePaths;
	vector<string> imageSs;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")||
				iter->path().extension() == string(".png")){
				imagePaths.push_back(currentImagePath);
				imageSs.push_back(currentImageS);
			}
		}
	}

	_infsWithSecs.resize(imagePaths.size());

	//for rergion images
	//string outdirR(_reg_outdir);

	//fs::path fullpathR(outdirR, fs::native);
	//cout << "=======> Start comp node in folder: " << outdirR << " ### output to" << outdirS << endl;
	//if(!fs::exists(fullpath)) return; 

	//fs::recursive_directory_iterator end_iterR;
	//vector<string> imagePathsR;
	//vector<string> imageSsR;
	//for (fs::recursive_directory_iterator iter(fullpathR); iter!=end_iterR; iter++)
	//{
	//	if(!fs::is_directory(*iter)){
	//		string currentImagePathR = iter->path().string();
	//		string currentImageSR = iter->path().filename().string();
	//		if(iter->path().extension() == string(".jpg")||
	//			iter->path().extension() == string(".png")){
	//			imagePathsR.push_back(currentImagePathR);
	//			imageSsR.push_back(currentImageSR);
	//		}
	//	}
	//}



#pragma omp parallel for
	for (int i = 0; i < imagePaths.size(); i++)
	{
		cout << "current image" << imagePaths[i] << endl;
		string currentImageBase = imageSs[i].substr(0, imageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;
		if(currentImageBase == string(_ne_section))
			continue;

		oneFindRegRes_file_mine(_infsWithSecs[i], currentImageBase, outdirS, outDirR, resSaveDirS, imagePaths[i]);
		cout << "end of one image" << endl;
	}

// 	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
// 	{
// 		if(!fs::is_directory(*iter)){
// 			cout << "current image" << iter->path().string() << endl;
// 			string currentImagePath = iter->path().string();
// 			string currentImageS = iter->path().filename();
// 			string currentImageBase = currentImageS.substr(0, currentImageS.find_first_of('.'));
// 			if(currentImageBase == string(_ne_section))
// 				continue;
// 			if(iter->path().extension() == string(".jpg"))
// 				cout << "current _ne_section: " << currentImageBase  << "-" << iter->path().native_file_string()<< endl;
// 
// 			//		oneCompPatRes(currentImageBase, outdirS, currentImagePath);
// 			oneFindRegRes_file(currentImageBase, outdirS, resSaveDirS, currentImagePath);
// 			cout << "end of one image" << endl;
// 		}
// 	}

	/*oneFindRegRes_file(_infsWithSecs[i], currentImageBase, outdirS, resSaveDirS, imagePaths[i]);
	_infsWithSecs记录结果
	_ne_section对应匹配的图像
	regionID匹配match的id
	可以从this->getRegionFromMap(Pallete, comMaps[0], bestMatchsCom[0].second, foundRegions);或
	comMaps是_ccx0....组成的vector<Mat>
	this->getRegionFromMap(Pallete, baseMap, bestMatchBase.second, foundRegions);	得到
	baseMap: _bmap Mat
	matchedRegions:的记录匹配的region  多个contour
	_bestpatdis:与userpatch对应的最近的patch  id:  _finalPatch[i];
	*/
	
	sort(_infsWithSecs, betterThan);
	if(matchedSecs.size() != 0)matchedSecs.clear();

	for (int i = 0; i < _infsWithSecs.size(); i++)
	{
		if(_infsWithSecs[i].isValid == false)
			continue;
		matchedSecs.push_back(_infsWithSecs[i]);
		CmLog::LogLine(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "%s\n", _infsWithSecs[i]._secN.c_str());
	}

	/*Mat
	getRegionFromMap(*/

	cout << "##############File Run here###################" << endl;
}





