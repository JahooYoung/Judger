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
		cout << "����������..." << endl;
		if (!inGener->GeneratorInput())
		{
			ShowJudger();
			cout << "��������ʧ�ܣ����������������" << endl;
			WaitAKey();
			continue;
		}

		cout << "��ȷ���� " << force->ProgramName() << " ---> ";
		if (force->Run())
			cout << force->LastRunningTime() << "ms\n";
		CheckFile(outputFile);
		CopyFile(outputFile.c_str(), stdOutput.c_str(), false);
		DeleteFile(outputFile.c_str());

	ReCompare:
		cout << "��ĳ��� " << prog->ProgramName() << " ---> ";
		if (prog->Run())
			cout << prog->LastRunningTime() << "ms\n";
		CheckFile(outputFile);

		if (!comp->Compare())
		{
			ShowJudger();
			acCnt = 0;
			keyPress['p'] = false;
			int key;
			while (true)
			{
				cout << "-------i���������ļ���c������������ĳ��򲢱Ƚϣ�n���������飬Esc���˳�-------" << endl << endl;
				for (key = WaitAKey(); key != 'c' && key != 'n'; key = WaitAKey())
					if (key == 'i') system(openInput.c_str());
				if (key == 'c') goto ReCompare;
				else {
					cout << "-------------------------!!-Enterȷ��-!!----------------------------" << endl << endl;
					if (WaitAKey() == VK_RETURN) break;
				}
			}
			if (key == 'n') continue;
		}

		cout << "������^_^" << endl;
		acCnt++;
		cout << "����Accepted����: " << acCnt << endl << endl;

		if (keyPress['p'])
		{
			keyPress['p'] = false;
			cout << "��ȷ���� " << force->ProgramName()
				<< " ƽ��ʱ�䣺" << force->AverageRunningTime() << "ms" << endl;
			cout << "��ĳ��� " << prog->ProgramName()
				<< " ƽ��ʱ�䣺" << prog->AverageRunningTime() << "ms" << endl;
			cout << "-----------------��ͣ��Esc�˳������������--------------------" << endl << endl;
			WaitAKey();
		}
	}
}

bool MakeData()
{
	string s;
	vector<string> inst;
	ifstream fin(infoFile);
	for (fin >> s; !fin.eof() && s != "makedata:"; fin >> s);
	if (s != "makedata:")
	{
		fin.close();
		return false;
	}
	getline(fin, s);
	while (getline(fin, s) && !IsControlString(s))
		if (s.find("//") != 0 && s != "") inst.push_back(s);
	fin.close();

	ChangeTitle("������������");
	CreateDirectory("./data", 0);
	int id = 0;
	for (vector<string>::iterator i = inst.begin(); i != inst.end(); ++i)
	{
		s = *i;
		int t = s.find(':');
		if (t <= 0)
		{
			cout << "��������makedataָ��ȱ��ð��:" << endl
				<< s << endl << "�밴������˳����޸���ȷ����������*_*" << endl;
			WaitAKey();
			exit(0);
		}
		int cnt = atoi(s.substr(0, t).c_str());
		s.substr(t + 1);
		char *sc = new char[s.size() + 5];
		strcpy(sc, s.c_str());
		int len = strlen(sc);
		//cerr << cnt << ':' << s << endl;
		while (cnt--)
		{
			id++;
			cout << "�������ɵ� " << id << " ������" << endl;
			cout << "    ����������..." << endl;
			inGener->Process(sc, sc + len);
			if (!inGener->GeneratorInput(id))
			{
				cout << "    ���������ļ�����������˳�" << endl;
				WaitAKey();
				exit(0);
			}
			cout << "    ���г��� " << prog->ProgramName() << " ��..." << endl;
			if (!prog->Run())
			{
				cout << "    ���� " << prog->ProgramName() << " ���д���������˳�" << endl;
				WaitAKey();
				exit(0);
			}
			s = "./data/" + prog->ProgramName() + to_string(id);
			CopyFile(inputFile.c_str(), (s + ".in").c_str(), false);
			CopyFile(outputFile.c_str(), (s + ".out").c_str(), false);
		}
		delete[] sc;
	}
	cout << "����������ϣ��밴������˳�" << endl;
	WaitAKey();
	return true;
}

int main()
{
	Init();
	if (MakeData()) return 0;
	Judge();
    return 0;
}

