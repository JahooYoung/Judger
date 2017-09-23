#pragma once

#include "stdafx.h"

class Comparer
{
public:
	Comparer(string infoFile, string userOutput, string stdOutput);
	bool Compare();
private:
	string userOutput, stdOutput;
	int mode; //0:normal, 1:strict, 2:real, 3:custom
	double eps;
	void Display(int line, string user, string stdo);
	bool CompareNormal();
	bool CompareStrict();
	bool CompareReal();
};

