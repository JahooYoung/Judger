#if !defined(_EXPRESSION_H)
#define _EXPRESSION_H

#include "stdafx.h"

class Varible
{
public:
	LL x, y, val;
};

class Expression
{
public:
	Expression(Varible *_var = NULL);
	LL GetValue(char *begin, char *end); 
	inline LL RandBig();
	inline LL RandInRange(const LL &l, const LL &r);
private:
	Varible *var;
	void InfixToSuffix(char* infix, char* suffix);
	LL SuffixValue(char *suffix);
};

inline LL Expression::RandBig()
{
	return ((LL)(rand() & 7) << 60) | ((LL)rand() << 45) | ((LL)rand() << 30) 
		| (rand() << 15) | rand();
}

inline LL Expression::RandInRange(const LL &l, const LL &r)
{
	if (r < l) return r + RandBig() % (l - r + 1);
	return l + RandBig() % (r - l + 1);
}

#endif