#include "FacBuilder.h"


FacBuilder::FacBuilder(void)
{
}


FacBuilder::~FacBuilder(void)
{
}

FacBuilder::FacBuilder(FacGraph g){
	_facGraph = g;
}

void FacBuilder::buildGraph(Mat& img, Mat& region){
	FacGraph g;

	Mat regionmine = getRegionFromImg(img);//自己的代码wubo


	vector<FacNode> nodes = getNodesFromImg(img,region);
	vector<FacEdge> edges = getEdgesFromNodes(img,nodes);

	_facGraph._nodes = nodes;
	_facGraph._edges = edges;

	//check the nodes pair...
	/*Mat show = Mat::zeros(region.rows,region.cols,CV_8UC3);
	for(int i=0;i<edges.size();i++){
		Mat result = show.clone();
		FacNode* n1 = edges[i].node1;
		FacNode* n2 = edges[i].node2;

		n1->drawNode(*n1,result,Scalar(0,255,0));
		n1->drawNode(*n2,result,Scalar(0,0,255));
		namedWindow("region",0);imshow("region",result);waitKey(0);
	}*/

}

Mat FacBuilder::getRegionFromImg(Mat& img){
	Mat region;
	return region;
}
 
vector<FacNode> FacBuilder::getNodesFromImg(Mat& img, Mat& region){
	vector<FacNode> nodes;

	//get all regions from Mat:region
	int **marked;
	marked = new int*[region.rows];
	for(int i=0;i<region.rows;i++){//initialization
		marked[i] = new int[region.cols];
	}
	for(int i=0;i<region.rows;i++)
		for(int j=0;j<region.cols;j++)
			marked[i][j] = 0;

	//find all nodes from image and region file
	//use bfs search method
	queue<Point> q;
	int label;
	Point seed;
	//vector<Point> contour;
	FacNode node;
	int id=0;
	for(int i=0;i<region.rows;i++)
		for(int j=0;j<region.cols;j++){

			if(!marked[i][j]){
				marked[i][j] = 1;
				q.push(Point(i,j));
				node._ID = id++;
				label = region.at<int>(i,j);
				node._cluster = label;//类别
				node._contour.push_back( Point(i,j) );

				while(!q.empty()){
					seed = q.front();
					q.pop();

					if((seed.y-1)>=0 && !marked[seed.x][seed.y-1])//left
						if(region.at<int>(seed.x,seed.y-1) == label){
							q.push( Point(seed.x,seed.y-1) );
							node._contour.push_back( Point(seed.x,seed.y-1) );
							marked[seed.x][seed.y-1] = 1;
						}
					if((seed.y+1)<region.cols && !marked[seed.x][seed.y+1])//right
						if(region.at<int>(seed.x,seed.y+1) == label){
							q.push( Point(seed.x,seed.y+1) );
							node._contour.push_back( Point(seed.x,seed.y+1) );
							marked[seed.x][seed.y+1] = 1;
						}
					if((seed.x-1)>=0 && !marked[seed.x-1][seed.y])//top
						if(region.at<int>(seed.x-1,seed.y) == label){
							q.push( Point(seed.x-1,seed.y) );
							node._contour.push_back( Point(seed.x-1,seed.y) );
							marked[seed.x-1][seed.y] = 1;
						}
					if((seed.x+1)<region.rows  && !marked[seed.x+1][seed.y])//dowm
						if(region.at<int>(seed.x+1,seed.y) == label){
							q.push( Point(seed.x+1,seed.y) );
							node._contour.push_back( Point(seed.x+1,seed.y) );
							marked[seed.x+1][seed.y] = 1;
						}						
					
				}//end while(!q.empty())

				nodes.push_back(node);
				node._contour.clear();	 
			}
		}
		delete[] marked;	

		//check the nodes...
		/*Mat show = Mat::zeros(region.rows,region.cols,CV_8UC3);
		for(int i=0;i<nodes.size();i++){
			Mat result = show.clone();
			nodes[i].showFacNode(nodes[i],result,Scalar(0,255,0));
			imshow("region",result);waitKey(0);
		}*/

		for(int i=0;i<nodes.size();i++){
			nodes[i].compAttributes(img);
		}
		
	return nodes;
}

vector<FacEdge> FacBuilder::getEdgesFromNodes(Mat& img, vector<FacNode>& nodes){
	vector<FacEdge> edges;
	//vector<pair<FacNode,FacNode>> pairs;
	
	FacEdge edge;
	for(int i=0;i<nodes.size()-1;i++)
		for(int j=i+1;j<nodes.size();j++){
			if(nodes[i].isAdjacentWith(nodes[j])){
				edge._node1 = nodes[i];
				edge._node2 = nodes[j];
				edge._rel_direction = nodes[i]._position - nodes[j]._position;
				edges.push_back(edge);
			}
		}
	return edges;
	
}

void FacBuilder::buildSubGraph(FacNode& node, FacGraph& facGraph){

	_subGraph._centerNode = node;
	FacEdge edge;
	edge._node1 = node;
	for(int i=0;i<facGraph._edges.size();i++){
		if(facGraph._edges[i]._node1._ID == node._ID){
			edge._node2 = facGraph._edges[i]._node2;
			_subGraph._edges.push_back(edge);
		}
		else if(facGraph._edges[i]._node2._ID == node._ID){
			edge._node2 = facGraph._edges[i]._node1;	
			_subGraph._edges.push_back(edge);
		}
		
	}
}