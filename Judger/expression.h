#if !defined(_EXPRESSION_H)
#define _EXPRESSION_H

#include "stdafx.h"

class Expression
{
public:
	Expression();
	void SetVarible(LL *var);
	char *GetExpression(const char *s, const char *ed = NULL);
	LL GetValue(char *expr);
private:
	LL *var;
	void InfixToSuffix(char *infix, char *suffix);
	void PreCalculate(char *in, char *out);
};

#endif