#include "stdafx2.h"

extern "C" {

//#include "kmeans.h"
}
#include "patch.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "image_kit.h"
#include "graphBuilder.h"
#include "LocalNet.h"
#include "Summrarizer.h"


//extern "C" VlKMeans * 	vl_kmeans_new (vl_type dataType, VlVectorComparisonType distance);

const int kPatch_w = 17;
const float kWeight_grad = 10.f;

void getLabelGuess(cv::Mat& out_msk, LocalNet& tgtNet, vector<LocalNet>& nets)
{
	vector<Vec3b> attachedLabels;
	vector<cv::Mat> attachedRels;
	for (int i = 0; i < tgtNet.holeRelMaxvalues.size(); i++)
	{
		if (tgtNet.holeRelMaxvalues[i] > 10)
		{
			attachedLabels.push_back(tgtNet.nodes[i].label);
			attachedRels.push_back(tgtNet.relMatsTohole[i]);
		}
	}

//	for (int i = 0; i < attachedRels.size(); i++)
//		cv::imwrite(outdir + "t_holeAtRel" + boost::lexical_cast<string>(i) + ".png", attachedRels[i]);
	//Get valid relationships

	//Through the same label summary patches' searching
	vector<vector<float>> sumpatsSims;
	//For every attacedLabels, every local nets' nodes' similarity
	sumpatsSims.resize(attachedLabels.size());
	for (int i = 0; i < attachedLabels.size(); i++)
	{
		sumpatsSims[i].resize(nets.size());
		int oriID = tgtNet.getLabelNum(attachedLabels[i]);
		cv::Mat& srcSumPat = tgtNet.nodes[oriID].sumPatches[0];
		//For each attached labels, get the nearest
		for (int k = 0; k < nets.size(); k++)
		{
			int inside_oriID = nets[k].getLabelNum(attachedLabels[i]);
			sumpatsSims[i][k] = cv::norm(srcSumPat, nets[k].nodes[inside_oriID].sumPatches[0], CV_L2);
			cout << sumpatsSims[i][k] << endl;
		}//get the similarity with the same labels in different images

	} 

	//In every local net, search the rest label, record every label the relation distance
	vector<vector<pair<Vec3b, float>>> labelRelDiss;
	labelRelDiss.resize(nets.size());
	for (int i = 0; i < nets.size(); i++)
	{
		LocalNet& curNet = nets[i];

		vector<int> attachedNodeIDs;
		attachedNodeIDs.resize(attachedLabels.size());
		for (int k = 0; k < attachedLabels.size(); k++)
			attachedNodeIDs[k] = curNet.getLabelNum(attachedLabels[k]);
	//	labelRelDiss.resize(curNet.nodes.size());
		for (int k = 0; k < curNet.nodes.size(); k++)
		{
			Vec3b& curLab = curNet.nodes[k].label;
			float dis = 0;
			for (int n = 0; n < attachedNodeIDs.size(); n++)
			{
				dis += cv::norm(curNet.relationMats[attachedNodeIDs[n]][k], 
				               attachedRels[n], CV_L2);
				cout << "Dis with" << k << "," << attachedNodeIDs[n] << endl;
			}
			pair<Vec3b, float> p;
			p.first = curLab; p.second = dis;
			labelRelDiss[i].push_back(p);

			cout << "Local Net: " << i << endl;
			cout << "Dis: " << dis << endl;
			cout << "Label:" << int(curLab[0]) << "," << int(curLab[1]) << "," << int(curLab[2])<<endl;
			cout << "XXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
		}
		//Get all the distance from a node in a net to the attached node with the
		//unknown region with the attached node
	}

	float minDis = 1E10;
	Vec3b minLabel;
	int minNet;
	for (int i = 0; i < nets.size(); i++)
	{
		float labelSimSum = 0.0;
		for (int k = 0; k < sumpatsSims.size(); k++)
			labelSimSum += sumpatsSims[k][i];
		if(labelSimSum < 0.001)
			continue;
		for (int k = 0; k < labelRelDiss[i].size(); k++)
		{
			//current distance with attached labels
			float curDis = labelRelDiss[i][k].second;
			float sumWithLabSim = curDis + labelSimSum;
			if(sumWithLabSim < minDis){
				minDis = sumWithLabSim;
				minLabel = labelRelDiss[i][k].first;
				minNet = i;
			}
		}
	}
	cout << "Min Label:" << int(minLabel[0]) << "," << int(minLabel[1]) << "," << int(minLabel[2])<<endl;
	cout << "MinNet:" << minNet << endl; 
}

