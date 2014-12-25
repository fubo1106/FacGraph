#include "stdafx2.h"
#include "Summrarizer.h"
#include "LabelName.h"

Summrarizer::Summrarizer(void)
{
}


Summrarizer::~Summrarizer(void)
{

	cout << "decompose summer" << endl;
	cout << "_patches number:" << _summaryPatches.size() << endl; 
	_summaryPatches.clear();
	_loc_per_patch.clear();
	_spMasks.clear();
	_centersMasks.clear();
	_centersMasks.clear();
	_similaritys.clear();
	_allOverContours.clear();
	_finalPatches.clear();
	_finalRegions.clear();
	cout << "decompose end" << endl;

}
//New

void Summrarizer::RandPatchCluster( cv::Mat& img )
{
	cv::Mat gradMap8U, Atan32F;
	cv::Mat tShowImg;
	img.copyTo(tShowImg);
	Shared::getGradMap(img, gradMap8U, Atan32F);

	cv::imwrite(string(_sm_out) + "grad.png", gradMap8U);
}

void Summrarizer::initialRandPick( string section, string outdir, cv::Mat& img, int patchRad )
{
	_sm_sec = section;
	secName = section;
	//cv::countNonZero();
	//_paraTol = (*gpSet)("Tol");
	_paraTol = 200000;//default
	//_sm_out = outdir;
	_sm_out = outdir;
	string out = _sm_out + section +"\\" ;
	if(img.rows > 600 || img.cols > 600)
		_paraTol = _paraTol * 2;
	//_paraAveGradTol = (*gpSet)("RelGradTol");
	_paraAveGradTol = 4;
	_width = img.cols;
	_height = img.rows;
	_gradStep = 8;
	_patchRadius = patchRad;
	cv::Mat Atan32F;
	img.convertTo(_srcImg, img.type());
	_srcImg.copyTo(_nonBlured);
	Shared::getGradMap(_srcImg, _imgGrad, Atan32F);
	_imgGrad = _imgGrad + 1;
	//SHOW_IMG_TITLE("grad",_imgGrad,1);

	_inverseGrad = _imgGrad * (-1) + 255;
	_overallMask.create(_srcImg.size(), CV_8UC1);
	_overallMask.setTo(0);
	insideRect = cv::Rect(_patchRadius, _patchRadius, _overallMask.cols - 2 * _patchRadius, _overallMask.rows - 2 * _patchRadius);
	cv::Mat tempM;
	cv::cvtColor(_srcImg, tempM, CV_BGR2Lab);

//	cv::resize(tempM, _thumbLab, cv::Size(_width / (_patchRadius*2+1), _height / (_patchRadius*2+1)));
	cv::resize(tempM, _thumbLab, cv::Size(_width / (_patchRadius), _height / (_patchRadius)));
	cv::GaussianBlur(_srcImg, _averImg, cv::Size(_patchRadius * 2 + 1, _patchRadius * 2 + 1) , _patchRadius-1);
	cv::GaussianBlur(_imgGrad, _aveGrad, cv::Size(_patchRadius * 2 + 1, _patchRadius * 2 + 1) , _patchRadius-1);
	cv::imwrite(string(out) + "BigBlur.png", _averImg);
	cv::imwrite(string(out) + "BlurGrad.png", _aveGrad);
	cv::cvtColor(_averImg, _averImg, CV_BGR2Lab);
	cv::GaussianBlur(_thumbLab, _thumbLab, cv::Size(3,3), 1);
	//	cv::GaussianBlur(_thumbHSV, _thumbHSV, cv::Size(3,3), 0.5);
	vector<cv::Mat> Labs;
//	cv::cvtColor(_thumbLab, _thumbLab, CV_BGR2Lab);
	cv::split(_thumbLab, Labs);
	cv::imwrite(string(out) + "thumbb.png", Labs[2]);
	cv::imwrite(string(out) + "thumba.png", Labs[1]);	cv::imwrite(string(out) + "thumbL.png", Labs[0]);

	cv::imwrite(string(out) + "thumb.png", _thumbLab);
}

int transLocMatToSet(IN const Mat& cover_center, OUT vector<Point>& loc_set)
{
	loc_set.clear();
	for(int y=0; y< cover_center.rows;++y)
	{
		const uchar * ptr = cover_center.ptr<uchar>(y);
		for(int x=0;x< cover_center.cols;++x)
		{
			if(ptr[x] >128)
				loc_set.push_back(Point(x,y));
		}
	}
	return loc_set.size();
}

