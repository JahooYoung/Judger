#pragma once

#include "stdafx.h"
#include <windows.h>

class Program
{
public:
	Program(string programName, bool showContent, int timeLimit = inf);
	void SetArgument(string argc);
	bool Run();
	SIZE_T LastRunningTime();
	double AverageRunningTime();
	double LastRunningMemory();
	double MaxRunningMemory();
	string ProgramName();
	string GetLastOutput();
private:
	string prog, argc;
	LPSTR cmd;
	SIZE_T totalRunningTime, runCount, lastRunningTime, timeLimit;
	SIZE_T lastRunningMemory, maxRunningMemory;
	bool showContent;
	string lastOutput;
};