int mainL()
{

	vector<LocalNet> LNs;
	LNs.resize(2);
	LocalNet LN2;

	string srcN2 = "D:\\Patch\\data\\1.jpg";
	string labN2 = "D:\\Patch\\data\\1_L.png";
	string holeN2 = "D:\\Patch\\data\\1_h.png";
	LNs[0].Load(labN2, srcN2);
	LNs[0].fromHole(holeN2);
	LocalNet LN;
	string srcN = "D:\\Patch\\data\\2.jpg";
	string labN = "D:\\Patch\\data\\2_L.png";
	LNs[1].Load(labN, srcN);


	LNs[0].loadSumPatch(193,162);
	LNs[0].loadSumPatch(244,130);
	LNs[0].loadSumPatch(205,278);
	LNs[0].loadSumPatch(219,11);

	LNs[1].loadSumPatch(79,86);
	LNs[1].loadSumPatch(72,244);
	LNs[1].loadSumPatch(122,91);
	LNs[1].loadSumPatch(103,189);
	LNs[1].loadSumPatch(71,5);

	cv::Mat outMask;
	getLabelGuess(outMask, LNs[0], LNs);

	return 0;
}
int main()
{
	const string imgA_path = "D:\\Patch\\Data\\4_m_l.jpg";
//	const string imgA_path = "D:\\Patch\\Data\\1.jpg";
//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	string mskA_path = "D:\\Patch\\data\\House1_m.png";

	const int cluster_num = 8;
	Mat imgA = imread(imgA_path);
//	Mat imgB = imread(imgB_path);

	cv::Mat maskA;

//	maskA = imread(mskA_path, 0);

	Summrarizer summer;
	summer.initialRandPick(imgA, 6);
	summer.RandPatchCluster();
	return 0;
}