void Summrarizer::RandPatchCluster()
{
	cv::Mat validPosMat;
	cv::Mat lowGrad;
	cv::threshold(_inverseGrad, lowGrad, 255 - _gradStep, 255, 3);
	//cout<< cv::THRESH_TOZERO <<endl;
	cv::imwrite(string(_sm_out)+"lowgrad.png", lowGrad);
	sPatch curPatch;
	curPatch.width = _patchRadius * 2 + 1;
	cv::Mat out_cover;
	cv::Mat out_centerCover;
	pickOnePatch(lowGrad, curPatch, out_cover, out_centerCover, 0, 1);
	
	_spMasks.push_back(out_cover.clone());
	_centersMasks.push_back(out_centerCover);
	_summaryPatches.push_back(curPatch);

	refineOveredRegion(0);

	string strOnImg = "0";
	cv::Mat forShow;
	_nonBlured.copyTo(forShow);
	Shared::drawPatchFontOnImg(curPatch, strOnImg, forShow, cv::Scalar(0,0,255));
	
//	vector<Point> patch_loc;
//	int loc_num = transLocMatToSet(out_centerCover,patch_loc);
//	_loc_per_patch.push_back(patch_loc);
//	_spMasks.push_back(out_cover.clone());
	//SHOW_IMG(_spMasks[_spMasks.size()-1],1);

//	cv::imwrite(string(_sm_out) + "_patch.png", forShow);
//	cv::imwrite(string(_sm_out) + "_overCenter.png", out_centerCover);
	cv::imwrite(string(_sm_out) + "_cover.png", out_cover);

	out_cover.copyTo(_overallMask);
	cv::Mat remainInCurGrad;
	cv::Mat curValidPos;	
	//	cv::bitwise_xor(lowGrad, _overallMask, curValidPos, lowGrad);
	Shared::myXor(lowGrad, _overallMask, curValidPos);
	//SHOW_IMG_TITLE("curValid",curValidPos,1);
	cout << cv::countNonZero(curValidPos) << ":" << cv::countNonZero(lowGrad) << endl;
	int time = 1, curG = _gradStep;
	int overallNum = (_overallMask.rows - 2 * _patchRadius) * (_overallMask.cols - 2 * _patchRadius);
	cv::Mat tShowCoverOnImg, outCover_3C;

	//int restPos = (*gpSet)("RestPos");
	int restPos = 1000;
	while(cv::countNonZero(_overallMask(insideRect)) < overallNum - restPos)
	{
		cout << "XXXXX" << cv::countNonZero(_overallMask(insideRect)) << ":" << overallNum << endl;
		//loop different gradient 
		Shared::myXor(lowGrad, _overallMask, curValidPos); // find in current low grad
		string s = boost::lexical_cast<string>(curG);
		cv::Mat curShow;
		_nonBlured.copyTo(curShow);
		while(cv::countNonZero(curValidPos(insideRect)) > restPos / 10)
		{
			//loop in gradient to cover pixels in current gradient
			//			cv::imwrite(string(_sm_out) + "_curValid" + strOnImg + "_" + s + ".png", curValidPos);
			cout << "XXXXX" << cv::countNonZero(curValidPos(insideRect)) << ":" << overallNum << endl;
			if(!pickOnePatch(curValidPos, curPatch, out_cover, out_centerCover, time, curG))
				continue;
			//SHOW_IMG(out_centerCover,1);
		
			_summaryPatches.push_back(curPatch);
			_spMasks.push_back(out_cover.clone());
			_centersMasks.push_back(out_centerCover.clone());
			//SHOW_IMG(_spMasks[_spMasks.size()-1],1);
			//SHOW_IMG(out_cover,1);

			strOnImg = boost::lexical_cast<string>(time);
			Shared::drawPatchFontOnImg(curPatch, strOnImg, curShow, cv::Scalar(0,0,255));
		
			cv::scaleAdd(_overallMask, 1, _spMasks[time], _overallMask);
//			cv::imwrite(string(_sm_out) + "_overallCover" + strOnImg + "_" + s + ".png", _overallMask);
			Shared::myXor(lowGrad, _overallMask, curValidPos);

			refineOveredRegion(time);
			cv::bitwise_and(_centersMasks[time], _spMasks[time], _centersMasks[time]);
			//Show cover on image
			cv::cvtColor(_spMasks[time], outCover_3C, CV_GRAY2BGR);
			cv::addWeighted(outCover_3C, 0.5, _nonBlured, 0.5, 0, tShowCoverOnImg);
//			cv::imwrite(string(_sm_out) + "_cover" + strOnImg + "_" + s + ".png", tShowCoverOnImg);

			time++;
		}
//		cv::imwrite(string(_sm_out) + "_Show" + strOnImg + "_" + s + ".png", curShow);

		curG += _gradStep;
		cv::threshold(_inverseGrad, lowGrad, 255 - curG, 255, 3);
	
	}
	_relGroups.create(_spMasks.size(), _spMasks.size(), CV_32FC1);
	_relGroups.setTo(0);
	refineBeforeCluster();
	cout << "final refine done!" << endl;
	for (int i = 0; i < _spMasks.size(); i++)
	{
		string tempS = boost::lexical_cast<string>(i);
		cv::imwrite(string(_sm_out) + "_cover" + tempS + "X.png", _spMasks[i]);
		vector<Point> patch_loc;
		int loc_num = transLocMatToSet(_centersMasks[i],patch_loc);
		//cout<<loc_num<<endl;
		_loc_per_patch.push_back(patch_loc);
	}

	cv::imwrite(string(_sm_out) + "_patch.png", forShow);
}

void Summrarizer::getGradSimMap( cv::Mat& srcGrad, sPatch& spat, cv::Mat& res )
{
	cv::Mat pat;
	getPatchFromImg(srcGrad, pat, spat);
	cv::matchTemplate(srcGrad, pat, res, CV_TM_CCORR_NORMED);
	cv::Mat flipMX, flipMY, flipMXY, resFlipX, resFlipY, resFlipXY;
	cv::flip(pat, flipMX, 0);
	cv::flip(pat, flipMY, 1);
	//	cv::flip(pat, flipMXY, -1);
	cv::matchTemplate(srcGrad, flipMX, resFlipX, CV_TM_CCORR_NORMED);
	cv::matchTemplate(srcGrad, flipMY, resFlipY, CV_TM_CCORR_NORMED);
	//	cv::matchTemplate(src, flipMXY, resFlipXY, CV_TM_SQDIFF);
	float* pRes, *pResX, *pResY, *pResXY;
	for (int i = 0; i < res.rows; i++)
	{
		pRes = res.ptr<float>(i);
		pResX = resFlipX.ptr<float>(i);
		pResY = resFlipY.ptr<float>(i);
		//		pResXY = resFlipXY.ptr<float>(i);
		for (int j = 0; j < res.cols; j++, pRes++, pResX++, pResY++)
		{
			pRes[0] = (pResX[0] < pRes[0]) ? pResX[0] : pRes[0];
			pRes[0] = (pResY[0] < pRes[0]) ? pResY[0] : pRes[0];
			//			pRes[0] = (pResXY[0] > pRes[0]) ? pResXY[0] : pRes[0];
		}
	}

}

