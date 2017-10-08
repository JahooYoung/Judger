#include "stdafx.h"
#include "windowMessage.h"
#include <windows.h>
#include <conio.h>

bool keyPress[256];
HWND JudgerWindow;

void ChangeTitle(string title)
{
	if (JudgerWindow) title += " Alt+J键缩小或放大";
	system(("title " + title).c_str());
}

void InitWindow()
{
	//LPWSTR strTitle = new wchar_t[256];
	LPSTR strTitle = new char[255];
	GetConsoleTitle(strTitle, 255);
	JudgerWindow = FindWindow("ConsoleWindowClass", strTitle);
	if (!JudgerWindow || !RegisterHotKey(NULL, 1, MOD_ALT, 'J'))
		JudgerWindow = NULL;
}

void ShowJudger()
{
	ShowWindow(JudgerWindow, SW_SHOWNORMAL);
}

int WndPro(HANDLE hwnd)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_HOTKEY)
		{
			if (!IsIconic(JudgerWindow)) SendMessage(JudgerWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			else ShowWindow(JudgerWindow, SW_SHOWNORMAL);
		}
		else									// If Not, Deal With Window Messages
		{
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
	int key = 0;
	while (kbhit())
	{
		key = getch();
		if (key == VK_ESCAPE)
		{
			if (hwnd) TerminateProcess(hwnd, 0);
			exit(0);
		}
		keyPress[key] = true;
	}
	return key;
}

int WaitAKey()
{
	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_HOTKEY)
			{
				if (!IsIconic(JudgerWindow)) SendMessage(JudgerWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				else ShowWindow(JudgerWindow, SW_SHOWNORMAL);
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		if (kbhit())
		{
			int key = getch();
			while (kbhit()) getch();
			if (key == VK_ESCAPE) exit(0);
			return key;
		}
		Sleep(100);
	}
}

void ThrowError(const char *s, const char *ed, const string &info)
{
	cout << "错误：" << info << endl;
	cout << "      ";
	if (ed == NULL) ed = s + strlen(s);
	for (; s < ed; s++)
		cout << *s;
	cout << endl << "按任意键退出" << endl;
	WaitAKey();
	exit(0);
}