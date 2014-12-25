#include "stdafx2.h"
#include "LocalNet.h"
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Not Use!!!
void LocalNet::Load( string& labelName, string& srcName )
{
	labelMap = cv::imread(labelName);
	srcImg = cv::imread(srcName);

	nodes.clear();
	relationMats.clear();
//	cv::Mat labCopy;
//	labCopy.create(labelMap.size(), CV_8UC3);
	for (int i = 0; i < labelMap.rows; i++)
	{
		for (int j = 0; j < labelMap.cols; j++)
		{
			Vec3b curLabel = labelMap.ptr<Vec3b>(i)[j];
//			labCopy.ptr<Vec3b>(i)[j] = curLabel;
			if((curLabel[2] < 255 && curLabel[2] > 0)
				||(curLabel[1] < 255 && curLabel[1] > 0)
				||(curLabel[0] < 255 && curLabel[0] > 0))
				continue;
			bool exist = false;
			for (int k = 0; k < nodes.size(); k++)
			{
				if (curLabel == nodes[k].label)
					exist = true;
			}
			if(!exist)
			{
				PatchNetNode curNode;
				curNode.label = curLabel;
				nodes.push_back(curNode);
			}

		}//end for each position in label map
	}

	//After getting all labels, summarize it
	int labelNum = nodes.size();
	int patch_width = REL_WIDTH;
	relationMats.resize(labelNum);
	relMaxvalues.resize(labelNum);
	for (int i = 0; i < labelNum; i++)
	{	
		relMaxvalues[i].resize(labelNum);
		relationMats[i].resize(labelNum);
		for (int j = 0; j < labelNum; j++)
		{
			relationMats[i][j].create(patch_width * 2 + 1, patch_width * 2 + 1, CV_32FC1);
			relationMats[i][j].setTo(0);
		}
	}		

	Vec3b srcLabel, tgtLabel;
	for (int i = 0; i < labelMap.rows; i++)
	{
		//	cout << i << endl;
		for (int j = 0; j < labelMap.cols; j++)
		{
			int x_offset, y_offset;
			srcLabel = labelMap.ptr<Vec3b>(i)[j];
			for (y_offset = -patch_width; y_offset <= patch_width; y_offset ++)
			{
				for (x_offset = -patch_width; x_offset <= patch_width; x_offset++)
				{
					if(j + x_offset <0 || i + y_offset < 0 
						|| j + x_offset >= labelMap.cols || i + y_offset >= labelMap.rows)
						continue;
					int srcLabelNum = -1, tgtLabelNum = -1;
					tgtLabel = labelMap.ptr<Vec3b>(i + y_offset)[j + x_offset];
					for (int k = 0; k < nodes.size(); k++)
					{
						if(srcLabel == nodes[k].label)
							srcLabelNum = k;
						if(tgtLabel == nodes[k].label)
							tgtLabelNum = k;
					}
					if(srcLabelNum >= 0 && tgtLabelNum >= 0)
					relationMats[srcLabelNum][tgtLabelNum].ptr<float>(patch_width + y_offset)[patch_width + x_offset] += 1;
				}
			}

			//end for each label in Map
		}
	}
	for (int i = 0; i < relationMats.size(); i++)
	{
		for (int j = 0; j < relationMats[i].size(); j++)
		{
			string s = "D:\\Patch\\data\\relationMat" + boost::lexical_cast<string>(i)
				+ "_" + boost::lexical_cast<string>(j) + ".png";
	//		relMaxvalues[i][j] = drawMats(relationMats[i][j], s);
		}
	}
//	labelMap.copyTo(labCopy);
//	cv::imwrite("Label_copy.png", labCopy);
}

int LocalNet::getLabelNum( Vec3b& label )
{
	int res = -1;
	for (int k = 0; k < nodes.size(); k++)
	{
		if (label == nodes[k].label)
			res = k;
	}
	return res;
}


float LocalNet::drawMats( cv::Mat& relMat, string& saveName )
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
	//	return maxV;
}

LocalNet::LocalNet( void )
{

}

LocalNet::~LocalNet( void )
{

}

void LocalNet::fromHole( string& holeMaskName )
{
	relMatsTohole.resize(nodes.size());
	holeRelMaxvalues.resize(nodes.size());
	for (int i = 0; i < relMatsTohole.size(); i++)
	{
		relMatsTohole[i].create(REL_WIDTH*2 + 1, REL_WIDTH*2 + 1, CV_32FC1);
		relMatsTohole[i].setTo(0);
	}

	cv::Mat holeMsk = cv::imread(holeMaskName, 0);

	Vec3b srcLabel, tgtLabel;
	uchar mskValue;
	for (int i = 0; i < labelMap.rows; i++)
	{
		//	cout << i << endl;
		for (int j = 0; j < labelMap.cols; j++)
		{
			int x_offset, y_offset;
	//		srcLabel = labelMap.ptr<Vec3b>(i)[j];
			uchar mskValue = holeMsk.ptr<uchar>(i)[j];
			if(mskValue < 128)
				continue;
			for (y_offset = -REL_WIDTH; y_offset <= REL_WIDTH; y_offset ++)
			{
				for (x_offset = -REL_WIDTH; x_offset <= REL_WIDTH; x_offset++)
				{
					if(j + x_offset <0 || i + y_offset < 0 
						|| j + x_offset >= labelMap.cols || i + y_offset >= labelMap.rows)
						continue;
					int tgtLabelNum = -1;
					tgtLabel = labelMap.ptr<Vec3b>(i + y_offset)[j + x_offset];
					for (int k = 0; k < nodes.size(); k++)
					{
	//					if(srcLabel == nodes[k].label)
	//						srcLabelNum = k;
						if(tgtLabel == nodes[k].label)
							tgtLabelNum = k;
					}
					if(tgtLabelNum >= 0 && holeMsk.ptr<uchar>(i + y_offset)[j + x_offset] < 128)
						relMatsTohole[tgtLabelNum].ptr<float>(REL_WIDTH - y_offset)[REL_WIDTH - x_offset] += 1;
				}
			}

			//end for each label in Map
		}
	}
	for (int j = 0; j < relMatsTohole.size(); j++)
	{
		string s = "D:\\Patch\\data\\holeMat" + string("_") + boost::lexical_cast<string>(j) + ".png";
	//	holeRelMaxvalues[j] = drawMats(relMatsTohole[j], s);
	}
}

void LocalNet::loadSumPatch( int x, int y )
{
	int nodesID = -1;
	sumPatchWidth = 13;
	Vec3b curLabel = labelMap.ptr<Vec3b>(y)[x];
	for (int i = 0; i < nodes.size(); i++)
	{
		if (curLabel == nodes[i].label)
		{
			nodesID = i;
			nodes[i].sumPatches.push_back(srcImg(cv::Rect(x, y, sumPatchWidth, sumPatchWidth)));
		}
	}

}