void Summrarizer::getSimMap(cv::Mat& src, sPatch& spat, cv::Mat& res)
{	
	cv::Mat pat;
	getPatchFromImg(src, pat, spat);
	cv::matchTemplate(src, pat, res, CV_TM_SQDIFF);
	cv::Mat flipMX, flipMY, flipMXY, resFlipX, resFlipY, resFlipXY;
	cv::flip(pat, flipMX, 0);
	cv::flip(pat, flipMY, 1);
//	cv::flip(pat, flipMXY, -1);
	cv::matchTemplate(src, flipMX, resFlipX, CV_TM_SQDIFF);
	cv::matchTemplate(src, flipMY, resFlipY, CV_TM_SQDIFF);
//	cv::matchTemplate(src, flipMXY, resFlipXY, CV_TM_SQDIFF);
	float* pRes, *pResX, *pResY, *pResXY;
	for (int i = 0; i < res.rows; i++)
	{
		pRes = res.ptr<float>(i);
		pResX = resFlipX.ptr<float>(i);
		pResY = resFlipY.ptr<float>(i);
//		pResXY = resFlipXY.ptr<float>(i);
		for (int j = 0; j < res.cols; j++, pRes++, pResX++, pResY++)
		{
			pRes[0] = (pResX[0] < pRes[0]) ? pResX[0] : pRes[0];
			pRes[0] = (pResY[0] < pRes[0]) ? pResY[0] : pRes[0];
//			pRes[0] = (pResXY[0] > pRes[0]) ? pResXY[0] : pRes[0];
		}
	}



}

