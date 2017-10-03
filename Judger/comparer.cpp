#include "stdafx.h"
#include "comparer.h"
#include "windowMessage.h"
#include <sstream>

Comparer::Comparer(string infoFile, string _userOutput, string _stdOutput)
	: userOutput(_userOutput), stdOutput(_stdOutput)
{
	string s;
	ifstream fin(infoFile);
	for (fin >> s; !fin.eof() && s != "compare:"; fin >> s);
	if (s != "compare:")
	{
		fin.close();
		mode = 0;
		return;
	}
	while (getline(fin, s) && (s.find("//") == 0 || s == ""));
	if (s == "normal") mode = 0;
	else if (s == "strict") mode = 1;
	else if (s == "real") mode = 2;
	//else if (s == "custom") mode = 3;
	if (mode == 2)
	{
		if (!(fin >> eps))
		{
			fin.close();
			cout << "请在real的下一行输入一个浮点数作为误差最大值" << endl;
			cout << "按任意键退出" << endl;
			WaitAKey();
			exit(0);
		}
	}
	/*if (mode == 3)
	{
		fin >> file;
		sprintf(compfile, "%s %s %s", file, outfile, correct);
	}*/
	fin.close();
}

bool Comparer::Compare()
{
	//cerr << mode << endl;
	switch (mode)
	{
	case 0: return CompareNormal();
	case 1: return CompareStrict();
	case 2: return CompareReal();
	//case 3: return !RunProcess(compfile, tmp, oo, true);
	}
}

void Comparer::Display(int line, string s1, string s2)
{
	int len = min(s1.size(), s2.size());
	int i, wordCnt = 0;
	for (i = 0; i < len; i++)
	{
		if (s1[i] != ' ' && (i == 0 || s1[i - 1] == ' '))
			wordCnt++;
		if (s1[i] != s2[i]) break;
	}
	int j, k;
	len = s1.size();
	for (j = i; j < len && s1[j] != ' '; j++);
	for (k = i; k >= 0 && s1[k] != ' '; k--);
	s1 = s1.substr(k + 1, j - k - 1);
	len = s2.size();
	for (j = i; j < len && s2[j] != ' '; j++);
	for (k = i; k >= 0 && s2[k] != ' '; k--);
	s2 = s2.substr(k + 1, j - k - 1);
	cout << "找到文件不同处于第 " << line << " 行第 " << wordCnt << " 个数字（串）" << endl;
	cout << "标准输出：" << s1 << endl;
	cout << "你的输出：" << s2 << endl;
}

bool Comparer::CompareNormal()
{
	ifstream user(userOutput), stdo(stdOutput);
	int curLine = 0;
	string s1, s2;
	while (true)
	{
		//WndPro();
		curLine++;
		s1 = s2 = "";
		bool a = getline(stdo, s1) ? true : false;
		s1.erase(s1.find_last_not_of(" ") + 1);
		bool b = getline(user, s2) ? true : false;
		s2.erase(s2.find_last_not_of(" ") + 1);
		if (s1 != s2)
		{
			Display(curLine, s1, s2);
			stdo.close(); user.close();
			return false;
		}
		if (!a && !b)
		{
			stdo.close(); user.close();
			return true;
		}
	}
}

bool Comparer::CompareStrict()
{
	ifstream user(userOutput), stdo(stdOutput);
	int curLine = 0;
	string s1, s2;
	while (true)
	{
		//WndPro();
		curLine++;
		s1 = s2 = "";
		bool a = getline(stdo, s1) ? true : false;
		bool b = getline(user, s2) ? true : false;
		if ((a && !b) || (!a && b) || s1 != s2)
		{
			Display(curLine, s1, s2);
			stdo.close(); user.close();
			return false;
		}
		if (!a && !b)
		{
			stdo.close(); user.close();
			return true;
		}
	}
}

bool Comparer::CompareReal()
{
	//cerr << "entry:" << endl;
	ifstream my_out(userOutput), ac_out(stdOutput);
	string s1, s2; stringstream l1, l2;
	double a, b; int line = 0;
	bool ac_end, my_end, isd1, isd2;

	while (true)
	{
		//WndPro();
		line++;
		s1 = s2 = "";
		ac_end = getline(ac_out, s1) ? true : false;
		s1.erase(s1.find_last_not_of(" ") + 1);
		my_end = getline(my_out, s2) ? true : false;
		s2.erase(s2.find_last_not_of(" ") + 1);
		if (!ac_end && !my_end)
		{
			ac_out.close(); my_out.close();
			return true;
		}
		//cerr << s1 << endl << s2 << endl;
		l1.clear(); l2.clear();
		l1 << s1; l2 << s2;
		while (true)
		{
			string ss1 = "", ss2 = "";
			ac_end = l1 >> ss1 ? true : false;
			my_end = l2 >> ss2 ? true : false;
			if (!ac_end || !my_end) break;

			const char *ss;
			char *endptr;
			a = strtod(ss = ss1.c_str(), &endptr);
			isd1 = ss != endptr;
			b = strtod(ss = ss2.c_str(), &endptr);
			isd2 = ss != endptr;
			//cerr << "a: " << a << "b: " << b << endl;
			if (!(isd1 && isd2 && abs(a - b) < eps || ss1 == ss2))
			{
				Display(line, s1, s2);
				ac_out.close(); my_out.close();
				return false;
			}
		}
		if (ac_end || my_end)
		{
			Display(line, s1, s2);
			ac_out.close(); my_out.close();
			return false;
		}
	}
}