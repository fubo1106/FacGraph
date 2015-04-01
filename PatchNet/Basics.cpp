#include "Basics.h"

cv::Point calcCenterPoint(vector<cv::Point>& points){
	int sum_x = 0,sum_y = 0;
	for(int i=0;i<points.size();i++){
		sum_x += points[i].x;
		sum_y += points[i].y;
	}
	return cv::Point(sum_x/points.size(),sum_y/points.size());
}

int calcAvgGray(vector<cv::Point>& contour, cv::Mat& grayImg){
	int sumGray=0;
	for(int i=0;i<contour.size();i++){
		sumGray += grayImg.at<uchar>(contour[i].x,contour[i].y);
	}
	return sumGray/contour.size();
}

cv::Vec3b calcAvgColor(vector<cv::Point>& contour, cv::Mat& colorImg){
	if(colorImg.channels() == 1){
		cout<<"calcAvgColor(): input img must be 3 channels image!!"<<endl;
		return 0;
	}

	int sum_b=0,sum_g=0,sum_r=0;
	for(int i=0;i<contour.size();i++){
		sum_b += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[0];
		sum_g += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[1];
		sum_r += colorImg.at<cv::Vec3b>(contour[i].x,contour[i].y)[2];
	}
	return cv::Vec3b(sum_b/contour.size(),sum_g/contour.size(),sum_r/contour.size());
}

cv::Scalar randColor(){
	int x=0,y=255;
	int b = x+rand()%(y-x+1);
	int g = x+rand()%(y-x+1);
	int r = x+rand()%(y-x+1);
	return cv::Vec3b(b,g,r);
}

double distanceP2P(const cv::Point& p1,const cv::Point& p2){
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) );
}

double calcAngleOf2Vec(cv::Point& vec1,cv::Point& vec2){
	double cos_theta = (vec1.x * vec2.x + vec1.y * vec2.y) / (distanceP2P(cv::Point(0,0),vec1) * distanceP2P(cv::Point(0,0),vec2));
	return cos_theta;
}

cv::Point getGravityCenter(vector<cv::Point>& contour){
	Moments mu;
	mu = moments(contour,false);
	return cv::Point( mu.m10/mu.m00 , mu.m01/mu.m00 );
	//return cv::Point( mu.m01/mu.m00 , mu.m10/mu.m00 );//x y相反 代表图像中的位置
}

bool ColorImageSegmentByKMeans2 ( const   IplImage *  img ,  IplImage *  pResult, int nClusters,  int   sortFlag )

{
        assert ( img  !=  NULL &&  pResult  !=  NULL );

        assert ( img -> nChannels == 3 &&  pResult -> nChannels  == 1);

        int   i , j ;

        CvMat * samples = cvCreateMat (( img -> width )*( img -> height ),1, CV_32FC3 ); // 创建样本矩阵， CV_32FC3 代表位浮点通道（彩色图像）

        CvMat * clusters = cvCreateMat (( img -> width )*( img -> height ),1, CV_32SC1 ); // 创建类别标记矩阵， CV_32SF1 代表位整型通道

        int   k =0;

        for  ( i =0; i < img -> width ; i ++)

       {

               for ( j =0; j < img -> height ; j ++)

              {

                      CvScalar s ;

                      // 获取图像各个像素点的三通道值（ RGB ）

                      s . val [0]=( float ) cvGet2D ( img , j , i ). val [0];

                      s . val [1]=( float ) cvGet2D ( img , j , i ). val [1];

                      s . val [2]=( float ) cvGet2D ( img , j , i ). val [2];

                      cvSet2D ( samples , k ++,0, s ); // 将像素点三通道的值按顺序排入样本矩阵

              }

       }

		
        cvKMeans2 ( samples , nClusters , clusters , cvTermCriteria ( CV_TERMCRIT_ITER ,50,1.0)); // 开始聚类，迭代次，终止误差 .0

        k =0;

        int   val =0;

        float   step =255/( nClusters -1);

        for  ( i =0; i < img -> width ; i ++)

       {

               for ( j =0; j < img -> height ; j ++)

              {

                      val =( int ) clusters -> data . i [ k ++];

                      CvScalar s ;

                      s . val [0]=255- val * step ; // 这个是将不同类别取不同的像素值，

                      cvSet2D ( pResult , j , i , s );         // 将每个像素点赋值       

              }

       }

        cvReleaseMat (& samples );

        cvReleaseMat (& clusters );

        return   true ;

}

bool GrayImageSegmentByKMeans2 ( const   IplImage *  pImg ,  IplImage * pResult , int nClusters, int sortFlag )

{

        assert ( pImg  !=  NULL &&  pImg -> nChannels == 1);

        // 创建样本矩阵， CV_32FC1 代表位浮点通道（灰度图像）

        CvMat * samples  =  cvCreateMat (( pImg -> width )* ( pImg -> height ),1,  CV_32FC1 );

        // 创建类别标记矩阵， CV_32SF1 代表位整型通道

        CvMat * clusters  =  cvCreateMat (( pImg -> width )* ( pImg -> height ),1,  CV_32SC1 );

        // 创建类别中心矩阵

        CvMat * centers  =  cvCreateMat ( nClusters , 1,  CV_32FC1 );

        //  将原始图像转换到样本矩阵

       {

               int k  = 0;

               CvScalar s ;

               for ( int   i  = 0;  i  <  pImg -> width ;  i ++)

              {

                      for ( int   j =0; j  <  pImg -> height ;  j ++)

                     {

                             s . val [0] = ( float ) cvGet2D ( pImg ,  j ,  i ). val [0];

                             cvSet2D ( samples , k ++, 0,  s );

                     }

              }

       }

        // 开始聚类，迭代次，终止误差 .0

       cvKMeans2 ( samples ,  nClusters , clusters ,  cvTermCriteria ( CV_TERMCRIT_ITER  +  CV_TERMCRIT_EPS ,50, 1.0), 1, 0, 0, centers );

        //  无需排序直接输出时

        if  ( sortFlag  == 0)

       {

               int k  = 0;

               int val  = 0;

               float step  = 255 / (( float ) nClusters  - 1);

               CvScalar s ;

               for ( int   i  = 0;  i  <  pImg -> width ;  i ++)

              {

                      for ( int   j  = 0; j  <  pImg -> height ;  j ++)

                     {

                             val  = ( int ) clusters -> data . i [ k ++];

                             s . val [0] = 255-  val  *  step ; // 这个是将不同类别取不同的像素值，

                             cvSet2D ( pResult , j ,  i ,  s );   // 将每个像素点赋值

                     }

              }

               return true ;

       }    
}

