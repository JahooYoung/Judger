#include "stdafx.h"
#include "program.h"
#include "windowMessage.h"
#include <windows.h>
#include <Psapi.h>

const int minDelay = 5, maxDelay = 100;

//void StringToWString(const string &str, LPWSTR wstr)
void StringToWString(const string &str, LPSTR &lstr)
{
	//size_t size = str.length();
	//wstr = new wchar_t[size + 1];
	//MultiByteToWideChar(CP_UTF8, 0, str.c_str(), size, wstr, size * sizeof(wchar_t));
	//wstr[size] = 0;  //确保以 '\0' 结尾 
	size_t size = str.length();
	lstr = new char[size + 1];
	strcpy(lstr, str.c_str());
}

Program::Program(string programName, bool _showContent, int _timeLimit)
	: prog(programName), showContent(_showContent), timeLimit(_timeLimit),
	totalRunningTime(0), lastRunningTime(0), runCount(0), argc(""),
	lastRunningMemory(0), maxRunningMemory(0)
{
	StringToWString(prog, cmd);
}

void Program::SetArgument(string _argc)
{
	argc = _argc;
	if (argc != "") StringToWString(prog + " " + argc, cmd);
	else StringToWString(prog, cmd);
}

int Program::LastRunningTime()
{
	return lastRunningTime;
}

double Program::AverageRunningTime()
{
	if (!runCount) return 0;
	return (double)totalRunningTime / runCount;
}

double Program::LastRunningMemory()
{
	return (double)lastRunningMemory / 1048576;
}

double Program::MaxRunningMemory()
{
	return (double)maxRunningMemory / 1048576;
}

string Program::ProgramName()
{
	return prog;
}

bool Program::Run()
{
	int timer = minDelay;
	if (runCount) timer = (totalRunningTime / runCount) >> 1;

	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	PROCESS_MEMORY_COUNTERS pmc;
	DWORD exitCode;
	DWORD runTime = GetTickCount();
	CreateProcess(NULL, cmd, NULL, NULL, FALSE,
		//(!show_content) * DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
		(!showContent) * DETACHED_PROCESS | HIGH_PRIORITY_CLASS,
		NULL, NULL, &si, &pi);

	while (true)
	{
		GetExitCodeProcess(pi.hProcess, &exitCode);
		if (exitCode != STILL_ACTIVE) break;
		//GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc));
		//cerr << "cur:" << pmc.WorkingSetSize / 1048576 << endl
		//	<< "peak:" << pmc.PeakWorkingSetSize / 1048576 << endl;
		WndPro(pi.hProcess);
		if (int(GetTickCount() - runTime) > timeLimit)
		{
			TerminateProcess(pi.hProcess, 0);
			ShowJudger();
			cout << "程序运行超过给定时间限制（" << timeLimit << "ms）！" << endl;
			cout << "-----------------暂停，Esc退出，任意键继续--------------------" << endl << endl;
			WaitAKey();
			exitCode = inf;
			break;
		}
		Sleep(min(maxDelay, max(timer, minDelay)));
		timer >>= 1;
	}
	runTime = GetTickCount() - runTime;

	GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc));
	/*cerr << "cur:" << pmc.WorkingSetSize / 1048576 << endl
		<< "peak:" << pmc.PeakWorkingSetSize / 1048576 << endl;*/

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	lastRunningMemory = pmc.PeakWorkingSetSize;
	maxRunningMemory = max(maxRunningMemory, lastRunningMemory);

	if (runTime > timeLimit) runTime = timeLimit;
	totalRunningTime += lastRunningTime = runTime;
	runCount++;

	if (exitCode == inf) return false;
	if (exitCode != 0)
	{
		ShowJudger();
		cout << "程序" << prog << "异常结束" << endl;
		cout << "-----------------暂停，Esc退出，任意键继续--------------------" << endl << endl;
		WaitAKey();
		return false;
	}

	return true;
}