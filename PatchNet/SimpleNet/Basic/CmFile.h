#pragma once

#ifndef CM_FILE_H
#define  CM_FILE_H
struct CmFile
{
	static string BrowseFile(bool isOpen = true);
	static string BrowseFolder(); 

	static inline string GetFolder(const string& path);
	static inline string GetName(const string& path);
	static inline string GetNameNE(const string& path);

	// Get file names from a wildcard. Eg: GetNames("D:\\*.jpg", imgNames);
	static int GetNames(const string & nameW, vecS & names);
	static int GetNames(const string &nameW, vecS &names, string &dir);
	static int GetNamesNE(const string& nameWC, vecS &names);
	static int GetNamesNE(const string& nameWC, vecS &names, string &dir);
	static inline string GetExtention(const string name);

	static bool FileExist(const string& filePath);
	static bool FolderExist(const string& strPath);

	static string GetWkDir();

	static BOOL MkDir(const string&  path);

	// Eg: RenameImages("D:/DogImages/*.jpg", "F:/Images", "dog", ".jpg");
	static int Rename(const string& srcNames, const string& dstDir, const char* nameCommon, const char* nameExt);

	static void RmFile(const string& fileN) { ::DeleteFileA(_S(fileN));}
	static void RmFolder(const string& dir);
	static void CleanFolder(const string& dir);

	static int GetSubFolders(const string& folder, vecS& subFolders);

	inline static BOOL Copy(const string &src, const string &dst, BOOL failIfExist = FALSE);
	inline static BOOL Move(const string &src, const string &dst, DWORD dwFlags = MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);

	//Load mask image and threshold thus noisy by compression can be removed
	static Mat LoadMask(const string& fileName);
};

/************************************************************************/
/* Implementation of inline functions                                   */
/************************************************************************/
string CmFile::GetFolder(const string& path)
{
	return path.substr(0, path.find_last_of("\\/")+1);
}

string CmFile::GetName(const string& path)
{
	int start = path.find_last_of("\\/")+1;
	int end = path.find_last_not_of(' ')+1;
	return path.substr(start, end - start);
}

string CmFile::GetNameNE(const string& path)
{
	int start = path.find_last_of("\\/")+1;
	int end = path.find_last_of('.');
	if (end >= 0)
		return path.substr(start, end - start);
	else
		return path.substr(start,  path.find_last_not_of(' ')+1 - start);
}

string CmFile::GetExtention(const string name)
{
	return name.substr(name.find_last_of('.'));
}

BOOL CmFile::Copy(const string &src, const string &dst, BOOL failIfExist)
{
	return ::CopyFileA(src.c_str(), dst.c_str(), failIfExist);
}

BOOL CmFile::Move(const string &src, const string &dst, DWORD dwFlags)
{
	return MoveFileExA(src.c_str(), dst.c_str(), dwFlags);
}

#endif