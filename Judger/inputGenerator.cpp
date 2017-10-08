#include "stdafx.h"
#include "inputGenerator.h"
#include "windowMessage.h"
#include "program.h"

const string inGenerName = "inputGen";
const int maxBuffer = 100000;

void InputGenerator::Printt()
{
	for (int i = 0; i < inst.size(); i++)
	{
		cerr << i << ": ";
		inst[i].Print();
	}
	cerr << "------------------------------------------" << endl;
}

InputGenerator::InputGenerator(string infoFile, string outputFile) 
	: expr(), file(NULL), cusGener(NULL)
{
	output = outputFile;
	ifstream fin(infoFile);
	string s, ss;
	for (fin >> s; !fin.eof() && s != "input:"; fin >> s);
	if (s != "input:")
	{
		fin.close();
		if (!ExistFile(inGenerName + ".exe"))
		{
			cout << "并没有找到" << inGenerName + ".exe" << " (@.@)" << endl;
			cout << "也没有找到\"input:\"，输入咋办捏" << endl;
			cout << "按任意键退出吧" << endl;
			WaitAKey();
			exit(0);
		}
		cusGener = new Program(inGenerName, false);
		return;
	}
	for (ss = ""; getline(fin, s) && !IsControlString(s); )
		if (s.find("//") != 0) ss += s;
	fin.close();
	int sourceLen = ss.size();
	char *source = new char[sourceLen + 1];
	strcpy(source, ss.c_str());
	source[sourceLen] = '\0';

	inst.clear();
	varCnt = 123;
	Compile(source, source + sourceLen, 0);
	inst.push_back(Instruction(0));
	var = new LL[varCnt];
	memset(var, 0, sizeof(LL)*varCnt);
	expr.SetVarible(var);

	bufEnd = buffer = new char[maxBuffer];
}

void InputGenerator::InitBuffer()
{
	bufEnd = buffer;
}

void InputGenerator::PushToBuffer(const char &ch)
{
	if (bufEnd - buffer == maxBuffer)
	{
		if (file) fwrite(buffer, 1, maxBuffer, file);
		bufEnd = buffer;
	}
	*(bufEnd++) = ch;
}

void InputGenerator::FlushBuffer()
{
	if (bufEnd != buffer && file) fwrite(buffer, 1, bufEnd - buffer, file);
	bufEnd = buffer;
}

char *InputGenerator::FindChar(char *st, const char *ed, const char &tar)
{
	for (; st < ed; st++)
		if (*st == tar) return st;
	return NULL;
}

char *InputGenerator::FindChar(char *s, const char *ed, const char &tar, const char &opp)
{
	for (int depth = 1; s < ed; s++)
	{
		if (*s == tar) depth--;
		if (*s == opp) depth++;
		if (depth == 0) return s;
	}
	return NULL;
}

