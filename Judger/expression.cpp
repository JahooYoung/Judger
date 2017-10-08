#include "stdafx.h"
#include "expression.h"
#include "windowMessage.h"

bool opPriorityInit = false;
int opPriority[256];

inline bool CmpOp(const char &a, const char &b)
{
	if (b == '(') return true; //������ֱ����ջ
	return opPriority[b] > opPriority[a];
}

void InitOpPriority()
{
	if (opPriorityInit) return;
	opPriorityInit = true;
	mmst(opPriority, 0);
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
	//if (a == 1 || b == 0) return 1;
	LL ret = 1;
	for (; b; b >>= 1)
	{
		if (b & 1) ret *= a;
		a *= a;
	}
	return ret;
}

Expression::Expression()
{
	InitOpPriority();
}

void Expression::SetVarible(LL *_var)
{
	var = _var;
}

/* ��׺���ʽת��׺���ʽ����׺���ʽ֮���޷ָ�
 * ��׺���ʽ��������������֮���ÿո�ָ�������ֲ�ͬ������
 */
void Expression::InfixToSuffix(char *infix, char *suffix) 
{
	stack<char> op; //�洢�������ջ
	int curOperands = 0;
	char *infixBegin = infix;
	for (; *infix; infix++)
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
				*(suffix++) = ' ', curOperands++;
			if (*infix == '-' && (infix == infixBegin || !IsValue(*(infix - 1))))
			{
				*(suffix++) = '0';
				*(suffix++) = ' ';
				curOperands++;
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
					curOperands -= 2;
					if (curOperands < 0)
						ThrowError(infixBegin, infixBegin + strlen(infixBegin), "���ʽȱ��������");
				}
				//��ǰ��������ջ
				op.push(*infix);
			}
		}
	}
	//����ո�ָ�
	//cerr << *suffix << endl;
	if (*(suffix-1) != ' ') *(suffix++) = ' ', curOperands++;
	//���������ջ��Ϊ�գ��������в�����
	for (; !op.empty(); op.pop())
	{
		*(suffix++) = op.top();
		*(suffix++) = ' ';
		curOperands -= 2;
		if (curOperands < 0)
			ThrowError(infixBegin, infixBegin + strlen(infixBegin), "���ʽȱ��������");
	}
	*suffix = '\0';
}

/* ��׺���ʽ��ֵ
 */
LL Expression::GetValue(char *suffix) 
{
	//cerr << suffix << endl;
	LL value = 0;
	stack<LL> operand;
	for (; *suffix; suffix++) 
	{
		if (IsValue(*suffix)) 
		{
			if (*suffix >= '0' && *suffix <= '9') 
				value = value * 10 + *suffix - '0';
			else value = var ? var[*suffix] : 0;
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

void Expression::PreCalculate(char *in, char *out)
{
	LL value = 0;
	bool numFlag = false;
	stack<LL> operand;
	for (; *in; in++)
	{
		if (*in >= '0' && *in <= '9')
		{
			value = value * 10 + *in - '0';
			numFlag = true;
		}
		else if (*in == ' ')
		{
			if (numFlag) operand.push(value);
			value = 0;
			numFlag = false;
		}
		else if (opPriority[(int)*in] && operand.size() > 1)
		{
			LL opr2 = operand.top();
			operand.pop();
			LL opr1 = operand.top();
			operand.pop();
			switch (*in)
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
			numFlag = true;
		}
		else
		{
			stack<char> tmp;
			for (; !operand.empty(); operand.pop())
			{
				tmp.push(' ');
				value = operand.top();
				bool minus = (value < 0);
				if (minus)
					tmp.push('-'), tmp.push(' '), value = -value;
				do {
					tmp.push('0' + value % 10);
					value /= 10;
				} while (value);
				if (minus)
					tmp.push(' '), tmp.push('0');
			}
			for (; !tmp.empty(); tmp.pop())
				*(out++) = tmp.top();
			*(out++) = *in;
			*(out++) = ' ';
			numFlag = false;
		}
	}
	stack<char> tmp;
	for (; !operand.empty(); operand.pop())
	{
		tmp.push(' ');
		value = operand.top();
		bool minus = (value < 0);
		if (minus)
			tmp.push('-'), tmp.push(' '), value = -value;
		do {
			tmp.push('0' + value % 10);
			value /= 10;
		} while (value);
		if (minus)
			tmp.push(' '), tmp.push('0');
	}
	for (; !tmp.empty(); tmp.pop())
		*(out++) = tmp.top();
	*out = '\0';
}

char *Expression::GetExpression(const char *s, const char *ed)
{
	if (!ed) ed = s + strlen(s);
	int len = 0;
	for (const char *i = s; i != ed; i++)
	{
		if (*i == ' ') continue;
		len++;
		if (!IsValue(*i) && !opPriority[*i]) 
			ThrowError(s, ed, string("����ʶ��ı��ʽ����'") + *i + "'");
	}
	if (len == 0)
		ThrowError(s, ed, "ȱ�ٱ��ʽ");
	char *infix = new char[40 * (len + 5)], *suffix = new char[40 * (len + 5)];
	for (int i = 0; s != ed; s++)
		if (*s != ' ') infix[i++] = *s;
	infix[len] = '\0';
	InfixToSuffix(infix, suffix);
	PreCalculate(suffix, infix);
	// cerr << "Suffix: " << suffix << endl;
	// cerr << "Auffix: " << infix << endl;

	len = strlen(infix);
	char *ret = new char[len + 1];
	strcpy(ret, infix);
	ret[len] = '\0';
	delete[] infix;
	delete[] suffix;
	// cerr << "GE: " << ret << endl;
	return ret;
}