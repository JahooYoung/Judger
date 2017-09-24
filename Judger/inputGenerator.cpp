#include "stdafx.h"
#include "inputGenerator.h"
#include "windowMessage.h"
#include "program.h"

const string inGenerName = "inputGen";
const int maxBuffer = 100000;

InputGenerator::InputGenerator(string infoFile, string outputFile) 
	: exp(var), buffer(NULL), bufEnd(NULL)
{
	for (int i = 0; i < 256; i++)
		var[i].val = 0;
	output = outputFile;
	ifstream fin(infoFile);
	string s, ss;
	for (fin >> s; !fin.eof() && s != "input:"; fin >> s);
	if (s != "input:")
	{
		sourceLen = -1;
		fin.close();
		cusGener = new Program(inGenerName + ".exe", false);
		return;
	}
	for (ss = ""; getline(fin, s) && !IsControlString(s); )
		if (s.find("//") != 0) ss += s;
	fin.close();
	sourceLen = ss.size();
	source = new char[sourceLen + 5];
	strcpy(source, ss.c_str());
	source[sourceLen] = '\0';
	//cerr << source << endl;
}

void InputGenerator::InitBuffer()
{
	if (!buffer) buffer = new char[maxBuffer];
	bufEnd = buffer;
}

void InputGenerator::PushToBuffer(const char &ch)
{
	if (bufEnd - buffer == maxBuffer)
	{
		fwrite(buffer, 1, maxBuffer, file);
		bufEnd = buffer;
	}
	*(bufEnd++) = ch;
}

void InputGenerator::FlushBuffer()
{
	if (bufEnd != buffer) fwrite(buffer, 1, bufEnd - buffer, file);
	bufEnd = buffer;
}

char *InputGenerator::FindChar(char *st, char *ed, const char &tar)
{
	for (; st < ed; st++)
		if (*st == tar) return st;
	return NULL;
}

char *InputGenerator::FindChar(char *s, char *ed, const char &tar, const char &opp)
{
	for (int depth = 1; s < ed; s++)
	{
		if (*s == tar) depth--;
		if (*s == opp) depth++;
		if (depth == 0) return s;
	}
	return NULL;
}

void InputGenerator::Process(char *s, char *ed)
{
	while (s < ed)
	{
		if ((++timer) == 50000) WndPro(), timer = 0;
		if (*s == '[')
		{
			char *loopDefEnd = FindChar(s, ed, ']');
			char *loopEnd = FindChar(s + 1, ed, ';', '[');
			char *comma = FindChar(s, loopDefEnd, ',');
			if (!comma)
			{
				LL loopCnt = exp.GetValue(s + 1, loopDefEnd);
				for (LL i = 0; i < loopCnt; i++)
					Process(loopDefEnd + 1, loopEnd);
			}
			else {
				LL loopCnt = exp.GetValue(s + 1, comma);
				Varible &i = var[*comma + 1];
				comma += 2;
				i.val = 1;
				if (comma != loopDefEnd)
					loopCnt += (i.val = exp.GetValue(comma, loopDefEnd) - 1);
				for (; i.val <= loopCnt; i.val++)
					Process(loopDefEnd + 1, loopEnd);
			}
			s = loopEnd + 1;
			continue;
		}
		if (*s == '{')
		{
			char *selectEnd = FindChar(s + 1, ed, '}', '{');
			vector<char*> option;
			option.clear();
			option.push_back(s);
			for (char *ch = s + 1; ch != selectEnd; ch++)
			{
				if (*ch == '|') option.push_back(ch);
				if (*ch == '{') ch = FindChar(ch + 1, selectEnd, '}', '{');
			}
			option.push_back(selectEnd);
			LL t = exp.RandInRange(0, option.size() - 2);
			Process(option[t] + 1, option[t + 1]);
			s = selectEnd + 1;
			continue;
		}
		if (('a' <= *s && *s <= 'z' || 'A' <= *s && *s <= 'Z') 
			&& (*(s + 1) == '(' || *(s + 1) == '$' && *(s + 2) == '('))
		{
			bool unprint = (*(s + 1) == '$');
			char *varibleEnd = FindChar(s + 2 + unprint, ed, ')', '(');
			char *comma = FindChar(s, varibleEnd, ',');
			Varible &i = var[*s];
			if (!comma)
				i.x = i.y = exp.GetValue(s + 2 + unprint, varibleEnd);
			else {
				i.x = exp.GetValue(s + 2 + unprint, comma);
				i.y = exp.GetValue(comma + 1, varibleEnd);
			}
			i.val = exp.RandInRange(i.x, i.y);
			if (!unprint)
			{
				if (*s != 'c')
				{
					char num[20];
					int len = -1;
					if (i.val < 0) PushToBuffer('-');
					for (LL temp = abs(i.val); temp; temp /= 10)
						num[++len] = '0' + temp % 10;
					if (len == -1) num[++len] = '0';
					for (; len >= 0; len--)
						PushToBuffer(num[len]);
				}
				else PushToBuffer(i.val);
			}
			s = varibleEnd + 1;
			continue;
		}
		if (*s != '\\') PushToBuffer(*s);
		else {
			s++;
			PushToBuffer(*s == 'n' ? '\n' : *s);
		}
		s++;
	}
}

bool InputGenerator::GeneratorInput(string outputFile)
{
	if (outputFile == "" && output == "") return false;
	if (output == "") output = outputFile;
	if (sourceLen != -1)
	{
		timer = 0;
		if (!(file = fopen(output.c_str(), "w"))) return false;
		InitBuffer();
		Process(source, source + sourceLen);
		FlushBuffer();
		fclose(file);
		return true;
	}
	else {
		// Run inputGen.exe
		cusGener->SetArgument(to_string(exp.RandBig() % inf));
		return cusGener->Run();
	}
}