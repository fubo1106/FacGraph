#include "DP.h"


DP::DP(void)
{
}


DP::~DP(void)
{
	_path.clear();
}

DP::DP(double** distance, int rows,int cols){
	_rows = rows;
	_cols = cols;
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
			_matrix[i][j] = distance[i][j];
}

void DP::initilize(){
	_cols =0;//number of column or row
	_rows = 0;
	_path.clear();
}

void DP::printMatrix(int rows,int cols){
	for(int i=0;i<rows;i++){
		for(int j=0;j<cols;j++)
			cout<<_matrix_min[i][j]<<" ";
			cout<<endl;
	}
	cout<<endl;		
}

double DP::MinMatrix(int i,int j)
{
    if(i == -1 && j == -1) return 0;
	if(i == -1 && j != -1) return INF_EDGE;
	if(i != -1 && j == -1) return INF_EDGE;

    if(_matrix_min[i][j] == -1)
    {
        //int min_matrix1=MinMatrix((i-1+_rows)%_rows,j+1);
        double min_matrix2=MinMatrix(i,j-1);//往右走
        double min_matrix3=MinMatrix(i-1,j-1);//右下

		//int min_matrix4=MinMatrix(i+1,(j-1+_columns)%_columns);
        double min_matrix5=MinMatrix(i-1,j);//往下走
      //  int min_matrix6=MinMatrix(i+1,(j+1+_columns)%_columns);//右下  等价min_matrix3

        //row
		double temp_min = min_matrix2;
       /* _matrix_row[i][j] = (i+_rows)%_rows;
        _matrix_min[i][j]=min_matrix2+_matrix[i][j];*/

		if(min_matrix3 < temp_min){
			if(min_matrix5<min_matrix3){//下
				temp_min = min_matrix5;
				_matrix_row[i][j] = (i-1+_rows)%_rows;
				_matrix_col[i][j] = (j+_cols)%_cols;
				_matrix_min[i][j] = min_matrix5+_matrix[i][j];
			}
			else{//右下
				temp_min = min_matrix3;
				_matrix_row[i][j] = (i-1+_rows)%_rows;
				_matrix_col[i][j] = (j-1+_cols)%_cols;
				_matrix_min[i][j] = min_matrix3+_matrix[i][j];
			}
		}
		else{
			if(min_matrix5<temp_min){//下
				temp_min = min_matrix5;
				_matrix_row[i][j] = (i-1+_rows)%_rows;
				_matrix_col[i][j] = (j+_cols)%_cols;
				_matrix_min[i][j] = min_matrix5+_matrix[i][j];
			}
			else{//右
				temp_min = min_matrix2;
				_matrix_row[i][j] = (i+_rows)%_rows;
				_matrix_col[i][j] = (j-1+_cols)%_cols;
				_matrix_min[i][j] = min_matrix2+_matrix[i][j];
			}
		}


		////col
		//int temp_min_col = min_matrix5;
  //      _matrix_col[i][j] = (j-1+_columns)%_columns;
  //      _matrix_min[i][j]=min_matrix5+_matrix[i][j];

		////row
  //      //if(min_matrix2 < temp_min)//右
  //      //{
  //      //    temp_min = min_matrix2;
  //      //    _matrix_row[i][j] = (i+_rows)%_rows;
  //      //    _matrix_min[i][j]=min_matrix2+_matrix[i][j];
  //      //}

  //      if(min_matrix3 < temp_min)//右
  //      {
  //          temp_min = min_matrix3;
  //          _matrix_row[i][j] = (i+1+_rows)%_rows;
  //          _matrix_min[i][j]=min_matrix3+_matrix[i][j];
  //      }

		////col
		////if(min_matrix5 < temp_min_col)//下
  ////      {
  ////          temp_min_col = min_matrix5;
  ////          _matrix_col[i][j] = (j+_columns)%_columns;
  ////          _matrix_min[i][j]=min_matrix5+_matrix[i][j];
  ////      }

  //      if(min_matrix6 < temp_min_col)//下
  //      {
  //          temp_min_col = min_matrix6;
  //          _matrix_col[i][j] = (j+1+_columns)%_columns;
  //          _matrix_min[i][j]=min_matrix6+_matrix[i][j];
  //      }

    }//end if(_matrix_min[i][j] == -1)
    return _matrix_min[i][j];
}

