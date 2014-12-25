#include "stdafx2.h"
#include "graphBuilder.h"

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Not Use!!!

graphBuilder::graphBuilder(void)
{
}

graphBuilder::graphBuilder( int classNum, int pw)
{
	fixed_ClassNum = classNum;
	patch_width = (pw - 1) / 2;
	relationMat.resize(classNum);
	relationStrength.resize(classNum);
	for (int i = 0; i < relationMat.size(); i++)
	{
		relationMat[i].resize(classNum);
		relationStrength[i].resize(classNum);
		for (int j = 0; j < classNum; j++)
		{
			relationMat[i][j].create(17, 17, CV_32FC1);
			relationMat[i][j].setTo(0);
			relationStrength[i][j] = 0;
		}
	}

}


graphBuilder::~graphBuilder(void)
{
	for (int i = 0; i < relationMat.size(); i++)
	{
		for (int j = 0; j < relationMat[i].size(); j++)
		{
			relationMat[i][j].release();
		}
	}
}

void graphBuilder::collectDirs( cv::Mat& img, int* labels )
{
	int imgW = img.cols, imgH = img.rows;
	int patchCols = imgW - patch_width * 2 - 1;
	int patchRows = imgH - patch_width * 2 - 1;

	int srcLabelDataIndex, tgtLabelDataIndex;
	for (int i = 0; i < patchRows; i++)
	{
	//	cout << i << endl;
		for (int j = 0; j < patchCols; j++)
		{
			srcLabelDataIndex = i * patchCols + j;
			int x_offset, y_offset;
			for (y_offset = -patch_width; y_offset <= patch_width; y_offset ++)
			{
				for (x_offset = -patch_width; x_offset <= patch_width; x_offset++)
				{
					if(j + x_offset <0 || i + y_offset < 0 || j + x_offset >= patchCols || i + y_offset >= patchRows)
						continue;
				
					tgtLabelDataIndex = (i + y_offset) * patchCols + j + x_offset;
// 					if(labels[srcLabelDataIndex] >= fixed_ClassNum || labels[tgtLabelDataIndex] >= fixed_ClassNum)
// 						cout << "Out of label range" ;
					relationMat[labels[srcLabelDataIndex]][labels[tgtLabelDataIndex]].ptr<float>(patch_width + y_offset)[patch_width + x_offset] += 1;
				}
			}
		}
	}

}

int graphBuilder::drawDir( cv::Mat& relMat, string& saveName)
{
	int show_col = relMat.cols * 10;
	int show_row = relMat.rows * 10;
	cv::Mat img_show;
	cv::Mat temp;
	img_show.create(show_row, show_col, CV_8UC1);
//	cv::normalize(relMat, relMat);
//	relMat = relMat * 1800;
	double minV, maxV;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(relMat, &minV, &maxV, &minLoc, &maxLoc, cv::Mat());

	relMat = relMat - minV;
	relMat = 255.0 / (maxV - minV) * relMat; 
	//	temp.convertTo(temp, CV_8UC1);
	cv::resize(relMat, img_show, img_show.size());

	imwrite(saveName, img_show);
	return maxV;
}

