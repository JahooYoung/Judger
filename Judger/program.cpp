#include "stdafx.h"
#include "program.h"
#include "windowMessage.h"
#include <windows.h>

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
	totalRunningTime(0), lastRunningTime(0), runCount(0), argc("")
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

string Program::ProgramName()
{
	return prog;
}

bool Program::Run()
{
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	int timer = minDelay;
	if (runCount) timer = (totalRunningTime / runCount) >> 1;

	//cerr << cmd << endl;

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

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	totalRunningTime += lastRunningTime = runTime;
	runCount++;

	if (exitCode == inf) return false;
	if (exitCode != 0)
	{
		ShowJudger();
		cout << "程序异常结束" << endl;
		cout << "-----------------暂停，Esc退出，任意键继续--------------------" << endl << endl;
		WaitAKey();
		return false;
	}

	return true;
}