void ImageSegmentByKMeans2(cv::Mat& src,cv::Mat& dst,int nClusters, int sortFlag ){
	IplImage srcI = src;

	/*IplImage *hsv_img = cvCreateImage(cvGetSize(&src), 8 , 3);  
	IplImage *h_img = cvCreateImage(cvGetSize(&src), 8, 1);  
	IplImage *s_img = cvCreateImage(cvGetSize(&src), 8, 1);  
	IplImage *v_img = cvCreateImage(cvGetSize(&src), 8, 1);  */

	//IplImage* src = cvLoadImage(name,1);
	IplImage* dstI = cvCreateImage(cvGetSize(&srcI), 8, 1);
	
	if(src.channels() == 3)
		ColorImageSegmentByKMeans2(&srcI,dstI,nClusters,sortFlag);
	else
		GrayImageSegmentByKMeans2(&srcI,dstI,nClusters,sortFlag);

	cv::Mat mtx(dstI);
	mtx.copyTo(dst);
	//imshow("d",dst);waitKey(0);
	cvReleaseImage(&dstI);

	return;
}

void maskProcess(Mat& src,Mat& mask){

	if(mask.channels() != 1)
		cvtColor(mask,mask,CV_BGR2GRAY);

	for(int i=0;i<src.rows;i++)
		for(int j=0;j<src.cols;j++){
			if(mask.at<uchar>(i,j) != 0)
				src.at<Vec3b>(i,j) = Vec3b(0,0,0);
		}
}

void ImageSefmentByMeanshift(cv::Mat& src,cv::Mat& dst){
	//#ifndef SPATIAL_RAD
	//	#define SPATIAL_RAD 10
	//#endif
	//#ifndef COLOR_RAD
	//	#define COLOR_RAD 10
	//#endif
	//#ifndef MAX_PRY_LEVEL
	//	#define MAX_PRY_LEVEL 3
	//#endif
	int spatialRad = 10;
	int colorRad = 10;
	int maxPryLevel = 1;

    //调用meanshift图像金字塔进行分割
	pyrMeanShiftFiltering(src,dst,spatialRad,colorRad,maxPryLevel);
	
	RNG rng=theRNG();
    Mat mask(dst.rows+2,dst.cols+2,CV_8UC1,Scalar::all(0));
    for(int i=0;i<dst.rows;i++)    //opencv图像等矩阵也是基于0索引的
        for(int j=0;j<dst.cols;j++)
            if(mask.at<uchar>(i+1,j+1)==0)
            {
                Scalar newcolor(rng(256),rng(256),rng(256));
                floodFill(dst,mask,Point(j,i),newcolor,0,Scalar::all(1),Scalar::all(1));
        //        floodFill(dst,mask,Point(i,j),newcolor,0,colorDiff,colorDiff);
            }

	imshow("src",src);
	imshow("dst",dst);waitKey(0);

	/*createTrackbar("spatialRad","dst",&spatialRad,80,meanshift_seg);
    createTrackbar("colorRad","dst",&colorRad,60,meanshift_seg);
    createTrackbar("maxPryLevel","dst",&maxPryLevel,5,meanshift_seg);

	imshow("src",src_S);
    imshow("dst",dst_S);
	waitKey();*/
	
}

void putTextOnImg(Mat& src, string text){
	//create a new image
	if(src.channels() == 1)
		cvtColor(src,src,CV_GRAY2BGR);
	Mat show = Mat::zeros(src.rows+20,src.cols,CV_8UC3);
	for(int i=20;i<show.rows;i++)
		for(int j=0;j<show.cols;j++)
			show.at<Vec3b>(i,j) = src.at<Vec3b>(i-20,j);
	putText(show, text, cvPoint(10,10),FONT_HERSHEY_SIMPLEX,0.4,Scalar(255,255,255));
	src = show;
}

double variance(vector<double>& data){
	double sum = 0;
	double avg,v=0;
	for(int i=0;i<data.size();i++)
		sum+=data[i];
	avg=sum/data.size();

	for(int i=0;i<data.size();i++)
		v+=(data[i]-avg)*(data[i]-avg);
	v/=data.size();
	return v;

}

double EulerDistance(vector<double>& v1, vector<double>& v2, vector<double>& weight=vector<double>()){
	//weight.size() == 0
	if(v1.size() == v2.size()){
		if(weight.size() == 0){
			weight.resize(v1.size());
			for(int i=0;i<weight.size();i++)
				weight[i]=1;
		}
		else
			normalize(weight,weight,1.0,2.0,NORM_MINMAX);

		double square = 0;
		for(int i=0;i<v1.size();i++){
			square += weight[i]*(v1[i]-v2[i])*(v1[i]-v2[i]);
		}
		return sqrt(square);
	}
	else{
		std::cout<<"EulerDistance(): input array size unequal error"<<endl;
		return 0;
	}
	
}