/*
int mainR(int argc, _TCHAR* argv[])
{
	const string imgA_path = "D:\\Data\\Patch\\palace\\imageA.jpg";
	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	string labelA_path = "D:\\Patch\\data\\analysis\\label1u_A.png";
	string labelB_path = "D:\\Patch\\data\\analysis\\label1u_B.png";
	const int cluster_num = 8;
	Mat imgA = imread(imgA_path);	Mat imgB = imread(imgB_path);
	image_kit::ik_resizeFixedLong(imgA,imgA,256);
	image_kit::ik_resizeFixedLong(imgB,imgB,256);
	imgA.convertTo(imgA,CV_32F,1.0/255.);
	imgB.convertTo(imgB,CV_32F,1.0/255.);

	int patch_number_in_A = (imgA.rows-kPatch_w)*(imgA.cols-kPatch_w);
	int patch_number_in_B = (imgB.rows-kPatch_w)*(imgB.cols-kPatch_w);

	patch::Patch t_patch;
	t_patch.init(imgA, 0, 0, kPatch_w, kWeight_grad);
	int feature_length = t_patch.feature_len();
	float *data = new float[(patch_number_in_A+patch_number_in_B)*feature_length];
	int datum_num = 0;

	for(int y=0;y+kPatch_w<imgA.rows;++y)
	{
		Vec3f * img_ptr = imgA.ptr<Vec3f>(y);
		for(int x=0;x+kPatch_w<imgA.cols;++x)
		{
			patch::Patch t_patch;
			t_patch.init(imgA, x, y, kPatch_w, kWeight_grad);
			const float* datum = t_patch.feature();
			memcpy(&data[datum_num*feature_length], datum, sizeof(float)* feature_length);
			datum_num++;
		}
	}
	for(int y=0;y+kPatch_w<imgB.rows;++y)
	{
		Vec3f * img_ptr = imgB.ptr<Vec3f>(y);
		for(int x=0;x+kPatch_w<imgB.cols;++x)
		{
			patch::Patch t_patch;
			t_patch.init(imgB, x, y, kPatch_w, kWeight_grad);
			const float* datum = t_patch.feature();
			memcpy(&data[datum_num*feature_length], datum, sizeof(float)* feature_length);
			datum_num++;
		}
	}
	CV_Assert( datum_num == (patch_number_in_A+patch_number_in_B));
	int * label = new int[datum_num];

	Mat label1uA = Mat::zeros(imgA.rows, imgA.cols, CV_8UC1);
	Mat label1uB = Mat::zeros(imgB.rows, imgB.cols, CV_8UC1);
	Mat	imgClusterA = Mat::zeros(imgA.rows,imgA.cols, CV_8UC3);
	Mat imgClusterB = Mat::zeros(imgB.rows,imgB.cols, CV_8UC3);
	
	Vec3b color[cluster_num];
	for(int i=0;i<cluster_num;++i)
	{
		color[i][0] = (8 - i) * 60 % 255;
		color[i][1] = (i + 1) * 40 % 255;
		color[i][2] = (6 - i) * 35 % 255;
	}

	if(!boost::filesystem::exists(labelA_path))
	{
		cout << "Start kmeans" << endl;
		VlKMeansAlgorithm algorithm = VlKMeansLloyd ;
		VlVectorComparisonType distance = VlDistanceL2 ;
		VlKMeans * kmeans = vl_kmeans_new (VL_TYPE_FLOAT, distance);
		vl_kmeans_seed_centers_with_rand_data (kmeans, data, feature_length, datum_num, cluster_num) ;
		vl_kmeans_set_max_num_iterations (kmeans, 100) ;
		vl_kmeans_refine_centers (kmeans, data, datum_num) ;
		float * dis	= new float[datum_num];
		vl_kmeans_quantize(kmeans, label, dis, data, datum_num);

		cout << "end kmeans" << endl;

		for (int i = 0; i < cluster_num; ++i)
		{
			cout << i << endl;
			for (int j = 0; j < 3; ++j)
				cout << int(color[i][j]) << " ";
			cout << endl << "#############" << endl;
		}

		
		int datum_idx =0;

		for(int y=0;y+kPatch_w<imgClusterA.rows;++y)
		{														 
			Vec3b * img_ptr = imgClusterA.ptr<Vec3b>(y);
			uchar* p_label1u = label1uA.ptr<uchar>(y);
			for(int x=0;x+kPatch_w<imgClusterA.cols;++x)
			{
				p_label1u[x] = label[datum_idx] + 1;
				img_ptr[x] = color[label[datum_idx++]];
			}
		}
		cout << "!" << endl;
		for(int y=0;y+kPatch_w<imgClusterB.rows;++y)
		{
			Vec3b * img_ptr = imgClusterB.ptr<Vec3b>(y);
			uchar* p_label1u = label1uB.ptr<uchar>(y);
			for(int x=0;x+kPatch_w<imgClusterB.cols;++x)
			{
				p_label1u[x] = label[datum_idx] + 1;
				img_ptr[x] = color[label[datum_idx++]];	
			}
		}
		// 	SHOW_IMG_TITLE("res//ClusterA",imgClusterA,0);
		// 	SHOW_IMG_TITLE("res//ClusterB",imgClusterB,0);
		//    	SHOW_IMG_TITLE("res//imgA",imgA,0);
		// 	SHOW_IMG_TITLE("res//imgB",imgB,1);



		//Draw the color-label
		cv::Mat pallete;
		pallete.create(100, 800, CV_8UC3);
		pallete.setTo(255);
		for (int i = 0; i < cluster_num; ++i)
		{
			cv::circle(pallete, cv::Point(i * 60 + 40, 50), 30, cv::Scalar(color[i][0],color[i][1],color[i][2]), 10);
		}
		imwrite("D:\\Patch\\data\\analysis\\clusterA.png", imgClusterA);
		imwrite("D:\\Patch\\data\\analysis\\clusterB.png", imgClusterB);
		imwrite("D:\\Patch\\data\\analysis\\imgA.png", imgA);
		imwrite("D:\\Patch\\data\\analysis\\imgB.png", imgB);
		imwrite("D:\\Patch\\data\\analysis\\color_label.png", pallete);

		imwrite(labelA_path, label1uA);
		imwrite(labelB_path, label1uB);

		delete [] dis;

	}
	else
	{
		//Get label from the saved files
		label1uA = imread(labelA_path, 0);
		int label_idx =0;

		for(int y=0;y+kPatch_w<imgClusterA.rows;++y)
		{														 
			Vec3b * img_ptr = imgClusterA.ptr<Vec3b>(y);
			uchar* p_label1u = label1uA.ptr<uchar>(y);
			for(int x=0;x+kPatch_w<imgClusterA.cols;++x)
			{
				label[label_idx] = p_label1u[x] - 1;
				img_ptr[x] = color[label[label_idx++]];
			}
			imwrite("D://Patch//data//analysis//clusterA_load.png", imgClusterA);
		}
	}


	graphBuilder gB(cluster_num, kPatch_w);
	gB.collectDirs(imgA, label);

	for (int i = 0; i < cluster_num; i++)
	{
		for (int j = 0; j < cluster_num; j++)
		{
			string saveName = string("D://Patch//data//analysis//intme//") + 
				boost::lexical_cast<string>(i) + string("_") + boost::lexical_cast<string>(j) + string(".png");
			
			gB.relationStrength[i][j] = gB.drawDir(gB.relationMat[i][j], saveName);
		}
	}
	delete [] label;
//	delete kmeans;
	return 0;
}				*/									  