double DP::MaxMatrix(int i,int j)
{
    if(j == _cols) return 0;

    if(_matrix_min[i][j] == -1)
    {
        double min_matrix1=MinMatrix((i-1+_rows)%_rows,j+1);
        double min_matrix2=MinMatrix((i+_rows)%_rows,j+1);
        double min_matrix3=MinMatrix((i+1+_rows)%_rows,j+1);

        double temp_min = min_matrix1;
        _matrix_row[i][j] = (i-1+_rows)%_rows;
        _matrix_min[i][j]=min_matrix1+_matrix[i][j];

        if(min_matrix2 > temp_min)
        {
            temp_min = min_matrix2;
            _matrix_row[i][j] = (i+_rows)%_rows;
            _matrix_min[i][j]=min_matrix2+_matrix[i][j];
        }

        if(min_matrix3 > temp_min)
        {
            temp_min = min_matrix3;
            _matrix_row[i][j] = (i+1+_rows)%_rows;
            _matrix_min[i][j]=min_matrix3+_matrix[i][j];
        }
    }
    return _matrix_min[i][j];
}

void DP::performDP(){

	/*fstream fin("matrix.txt");
	fin>>_rows>>_cols;*/
	int i,j;

	//read matrix

	for(i=0;i<_rows;i++){
		for(j=0;j<_cols;j++){
			//fin>>_matrix[i][j];
			_matrix_min[i][j] = -1;
			//cout<<_matrix[i][j]<<" ";
		}
		//cout<<endl;
	}


	double rsum = 0;
	int rentry = -1;
	rsum = MinMatrix(0,_cols-1);
	rentry = 0;
	//printMatrix(_rows,_cols);
	for(i=0;i<_rows;i++){
		double temp = MinMatrix(i,_cols-1);//最右边的出口
		//printMatrix(_rows,_cols);
		if(temp < rsum)
		{
			rsum = temp;
			rentry = i;
		}
	}

	double csum = 0;
	int centry = -1;
	csum = MinMatrix(_rows-1,0);
	centry = 0;
	for(j=0;j<_cols;j++){
		int temp = MinMatrix(_rows-1,j);//最下面的出口
		//printMatrix(_rows,_cols);
		if(temp < csum)
		{
			csum = temp;
			centry = j;
		}
	}

	pair<int,int>p;
	if(csum < rsum){
		_mindistance = csum;
		cout<<"the min of the path is :"<<csum<<endl;
		int old_row = _rows-1;
		int old_col = centry;
		cout<<old_row<<","<<old_col;

		p.first=old_row;p.second=old_col;
		_path.push_back(p);

		for(i=0;i<_rows-1;i++)
		{
			int next_row = _matrix_row[old_row][old_col];
			int next_col = _matrix_col[old_row][old_col];
			old_row = next_row;
			old_col = next_col;
			cout<<"<-"<<next_row<<","<<next_col;
		}
		cout<<endl;
	}
	else{
		_mindistance = rsum;
		cout<<"the min of the path is :"<<rsum<<endl;
		int old_row = rentry;
		int old_col = _cols-1;
		cout<<old_row<<","<<old_col;

		p.first=old_row;p.second=old_col;
		_path.push_back(p);

		for(i=0;i<_cols-1;i++)
		{
			int next_row = _matrix_row[old_row][old_col];
			int next_col = _matrix_col[old_row][old_col];
			old_row = next_row;
			old_col = next_col;
			cout<<"<-"<<next_row<<" "<<next_col;
		}
		cout<<endl;
	}

}