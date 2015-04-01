#pragma once
#include<vector>
#include<fstream>

namespace FileUtil
{
	const std::vector<std::string> readLines(const std::string& filename);
	void loadAttribute(const std::string& filename,const std::string attributeName, std::vector<double>& weights);
}