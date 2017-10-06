#pragma once

#include "stdafx.h"
#include "program.h"

extern const string customCheckerName;

class Comparer
{
public:
	Comparer(string infoFile, string inputFile, string userOutput, string stdOutput);
	bool Compare();
private:
	string inputFile, userOutput, stdOutput;
	int mode; //0:normal, 1:strict, 2:real, 3:custom
	double eps;
	Program *cusChecker;
	void Display(int line, string user, string stdo);
	bool CompareNormal();
	bool CompareStrict();
	bool CompareReal();
	bool CompareCustom();
};

