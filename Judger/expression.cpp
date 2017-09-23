#include "stdafx.h"
#include "expression.h"

bool opPriorityInit = false;
int opPriority[256];

inline bool CmpOp(const char &a, const char &b)
{
	if (b == '(') return true; //������ֱ����ջ
	return opPriority[b] > opPriority[a];
}

inline void InitOpPriority()
{
	if (opPriorityInit) return;
	opPriorityInit = true;
	opPriority['('] = opPriority[')'] = 1;
	opPriority['='] = 2;
	opPriority['<'] = opPriority['>'] = 3;
	opPriority['+'] = opPriority['-'] = 4;
	opPriority['*'] = opPriority['/'] = opPriority['%'] = 5;
	opPriority['^'] = 6;
}

inline bool IsValue(const char &a)
{
	return '0' <= a && a <= '9' || 'a' <= a && a <= 'z' || 'A' <= a && a <= 'Z' || a == '\'';
}

LL power(LL a, LL b)
{
	if (a == 1 || b == 0) return 1;
	LL ret = 1;
	for (; b; b >>= 1)
	{
		if (b & 1) ret *= a;
		a *= a;
	}
	return ret;
}


/* ��׺���ʽת��׺���ʽ����׺���ʽ֮���޷ָ�
 * ��׺���ʽ��������������֮���ÿո�ָ�������ֲ�ͬ������
 */
void Expression::InfixToSuffix(char* infix, char* suffix) 
{
	stack<char> op; //�洢�������ջ
	for (char *infixBegin = infix; *infix; infix++)
	{
		if (IsValue(*infix))
		{
			//��������ֱ�����
			if (*infix == '\'')
			{
				int tmp = *(infix + 1);
				*(suffix++) = tmp / 100 + '0';
				*(suffix++) = tmp / 10 % 10 + '0';
				*(suffix++) = tmp % 10 + '0';
				infix += 2;
			}
			else *(suffix++) = *infix;
		}
		else {
			//�������󲹳�ո�ָ�
			if (infix != infixBegin && IsValue(*(infix - 1)))
				*(suffix++) = ' ';
			if (*infix == '-' && (infix == infixBegin || !IsValue(*(infix - 1))))
			{
				*(suffix++) = '0';
				*(suffix++) = ' ';
			}
			if (*infix == ')')
			{
				//������������һֱ����ֱ�������ţ��������Ų����
				for (; op.top() != '('; op.pop())
				{
					*(suffix++) = op.top();
					*(suffix++) = ' ';
				}
				op.pop();
			}
			else {
				//��ǰ���������ȼ����ڻ����ջ���������򵯳�ջ��
				for (; !op.empty() && !CmpOp(op.top(), *infix); op.pop())
				{
					*(suffix++) = op.top();
					*(suffix++) = ' ';
				}
				//��ǰ��������ջ
				op.push(*infix);
			}
		}
	}
	//����ո�ָ�
	//cerr << *suffix << endl;
	if (*(suffix-1) != ' ') *(suffix++) = ' ';
	//���������ջ��Ϊ�գ��������в�����
	for (; !op.empty(); op.pop())
	{
		*(suffix++) = op.top();
		*(suffix++) = ' ';
	}
	*suffix = '\0';
}

/* ��׺���ʽ��ֵ
 */
LL Expression::SuffixValue(char *suffix) 
{
	LL value = 0;
	stack<LL> operand;
	for (; *suffix; suffix++) 
	{
		if (IsValue(*suffix)) 
		{
			if (*suffix >= '0' && *suffix <= '9') 
				value = value * 10 + *suffix - '0';
			else value = var ? var[*suffix].val : 0;
		}
		else if (*suffix == ' ') {
			//��������ջ
			operand.push(value);
			value = 0;
		}
		else {
			//���ݲ���������ջ���������������м��㲢�õ����
			LL opr2 = operand.top();
			operand.pop();
			LL opr1 = operand.top();
			operand.pop();
			switch (*suffix) 
			{
				case '+': value = opr1 + opr2; break;
				case '-': value = opr1 - opr2; break;
				case '*': value = opr1 * opr2; break;
				case '/': value = opr1 / opr2; break;
				case '<': value = min(opr1, opr2); break;
				case '>': value = max(opr1, opr2); break;
				case '^': value = power(opr1, opr2); break;
				case '%': value = opr1 % opr2; break;
				case '=': value = (opr1 == opr2); break;
			}
		}
	}

	return operand.top();
}

Expression::Expression(Varible *_var)
{
	InitOpPriority();
	var = _var;
}

LL Expression::GetValue(char *begin, char *end)
{
	static int bufSize = 0;
	static char *infix = NULL, *suffix = NULL;
	if (end - begin >= bufSize)
	{
		if (bufSize)
		{
			delete[] infix;
			delete[] suffix;
		}
		bufSize = max((end - begin) << 2, 100);
		infix = new char[bufSize];
		suffix = new char[bufSize];
	}

	int i;
	for (i = 0; begin != end; begin++)
		if (*begin != ' ') infix[i++] = *begin;
	infix[i] = '\0';

	//cerr << "infix: " << infix << endl;

	InfixToSuffix(infix, suffix);
	//cerr << "suffix: " << suffix << '~' << endl;
	return SuffixValue(suffix);
}