bool Summrarizer::pickOnePatch( cv::Mat& in_ValidPosMat, sPatch& out_sPatch, 
	cv::Mat& out_CoveredMask, cv::Mat& out_CenterMask, int ID, float curG /*= 3*/ )
{
	int validPosN = 0;
	int count = 0;
	bool flag = false;

/*

	for(int i = _patchRadius; i < in_ValidPosMat.rows - _patchRadius; i++)
	{
		uchar* p = in_ValidPosMat.ptr<uchar>(i);

		for (int j = _patchRadius; j < in_ValidPosMat.cols - _patchRadius; j++)
		{
			if(p[j] == 0)
				continue;
			else
				validPosN ++;
		}
	}*/

	validPosN = cv::countNonZero(in_ValidPosMat(insideRect));
	cout << "inside number of valid:" << validPosN << endl;
//	cv::imwrite(string(_sm_out) + "_IncurValid" + "_" + boost::lexical_cast<string>(ID) + ".png", in_ValidPosMat);
	int randOrder = rand()%(validPosN);
//	cout << ", RandOrder = " << randOrder << endl;

	for(int i = 0; i < in_ValidPosMat.rows - _patchRadius; i++)
	{
		uchar* p = in_ValidPosMat.ptr<uchar>(i);
		if(flag)
			break;
		for (int j = 0; j < in_ValidPosMat.cols - _patchRadius; j++)
		{
			if(p[j] == 0)
				continue;
			if(count == randOrder)
			{
				out_sPatch.center.x = j;
				out_sPatch.center.y = i;
// 				out_sPatch.center.x = (j < _patchRadius) ? _patchRadius:j;
// 				out_sPatch.center.x = (j >= in_ValidPosMat.cols -_patchRadius) ? (in_ValidPosMat.cols -_patchRadius):j;
// 				out_sPatch.center.y = (i < _patchRadius) ? _patchRadius:i;
// 				out_sPatch.center.y = (i >= in_ValidPosMat.rows - _patchRadius) ?  (in_ValidPosMat.rows - _patchRadius):i;

				cout << ";  x:" << out_sPatch.center.x << ", y:" << out_sPatch.center.y << endl;
				flag = true;
				break;
			}
			count ++;
		}
	} // get the patch position
	//Find the biggest gradient in patch, move center to
	int newX, newY, maxX, maxY; int maxGrad = 0;
	double sumX = 0.0, sumY = 0.0, sumGrad = 0.0;
	for (int m = -_patchRadius; m <= _patchRadius; m++)
	{
		for (int n = -_patchRadius; n <= _patchRadius; n++)
		{
			newX = out_sPatch.center.x + n;
			newY = out_sPatch.center.y + m;
			if(newY < _patchRadius || newY >= _imgGrad.rows - _patchRadius 
				|| newX < _patchRadius || newX >= _imgGrad.cols - _patchRadius)
				continue;
			int curGrad = _imgGrad.ptr<uchar>(newY)[newX];
			sumX += curGrad * newX; sumY += curGrad * newY; sumGrad += curGrad;
			if(curGrad >= maxGrad)
			{
				maxGrad = curGrad;
				maxX = newX;
				maxY = newY;
			}
		}
	}

	double aveGrad = sumGrad / (_patchRadius * 2 + 1) / (_patchRadius * 2 + 1);
	out_sPatch.width = _patchRadius * 2 + 1;
	if(sumGrad > 0)
	{
		out_sPatch.center.x = sumX / sumGrad;
		out_sPatch.center.y = sumY / sumGrad;
	// 	out_sPatch.center.x = (out_sPatch.center.x < _patchRadius) ? _patchRadius:out_sPatch.center.x;
	// 	out_sPatch.center.x = (out_sPatch.center.x >= in_ValidPosMat.cols -_patchRadius) ? (in_ValidPosMat.cols -_patchRadius):out_sPatch.center.x;
	// 	out_sPatch.center.y = (out_sPatch.center.y < _patchRadius) ? _patchRadius:out_sPatch.center.y;
	// 	out_sPatch.center.y = (out_sPatch.center.y >= in_ValidPosMat.rows - _patchRadius) ?  (in_ValidPosMat.rows - _patchRadius):out_sPatch.center.y;

	}
	cout << "Grad center: " << out_sPatch.center.x << "__" << out_sPatch.center.y << endl;
//	out_sPatch.center.x = maxX;
//	out_sPatch.center.y = maxY;

	
	
	//Get the mask it covered
	cv::Mat des, gradDes;
	cv::Mat overlappedRegion;
	cv::Mat currentMatchRes, currentGradMatchRes;
	cv::Mat showMatch, showDes;
	getSimMap(_srcImg, out_sPatch, currentMatchRes);
//	cv::cvtColor(_srcImg, _srcImg, CV_BGR2HSV);
	getGradSimMap(_imgGrad, out_sPatch, currentGradMatchRes);

	cv::normalize(currentMatchRes, showMatch, 0, 5000, cv::NORM_MINMAX);
	showMatch = showMatch * (-1) + 5000;
	cv::threshold(showMatch, showDes, 4765, 5000, 3);
	showDes = showDes - 4765;
//	cv::imwrite(string(_sm_out) + boost::lexical_cast<string>(ID) + "_sim.png", showDes);

	_similaritys.push_back(showDes);

	des.create(currentMatchRes.size(), CV_32FC1);
	gradDes.create(currentMatchRes.size(), CV_32FC1);
	double curTol, tempCurTol;
	for (int i = 0; i < des.rows; i++)
	{
		float* pDes = des.ptr<float>(i);
		float* pDesT = gradDes.ptr<float>(i);
		float* pRes = currentMatchRes.ptr<float>(i);
		float* pGradRes = currentGradMatchRes.ptr<float>(i);
		for (int j = 0; j < des.cols; j++)
		{
			curTol = getToleranceTemp(out_sPatch.center.x, out_sPatch.center.y, j, i, aveGrad);
			tempCurTol = getTolerance(out_sPatch.center.x, out_sPatch.center.y, j, i, aveGrad);
			if(pRes[j] < curTol * _paraTol)
				pDes[j] = 255;
			else
				pDes[j] = 0;
			if(pRes[j] < curTol * _paraTol && pGradRes[j] > 0.5)
				pDesT[j] = 255;
			else
				pDesT[j] = 0;
		}
	}
//		cv::imwrite(string(_sm_out) + boost::lexical_cast<string>(ID) + "matchGrad" + ".png", gradDes);
//		boost::lexical_cast<string>(cv::countNonZero(des)) 
//		cv::dilate(des, des, cv::Mat());
//		cv::erode(des, des, cv::Mat());
		Shared::myAnd(des, showMatch, des, 0, 4700);

//		cv::imwrite(string(_sm_out) + boost::lexical_cast<string>(ID) + "match" + ".png", des);
	//	tempDes.copyTo(des);

	out_CenterMask.create(_srcImg.size(), CV_8UC1);
	out_CenterMask.setTo(0);
	des.convertTo(out_CenterMask(cv::Rect(_patchRadius, _patchRadius, des.cols, des.rows)), CV_8UC1);

	cv::Mat overlappedPatch;
	out_CenterMask.convertTo(overlappedPatch, CV_8UC1);
	int radius = _patchRadius;
	int i, j;
	for ( i = 0; i < overlappedPatch.rows; i++)
	{
		uchar* imgP = overlappedPatch.ptr<uchar>(i);
		for (j = 0; j < overlappedPatch.cols; j++)
		{
			for (int m = i - radius; m <= i+radius; m++)
			{
				for (int n = j - radius; n <= j+radius; n++)
				{
					if(m < 0 || m >= out_CenterMask.rows || n < 0 || n >= out_CenterMask.cols)
						continue;
					if(out_CenterMask.ptr<uchar>(m)[n] > 200)
						imgP[j] = 255;
				}
			}
		}
	}
	overlappedPatch.copyTo(out_CoveredMask);
//	cv::scaleAdd(des, 1, overlappedRegion, overlappedRegion);
	if(!flag)
		cout << "Final count" << count << "; Rand order:" << randOrder << endl;
	return flag;
}

