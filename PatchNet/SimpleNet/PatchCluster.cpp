#include "stdafx2.h"

extern "C" {

//#include "kmeans.h"
}
//#include "patch.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "Basic/CmSetting.h"
//#include "image_kit.h"
#include "graphBuilder.h"
#include "LocalNet.h"
#include "SingleNet.h"
#include "Summrarizer.h"
#include "union_find_set.h"


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

double dist(IN vector<float>& a, IN vector<float> &b)
{
	double res =0.0;
	for(int i=0;i<a.size();++i)
		res+=(a[i]-b[i])*(a[i]-b[i]);
	return sqrt(res);
}

int cluster(IN vector<vector<float>> &feature, OUT vector<vector<float>> &center, OUT vector<vector<int>>& cluster_idx_set, double phi)
{
	int n = feature.size();
	int k = 1;
	center.clear();
	cluster_idx_set.clear();
	center.push_back(feature[0]);
	cluster_idx_set.resize(feature.size());

	for(int i=1; i<n; ++i)
	{
		double min_dist= 1e8;
		int cluster_idx =-1;
		
		for(int j=0;j<center.size();++j)
		{
			double d = dist(feature[i],center[j]);
			if(d< min_dist)
			{
				min_dist= d;
				cluster_idx = j;
			}
		}

		if( min_dist< phi)
			cluster_idx_set[cluster_idx].push_back(i);
		else
			center.push_back(feature[i]);
	}
	return center.size();
}

