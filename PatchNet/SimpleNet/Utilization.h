#include "stdafx2.h"
extern "C" {

//#include "kmeans.h"
}
//#include "patch.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "Basic/CmSetting.h"
//#include "image_kit.h"
#include "graphBuilder.h"
#include "LocalNet.h"
#include "SingleNet.h"
#include "Summrarizer.h"
#include "NetExplorer.h"
#include "union_find_set.h"
#include <fstream>

namespace utilization{
void runOneImage(string& sectionS, string& outdirS, string& sourcePath);
void runAfolder(int argc, char* argv[]);
void runAfolder(string SrcDir,string Outdir);
void runFolder(string regionDir,string srcDir,string regionOutDir,string srcOutDir);

double dis(IN vector<float>& a, IN vector<float> &b);
}
