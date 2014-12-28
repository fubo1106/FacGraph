
#include "FacBuilder.h"
#include "Basic_File.h"
#include "Basics.h"
#include "GraphMatching.h"
#include "DP.h"



int main(){
	//string imgA_path = "facades\\";
//	const string imgA_path = "D:\\Patch\\Data\\1.jpg";
//	const string imgB_path = "D:\\Data\\Patch\\palace\\imageB.jpg";
	
	/*string mskA_path = "..\\facades\\fac (34).jpg";
	string path;
	string out = "facades\\output\\";
	string outpath;
	Summrarizer summer;
	const int cluster_num = 8;*/

	/*string sec = "100";
	string path2 = imgA_path + sec + ".jpg";*/

	/*this is for patchnet building*/
	//string regionDir = "image-region\\";
	//string srcDir = "image-src\\";
	//string rOutDir = "image-region-result\\";
	//string sOutDir = "image-src-result\\";
	////runAfolder(regionDir,OutDir );
	//runFolder(regionDir,srcDir,rOutDir,sOutDir);
	
	//end building

	//runOneImage(sec,out,path2);

//	for(int i=1;i<=25;i++){
//		
//		path = imgA_path + num2str(i) + ".jpg";
//		outpath = out + num2str(i) + "\\";
//		patchesFN =  outpath + "rpatches.txt";
//		ofstream ofs(patchesFN);
//		Mat imgA = imread(path);
//		
//		summer.initialRandPick(num2str(i),outpath,imgA, 6);
//		summer.RandPatchCluster();
//
//		for(int j=0;j<summer._summaryPatches.size();j++){
//			summer._finalPatches.push_back(summer._summaryPatches[j]);
//			summer._finalRegions.push_back(summer._spMasks[j]);
//			ofs << summer._finalPatches[j].center.x << " " << summer._finalPatches[j].center.y << endl;
//		}
//		ofs.close();
//		//cout<<"done"<<endl;
//		//summer.~Summrarizer();
//
//		SingleNet net1(&summer);
//		net1.initNet(num2str(i), outpath, outpath);
//		net1.collectRel();
////		net1.userRelGet(userM);
////		cout << net1.getSimUserRel() << " !!!!" << endl;
//		net1.getSubRegions();
//		net1.getAtoms();
//		net1.buildHierGraph();
//		net1.getRealContext();
//		net1.saveToFiles();
//		//summer.~Summrarizer();
//		
//		cout << "Start Explorer" << endl;
//		char* userMaskFN = "facades\\userMask.png";//1.jpg
//		string resSaveDirS = "D:\\Project\\PatchNet\\PatchNet\\facades\\_ne_outdir";
//		NetExplorer ne(&summer);
//		ne.getUserNet(userMaskFN);
//	 	ne.compNodeInFolder_file(resSaveDirS);
//		
//	}

	//explore code
//	section = "10";
//
//	string depthDir = "image-depth\\";
//
//	outdir = "image-src-result\\";
//	srcDir = "iamage-src\\";
//
//	regionOutDir = "image-region-result\\";
//	regionSrcDir = "image-region\\";
//
//	const char* userMaskFN = "patchnet-userMask\\mask-10.png";
//
//	string imgSrc_path = string(srcDir) + string(section) + ".png";
//	string tempSrc = string( outdir) + string(section) + "\\";
//	string imgReg_path = string(regionSrcDir) + string(section) +".png";
//	string tempReg = string(regionOutDir) + string(section)+"\\";
//
//	string resSaveDirS = "patchnet-output\\";
//	//outdir = temp.c_str();
//	if(!boost::filesystem::exists(tempSrc))
//		boost::filesystem::create_directory(tempSrc);
//	if(!boost::filesystem::exists(tempReg))
//		boost::filesystem::create_directories(tempReg);
//
//	string patchesSrcFN = string(tempSrc) + "rpatches.txt";
//	string patchesRegFN = string(tempReg) + "rpatches.txt";
//	
//	Summrarizer summer_src,summer_reg;
//
//	Mat imgSrc = imread(imgSrc_path);
//	Mat imgReg = imread(imgReg_path);
//
//	if (imgSrc.rows >= 800 || imgSrc.cols >= 800)
//		cv::resize(imgSrc,imgSrc, cv::Size(640, 480));
//	if (imgReg.rows >= 800 || imgReg.cols >= 800)
//		cv::resize(imgReg,imgReg, cv::Size(640, 480));
//
//	Mat userM = imread(userMaskFN, 0);
//
//	summer_src.initialRandPick(string(section), string(outdir), imgSrc, 6);
//	summer_reg.initialRandPick(string(section), string(regionOutDir), imgReg, 6);
//
//	cout << "Start Loading User Mask" << endl;
//	if(boost::filesystem::exists(patchesSrcFN))
//	{
//		ifstream ifs(patchesSrcFN);
//		int x, y; int curCount = 0;
//		summer_src._finalPatches.clear();
//		summer_src._finalRegions.clear();
//		while(!ifs.eof())
//		{
//			ifs >> x;
//			if(ifs.eof())
//				break;
//			ifs >> y;
//			sPatch sp;
//			sp.center.x = x; sp.center.y = y;
//			sp.width = summer_src._patchRadius * 2 + 1;
//			sp.aveClr = summer_src._averImg.ptr<Vec3b>(y)[x];
//			sp.aveGrad = summer_src._aveGrad.ptr<uchar>(y)[x];
//			summer_src._finalPatches.push_back(sp);
//			cv::Mat reg;
//			string fn = string(tempSrc) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
//			reg = imread(fn, 0);
//			summer_src._finalRegions.push_back(reg.clone());
//			curCount ++;
//		}
//
//	//	SingleNet net1(&summer);
//	//	net1.initNet();
////		net1.collectRel();
//	//	net1.userRelGet(userM);
//		//net1.getSimUserRel() << " !!!!" << endl;
//		/*cout << "Start Explorer" << endl;
//		NetExplorer ne(&summer_src);
//		ne.getUserNet(userMaskFN);
// 		ne.compNodeInFolder_file(resSaveDirS);*/	
//	}
//
//	//summer Reg
//	if(boost::filesystem::exists(patchesRegFN))
//	{
//		ifstream ifs(patchesRegFN);
//		int x, y; int curCount = 0;
//		summer_reg._finalPatches.clear();
//		summer_reg._finalRegions.clear();
//		while(!ifs.eof())
//		{
//			ifs >> x;
//			if(ifs.eof())
//				break;
//			ifs >> y;
//			sPatch sp;
//			sp.center.x = x; sp.center.y = y;
//			sp.width = summer_reg._patchRadius * 2 + 1;
//			sp.aveClr = summer_reg._averImg.ptr<Vec3b>(y)[x];
//			sp.aveGrad = summer_reg._aveGrad.ptr<uchar>(y)[x];
//			summer_reg._finalPatches.push_back(sp);
//			cv::Mat reg;
//			string fn = string(tempSrc) + "zcover_" + boost::lexical_cast<string>(curCount)+".png";
//			reg = imread(fn, 0);
//			summer_reg._finalRegions.push_back(reg.clone());
//			curCount ++;
//		}
//
//	//	SingleNet net1(&summer);
//	//	net1.initNet();
////		net1.collectRel();
//	//	net1.userRelGet(userM);
//		//net1.getSimUserRel() << " !!!!" << endl;
//		/*cout << "Start Explorer" << endl;
//		NetExplorer ne(&summer_src);
//		ne.getUserNet(userMaskFN);
// 		ne.compNodeInFolder_file(resSaveDirS);*/	
//	}
//		cout << "Start Explorer" << endl;
//		vector<SecInf> matchedSecs;
//		NetExplorer ne(&summer_src, &summer_reg);
//
//		ne.getUserNet(userMaskFN);
//		
// 		ne.compNodeInFolder_file(resSaveDirS);
//		ne.compNodeInFolder_file_mine(resSaveDirS,matchedSecs);
//		int truth_depth,esti_depth;
//		getEstDepth(matchSecs,depthDir,esti_depth);
//end  explore
		
//		return 0;
//	}

//end explore	
	Basic_File fileop;
	Mat src = imread("12.jpg");Mat dst;
	Mat mask = imread("12-cut.png",0);
	
	maskProcess(src,mask);

	ImageSegmentByKMeans2(src,dst,3,1);
	namedWindow("cluster",0);imshow("cluster",dst);waitKey(0);

	Mat region;
	int flag = fileop.LoadData("1.txt",region,src.rows,src.cols);

	FacBuilder builder;
	GraphMatching gMatch;

	//builder.getNodesFromImg(src,region);
	builder.buildGraph(src,region);

	FacGraph facG= builder._facGraph;

	builder.buildAllSubGraphes(facG);
	
	SubGraph sub1,sub2;
	for(int i=0;i<builder._allSubGraphes.size();i++)
		for(int j=0;j<builder._allSubGraphes.size();j++){
			sub1 = builder._allSubGraphes[i];
			sub2 = builder._allSubGraphes[j];
			builder._allSubGraphes[i].drawSubGraph(src.clone(),"sub#1");
			builder._allSubGraphes[j].drawSubGraph(src.clone(),"sub#2");
			gMatch.initMatching(sub1,sub2);
			gMatch.oneSubGraphMatching(sub1,sub2);
			cout<<"matchScore:"<<gMatch._oneMatch.matchScore<<endl;
		}
	//for(int i=0;i<facG._nodes.size();i++){
	//	
	//	builder.buildSubGraph(facG._nodes[i],facG);
	//	//builder._facGraph.drawGraph(src,Scalar(0,255,0),Scalar(255,0,0),Scalar(0,0,255));
	//	//builder._subGraph.drawSubGraph(src.clone());
	//	builder.~FacBuilder();
	//}
	
	double distance = gMatch.disOfTwoNodes(facG._nodes[0],facG._nodes[2]);

	/*DP dp;
	dp.performDP();*/

	return 0;
}