int mainO(int argc, char* argv[])
{

	CM_INITIAL("PatchNet");
//	const string imgA_path = "D:\\Patch\\Data\\4_m_l.jpg";
//		const string imgA_path = "D:\\Patch\\Data\\10.jpg";
	//	const string imgA_path = "D:\\Patch\\Data\\1.jpg";
	section = gCmSet.Val("section");
	outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");
		string imgA_path = string(srcDir) + string(section) + ".jpg";
		string temp = string(outdir) + string(section) + "\\";
		outdir = temp.c_str();
	if(!boost::filesystem::exists(outdir))
		boost::filesystem::create_directory(outdir);

//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	//string mskA_path = "D:\\Patch\\data\\House1_m.png";

	//const int cluster_num = 8;
	Mat imgA = imread(imgA_path);
	//image_kit::ik_resizeFixedLong(imgA,imgA,500);
	//resize(imgA,imgA,Size(500,500));
//	SHOW_IMG(imgA,1);
//	Mat imgB = imread(imgB_path);

// 	cv::Mat maskA;
// 
// 	maskA = imread(mskA_path, 0);

//	maskA = imread(mskA_path, 0);

	Summrarizer summer;
	summer.initialRandPick(string(section), string(outdir), imgA, 6);
	summer.RandPatchCluster();

	cout<<summer._summaryPatches.size()<<endl;
	cout<<summer._loc_per_patch.size()<<endl;
	cout<<summer._spMasks.size()<<endl;

	int n = summer._spMasks.size();
// 	for(int i=0;i<n;++i)
// 		SHOW_IMG(summer._spMasks[i],1);

	UnionFindSet ufs(n);								   
	for(int i=0;i<ufs.n();++i)
		ufs.setRank(i,cv::countNonZero(summer._spMasks[i]));

	vector<Mat> masks = summer._spMasks;

	vector<vector<float>> feature;
	feature.resize(summer._summaryPatches.size());
	for(int i=0; i < summer._summaryPatches.size(); ++i)
	{
		sPatch sp = summer._summaryPatches[i];
		Mat patch = summer._srcImg(Rect(sp.center.x - sp.width/2,sp.center.y - sp.width/2,sp.width,sp.width));
		
		//SHOW_IMG(patch,1)
		for(int y=0;y<patch.rows;++y)
		{
			for(int x=0;x<patch.cols;++x)
			{
				for(int c=0;c<3;++c)
				{
					Vec3b b = patch.at<Vec3b>(y,x);
					feature[i].push_back(static_cast<float>(b[c]));
					//cout<<static_cast<float>(b[0])<<"	"<<static_cast<float>(b[1])<<"	"<<static_cast<float>(b[2])<<endl;
				}
			}
		}
	}


	double w = 0.3;
	while(1)
	{
		bool flag = 0;
		for(int i=0;i<n;++i)
		{
			for(int j=0;j<i;++j)
			{
				int x = ufs.find(i);
				int y = ufs.find(j);
				if(x != y)
				{
					Mat bAnd;
					cv::bitwise_and(masks[x], masks[y], bAnd);
					
					int count_x = cv::countNonZero(masks[x]);
					int count_y = cv::countNonZero(masks[y]);
					int count_and = cv::countNonZero(bAnd);

					if(
						((count_and>=w*count_x && count_and>=w*count_y) 
						|| (count_and >= 2.5*w*count_x) 
						|| (count_and >= 2.5 * w * count_y) 
						||(summer._relGroups.ptr<float>(x)[y] > 3000 && summer._relGroups.ptr<float>(y)[x] > 3000))
						&& dist(feature[i],feature[j])<1000)
						
					
					{
						flag = 1;
						ufs.uni(i,j);
						Mat bOr;
						cv::bitwise_or(masks[x],masks[y],bOr);
						ufs.setRank(ufs.find(i), countNonZero(bOr));
						masks[ufs.find(i)] = bOr.clone();
					}

				}
			}
		}
		if(flag == 0)
			break;
	}

	cout<<ufs.n()<<endl;
	


	/********************Show Cluster Result***************************/
	Scalar color[100];
	for(int i=0;i<100;++i)
		color[i] = Scalar(rand()%255,rand()%255,rand()%255);

	Mat show = Mat ::zeros(summer._srcImg.size(),CV_8UC3);
	struct sorter
	{
		int val;
		Mat shower;
		Mat region;
		bool operator<(const sorter & rhs){return val> rhs.val;}
	};

	vector<sorter> st;
	bool used[100] = {0};
	fill_n(used,100,0);
	for(int i=0;i<n;++i)
	{
		int x = ufs.find(i);
		if(!used[x])
		{
			used[x]=1;
			for(int j=0;j<summer._loc_per_patch[i].size();++j)
				circle(show,summer._loc_per_patch[i][j],1,color[x],3);
			Mat tp = Mat::zeros(show.size(),CV_8UC3);
			tp.setTo(color[x]*0.8,masks[x]);

			sPatch sp = summer._summaryPatches[x];
			Rect roi = Rect(sp.center.x-sp.width/2,sp.center.y-sp.width/2,sp.width,sp.width);

			Mat ttp;
			addWeighted(summer._nonBlured,0.5,tp,1,0,ttp);
			rectangle(ttp,roi,CV_RGB(255,0,0),2);
			//SHOW_IMG(ttp,1);
			//addWeighted(show,1.0,tp,1,0,show);
			sorter tmp_sorter;
			tmp_sorter.val = cv::countNonZero(masks[x]);
			tmp_sorter.region = masks[x].clone();
			tmp_sorter.shower = ttp.clone();
			st.push_back(tmp_sorter);


	
		}
	}
	sort(st.begin(),st.end());

	//按面积降序输出cover
	for(int i=0;i<st.size();++i)
	{
		string na = "show_cover" + boost::lexical_cast<string>(i)+".png";
	//	SHOW_IMG(st[i].shower,1);
	//	imwrite(outdir+na,st[i].shower);
		summer._finalRegions.push_back(st[i].region.clone());
	}


	//画patch中心以及代表型矩形框
    fill_n(used,100,0);
	Mat show_center = Mat::zeros(summer._srcImg.size(),CV_8UC3);

 	for(int i=0;i<n;++i)
	{
		int x = ufs.find(i);
		sPatch sp = summer._summaryPatches[x];
		summer._finalPatches.push_back(sp);
		if(!used[x])
		{
			//used[x] = 1;
			if(summer._loc_per_patch[i].size()>10)
			{
				for(int j=0;j<summer._loc_per_patch[i].size();++j)
				{
					circle(show_center,summer._loc_per_patch[i][j],2,color[x],2);
				}
				Rect roi = Rect(sp.center.x-sp.width/2,sp.center.y-sp.width/2,sp.width,sp.width);
				//rectangle(show_center,roi,CV_RGB(255,0,0),2);
			}
		}
	}



	//	SHOW_IMG(show_center,1);
	string temps = string(outdir);
	temps = temps.substr(0, temps.find_last_of("\\"));
	imwrite(temps+"zcenter.png",show_center);
	Mat res ;
	addWeighted(summer._nonBlured,0.4,show_center,0.6,0,res);
// 	SHOW_IMG(res,1);
	int nameidx = imgA_path.find_last_of("/")+1;
	string savename = imgA_path.substr(nameidx,imgA_path.find_last_of(".")-nameidx);
	cout<<		imgA_path.find_last_of(".")<<endl;
	cout<<savename<<endl;
 //   imwrite(outdir+"cluster_"+savename+".png",res);
	imwrite(temps+"zcluster.png",res);


	//z
	SingleNet net1(&summer);
	net1.initNet(string(section), string(outdir), string(outdir));
	net1.collectRel();
	return 0;
	/*******************Cluster 1*****************************/
	/*vector<vector<float>> feature;
	vector<vector<float>> center;
	vector<vector<int>> cluster_idx_set;

	feature.resize(summer._summaryPatches.size());
	for(int i=0; i < summer._summaryPatches.size(); ++i)
	{
		sPatch sp = summer._summaryPatches[i];
		Mat patch = summer._srcImg(Rect(sp.center.x - sp.width/2,sp.center.y - sp.width/2,sp.width,sp.width));
		
		//SHOW_IMG(patch,1)
		for(int y=0;y<patch.rows;++y)
		{
			for(int x=0;x<patch.cols;++x)
			{
				for(int c=0;c<3;++c)
				{
					Vec3b b = patch.at<Vec3b>(y,x);
					feature[i].push_back(static_cast<float>(b[c]));
					//cout<<static_cast<float>(b[0])<<"	"<<static_cast<float>(b[1])<<"	"<<static_cast<float>(b[2])<<endl;
				}
			}
		}
	}

	int cluster_num;
	double phi = 5000;
	for( ; phi>0.0; phi-=50)
	{	
		cluster_num = cluster(feature, center, cluster_idx_set, phi);
		if(cluster_num>10)
			break;
		cout<<"phi "<<phi<<"	"<<endl;
	}
	
	Mat show_cluster= Mat::zeros(summer._srcImg.size(),CV_8UC3);
	Mat mask_show = Mat::zeros(summer._srcImg.size(),CV_8UC1);
	SHOW_IMG(show_cluster,1);
	//Colors 
	Scalar color[50];
	for(int i=0;i<50;++i)
		color[i] = Scalar(rand()%255,rand()%255,rand()%255);

	//Mask color for show

	int rad = summer._patchRadius;
	//Mat colorMat=Mat::zeros(2*rad+1,2*rad+1,CV_8UC3);
	for(int i=0;i<cluster_num;++i)
	{
		//colorMat.setTo(color[i]);
		//SHOW_IMG(colorMat,1);
		Mat now_mask=Mat::zeros(mask_show.size(),CV_8UC1);
		Mat now_show=Mat::zeros(mask_show.size(),CV_8UC3);
		for(int j=0;j<cluster_idx_set[i].size();++j)
		{
			int sp_idx = cluster_idx_set[i][j];
			vector<Point> locs = summer._loc_per_patch[sp_idx];

			for(int k = 0;k<locs.size();++k)
			{
				Rect roi(locs[k].x-rad,locs[k].y-rad,2*rad+1,2*rad+1);
				if(cv::countNonZero(mask_show(roi))==0)
				{
					//addWeighted(show_cluster(roi),0.5,colorMat,0.5,0,show_cluster(roi));
					//bitwise_or(now_mask(roi),Mat::ones(now_mask(roi).size(),CV_8UC1),now_mask(roi));
					now_mask(roi).setTo(Scalar(255,255,255));
					now_show(roi).setTo(color[i]);
				}
			}
		}
		mask_show.setTo(Scalar(255,255,255),now_mask);
		SHOW_IMG(mask_show,1);
		show_cluster.setTo(color[i],now_mask);
		SHOW_IMG(show_cluster,1);
		
	}
	Mat final;
	addWeighted( summer._nonBlured,0.5,show_cluster,0.5,0,final);
	SHOW_IMG(final,1);
	imwrite(outdir+"cluster.png",final);
    return 0;
*/

}




/*
int mainR(int argc, _TCHAR* argv[])
{
	const string imgA_path = "D:\\Data\\Patch\\palace\\imageA.jpg";
	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	string labelA_path = "D:\\Patch\\data\\analysis\\label1u_A.png";
	string labelB_path = "D:\\Patch\\data\\analysis\\label1u_B.png";
	const int cluster_num = 8;
	Mat imgA = imread(imgA_path);
	Mat imgB = imread(imgB_path);
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
	vl_uint32 * label = new vl_uint32[datum_num];

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
}			*/										  