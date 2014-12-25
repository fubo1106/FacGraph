#include "Basic_File.h"


Basic_File::Basic_File(void)
{
}


Basic_File::~Basic_File(void)
{
}


/*----------------------------
 * 功能 : 将 cv::Mat 数据写入到 .txt 文件
 *----------------------------
 * 函数 : WriteData
 * 访问 : public 
 * 返回 : -1：打开文件失败；0：写入数据成功；1：矩阵为空
 *
 * 参数 : fileName	[in]	文件名
 * 参数 : matData	[in]	矩阵数据
 */
int Basic_File::WriteData(string fileName, cv::Mat& matData)
{
	int retVal = 0;

	// 检查矩阵是否为空
	if (matData.empty())
	{
		cout << "矩阵为空" << endl; 
		retVal = 1;
		return (retVal);
	}

	// 打开文件
	ofstream outFile(fileName.c_str(), ios_base::out);	//按新建或覆盖方式写入
	if (!outFile.is_open())
	{
		cout << "打开文件失败" << endl; 
		retVal = -1;
		return (retVal);
	}

	// 写入数据
	for (int r = 0; r < matData.rows; r++)
	{
		for (int c = 0; c < matData.cols; c++)
		{
			int data = matData.at<uchar>(r,c);	//读取数据，at<type> - type 是矩阵元素的具体数据格式
			outFile << data << "\t" ;	//每列数据用 tab 隔开
		}
		outFile << endl;	//换行
	}

	return (retVal);
}

/*----------------------------
 * 功能 : 从 .txt 文件中读入数据，保存到 cv::Mat 矩阵
 *		- 默认按 float 格式读入数据，
 *		- 如果没有指定矩阵的行、列和通道数，则输出的矩阵是单通道、N 行 1 列的
 *----------------------------
 * 函数 : LoadData
 * 访问 : public 
 * 返回 : -1：打开文件失败；0：按设定的矩阵参数读取数据成功；1：按默认的矩阵参数读取数据
 *
 * 参数 : fileName	[in]	文件名
 * 参数 : matData	[out]	矩阵数据
 * 参数 : matRows	[in]	矩阵行数，默认为 0
 * 参数 : matCols	[in]	矩阵列数，默认为 0
 * 参数 : matChns	[in]	矩阵通道数，默认为 0
 */
int Basic_File::LoadData(string fileName, cv::Mat& matData, int matRows, int matCols)
{
	int retVal = 0;

	// 打开文件
	ifstream inFile(fileName.c_str(), ios_base::in);
	if(!inFile.is_open())
	{
		cout << "读取文件失败" << endl;
		retVal = -1;
		return (retVal);
	}
	//char c;
	matData = Mat::zeros(matRows,matCols,CV_32S);

 	while(!inFile.eof()){
		for(int i=0;i<matRows;i++)
			for(int j=0;j<matCols;j++){
				inFile>>matData.at<int>(i,j);
			}
	}
	inFile.close();
	
	return (1);
}

