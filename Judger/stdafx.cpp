// stdafx.cpp : 只包括标准包含文件的源文件
// $safeprojectname$.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

LL RandBig()
{
	static mt19937_64 mt64(time(0));
	return mt64() & 0x7FFFFFFFFFFFFFFF;
}

LL RandInRange(const LL &l, const LL &r)
{
	if (r < l) return r + RandBig() % (l - r + 1);
	return l + RandBig() % (r - l + 1);
}

bool ExistFile(const string &file)
{
	ifstream fin(file);
	if (!fin) return false;
	fin.close();
	return true;
}

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用
