#include "../stdafx2.h"

#include <shlobj.h>
#include <Commdlg.h>


BOOL CmFile::MkDir(const string&  _path)
{
	if(_path.size() == 0)
	{
		CmLog::LogWarning("Creating empty path\n");
		return FALSE;
	}

	const char* path = _path.c_str();
	static char buffer[1024];
	strcpy(buffer, path);
	for (int i = 0; buffer[i] != 0; i ++)
	{
		if (buffer[i] == '\\')
		{
			buffer[i] = '\0';
			CreateDirectoryA(buffer, 0);
			buffer[i] = '\\';
		}
	}
	return CreateDirectoryA(path, 0);
}

// Test whether a file exist
bool CmFile::FileExist(const string& filePath)
{
	if (filePath.size() == 0)
		return false;

	return  GetFileAttributesA(filePath.c_str()) != INVALID_FILE_ATTRIBUTES; // ||  GetLastError() != ERROR_FILE_NOT_FOUND;
}

string CmFile::GetWkDir()
{	
	string wd;
	wd.resize(1024);
	DWORD len = GetCurrentDirectoryA(1024, &wd[0]);
	wd.resize(len);
	return wd;
}

string CmFile::BrowseFolder()   
{
	static char Buffer[MAX_PATH];
	BROWSEINFOA bi;//Initial bi 	
	bi.hwndOwner = NULL; 
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer; // Dialog can't be shown if it's NULL
	bi.lpszTitle = "BrowseFolder";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage = NULL;


	LPITEMIDLIST pIDList = SHBrowseForFolderA(&bi); // Show dialog
	if(pIDList)
	{	
		SHGetPathFromIDListA(pIDList, Buffer);
		if (Buffer[strlen(Buffer) - 1]  == '\\')
			Buffer[strlen(Buffer) - 1] = 0;

		return string(Buffer);
	}
	return string();   
}

string CmFile::BrowseFile(bool isOpen /* = true */)
{
	static char Buffer[MAX_PATH];
	OPENFILENAMEA   ofn;  
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = Buffer;
	ofn.lpstrFile[0] = '\0';   
	ofn.nMaxFile = MAX_PATH;   
	ofn.lpstrFilter = "Images (*.bmp;*.jpg;*.png;*.gif)\0*.bmp;*.jpg;*.png;*.gif\0All (*.*)\0*.*\0\0";   
	ofn.nFilterIndex = 1;    
	ofn.Flags = OFN_PATHMUSTEXIST;   

	if (isOpen)
	{
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//		GetOpenFileNameA(&ofn);
		return Buffer;
	}

//	GetSaveFileNameA(&ofn);
	return string(Buffer);

}

int CmFile::Rename(const string& _srcNames, const string& _dstDir, const char *nameCommon, const char *nameExt)
{
	vecS names;
	string inDir;
	int fNum = GetNames(_srcNames, names, inDir);
	for (int i = 0; i < fNum; i++)
	{
		string dstName = format("%s\\%.4d%s.%s", _dstDir.c_str(), i, nameCommon, nameExt);
		string srcName = inDir + names[i];
		::CopyFileA(srcName.c_str(), dstName.c_str(), FALSE);
	}
	return fNum;
}

void CmFile::RmFolder(const string& dir)
{
	if (FolderExist(dir))
	{
		CStringA cmd; 
		cmd.Format("rmdir /s /q \"%s\"", dir.c_str());
		system(cmd);
	}
}

void CmFile::CleanFolder(const string& dir)
{
	RmFolder(dir);
	MkDir(dir);
}

bool CmFile::FolderExist(const string& strPath)
{
	int i = (int)strPath.size() - 1;
	for (; i >= 0 && (strPath[i] == '\\' || strPath[i] == '/'); i--)
		;
	string str = strPath.substr(0, i+1);

	WIN32_FIND_DATAA  wfd;
	HANDLE hFind = FindFirstFileA(str.c_str(), &wfd);
	bool rValue = (hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);   
	FindClose(hFind);
	return rValue;
}


int CmFile::GetSubFolders(const string& folder, vecS& subFolders)
{
	subFolders.clear();
	WIN32_FIND_DATAA fileFindData;
	string nameWC = folder + "\\*";
	HANDLE hFind = ::FindFirstFileA(nameWC.c_str(), &fileFindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return 0;

	do 
	{
		if (fileFindData.cFileName[0] == '.')
			continue; // filter the '..' and '.' in the path
		if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			subFolders.push_back(fileFindData.cFileName);
	} while (::FindNextFileA(hFind, &fileFindData));
	FindClose(hFind);
	return (int)subFolders.size();
}

// Get image names from a wildcard. Eg: GetNames("D:\\*.jpg", imgNames);
int CmFile::GetNames(const string& nameWC, vector<string>& names)
{
	names.clear();
	WIN32_FIND_DATAA fileFindData;
	HANDLE hFind = ::FindFirstFileA(nameWC.c_str(), &fileFindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return 0;

	do 
	{
		if (fileFindData.cFileName[0] == '.')
			continue; // filter the '..' and '.' in the path
		if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue; // Ignore sub-folders
		names.push_back(fileFindData.cFileName);
	} while (::FindNextFileA(hFind, &fileFindData));
	FindClose(hFind);
	return (int)names.size();
}

int CmFile::GetNames(const string &nameW, vecS &names, string &dir)
{
	dir = GetFolder(nameW);
	return GetNames(nameW, names);
}

int CmFile::GetNamesNE(const string& nameWC, vector<string> &names, string &dir)
{
	int fNum = GetNames(nameWC, names, dir);
	for (int i = 0; i < fNum; i++)
		names[i] = GetNameNE(names[i]);
	return fNum;
}

int CmFile::GetNamesNE(const string& nameWC, vecS &names)
{
	string dir;
	return GetNamesNE(nameWC, names, dir);
}

// Load mask image and threshold thus noisy by compression can be removed
Mat CmFile::LoadMask(const string& fileName)
{
	Mat mask = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	CV_Assert_(mask.data != NULL, ("Can't find mask image: %s", fileName.c_str()));
	compare(mask, 128, mask, CV_CMP_GT);
	return mask;
}
