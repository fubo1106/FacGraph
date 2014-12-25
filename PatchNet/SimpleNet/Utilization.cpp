#include "Utilization.h"

double utilization::dis(IN vector<float>& a, IN vector<float> &b)
{
	double res = 0.0;
	for(int i=0;i<a.size();++i)
		res+=(a[i]-b[i])*(a[i]-b[i]);
	return sqrt(res);
}


void utilization::runAfolder(int argc, char* argv[])
{
	CM_INITIAL("PatchNetFolder");
	section = gCmSet.Val("section");
	outdir = gCmSet.Val("OutDir");
	srcDir = gCmSet.Val("SrcDir");
	new_outdir = gCmSet.Val("OutDir");
	string outdirS(outdir);

	fs::path fullpath(srcDir, fs::native);
	if(!fs::exists(fullpath)) return;
	fs::recursive_directory_iterator end_iter;
	vector<string> imagePaths;
	vector<string> imageSs;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")
				||iter->path().extension() == string(".png")){
					imagePaths.push_back(currentImagePath);
					imageSs.push_back(currentImageS);
			}
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < imagePaths.size(); i++)
	{
		cout << "current image" << imagePaths[i] << endl;
		string currentImageBase = imageSs[i].substr(0, imageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;

		CmInitial("PatchNetFolder.ini", "Log.txt", false, false, currentImageBase.c_str());

		//cout<< "currentImageBase"<<currentImageBase<<endl;

		runOneImage(currentImageBase, outdirS, imagePaths[i]);

		cout << "end of one image" << endl;
	}

	cout << "Run here" << endl;
}

void utilization::runAfolder(string SrcDir,string Outdir )
{
	//section = sec;
	outdir = Outdir.c_str();
	srcDir = SrcDir.c_str();
	new_outdir = Outdir.c_str();
	string outdirS(outdir);

	fs::path fullpath(srcDir, fs::native);
	if(!fs::exists(fullpath)) return;
	fs::recursive_directory_iterator end_iter;
	vector<string> imagePaths;
	vector<string> imageSs;
	for (fs::recursive_directory_iterator iter(fullpath); iter!=end_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")
				||iter->path().extension() == string(".png")){
					imagePaths.push_back(currentImagePath);
					imageSs.push_back(currentImageS);
			}
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < imagePaths.size(); i++)
	{
		cout << "current image " << imagePaths[i] << endl;
		string currentImageBase = imageSs[i].substr(0, imageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;

		CmInitial("PatchNetFolder.ini", "Log.txt", false, false, currentImageBase.c_str());

		//cout<< "currentImageBase"<<currentImageBase<<endl;

		runOneImage(currentImageBase, outdirS, imagePaths[i]);

		cout << "end of one image" << endl;
	}

	cout << "Run here" << endl;
}