//##################################################
//old
/*
void Summrarizer::getMSER(cv::Mat& img, cv::Mat& msk)
{
	cv::Mat image = img, msk_image = msk;
	cv::MSER mser;
	vector<vector<Point>> msers;
	mser.minArea = 150;
	mser.maxArea = 400;

	cv::Mat blurred;
	cv::GaussianBlur(image, blurred, cv::Size(3,3), 1);
	cv::imwrite(string(_sm_out) + "blurred.png", blurred);
	mser(blurred, msers, msk_image);
	
	cv::Mat tShow;
	tShow.create(image.size(), CV_8UC3);
// 	Vec3b green(0, 255, 0);
// 	tShow.at<Vec3b>(50, 50) = green;
// 	if(LABEL_2 == tShow.at<Vec3b>(50, 50))
// 		cout << "Yes, it is true" << endl;

//	cv::drawContours(tShow, msers, -1, cv::Scalar(0,255, 255), 1);
/ *	( int delta CV_DEFAULT(5), int min_area CV_DEFAULT(60),
		int max_area CV_DEFAULT(14400), float max_variation CV_DEFAULT(.25f),
		float min_diversity CV_DEFAULT(.2f), int max_evolution CV_DEFAULT(200),
		double area_threshold CV_DEFAULT(1.01),
		double min_margin CV_DEFAULT(.003),
		int edge_blur_size CV_DEFAULT(5) );* /
	cv::Mat gradMap8U, Atan32F;
	cv::Mat tShowImg;
	image.copyTo(tShowImg);
	Shared::getGradMap(image, gradMap8U, Atan32F);
	cv::imwrite("D:\\Patch\\data\\gradient.png", gradMap8U);
	for (int i = 0; i < msers.size(); i++)
	{
		int b = rand() % 100 + 155;
		int g = rand() % 100 + 155;
		int r = rand() % 100 + 155;
		for (int j = 0; j < msers[i].size(); j++)
		{
			tShow.ptr<uchar>(msers[i][j].y)[msers[i][j].x * 3] = b;
			tShow.ptr<uchar>(msers[i][j].y)[msers[i][j].x * 3+1] = g;
			tShow.ptr<uchar>(msers[i][j].y)[msers[i][j].x * 3+2] = r; 
		}
		cv::Point cen = getCenter(msers[i]);
		cv::Point d(-1, -1);
		cv::Point leftTop = getFarest(msers[i], cen, d);
		
		BoundGrad upBoundGrad, downBoundGrad;
		sPatch curPatch;
		curPatch.center.x = leftTop.x;
		curPatch.center.y = leftTop.y;
		curPatch.width = 13;
		//!
		_summaryPatches.push_back(curPatch);

		string s = "D:\\Patch\\data\\cPatchOri" + boost::lexical_cast<string>(i) + ".png";
		Shared::drawPatch(curPatch, gradMap8U, s);
		s = "D:\\Patch\\data\\cPatchOri" + boost::lexical_cast<string>(i) + "_g.png";;
		Shared::drawPatch(curPatch, image, s);
		getBoundGrads(cv::Point(0, -1), curPatch, Atan32F, gradMap8U, upBoundGrad);
		getBoundGrads(cv::Point(0, +1), curPatch, Atan32F, gradMap8U, downBoundGrad);

		int dis = compareHist(upBoundGrad, downBoundGrad);

		int countN = abs(leftTop.y - cen.y);
		cv::Point curPatCen = leftTop;
		
		Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(255,255, 0));
	
		while (dis > 3 && countN > 0)
		{
			curPatCen.y = curPatCen.y + 1;
			curPatch.center = curPatCen;		
			getBoundGrads(cv::Point(0, -1), curPatch, Atan32F, gradMap8U, upBoundGrad);
			getBoundGrads(cv::Point(0, +1), curPatch, Atan32F, gradMap8U, downBoundGrad);

			countN --;
			dis = compareHist(upBoundGrad, downBoundGrad);
		}
		cout << dis << "--" << countN << endl;
		if(dis > 3)
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(0,0,255));
		else
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(0,255,255));

		if(countN == abs(leftTop.y - cen.y))
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(255,0, 255));
		//!
		_summaryPatches.push_back(curPatch);


		BoundGrad leftBoundGrad, rightBoundGrad;
//		sPatch curPatch;
		curPatch.center.x = leftTop.x;
		curPatch.center.y = leftTop.y;
		curPatch.width = 13;
		s = "D:\\Patch\\data\\cPatchOri" + boost::lexical_cast<string>(i) + ".png";
		Shared::drawPatch(curPatch, gradMap8U, s);
		s = "D:\\Patch\\data\\cPatchOri" + boost::lexical_cast<string>(i) + "_g.png";
		Shared::drawPatch(curPatch, image, s);
		getBoundGrads(cv::Point(-1, 0), curPatch, Atan32F, gradMap8U, leftBoundGrad);
		getBoundGrads(cv::Point(+1, 0), curPatch, Atan32F, gradMap8U, rightBoundGrad);

		dis = compareHist(leftBoundGrad, rightBoundGrad);

		countN = abs(leftTop.x - cen.x);
		curPatCen = leftTop;
		while (dis > 3 && countN > 0)
		{
			curPatCen.x = curPatCen.x + 1;
			curPatch.center = curPatCen;		
			getBoundGrads(cv::Point(-1, 0), curPatch, Atan32F, gradMap8U, leftBoundGrad);
			getBoundGrads(cv::Point(+1, 0), curPatch, Atan32F, gradMap8U, rightBoundGrad);

			countN --;
			dis = compareHist(leftBoundGrad, rightBoundGrad);
		}
		cout << dis << "--" << countN << endl;
		if(dis > 3)
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(0,0,255));
		else
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(0,255,255));

		if(countN == abs(leftTop.y - cen.y))
			Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(255,0, 255));
		//!
		_summaryPatches.push_back(curPatch);


		//! center push in
		curPatch.center = cen;
		_summaryPatches.push_back(curPatch);
		Shared::drawPatchOnImg(curPatch, tShowImg, cv::Scalar(0,255,255));
	}
	cv::imwrite("D:\\Patch\\data\\tShow.png", tShowImg);
	cv::imwrite("D:\\Patch\\data\\contours.png", tShow);
}*/

cv::Point Summrarizer::getCenter( vector<cv::Point> points )
{
	cv::Point res;
	int sumX = 0, sumY = 0;
	for (int i = 0; i < points.size(); i++)
	{
		sumX += points[i].x;
		sumY += points[i].y;
	}
	res.x = sumX / points.size();
	res.y = sumY / points.size();
	return res;
}

cv::Point Summrarizer::getFarest( vector<cv::Point> points, cv::Point& c, cv::Point& dir )
{
	cv::Point res(0,0);
	int maxDis = -1;
	for (int i = 0; i < points.size(); i++)
	{
		if( (((points[i].x - c.x) * dir.x) >= 0)
			&& (((points[i].y - c.y) * dir.y) >= 0) )
		{
			int dis = abs(points[i].x - c.x) + abs(points[i].y - c.y);
			if(dis > maxDis)
			{
				maxDis = dis;
				res.x = points[i].x;
				res.y = points[i].y;
			}	
		}
			
	}
	return res;
}

