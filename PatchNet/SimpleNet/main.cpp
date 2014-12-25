#include "stdafx2.h"
#include "Utilization.h"
#include <omp.h>
#include "../Basics.h"
using namespace utilization;
//extern "C" VlKMeans * 	vl_kmeans_new (vl_type dataType, VlVectorComparisonType distance);




#if 0
int main1(int argc, char* argv[])  
{
	runAfolder(argc, argv); 
	return 0;
	CM_INITIAL("PatchNetFolder");
	section = gCmSet.Val("section");
	outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");
	const char* userMaskFN = gCmSet.Val("UserMaskPath");
	string imgA_path = string(srcDir) + "src\\" + string(section) + ".jpg";
	string temp = string( outdir) + string(section) + "\\";
	string resSaveDirS = temp.c_str() ;
	outdir = temp.c_str();
	if(!boost::filesystem::exists(outdir))
		boost::filesystem::create_directory(outdir);
	string patchesFN = string(outdir) + "structPat.txt";
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
	Mat userM = imread(userMaskFN, 0);
	summer.initialRandPick(string(section), string(outdir), imgA, 6);
	cout << "Start Loading User Mask" << endl;
	if(boost::filesystem::exists(patchesFN))
	{
		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
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
			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			reg = imread(fn, 0);
			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}

	//	SingleNet net1(&summer);
	//	net1.initNet();
//		net1.collectRel();
	//	net1.userRelGet(userM);
		//net1.getSimUserRel() << " !!!!" << endl;
		cout << "Start Explorer" << endl;
		NetExplorer ne(&summer);
		ne.getUserNet(userMaskFN);
 		ne.compNodeInFolder_file(resSaveDirS);
		
		return 0;
	}

	return 0;
}
#endif

#if 0
int mainOne(int argc, char* argv[])  
{
	//	_paraTol = (*gpSet)("Tol");
	CM_INITIAL("PatchNet");
	section = gCmSet.Val("section");
	outdir = gCmSet.Val("OutDir");
	new_outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");
	const char* userMaskFN = gCmSet.Val("UserMaskPath");
	string imgA_path = string(srcDir) + string(section) + ".jpg";
	string temp = string(outdir) + string(section) + "\\";
	outdir = temp.c_str();
	if(!boost::filesystem::exists(outdir))
		boost::filesystem::create_directory(outdir);
	string patchesFN = string(outdir) + "rpatches.txt";
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
	Mat userM = imread(userMaskFN, 0);
	summer.initialRandPick(string(section), string(outdir), imgA, 6);

	if(boost::filesystem::exists(patchesFN))
	{
		ifstream ifs(patchesFN);
		int x, y; int curCount = 0;
		summer._finalPatches.clear();
		summer._finalRegions.clear();
		while(!ifs.eof())
		{
			ifs >> x;
			if(ifs.eof())
				break;
			ifs >> y;
			sPatch sp;
			sp.center.x = x; sp.center.y = y;
			sp.width = summer._patchRadius * 2 + 1;
			summer._finalPatches.push_back(sp);
			cv::Mat reg;
			string fn = string(outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			reg = imread(fn, 0);
			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}

		SingleNet net1(&summer);
		net1.initNet(string(section), string(outdir), string(outdir));
		net1.collectRel();
//		net1.userRelGet(userM);
//		cout << net1.getSimUserRel() << " !!!!" << endl;
		net1.getSubRegions();
		net1.getAtoms();
		net1.buildHierGraph();
		net1.getRealContext();
		return 0;
	}

//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	//string mskA_path = "D:\\Patch\\data\\House1_m.png";

	//const int cluster_num = 8;
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
						&& dis(feature[i],feature[j])<1000)
						
					
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
	


	/ ********************Show Cluster Result*************************** /
	Scalar color[1000];
	for(int i=0;i<1000;++i)
		color[i] = Scalar(rand()%255,rand()%255,rand()%255);

	Mat show = Mat ::zeros(summer._srcImg.size(),CV_8UC3);
	struct sorter
	{
		int val;
		sPatch sp;
		Mat shower;
		Mat region;
		bool operator<(const sorter & rhs){return val> rhs.val;}
	};

	vector<sorter> st;
	bool used[1000] = {0};
	fill_n(used,1000,0);
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
			tmp_sorter.sp = sp;
			st.push_back(tmp_sorter);


	
		}
	}
	sort(st.begin(),st.end());
	ofstream ofs(patchesFN);

	//按面积降序输出cover
	for(int i=0;i<st.size();++i)
	{
		string na = "show_cover" + boost::lexical_cast<string>(i)+".png";
	//	SHOW_IMG(st[i].shower,1);
//		imwrite(outdir+na,st[i].shower);
		summer._finalRegions.push_back(st[i].region.clone());
		summer._finalPatches.push_back(st[i].sp);
		ofs << st[i].sp.center.x << " " << st[i].sp.center.y << endl;
		na = "zcover_" + boost::lexical_cast<string>(i)+".png";
		imwrite(outdir + na, st[i].region);
	}


	//画patch中心以及代表型矩形框
    fill_n(used,100,0);
	Mat show_center = Mat::zeros(summer._srcImg.size(),CV_8UC3);


 	for(int i=0;i<n;++i)
	{
		int x = ufs.find(i);
		sPatch sp = summer._summaryPatches[x];
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
	ofs.flush();
	ofs.close();


	//	SHOW_IMG(show_center,1);
	string temps = string(outdir);
	temps = temps.substr(0, temps.find_last_of("\\"));
//	temps = temps.substr(0, temps.find_last_of("\\"));
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

}
#endif

#if 0
int maindeno(int argc,char* argv[])
{
	const string imgA_path = "D:\\Projects\\code\\PatchNet\\PatchNet\\facades\\";
//	const string imgA_path = "D:\\Patch\\Data\\1.jpg";
//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	string mskA_path = "..\\facades\\fac (34).jpg";
	string path;
	string out = "D:\\Projects\\code\\PatchNet\\PatchNet\\facades\\output\\";
	string outpath;
	Summrarizer summer;
	const int cluster_num = 8;
	for(int i=1;i<=12;i++){
		
		path = imgA_path + num2str(i) + ".jpg";
		outpath = out + num2str(i) + "\\";
		Mat imgA = imread(path);
		
		summer.initialRandPick(num2str(i),outpath,imgA, 6);
		summer.RandPatchCluster();
		summer.~Summrarizer();
		
	}
	//Mat imgA = imread("D:\\Projects\\code\\PatchNet\\PatchNet\\facades\\1.jpg");
//	Mat imgB = imread(imgB_path);

	//string out = "D:\\Projects\\code\\PatchNet\\PatchNet\\facades\\output\\";
//	maskA = imread(mskA_path, 0);
	
	//Summrarizer summer;
	//summer.initialRandPick(num2str(i),out,imgA, 6);
	//summer.RandPatchCluster();
	return 0;

//	cout << "Start Explorer" << endl;
//	NetExplorer ne(&summer);
//	ne.getUserNet(userMaskFN);
// 	ne.compNodeInFolder_file(resSaveDirS);
}
#endif