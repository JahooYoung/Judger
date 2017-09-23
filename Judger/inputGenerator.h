#if !defined(_INPUT_GENERATOR_H)
#define _INPUT_GENERATOR_H

#include "stdafx.h"
#include "expression.h"
#include "program.h"

extern const string inGenerName;

class InputGenerator
{
public:
	InputGenerator(string infoFile, string outputFile = "");
	bool GeneratorInput(string outputFile = "");
private:
	string output;
	char *source; 
	int sourceLen;
	Varible var[256];
	Expression exp;
	int timer;
	FILE *file;
	Program *cusGener;
	char *FindChar(char *st, char *ed, const char &target);
	char *FindChar(char *st, char *ed, const char &target, const char &opp);
	void Process(char *s, char *ed);
};

#endif