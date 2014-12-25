#include "../stdafx2.h"
#include "CmComand.h"


void CmComand::RunProgram(const string &fileName, const string &parameters, bool waiteTillFlished)
{
	string runExeFile = fileName;
#ifdef _DEBUG
	runExeFile.insert(0, "..\\Debug\\");
#else
	runExeFile.insert(0, "..\\Release\\");
#endif // _DEBUG
	if (!CmFile::FileExist(_S(runExeFile)))
		runExeFile = fileName;

	SHELLEXECUTEINFOA  ShExecInfo  =  {0};  
	ShExecInfo.cbSize  =  sizeof(SHELLEXECUTEINFO);  
	ShExecInfo.fMask  =  SEE_MASK_NOCLOSEPROCESS;  
	ShExecInfo.hwnd  =  NULL;  
	ShExecInfo.lpVerb  =  NULL;  
	ShExecInfo.lpFile  =  _S(runExeFile);
	ShExecInfo.lpParameters  =  _S(parameters);         
	ShExecInfo.lpDirectory  =  NULL;  
	ShExecInfo.nShow  =  SW_SHOW;  
	ShExecInfo.hInstApp  =  NULL;              
	ShellExecuteExA(&ShExecInfo);  

	CmLog::LogLine("Run: %s %s\n", ShExecInfo.lpFile, ShExecInfo.lpParameters);
	
	if (waiteTillFlished)
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

void CmComand::Demo()
{
	printf("A simple demo: show path environment\n");
	RunProgram("cmd", "/k path");
}

void CmComand::SegOmpThrdNum(double ratio /* = 0.8 */)
{
	int thrNum = omp_get_max_threads();
	int usedNum = cvRound(thrNum * ratio);
	usedNum = max(usedNum, 1);
	CmLog::LogLine("Number of CPU cores used is %d/%d\n", usedNum, thrNum);
	omp_set_num_threads(usedNum);
}
