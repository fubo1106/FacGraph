#include "Basic_File.h"


Basic_File::Basic_File(void)
{
}


Basic_File::~Basic_File(void)
{
}


/*----------------------------
 * ���� : �� cv::Mat ����д�뵽 .txt �ļ�
 *----------------------------
 * ���� : WriteData
 * ���� : public 
 * ���� : -1�����ļ�ʧ�ܣ�0��д�����ݳɹ���1������Ϊ��
 *
 * ���� : fileName	[in]	�ļ���
 * ���� : matData	[in]	��������
 */
int Basic_File::WriteData(string fileName, cv::Mat& matData)
{
	int retVal = 0;

	// �������Ƿ�Ϊ��
	if (matData.empty())
	{
		cout << "����Ϊ��" << endl; 
		retVal = 1;
		return (retVal);
	}

	// ���ļ�
	ofstream outFile(fileName.c_str(), ios_base::out);	//���½��򸲸Ƿ�ʽд��
	if (!outFile.is_open())
	{
		cout << "���ļ�ʧ��" << endl; 
		retVal = -1;
		return (retVal);
	}

	// д������
	for (int r = 0; r < matData.rows; r++)
	{
		for (int c = 0; c < matData.cols; c++)
		{
			int data = matData.at<uchar>(r,c);	//��ȡ���ݣ�at<type> - type �Ǿ���Ԫ�صľ������ݸ�ʽ
			outFile << data << "\t" ;	//ÿ�������� tab ����
		}
		outFile << endl;	//����
	}

	return (retVal);
}

/*----------------------------
 * ���� : �� .txt �ļ��ж������ݣ����浽 cv::Mat ����
 *		- Ĭ�ϰ� float ��ʽ�������ݣ�
 *		- ���û��ָ��������С��к�ͨ������������ľ����ǵ�ͨ����N �� 1 �е�
 *----------------------------
 * ���� : LoadData
 * ���� : public 
 * ���� : -1�����ļ�ʧ�ܣ�0�����趨�ľ��������ȡ���ݳɹ���1����Ĭ�ϵľ��������ȡ����
 *
 * ���� : fileName	[in]	�ļ���
 * ���� : matData	[out]	��������
 * ���� : matRows	[in]	����������Ĭ��Ϊ 0
 * ���� : matCols	[in]	����������Ĭ��Ϊ 0
 * ���� : matChns	[in]	����ͨ������Ĭ��Ϊ 0
 */
int Basic_File::LoadData(string fileName, cv::Mat& matData, int matRows, int matCols)
{
	int retVal = 0;

	// ���ļ�
	ifstream inFile(fileName.c_str(), ios_base::in);
	if(!inFile.is_open())
	{
		cout << "��ȡ�ļ�ʧ��" << endl;
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

