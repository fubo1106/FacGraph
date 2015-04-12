#pragma once
#include "gco-v3.0\GCoptimization.h"
#include <string>
#include "type.h"

class GcoUtil
{
public:
	GcoUtil(int w, int h);
	~GcoUtil();

	bool read_labels(std::string filename);
	bool read_label_weight(std::string filename);
	bool read_label_center(std::string filename);
	bool read_float_color(std::string filename);
	bool read_globalPb(std::string filename);

	void initial_data(std::string label_file, std::string weight_file, std::string gpb_file, std::string colorf_file, std::string label_center_file, int sa);
	bool build_grid_graph();
	bool build_general_graph();
	void set_neighbors(); //only for general graph

	void solve_grid_graph();
	void solve_general_graph();
	void save_result(std::string filename);
	// regular grid graph
	GCoptimizationGridGraph *gc;
	GCoptimizationGeneralGraph * ggc; //general graph cut
	// general graph

	// data
	int width, height, num_pixels, num_labels;
	int symm_axis; // axis for symmetry
	int num_features; // num of features used for clustring
	MatrixXr globalPb, label_weight, color_mat, label_center;
	MatrixXi init_labels, result_labels;
	int *smooth, *vCosts, *hCosts;//modified double-->>int
	std::string label_file_name;
	bool bGrid;


};

