#pragma once
#include "../stdafx2.h"
#include <iostream>
#include <vector>
#include <map>
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define BUFFER_SIZE 1024
extern char gTmpStrBuf[BUFFER_SIZE];

const char* CmSprintf(const char* format, ...);
void CmInitial(const char* setName, const char* logName, bool openLogWindow = true, bool logAppend = true, const char* setSection = NULL);
void CmFinish();

class CmSetting
{
public:
	CmSetting(string name = "set.ini");
	CmSetting(string name, string section);
	~CmSetting(void);
	static void Demo(const string name = "set.ini");

public:
	// Load setting with section name and file name
	void LoadSetting(const string name = "set.ini");
	void LoadSetting(const string name, string section);

	// Get a string indicating the whole setting data
	const string Txt(void);

	// Get const char* value
	const char* Val(const char* key);

	// Get int setting value
	int operator [](const char* key);

	// Get double setting value
	double operator ()(const char* key);

	// Add new expression to the setting
	void Add(string expr);

	// Apply Expressions in the setting files
	void ApplyExpression(void);

	// Save setting files (The format is destroyed)
	void Save(const char* fileName = NULL);

	bool IsTrue(const char* key);

	bool LoadVector(const char* key, vector<double> &vd);
	bool LoadVector(const char* key, vector<int> &vi);
	bool LoadVector(const char* key, vector<string> &vs);

private:
	map<string, string> m_Setting;
	vector<string> m_Keys;
	static char* m_hostName;

	vector<string> Split(const char* key);
	bool Split(const string str , string& key, string& value);
	string Calc(string expression);
	int Split(string expression, int a, int b, int& value);
	double Calc(const string expression, int a, int b);
};

extern CmSetting* gpSet;
#define gCmSet (*gpSet)