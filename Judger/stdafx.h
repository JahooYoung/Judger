// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <random>

using namespace std;
#define mmst(a, b) memset(a, b, sizeof(a))
#define mmcp(a, b) memcpy(a, b, sizeof(b))

typedef long long LL;

const int inf = 2000000000;

inline bool IsControlString(string s)
{
	s.erase(s.find_last_not_of(" ") + 1);
	return s == "input:"
		|| s == "makedata:"
		|| s == "compare:"
		|| s == "file:";
}

LL RandBig();
LL RandInRange(const LL &l, const LL &r);
bool ExistFile(const string &file);

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
