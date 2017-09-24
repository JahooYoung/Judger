// Judger.cpp: �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "inputGenerator.h"
#include "windowMessage.h"
#include "comparer.h"
#include "program.h"
#include "fileInfo.h"

const string stdOutput = "CorrectOutput.txt";

string inputFile, outputFile;
Program *prog, *force;
InputGenerator *inGener;
Comparer *comp;

void Init()
{
	ChangeTitle("��ʼ���С�����");
	srand((unsigned)time(NULL));
	InitWindow();
	InitFile(prog, force, inputFile, outputFile);
	inGener = new InputGenerator(infoFile, inputFile);
	comp = new Comparer(infoFile, outputFile, stdOutput);
}

void Judge()
{
	ChangeTitle("Judge! P����ͣ");
	string openInput = "start notepad " + inputFile;
	int acCnt = 0;
	while (true)
	{
		cout << "����������...\n";
		inGener->GeneratorInput();

	ReCompare:
		cout << "��ȷ���� " << force->ProgramName() << " ---> ";
		if (force->Run())
			cout << force->LastRunningTime() << "ms\n";
		CheckFile(outputFile);
		CopyFile(outputFile.c_str(), stdOutput.c_str(), false);
		DeleteFile(outputFile.c_str());

		cout << "�ҵĳ��� " << prog->ProgramName() << " ---> ";
		if (prog->Run())
			cout << prog->LastRunningTime() << "ms\n";
		CheckFile(outputFile);

		while (!comp->Compare())
		{
			ShowJudger();
			cout << "-------i���������ļ���c���������г��򲢱Ƚϣ�n���������飬Esc���˳�-------" << endl;
			while (!keys['c'] && !keys['n'])
			{
				WndPro();
				if (keys['i']) system(openInput.c_str()), keys['i'] = false;
			}
			keys['p'] = false;
			acCnt = 0;
			if (keys['c'])
			{
				keys['c'] = false;
				goto ReCompare;
			}
			if (keys['n'])
			{
				cout << "----------------------------Enterȷ��----------------------------" << endl;
				if (WaitAKey() == VK_RETURN) break;
				else keys['n'] = false;
			}
		}
		if (keys['n'])
		{
			cout << endl;
			keys['n'] = false;
			continue;
		}

		cout << "������^_^" << endl;
		acCnt++;
		cout << "����Accepted����: " << acCnt << endl << endl;

		if (keys['p'])
		{
			keys['p'] = false;
			cout << "��ȷ���� " << force->ProgramName()
				<< " ƽ��ʱ�䣺" << force->AverageRunningTime() << "ms" << endl;
			cout << "�ҵĳ��� " << prog->ProgramName()
				<< " ƽ��ʱ�䣺" << prog->AverageRunningTime() << "ms" << endl;
			cout << "-----------------��ͣ��Esc�˳������������--------------------" << endl << endl;
			WaitAKey();
		}
	}
}

int main()
{
	Init();
	Judge();
    return 0;
}

