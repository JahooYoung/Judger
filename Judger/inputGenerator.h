#if !defined(_INPUT_GENERATOR_H)
#define _INPUT_GENERATOR_H

#include "stdafx.h"
#include "expression.h"
#include "program.h"

extern const string inGenerName;

class Instruction
{
public:
	char type;
	int var;
	char *expr;
	int *jumpTable;

	Instruction(char _type, int _var = 0, char *_expr = NULL, int *_jumpTable = NULL)
		: type(_type), var(_var), expr(_expr), jumpTable(_jumpTable)
	{}

	void Print()
	{
		cerr << (int)type << ' ';
		if (var) cerr << var << ' ';
		if (expr)
		{
			if (type != 5) cerr << '"' << expr << "\" ";
			else cerr << (int)*expr << ' ';
		}
		if (jumpTable) cerr << *jumpTable << ' ';
		cerr << endl;
	}
};

class InputGenerator
{
public:
	InputGenerator(string infoFile, string outputFile = "");
	bool GeneratorInput(int dataId = 0, int start = 0);
	int DataCompile(char *s, char *ed);
	void Printt();
private:
	string output; FILE *file;
	Program *cusGener;
	int timer, varCnt;
	LL *var; Expression expr;
	vector<Instruction> inst;
	char *buffer, *bufEnd;

	void InitBuffer();
	void PushToBuffer(const char &ch);
	void FlushBuffer();
	char *FindChar(char *st, const char *ed, const char &target);
	char *FindChar(char *st, const char *ed, const char &target, const char &opp);
	void Compile(char *s, char *ed, int mode); //1 unprint
	void Execute(int st);
};


#endif