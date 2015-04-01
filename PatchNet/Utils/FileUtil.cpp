#include "FileUtil.h"
#include <iostream>
using namespace std;

const vector<string> FileUtil::
readLines(const std::string& filename)
{
	vector<string> lines;
	ifstream fin(filename.c_str());
	if(!fin) return lines;
	char buffer[1024];
	while(fin.getline(buffer,1023)){
		if(buffer[0] == '#') continue;
		lines.push_back(buffer);
	}
	fin.close();
	return lines;

}

void FileUtil::
loadAttribute(const std::string& filename,const std::string attributeName, std::vector<double>& weights)
{
	ifstream fin(filename.c_str());
	if(!fin) return;
	return;
}