void Summrarizer::getBoundGrads(cv::Point dir, sPatch& inputPatch, cv::Mat& atanMap, cv::Mat& gradMap, BoundGrad& out_BoundGrad )
{
	int radius = inputPatch.width / 2; //normally odd
	int x_lower, y_lower, x_higher, y_higher;
	if(dir.x == -1)
		x_lower = inputPatch.center.x - radius, x_higher = inputPatch.center.x - radius,
		y_lower = inputPatch.center.y - radius, y_higher = inputPatch.center.y + radius;
	if(dir.x == 1)
		x_lower = inputPatch.center.x + radius, x_higher = inputPatch.center.x + radius,
		y_lower = inputPatch.center.y - radius, y_higher = inputPatch.center.y + radius;
	if(dir.y == -1)
		x_lower = inputPatch.center.x - radius, x_higher = inputPatch.center.x + radius,
		y_lower = inputPatch.center.y - radius, y_higher = inputPatch.center.y - radius;
	if(dir.y == 1)
		x_lower = inputPatch.center.x - radius, x_higher = inputPatch.center.x + radius,
		y_lower = inputPatch.center.y + radius, y_higher = inputPatch.center.y + radius;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 8; j++)
			out_BoundGrad.tanMagHist[i][j] = 0;

	for (int r = y_lower; r <= y_higher; r++)
	{
		for (int c = x_lower; c <= x_higher; c++)
		{
			double ang = atanMap.ptr<float>(r)[c];
			int gra = gradMap.ptr<uchar>(r)[c];
	//		cout << gra <<";";
			if(gra >= 32)
			out_BoundGrad.tanMagHist[int(abs(ang) / 0.8)][gra / 32] ++;
		}
	//	cout << endl;
	} 	
}

int Summrarizer::compareHist( BoundGrad& b1, BoundGrad& b2 )
{
	int res = 0;
	for (int i = 0; i< 2; i++)
	{
		for (int j = 0; j < 8; j++)
			res += abs(b1.tanMagHist[i][j] - b2.tanMagHist[i][j]);
	}
	return res;
}

void Summrarizer::getPatchFromImg( cv::Mat& src, cv::Mat& pat, sPatch& pt )
{
	int radius = pt.width / 2; //normally odd
	int x, y;
	x = pt.center.x - radius;
	y = pt.center.y - radius;
#ifdef _DEBUG
	if (src.channels()==3){
		pat.create(pt.width, pt.width, CV_8UC3);
		for (int i = 0; i < pat.rows; i++)
		{
			for (int j = 0; j < pat.cols; j++)
			{
				pat.ptr<Vec3b>(i)[j] = src.ptr<Vec3b>(i + y)[j + x];
			}
		}
	}
	else{
		pat.create(pt.width, pt.width, CV_8UC1);
		for (int i = 0; i < pat.rows; i++)
		{
			for (int j = 0; j < pat.cols; j++)
			{
				pat.ptr<uchar>(i)[j] = src.ptr<uchar>(i + y)[j + x];
			}
		}
	}
#else
	src(cv::Rect(x,y,pt.width, pt.width)).copyTo(pat);
#endif
}

void Summrarizer::getOverlapRegion( cv::Mat& img, cv::Mat& msk )
{
	cv::Mat des;
	cv::Mat overlappedRegion;
	for (int i = 0; i < _summaryPatches.size(); i++)
	{
		cv::Mat currentPatMat;
		cv::Mat currentPatGra;
		getPatchFromImg(img, currentPatMat, _summaryPatches[i]);
		cv::Mat currentMatchRes;
		cv::matchTemplate(img, currentPatMat, currentMatchRes, CV_TM_SQDIFF);
		cv::normalize(currentMatchRes, currentMatchRes, 0, 255, cv::NORM_MINMAX);
		currentMatchRes = currentMatchRes * (-1) + 255;
		cv::threshold(currentMatchRes, des, 253, 255, 3);
		cv::imwrite(string(_sm_out) + boost::lexical_cast<string>(i) + "match.png", des);
		if(i == 0)
		{
			overlappedRegion.create(des.size(), des.type());
			overlappedRegion.setTo(0);
		}
		cv::scaleAdd(des, 1, overlappedRegion, overlappedRegion);
	}
	overlappedRegion.convertTo(overlappedRegion, CV_8UC1);
	cv::imwrite(string(_sm_out) + "overall.png", overlappedRegion);
	
	cv::Mat overlappedPatch;
	overlappedRegion.copyTo(overlappedPatch);

	int radius = _summaryPatches[0].width / 2;
	int i, j;
	for ( i = 0; i < overlappedRegion.rows; i++)
	{
		uchar* mskP = msk.ptr<uchar>(i + radius);
		uchar* imgP = overlappedPatch.ptr<uchar>(i);
		for (j = 0; j < overlappedRegion.cols; j++)
		{
			if(mskP[radius + j] < 128)
			{
				imgP[j] = 0;
				continue;
			}
			for (int m = i - radius; m <= i+radius; m++)
			{
				for (int n = j - radius; n <= j+radius; n++)
				{
					if(overlappedRegion.ptr<uchar>(m)[n] > 128)
						imgP[j] = 255;
//						continue;
					
				}
			}
		}
	}
	cv::imwrite(string(_sm_out) + "overMask.png", msk);
	cv::imwrite(string(_sm_out) + "overPatch.png", overlappedPatch);
}