void utilization::runFolder(string regionDir,string SrcDir,string regionOutdir,string srcOutDir){
	//section = sec;
	outdir = srcOutDir.c_str();
	srcDir = SrcDir.c_str();
	new_outdir = srcOutDir.c_str();

	regionSrcDir = regionDir.c_str();
	regionOutDir = regionOutdir.c_str();

	//for src image
	string outdirS(outdir);

	fs::path sfullpath(srcDir, fs::native);
	if(!fs::exists(sfullpath)) return;
	fs::recursive_directory_iterator send_iter;
	vector<string> simagePaths;
	vector<string> simageSs;
	for (fs::recursive_directory_iterator iter(sfullpath); iter!=send_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")
				||iter->path().extension() == string(".png")){
					simagePaths.push_back(currentImagePath);
					simageSs.push_back(currentImageS);
			}
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < simagePaths.size(); i++)
	{
		cout << "current image " << simagePaths[i] << endl;
		string currentImageBase = simageSs[i].substr(0, simageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;

		CmInitial("PatchNetFolder.ini", "Log.txt", false, false, currentImageBase.c_str());

		//cout<< "currentImageBase"<<currentImageBase<<endl;

		runOneImage(currentImageBase, outdirS, simagePaths[i]);

		cout << "end of one image" << endl;
	}

	//for region image
	string regionOutdirS(regionOutDir);

	fs::path rfullpath(regionSrcDir, fs::native);
	if(!fs::exists(rfullpath)) return;
	fs::recursive_directory_iterator rend_iter;
	vector<string> rimagePaths;
	vector<string> rimageSs;
	for (fs::recursive_directory_iterator iter(rfullpath); iter!=rend_iter; iter++)
	{
		if(!fs::is_directory(*iter)){
			string currentImagePath = iter->path().string();
			string currentImageS = iter->path().filename().string();
			if(iter->path().extension() == string(".jpg")
				||iter->path().extension() == string(".png")){
					rimagePaths.push_back(currentImagePath);
					rimageSs.push_back(currentImageS);
			}
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < rimagePaths.size(); i++)
	{
		cout << "current image " << rimagePaths[i] << endl;
		string currentImageBase = rimageSs[i].substr(0, rimageSs[i].find_first_of('.'));
		cout << "current section:" << currentImageBase << endl;

		CmInitial("PatchNetFolder.ini", "Log.txt", false, false, currentImageBase.c_str());

		//cout<< "currentImageBase"<<currentImageBase<<endl;

		runOneImage(currentImageBase, regionOutdirS, rimagePaths[i]);

		cout << "end of one image" << endl;
	}

	cout << "Run here" << endl;
}

void utilization::runOneImage(string& sectionS, string& outdirS, string& sourcePath)
{

	//	_paraTol = (*gpSet)("Tol");
	//	CM_INITIAL("PatchNet");
	const char* runOne_section = sectionS.c_str();
	const char* runOne_outdir = outdirS.c_str();
	cout<<"runOne_section "<<string(runOne_section)<<endl;
	cout<<"runOne_outdir "<<string(runOne_outdir)<<endl;
	//	const char* userMaskFN = gCmSet.Val("UserMaskPath");
	string imgA_path = string(sourcePath);
	string temp = string(runOne_outdir) + string(runOne_section) + "\\";
	runOne_outdir = temp.c_str();
	if(!boost::filesystem::exists(runOne_outdir))
		boost::filesystem::create_directories(runOne_outdir);
	string patchesFN = string(runOne_outdir) + "rpatches.txt";
	Summrarizer summer1;
	Summrarizer summer;
	Mat imgA = imread(imgA_path);
	//	Mat userM = imread(userMaskFN, 0);
	if (imgA.rows >= 800 || imgA.cols >= 800)
		cv::resize(imgA,imgA, cv::Size(640, 480));
	if(imgA.rows >= 600 || imgA.cols >= 600)
		summer.initialRandPick(string(runOne_section), string(runOne_outdir), imgA, 6);
	else
		summer.initialRandPick(string(runOne_section), string(runOne_outdir), imgA, 6);
	summer.secName = string(runOne_section);
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
			string fn = string(runOne_outdir) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
			reg = imread(fn, 0);
			summer._finalRegions.push_back(reg.clone());
			curCount ++;
		}

		/*Mat a;a.create(summer._finalPatches[0].width,summer._finalPatches[0].width,CV_8UC3);
		for(int i=0;i<summer._finalPatches.size();i++){
			Mat b = a.clone();
			Shared::drawPatchOnImg(summer._finalPatches[i], b, Scalar(0,0,255));
			imshow("patch",b);waitKey(0);
		}*/

		SingleNet net1(&summer);
		string sectionS = runOne_section;
		string outdirS = runOne_outdir;
		string new_outdirS = new_outdir;
		net1.initNet(sectionS, outdirS, outdirS);
		//	net1.collectRel();
		//		net1.userRelGet(userM);
		//		cout << net1.getSimUserRel() << " !!!!" << endl;
		net1.getSubRegions();
		net1.getAtoms();
		net1.buildHierGraph();
		net1.getRealContext();
		net1.saveToFiles();
		return;
	}

	//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";

	//string mskA_path = "D:\\Patch\\data\\House1_m.png";

	//const int cluster_num = 8;
	summer.RandPatchCluster();

	cout<<summer._summaryPatches.size()<<endl;
	cout<<summer._loc_per_patch.size()<<endl;
	cout<<summer._spMasks.size()<<endl;

	//	int n = summer._spMasks.size();
	int n = summer._summaryPatches.size();
	// 	for(int i=0;i<n;++i)
	// 		SHOW_IMG(summer._spMasks[i],1);
	if(1)
	{
		UnionFindSet ufs(n);								   
		for(int i=0;i<ufs.n();++i)
			ufs.setRank(i,cv::countNonZero(summer._spMasks[i]));
		//z
		//	vector<Mat> masks = summer._spMasks;
		vector<Mat>& masks = summer._spMasks;

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



		/********************Show Cluster Result***************************/
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
					//imshow("_loc_per_patch",show);waitKey(0);
				
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
			//		imwrite(runOne_outdir+na,st[i].shower);
			summer._finalRegions.push_back(st[i].region.clone());
			summer._finalPatches.push_back(st[i].sp);
			ofs << st[i].sp.center.x << " " << st[i].sp.center.y << endl;
			na = "zcover_" + boost::lexical_cast<string>(i)+".png";
			imwrite(runOne_outdir + na, st[i].region);
		}


		//画patch中心以及代表型矩形框
		fill_n(used,1000,0);
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
		string temps = string(runOne_outdir);
		temps = temps.substr(0, temps.find_last_of("\\"));

		imwrite(temps+"zcenter.png",show_center);
		Mat res ;
		addWeighted(summer._nonBlured,0.4,show_center,0.6,0,res);
		// 	SHOW_IMG(res,1);
		int nameidx = imgA_path.find_last_of("/")+1;
		string savename = imgA_path.substr(nameidx,imgA_path.find_last_of(".")-nameidx);
		cout<<		imgA_path.find_last_of(".")<<endl;
		cout<<savename<<endl;
		//   imwrite(runOne_outdir+"cluster_"+savename+".png",res);
		imwrite(temps+"zcluster.png",res);

	}

	//z
	SingleNet net1(&summer);
	net1.initNet(string(runOne_section), string(runOne_outdir), string(runOne_outdir));
	net1.getSubRegions();
	net1.getAtoms();
	net1.buildHierGraph();
	net1.getRealContext();
	net1.saveToFiles();

}								  