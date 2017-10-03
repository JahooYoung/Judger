// Judger.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "inputGenerator.h"
#include "windowMessage.h"
#include "comparer.h"
#include "program.h"
#include "fileInfo.h"
#include <iomanip>

const string stdOutput = "CorrectOutput.txt";

string inputFile, outputFile;
Program *prog, *force;
InputGenerator *inGener;
Comparer *comp;

void Init()
{
	ChangeTitle("初始化中。。。");
	srand((unsigned)time(NULL));
	InitWindow();
	InitFile(prog, force, inputFile, outputFile);
	inGener = new InputGenerator(infoFile, inputFile);
	comp = new Comparer(infoFile, outputFile, stdOutput);
}

void Judge()
{
	ChangeTitle("Judge! P键暂停");
	string openInput = "start notepad " + inputFile;
	cout << fixed << setprecision(2);
	int acCnt = 0;
	while (true)
	{
		cout << "生成输入中..." << endl;
		if (!inGener->GeneratorInput())
		{
			ShowJudger();
			cout << "生成数据失败，按任意键重新生成" << endl;
			WaitAKey();
			continue;
		}

		cout << "正确程序 " << force->ProgramName() << " ---> ";
		if (force->Run())
			cout << force->LastRunningTime() << "ms | "
			<< force->LastRunningMemory() << "Mb" << endl;
		CheckFile(outputFile);
		CopyFile(outputFile.c_str(), stdOutput.c_str(), false);
		DeleteFile(outputFile.c_str());

	ReCompare:
		cout << "你的程序 " << prog->ProgramName() << " ---> ";
		if (prog->Run())
			cout << prog->LastRunningTime() << "ms | "
			<< prog->LastRunningMemory() << "Mb" << endl;
		CheckFile(outputFile);

		if (!comp->Compare())
		{
			ShowJudger();
			acCnt = 0;
			keyPress['p'] = false;
			int key;
			while (true)
			{
				cout << "-------i键打开输入文件，c键重新运行你的程序并比较，n键跳过此组，Esc键退出-------" << endl << endl;
				for (key = WaitAKey(); key != 'c' && key != 'n'; key = WaitAKey())
					if (key == 'i') system(openInput.c_str());
				if (key == 'c') goto ReCompare;
				else {
					cout << "-------------------------!!-Enter确认-!!----------------------------" << endl << endl;
					if (WaitAKey() == VK_RETURN) break;
				}
			}
			if (key == 'n') continue;
		}

		cout << "结果相符^_^" << endl;
		acCnt++;
		cout << "连续Accepted次数: " << acCnt << endl << endl;

		if (keyPress['p'])
		{
			keyPress['p'] = false;
			cout << "正确程序 " << force->ProgramName()
				<< " 平均时间：" << force->AverageRunningTime() << "ms" 
				<< " 最大内存：" << force->MaxRunningMemory() << "Mb" << endl;
			cout << "你的程序 " << prog->ProgramName()
				<< " 平均时间：" << prog->AverageRunningTime() << "ms"
				<< " 最大内存：" << prog->MaxRunningMemory() << "Mb" << endl;
			cout << "-----------------暂停，Esc退出，任意键继续--------------------" << endl << endl;
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

	ChangeTitle("我在生成数据");
	CreateDirectory("./data", 0);
	int id = 0;
	for (vector<string>::iterator i = inst.begin(); i != inst.end(); ++i)
	{
		s = *i;
		int t = s.find(':');
		if (t <= 0)
		{
			cout << "下面这行makedata指令缺少冒号:" << endl
				<< s << endl << "请按任意键退出，修改正确后重新运行*_*" << endl;
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
			cout << "正在生成第 " << id << " 组数据" << endl;
			cout << "    生成输入中..." << endl;
			inGener->Process(sc, sc + len);
			if (!inGener->GeneratorInput(id))
			{
				cout << "    产生输入文件错误，任意键退出" << endl;
				WaitAKey();
				exit(0);
			}
			cout << "    运行程序 " << prog->ProgramName() << " 中..." << endl;
			if (!prog->Run())
			{
				cout << "    程序 " << prog->ProgramName() << " 运行错误，任意键退出" << endl;
				WaitAKey();
				exit(0);
			}
			s = "./data/" + prog->ProgramName() + to_string(id);
			CopyFile(inputFile.c_str(), (s + ".in").c_str(), false);
			CopyFile(outputFile.c_str(), (s + ".out").c_str(), false);
		}
		delete[] sc;
	}
	cout << "生成数据完毕，请按任意键退出" << endl;
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