void InputGenerator::Compile(char *s, char *ed, int mode) //1 unprint
{
	while (s < ed)
	{
		if (*s == '[')
		{
			char *loopDefEnd = FindChar(s, ed, ']');
			if (!loopDefEnd)
				ThrowError(s, ed, "循环定义部分找不到']'");
			char *loopEnd = FindChar(s + 1, ed, ';', '[');
			if (!loopEnd)
				ThrowError(s, ed, "循环找不到结束符';'");
			char *comma = FindChar(s, loopDefEnd, ',');
			if (!comma)
			{
				int loopCnt = varCnt++;
				inst.push_back(Instruction(1, loopCnt, expr.GetExpression(s + 1, loopDefEnd)));
				int jmp = inst.size();
				inst.push_back(Instruction(8, 0, NULL, new int));
				int itStart = inst.size();
				Compile(loopDefEnd + 1, loopEnd, mode);
				inst.push_back(Instruction(4, loopCnt));
				*inst[jmp].jumpTable = inst.size();
				inst.push_back(Instruction(9, loopCnt, NULL, new int(itStart)));
			}
			else {
				int loopCnt = varCnt++;
				inst.push_back(Instruction(1, loopCnt, expr.GetExpression(s + 1, comma)));
				for (comma++; comma < loopDefEnd && *comma == ' '; comma++);
				if (!(('a' <= *comma && *comma <= 'z') || ('A' <= *comma && *comma <= 'Z')))
					ThrowError(comma, comma + 1, "不能识别的变量");
				int loopVar = *comma;
				for (comma++; comma < loopDefEnd && *comma == ' '; comma++);
				if (comma != loopDefEnd && *comma != ',')
					ThrowError(comma, comma + 1, "不能识别的部分");
				if (comma != loopDefEnd)
					inst.push_back(Instruction(1, loopVar, expr.GetExpression(comma + 1, loopDefEnd)));
				else inst.push_back(Instruction(1, loopVar, expr.GetExpression("1")));
				int itStart = inst.size();
				Compile(loopDefEnd + 1, loopEnd, mode);
				inst.push_back(Instruction(3, loopVar));
				inst.push_back(Instruction(4, loopCnt));
				inst.push_back(Instruction(9, loopCnt, NULL, new int(itStart)));
			}
			s = loopEnd + 1;
			continue;
		}
		if (*s == '{')
		{
			char *selectEnd = FindChar(s + 1, ed, '}', '{');
			if (!selectEnd)
				ThrowError(s, ed, "选择语句找不到结束符'}'");
			vector<char*> option;
			option.clear();
			option.push_back(s);
			for (char *ch = s + 1; ch != selectEnd; ch++)
			{
				if (*ch == '|') option.push_back(ch);
				if (*ch == '{') ch = FindChar(ch + 1, selectEnd, '}', '{');
			}
			option.push_back(selectEnd);
			int *jpt = new int[option.size() - 1];
			inst.push_back(Instruction(10, option.size() - 1, NULL, jpt));
			int jmp = inst.size();
			inst.push_back(Instruction(8, 0, NULL, new int));
			for (int i = 0; i < option.size() - 1; i++)
			{
				jpt[i] = inst.size();
				Compile(option[i] + 1, option[i + 1], mode);
				inst.push_back(Instruction(8, 0, NULL, new int(jmp)));
			}
			*(inst[jmp].jumpTable) = inst.size();
			s = selectEnd + 1;
			continue;
		}
		if (('a' <= *s && *s <= 'z' || 'A' <= *s && *s <= 'Z')
			&& (*(s + 1) == '(' || *(s + 1) == '$' && *(s + 2) == '('))
		{
			bool unprint = (*(s + 1) == '$');
			char *varibleEnd = FindChar(s + 2 + unprint, ed, ')', '(');
			if (!varibleEnd)
				ThrowError(s, ed, "赋值语句找不到结束符')'");
			char *comma = FindChar(s, varibleEnd, ',');
			int var = *s;
			if (!comma)
				inst.push_back(Instruction(1, var, expr.GetExpression(s + 2 + unprint, varibleEnd)));
			else {
				inst.push_back(Instruction(1, var, expr.GetExpression(s + 2 + unprint, comma)));
				inst.push_back(Instruction(1, 1, expr.GetExpression(comma + 1, varibleEnd)));
				inst.push_back(Instruction(2, var));
			}
			if (!unprint && !mode)
			{
				if (*s != 'c')
					inst.push_back(Instruction(6, var));
				else inst.push_back(Instruction(7, var));
			}
			s = varibleEnd + 1;
			continue;
		}
		if (*s != '\\')
		{
			if (!mode) 
				inst.push_back(Instruction(5, 0, new char(*s)));
		}
		else {
			s++;
			if (s == ed) 
				ThrowError(s-1, ed, "'\\'后缺少字符");
			if (!mode) 
				inst.push_back(Instruction(5, 0, new char(*s == 'n' ? '\n' : *s)));
		}
		s++;
	}
}

void InputGenerator::Execute(int st)
{
	vector<Instruction>::iterator i = inst.begin() + st;
	while (i->type)
	{
		if ((++timer) == 100000) WndPro(), timer = 0;
		// cerr << i-inst.begin() << ": ";
		// i->Print();
		char num[20]; int len = -1;
		switch (i->type)
		{
		case 1:
			var[i->var] = expr.GetValue(i->expr);
			/*cerr << "1: " << i->var << ' ' << var[i->var] << endl;
			WaitAKey();*/
			break;
		case 2:
			var[i->var] = RandInRange(var[i->var], var[1]);
			break;
		case 3:
			var[i->var]++;
			break;
		case 4:
			var[i->var]--;
			break;
		case 5:
			PushToBuffer(*(i->expr));
			break;
		case 6:
			len = -1;
			if (var[i->var] < 0)
				PushToBuffer('-');
			for (LL temp = abs(var[i->var]); temp; temp /= 10)
				num[++len] = '0' + temp % 10;
			if (len == -1) num[++len] = '0';
			for (; len >= 0; len--)
				PushToBuffer(num[len]);
			break;
		case 7:
			PushToBuffer(var[i->var]);
			break;
		case 8:
			i = inst.begin() + *(i->jumpTable);
			continue;
		case 9:
			if (var[i->var])
			{
				i = inst.begin() + *(i->jumpTable);
				continue;
			}
			break;
		case 10:
			int t = RandInRange(0, i->var - 1);
			i = inst.begin() + i->jumpTable[t];
			continue;
		}
		++i;
	}
}

bool InputGenerator::GeneratorInput(int dataId, int start)
{
	if (!cusGener)
	{
		timer = 0;
		if (!(file = fopen(output.c_str(), "w"))) return false;
		InitBuffer();
		//cerr << "start: " << start << endl;
		Execute(start);
		FlushBuffer();
		fclose(file);
		return true;
	}
	else {
		// Run inputGen.exe
		string arg = to_string(RandBig() % inf);
		if (dataId != 0) arg += " " + to_string(dataId);
		cusGener->SetArgument(arg);
		return cusGener->Run();
	}
}

int InputGenerator::DataCompile(char *s, char *ed)
{
	int ret = inst.size();
	Compile(s, ed, 1);
	inst.push_back(Instruction(8, 0, NULL, new int(0)));
	delete[] var;
	var = new LL[varCnt];
	expr.SetVarible(var);
	memset(var, 0, sizeof(LL)*varCnt);
	return ret;
}