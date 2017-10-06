// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// $safeprojectname$.pch ����ΪԤ�����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

LL RandBig()
{
	static mt19937_64 mt64(time(0));
	return mt64() & 0x7FFFFFFFFFFFFFFF;
}

LL RandInRange(const LL &l, const LL &r)
{
	// if (r < l) return r + RandBig() % (l - r + 1);
	return l + RandBig() % (r - l + 1);
}

bool ExistFile(const string &file)
{
	ifstream fin(file);
	if (!fin) return false;
	fin.close();
	return true;
}

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������
