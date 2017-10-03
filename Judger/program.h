#pragma once

#include "stdafx.h"
#include <windows.h>

class Program
{
public:
	Program(string programName, bool showContent, int timeLimit = inf);
	void SetArgument(string argc);
	bool Run();
	int LastRunningTime();
	double AverageRunningTime();
	double LastRunningMemory();
	double MaxRunningMemory();
	string ProgramName();
private:
	string prog, argc;
	LPSTR cmd;
	int totalRunningTime, runCount, lastRunningTime, timeLimit;
	SIZE_T lastRunningMemory, maxRunningMemory;
	bool showContent;
};