double Summrarizer::getTolerance( int src_x, int src_y, int tgt_x, int tgt_y, int curGrad )
{
	double adjV;
//	Vec3b srcLab = _thumbLab.ptr<Vec3b>(src_y / (_patchRadius * 2 + 1))[src_x / (_patchRadius * 2 + 1)];
//	Vec3b tgtLab = _thumbLab.ptr<Vec3b>(tgt_y / (_patchRadius * 2 + 1))[tgt_x / (_patchRadius * 2 + 1)];
//	Vec3b srcLab = _thumbLab.ptr<Vec3b>(src_y / (_patchRadius))[src_x / (_patchRadius)];
//	Vec3b tgtLab = _thumbLab.ptr<Vec3b>(tgt_y / (_patchRadius))[tgt_x / (_patchRadius)];
	Vec3b srcLab = _averImg.ptr<Vec3b>(src_y)[src_x];
	Vec3b tgtLab = _averImg.ptr<Vec3b>(tgt_y)[tgt_x];

	int hueDis = sqr(srcLab[1] - tgtLab[1]) + sqr(srcLab[2] - tgtLab[2]);  // 6 per degree
	int LDis = abs(srcLab[0] - tgtLab[0]);   // 5 * 2.55 per degree 
	double adjFromClr;
//	adjFromClr = hueDis / 16 + LDis / 20;
	adjFromClr = hueDis / 8 + LDis / 10;
	double aboutGrad = curGrad / _gradStep;
	
//	return adjV;
	
	if(adjFromClr > 0)
//		adjV = 2 / adjFromClr * aboutGrad;
		adjV = 2 / adjFromClr * sqrt(aboutGrad);
	else
//		adjV = (aboutGrad > 0) ? 2 * sqrt(aboutGrad) : 2;
		adjV = (aboutGrad > 0) ? 1 + sqrt(aboutGrad) : 2;
	return adjV;
}

double Summrarizer::getToleranceTemp( int src_x, int src_y, int tgt_x, int tgt_y, int curGrad )
{
	double adjV;
	Vec3b srcLab = _averImg.ptr<Vec3b>(src_y)[src_x];
	Vec3b tgtLab = _averImg.ptr<Vec3b>(tgt_y)[tgt_x];
	uchar srcAveGrad = _aveGrad.ptr<uchar>(src_y)[src_x];
	uchar tgtAveGrad = _aveGrad.ptr<uchar>(tgt_y)[tgt_x];

	int hueDis = sqr(srcLab[1] - tgtLab[1]) + sqr(srcLab[2] - tgtLab[2]);  // 6 per degree
	int LDis = abs(srcLab[0] - tgtLab[0]);   // 5 * 2.55 per degree 
	int AveGraDis = abs(srcAveGrad - tgtAveGrad);
	double adjFromClr, adjFromGrad;
	adjFromClr = hueDis / 8 + LDis / 10;
	adjFromGrad = AveGraDis / int(_paraAveGradTol);
//	adjFromClr = 0;
	double aboutGrad = curGrad / _gradStep;

	//	return adjV;

	if(adjFromClr == 0 && adjFromGrad == 0)
		adjV = 2 * sqrt(aboutGrad + 1);
	else
		adjV = 2 * sqrt(aboutGrad + 1) / sqrt(adjFromClr + adjFromGrad);
	return adjV;
}

void Summrarizer::refineOveredRegion(int ID)
{
//	cv::vector<cv::Vec4i> hierarchy;
	int smallArea = (_patchRadius * 2 + 1) * (_patchRadius * 2 + 1);

	{
		cv::Mat curPatchOver;
		_spMasks[ID].convertTo(curPatchOver, CV_8UC1);
		
		cv::dilate(curPatchOver, curPatchOver, cv::Mat());
		cv::erode(curPatchOver, curPatchOver, cv::Mat());
		
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(curPatchOver, contours, CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_NONE,cv::Point(0, 0));	

		_allOverContours.push_back(contours);

		if(contours.size() <= 2)
		{
			cout << "----------------->size less than 2" << endl;
			return;
		}
		vector<int> areas;
		areas.clear();
		double sum = 0.0, maxArea = 0.0, minArea = 1E10;
		for (int k = 0; k < contours.size(); k++)
		{
			int area = cv::contourArea(cv::Mat(contours[k]));
			areas.push_back(area);
			sum += area;
			maxArea = (area > maxArea) ? area : maxArea;
			minArea = (area < minArea) ? area : minArea;
		}
		cout << "$$$$$$$$$$$$$$$$$$$Max: " << maxArea << "    Min: " << minArea << endl;
		if (maxArea / minArea < 10)
		{
			cout << "------------------>Distribute Ave" << endl;
			return;
		}
		curPatchOver.setTo(0);
		for (int k = 0; k < contours.size(); k++)
		{
			if(areas[k] < 1.5 * smallArea && 
				cv::pointPolygonTest(cv::Mat(contours[k]), _summaryPatches[ID].center, false) < 0)
			{
				Shared::drawPoly(curPatchOver, contours[k], cv::Scalar(255), 1, true);
				cout << "$$$$$$$$$$$$$$$$Refine one small!  " << areas[k] << endl;
			}
		}
		Shared::myXor(_spMasks[ID], curPatchOver, _spMasks[ID]);
//		curPatchOver.copyTo(_spMasks[ID]);
	}

}

