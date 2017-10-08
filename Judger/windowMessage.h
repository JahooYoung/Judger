#if !defined(_WINDOW_MESSAGE_H)
#define _WINDOW_MESSAGE_H

#include "stdafx.h"
#include <windows.h>

extern bool keyPress[];

void ChangeTitle(string title);
void InitWindow();
void ShowJudger();
int WndPro(HANDLE runningProgram = NULL);
int WaitAKey();
void ThrowError(const char *s, const char *ed = NULL, const string &info = "");

#endif