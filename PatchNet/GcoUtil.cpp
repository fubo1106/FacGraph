#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "GcoUtil.h"

GcoUtil::GcoUtil(int w, int h)
{
	width = w;
	height = h;
	num_pixels = width*height;
	globalPb = MatrixXd::Zero(h, w);
	init_labels = MatrixXi::Constant(h, w, -1); //start from 0 to num_labels-1
	result_labels = MatrixXi::Constant(h, w, -1);
	color_mat = MatrixXd::Zero(h*w, 3);
	vCosts = new int[width*height]; //modified double to int
	hCosts = new int[width*height];
	//so far, only RGB
	num_features = 3;
	/*
	cv::Vec3f color;
	for(int i=0; i<h; ++i)
	{
	for(int j=0; j<w; ++j)
	{
	color = image.at<cv::Vec3b>(i, j);
	color_mat.row(i*w + j) << color.val[0], color.val[1], color.val[2];
	}
	}
	color_mat = color_mat / 255.0;
	*/
}

GcoUtil::~GcoUtil()
{
	if (bGrid)
	{
		delete gc;
		delete[]smooth;
	}
	else
	{
		delete ggc;
	}
	delete[]vCosts;
	delete[]hCosts;
}

bool GcoUtil::read_labels(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	if (inFile.is_open())
	{
		for (int i = 0; i<height; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for (int j = 0; j<width; ++j)
			{
				iss >> init_labels(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr << "open label file error\n";
	return false;
}

///////////////////////////////////////////
//// label_weight from cluster centers
bool GcoUtil::read_label_weight(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	std::vector<Real> first_line;
	if (inFile.is_open())
	{
		// get the number of clusters
		std::getline(inFile, line);
		std::istringstream iss(line);
		Real temp;
		while (iss >> temp)
		{
			first_line.push_back(temp);
		}
		label_weight = MatrixXr::Zero(num_pixels, first_line.size());
		////////////////////////////
		///fill the first row
		for (int i = 0; i<first_line.size(); ++i)
		{
			label_weight(0, i) = first_line.at(i);
		}
		double test_empty;
		///////////////////////////
		//fill the rest data
		for (int i = 1; i<num_pixels; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss2(line);
			for (int j = 0; j<first_line.size(); ++j)
			{
				iss2 >> label_weight(i, j);
			}
			if (iss2 >> test_empty)
			{
				std::cerr << "extra label weight data error\n";
				exit(-1);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr << "open label weight file error\n";
	return false;
}

/*
* ===  FUNCTION  ======================================================================
*         Name:  read_float_color
*  Description:  store color image as float
* =====================================================================================
*/
bool GcoUtil::read_float_color(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	if (inFile.is_open())
	{
		for (int i = 0; i<num_pixels; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for (int j = 0; j<num_features; ++j)
			{
				iss >> color_mat(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr << "open float color file error\n";
	return false;
}		/* -----  end of function read_float_color  ----- */

/*
* ===  FUNCTION  ======================================================================
*         Name:  read_label_center
*  Description:  read cluster centers
* =====================================================================================
*/
bool GcoUtil::read_label_center(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	if (inFile.is_open())
	{
		for (int i = 0; i<num_labels; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for (int j = 0; j<num_features; ++j)
			{
				iss >> label_center(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr << "open label center file error\n";
	return false;
}		/* -----  end of function read_label_center  ----- */

/*
* ===  FUNCTION  ======================================================================
*         Name:  read_globalPb
*  Description:  the smooth term
* =====================================================================================
*/
bool GcoUtil::read_globalPb(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	double threshold = 0.0;
	if (inFile.is_open())
	{
		for (int i = 0; i<height; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for (int j = 0; j<width; ++j)
			{
				iss >> globalPb(i, j);
				globalPb(i, j) > threshold ? globalPb(i, j) : 0.0;
			}
		}
		inFile.close();
		return true;
	}
	std::cerr << "open globalPb file error\n";
	return false;
}

/*
* ===  FUNCTION  ======================================================================
*         Name:  initial_data
*  Description:  prepared data needed
* =====================================================================================
*/
void GcoUtil::initial_data(std::string label_file, std::string weight_file, std::string gpb_file, std::string colorf_file, std::string label_center_file, int sa)
{
	if (!read_labels(label_file))
	{
		std::cerr << "read label file " << label_file << " failure\n";
		exit(-1);
	}
	if (!read_label_weight(weight_file))
	{
		std::cerr << "read label weight file " << weight_file << " failure\n";
		exit(-1);
	}
	if (!read_globalPb(gpb_file))
	{
		std::cerr << "read globalPb file " << gpb_file << " failure\n";
		exit(-1);
	}
	if (!read_float_color(colorf_file))
	{
		std::cerr << "read float color file error\n";
		exit(-1);
	}

	num_labels = init_labels.maxCoeff() + 1;
	label_center = MatrixXr::Zero(num_labels, num_features);
	if (!read_label_center(label_center_file))
	{
		std::cerr << "read label centers file " << label_center_file << " failure\n";
		exit(-1);
	}

	label_file_name = label_file;
	symm_axis = sa;
}		/* -----  end of function initial_data  ----- */

/*
* ===  FUNCTION  ======================================================================
*         Name:  build_grid_graph
*  Description:  build grid graph for image to solve
* =====================================================================================
*/
bool GcoUtil::build_grid_graph()
{
	bGrid = true;
	smooth = new int[num_labels * num_labels]; //modified double to int
	try{
		gc = new GCoptimizationGridGraph(width, height, num_labels);
		////////////////////////////////////////////
		// fill data term
		for (int i = 0; i<height; ++i)
			for (int j = 0; j<width; ++j)
			{
			for (int l = 0; l<num_labels; ++l)
			{
				gc->setDataCost(width*i + j, l, label_weight(width*i + j, l));
			}
			}

		//////////////////////////////////////////
		// file smooth term 
		double color_sigma2 = 0.01;
		double edge_sigma2 = 0.01; //squared sigma 
		double label_sigma2 = 4.0;
		double alpha = 0.9;
		//label cost
		for (int i = 0; i<num_labels; ++i)
			for (int j = 0; j<num_labels; ++j)
			{
			if (i == j)
			{
				smooth[i*num_labels + j] = 0.0;
			}
			else
			{
				//smooth[i*num_labels+j] = 10.0 * ( label_center.row(i) - label_center.row(j) ).norm();
				smooth[i*num_labels + j] = std::exp(0.5 * (label_center.row(i) - label_center.row(j)).squaredNorm() / label_sigma2);
			}
			}

		//use cluster center as label weight
		Vector3d diff;
		//cost according to spatial relation
		for (int i = 0; i<height; ++i)
			for (int j = 0; j<width; ++j)
			{
			if (i < height - 1)
			{
				diff = color_mat.row(i*width + j) - color_mat.row((i + 1)*width + j);
				vCosts[i*width + j] = alpha * std::exp(-0.5*std::max(globalPb(i, j), globalPb(i + 1, j)) / edge_sigma2) + (1 - alpha) * std::exp(-0.5*diff.squaredNorm() / color_sigma2);
				//vCosts[i*width+j] = std::exp(-1*std::max(globalPb(i, j), globalPb(i+1, j)) / sigma2 );
				vCosts[i*width + j] *= 5.0;
			}
			else
			{
				vCosts[i*width + j] = 0.0;
			}
			if (j < width - 1)
			{
				diff = color_mat.row(i*width + j) - color_mat.row(i*width + (j + 1));
				hCosts[i*width + j] = alpha * std::exp(-0.5 *std::max(globalPb(i, j), globalPb(i, j + 1)) / edge_sigma2) + (1 - alpha) * std::exp(-0.5 * diff.squaredNorm() / color_sigma2);
				//hCosts[i*width+j] = std::exp(-1*std::max(globalPb(i, j), globalPb(i, j+1)) / sigma2 );
				hCosts[i*width + j] *= 5.0;
			}
			else
			{
				hCosts[i*width + j] = 0.0;
			}
			}

		gc->setSmoothCostVH(smooth, vCosts, hCosts);

		//////////////////// checking /////////////////////////////
		std::string base_name = label_file_name.substr(12, label_file_name.length() - 17);
		std::string path = "data/cost/";
		std::string vcost_name = path + base_name + "vcost";
		std::string hcost_name = path + base_name + "hcost";
		std::string weight_name = path + base_name + "wei";
		std::ofstream outVcost(vcost_name.c_str());
		std::ofstream outHcost(hcost_name.c_str());
		std::ofstream outWeight(weight_name.c_str());
		for (int i = 0; i<height; ++i)
		{
			for (int j = 0; j<width; ++j)
			{
				outVcost << vCosts[i*width + j] << " ";
				outHcost << hCosts[i*width + j] << " ";
			}
			outVcost << "\n";
			outHcost << "\n";
		}
		for (int i = 0; i<num_pixels; ++i)
		{
			for (int j = 0; j<num_labels; ++j)
			{
				outWeight << label_weight(i, j) << " ";
			}
			outWeight << "\n";
		}
		outVcost.close();
		outHcost.close();
		outWeight.close();
		/*
		*/
		/////////////////////////////////////////////////////////////
	}
	catch (GCException e)
	{
		e.Report();
	}

	return true;
}

/*
* ===  FUNCTION  ======================================================================
*         Name:  solve
*  Description:  solve graph problem max flow/min cut
* =====================================================================================
*/
void GcoUtil::solve_grid_graph()
{
	try{
		std::cout << "Before Optimization energy is " << gc->compute_energy() << std::endl;
		//	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;

		gc->expansion(10);
		//gc->swap(10);

		std::cout << "After Optimization energy is " << gc->compute_energy() << std::endl;
		//	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;
		std::cout << std::endl;
		for (int i = 0; i<height; ++i)
			for (int j = 0; j<width; ++j)
			{
			result_labels(i, j) = gc->whatLabel(width*i + j);
			}
	}
	catch (GCException e)
	{
		e.Report();
	}
}		/* -----  end of function solve  ----- */


/*
* ===  FUNCTION  ======================================================================
*         Name:  save_result
*  Description: save the final labels
* =====================================================================================
*/
void GcoUtil::save_result(std::string filename)
{
	std::ofstream outFile(filename.c_str());
	if (outFile.is_open())
	{
		outFile << result_labels;
	}
	else
	{
		std::cerr << "open file for writting result error\n";
		exit(-1);
	}
	outFile.close();
}		/* -----  end of function save_result  ----- */


/*
* ===  FUNCTION  ======================================================================
*         Name:  build_general_graph
*  Description:
* =====================================================================================
*/
bool GcoUtil::build_general_graph()
{
	bGrid = false;
	ggc = new GCoptimizationGeneralGraph(num_pixels, num_labels);
	set_neighbors();
	////////////////////////////////////////////
	// fill data term
	for (int i = 0; i<height; ++i)
		for (int j = 0; j<width; ++j)
		{
		for (int l = 0; l<num_labels; ++l)
		{
			ggc->setDataCost(width*i + j, l, label_weight(width*i + j, l));
		}
		}

	///////////////////////////////////////////
	//fill smooth term (only label cost)
	//spatial varying cost already set in set_neighbors
	double temp;
	double label_sigma2 = 4.0;
	for (int i = 0; i<num_labels; ++i)
		for (int j = 0; j<num_labels; ++j)
		{
		if (i == j)
		{
			temp = 0.0;
		}
		else
		{
			temp = std::exp(0.5*(label_center.row(i) - label_center.row(j)).squaredNorm() / label_sigma2);
		}
		ggc->setSmoothCost(i, j, temp);
		}

	///////////////////////  checking  /////////////////////////
	std::string base_name = label_file_name.substr(12, label_file_name.length() - 17);
	std::string path = "data/cost/";
	std::string vcost_name = path + base_name + "vcost";
	std::string hcost_name = path + base_name + "hcost";
	std::string weight_name = path + base_name + "wei";
	std::ofstream outVcost(vcost_name.c_str());
	std::ofstream outHcost(hcost_name.c_str());
	std::ofstream outWeight(weight_name.c_str());
	for (int i = 0; i<height; ++i)
	{
		for (int j = 0; j<width; ++j)
		{
			outVcost << vCosts[i*width + j] << " ";
			outHcost << hCosts[i*width + j] << " ";
		}
		outVcost << "\n";
		outHcost << "\n";
	}
	for (int i = 0; i<num_pixels; ++i)
	{
		for (int j = 0; j<num_labels; ++j)
		{
			outWeight << label_weight(i, j) << " ";
		}
		outWeight << "\n";
	}
	outVcost.close();
	outHcost.close();
	outWeight.close();
	/*
	*/
	/////////////////////////////////////////////////////////////

	return true;
}		/* -----  end of function build_general_graph  ----- */


/*
* ===  FUNCTION  ======================================================================
*         Name:  set_neighbors
*  Description:
* =====================================================================================
*/
void GcoUtil::set_neighbors()
{
	Vector3d diff;
	double data_weight;
	double data_scale = 1.0;
	double color_sigma2 = 0.01;
	double edge_sigma2 = 0.01;
	double alpha = 0.9;
	// image pixel neighbors and its weight
	// essentially smooth term
	for (int i = 0; i<height; ++i)
		for (int j = 0; j<width; ++j)
		{
		//for vertical direction
		if (i < height - 1)
		{
			diff = color_mat.row(i*width + j) - color_mat.row((i + 1)*width + j);
			data_weight = alpha * std::exp(-0.5*std::max(globalPb(i, j), globalPb(i + 1, j)) / edge_sigma2) + (1 - alpha) * std::exp(-0.5*diff.squaredNorm() / color_sigma2);
			data_weight *= data_scale;
			ggc->setNeighbors(i*width + j, (i + 1)*width + j, data_weight);
			vCosts[i*width + j] = data_weight;
		}
		else
		{
			vCosts[i*width + j] = 0.0;
		}

		//for horizontal direction
		if (j < width - 1)
		{
			diff = color_mat.row(i*width + j) - color_mat.row(i*width + (j + 1));
			data_weight = alpha * std::exp(-0.5*std::max(globalPb(i, j), globalPb(i, j + 1)) / edge_sigma2) + (1 - alpha) * std::exp(-0.5*diff.squaredNorm() / color_sigma2);
			data_weight *= data_scale;
			ggc->setNeighbors(i*width + j, i*width + (j + 1), data_weight);
			hCosts[i*width + j] = data_weight;
		}
		else
		{
			hCosts[i*width + j] = 0.0;
		}
		}

	// only vertical symmetric
	// symmetric neighbors
	//	int symm_axis = 80; //vertical axis for symmetric
	int counter, index;
	double scale = 0.05;
	for (int i = 0; i<height; ++i)
		for (int j = 0; j<symm_axis; ++j)
		{
		counter = 0;
		for (int k = 0; k<num_features; ++k)
		{
			//0,0,0 is background
			if (color_mat(i*width + j, k) > 1.0e-3)
				counter++;
		}
		if (counter == 3) // not background
		{
			index = 2 * symm_axis - j;
			if (index >= width)
			{
				std::cerr << "out of image range\n";
				continue;
			}
			//center corresponding
			//        |   ___________
			//        |  | 1 | 2 | 1 |                  
			//  p     |  | 2 | 4 | 2 |
			//        |  | 1 | 2 | 1 |
			//        |  -------------
			ggc->setNeighbors(i*width + j, i*width + index, 4.0*scale);
			//right, left, top, buttom
			ggc->setNeighbors(i*width + j, i*width + (index + 1), 2.0*scale);
			//Caution on LEFT side!
			if (index - 1 > symm_axis)
			{
				ggc->setNeighbors(i*width + j, i*width + (index - 1), 2.0*scale);
			}
			ggc->setNeighbors(i*width + j, (i - 1)*width + index, 2.0*scale);
			ggc->setNeighbors(i*width + j, (i + 1)*width + index, 2.0*scale);

			//top(right,left), buttom(right, left)
			ggc->setNeighbors(i*width + j, (i - 1)*width + (index + 1), 1.0*scale);
			ggc->setNeighbors(i*width + j, (i - 1)*width + (index - 1), 1.0*scale);
			ggc->setNeighbors(i*width + j, (i + 1)*width + (index + 1), 1.0*scale);
			ggc->setNeighbors(i*width + j, (i + 1)*width + (index - 1), 1.0*scale);
		}

		}

}		/* -----  end of function set_neighbors  ----- */


/*
* ===  FUNCTION  ======================================================================
*         Name:  solve_general_graph
*  Description:
* =====================================================================================
*/
void GcoUtil::solve_general_graph()
{
	try{
		std::cout << "Before Optimization energy is " << ggc->compute_energy() << std::endl;
		//ggc->expansion(10);
		ggc->expansion(10);
		std::cout << "After Optimization energy is " << ggc->compute_energy() << std::endl << std::endl;
		for (int i = 0; i<height; ++i)
			for (int j = 0; j<width; ++j)
			{
			result_labels(i, j) = ggc->whatLabel(width*i + j);
			}
	}
	catch (GCException e)
	{
		e.Report();
	}
}		/* -----  end of function solve_general_graph  ----- */

