#pragma once

class CmComand
{
public:
	static void RunProgram(const string &fileName, const string &parameters = "", bool waiteTillFlished = false);

	static void SegOmpThrdNum(double ratio = 0.8);

	static void Demo();
};