void Summrarizer::refineBeforeCluster()
{
	//double thrMerge = (*gpSet)("MergeThr");
	double thrMerge = 100000;
	cv::Mat tempDraw1, tempDraw2, tempR;

	cv::Mat tCenters1, tCenters2;

	tempDraw1.create(_srcImg.size(), CV_8UC1);
	tempDraw2.create(_srcImg.size(), CV_8UC1);
	tempR.create(_srcImg.size(), CV_8UC1);

	vector<cv::Mat> allSumPats;
	allSumPats.resize(_summaryPatches.size());
	for (int i = 0; i < allSumPats.size(); i++)
		getPatchFromImg(_srcImg, allSumPats[i], _summaryPatches[i]);

	for (int i = 0; i < _allOverContours.size(); i++)
	{
		CContour& curContours = _allOverContours[i];
		for (int j = 0; j< curContours.size(); j++)
		{
			vector<Point>& curContour = curContours[j];
			int curArea = cv::contourArea(cv::Mat(curContour));
			if(curArea < 2000)
				continue;

			tempDraw1.setTo(0);
			Shared::drawPoly(tempDraw1,curContour, cv::Scalar(255), 1, true);
			cv::bitwise_and(tempDraw1, _spMasks[i], tempDraw1);
			cv::bitwise_and(tempDraw1, _centersMasks[i], tCenters1);
			for (int m = 0; m < _allOverContours.size(); m++)
			{
				if(cv::norm(allSumPats[i], allSumPats[m], CV_L2) > thrMerge)
					continue;
				if(m == i)
					continue;
				for (int n = 0; n < _allOverContours[m].size(); n++)
				{
					vector<Point>& curTgtCont = _allOverContours[m][n];
					int curTgtArea = cv::contourArea(cv::Mat(curTgtCont));
					if(curTgtArea > curArea)
						continue;
					//Big region eat overlapeed small region
					tempDraw2.setTo(0);
					Shared::drawPoly(tempDraw2,curTgtCont, cv::Scalar(255), 1, true);
					Shared::myXor(tempDraw2, tempDraw1, tempR);
					if(cv::countNonZero(tempR) < 10)
					{
						if(cv::pointPolygonTest(cv::Mat(curTgtCont), _summaryPatches[m].center, false) > 0)
							continue;
						
						Shared::myXor(_spMasks[m], tempDraw2, _spMasks[m]);

						Shared::myXor(_centersMasks[m], tempDraw2, _centersMasks[m]);

						if(cv::contourArea(cv::Mat(curTgtCont)) > 300)
						{
		//					SHOW_IMG(_centersMasks[m], 1);
						}
		//				cout << "done" << endl;
						continue;
					}
					
					//!!!!!End big region eat small

					//Big region merge smaller one, use centermasks as an evidence

					tempDraw2.setTo(0);
					Shared::drawPoly(tempDraw2,curTgtCont, cv::Scalar(255), 1, true);
					cv::bitwise_and(tempDraw2, _centersMasks[m], tCenters2);

					cv::bitwise_and(tCenters1, tCenters2, tempR);
					int sameCN = cv::countNonZero(tempR);
					if((sameCN > cv::countNonZero(tCenters1) * 0.2) || (sameCN > cv::countNonZero(tCenters2) * 0.2))
					{
						if(curTgtArea > 10000 && cv::arcLength(curTgtCont, true) < 800 )
							continue;
						cv::bitwise_or(_centersMasks[i], tCenters2, _centersMasks[i]);
						cv::bitwise_and(tempDraw2, _spMasks[m], tempDraw2);
						cv::bitwise_or(_spMasks[i], tempDraw2, _spMasks[i]);
			//			tCenters1.copyTo();
						cv::bitwise_or(tempDraw1, tempDraw2, tempR);
						CContour newContour;
						cv::findContours(tempR, newContour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
						curContour = newContour[0];
			//			SHOW_IMG(_spMasks[i], 1);
			//			SHOW_IMG(_spMasks[m], 1);

						Shared::myXor(_spMasks[m], tempDraw2, _spMasks[m]);
				//		SHOW_IMG(_spMasks[m], 1);

						Shared::myXor(_centersMasks[m], tempDraw2, _centersMasks[m]);
						
						if(cv::pointPolygonTest(cv::Mat(curTgtCont), _summaryPatches[m].center, false) > 0)
							pickNewSPatch(m);
						cout << "$$$ Merge one $$$$$$$$$$$" << "_" << i << "_" << m << endl; 
						if(_allOverContours[m].size() < 8 )
						{
							_relGroups.ptr<float>(m)[i] = curTgtArea;
							_relGroups.ptr<float>(i)[m] = curArea;
						}


						cout << "XXX--end" << endl;
					}
					//end big region merge
				}
			}// End for each contour to search mergable sub region

		}
	}//End for every contour


}

void Summrarizer::pickNewSPatch( int ID )
{

	cout << "XXXXXXXXXX-" << ID << "__" << _spMasks[ID].rows << "__" << _centersMasks[ID].rows << "__" <<_summaryPatches[ID].center<< endl;
	if(cv::countNonZero(_spMasks[ID]) < sqr(_patchRadius * 2 + 1))
	{
		cout << "Too little Non zeros!" << endl;
		return;
	}
	if(ID == 11)
	{
		cout << "Hit" << endl;
	}
	int num = cv::countNonZero(_centersMasks[ID]);
	int count = 0;
	if(num == 0)
	{
		return;
	}
	int randNum = rand() % num;
	bool flag = false;
	for (int i = 0; i < _centersMasks[ID].rows; i++)
	{
		for (int j = 0; j < _centersMasks[ID].cols; j++)
		{
			if(_centersMasks[ID].ptr<uchar>(i)[j] < 128)
				continue;
			if(count == randNum)
			{
				flag = true;
				_summaryPatches[ID].center.x = j;
				_summaryPatches[ID].center.y = i;
				cout << "New sP: i: " << i << "j: " << j << endl;
			}
			count++;
		}
		if(flag)
			break;
	}
}




