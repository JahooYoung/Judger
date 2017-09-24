// Judger.cpp: 定义控制台应用程序的入口点。
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
	int acCnt = 0;
	while (true)
	{
		cout << "生成数据中..." << endl;
		if (!inGener->GeneratorInput())
		{
			ShowJudger();
			cout << "生成数据失败，按任意键重新生成" << endl;
			WaitAKey();
			continue;
		}

	ReCompare:
		cout << "正确程序 " << force->ProgramName() << " ---> ";
		if (force->Run())
			cout << force->LastRunningTime() << "ms\n";
		CheckFile(outputFile);
		CopyFile(outputFile.c_str(), stdOutput.c_str(), false);
		DeleteFile(outputFile.c_str());

		cout << "我的程序 " << prog->ProgramName() << " ---> ";
		if (prog->Run())
			cout << prog->LastRunningTime() << "ms\n";
		CheckFile(outputFile);

		while (!comp->Compare())
		{
			ShowJudger();
			cout << "-------i键打开输入文件，c键重新运行程序并比较，n键跳过此组，Esc键退出-------" << endl;
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
				cout << "----------------------------Enter确认----------------------------" << endl;
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

		cout << "结果相符^_^" << endl;
		acCnt++;
		cout << "连续Accepted次数: " << acCnt << endl << endl;

		if (keys['p'])
		{
			keys['p'] = false;
			cout << "正确程序 " << force->ProgramName()
				<< " 平均时间：" << force->AverageRunningTime() << "ms" << endl;
			cout << "我的程序 " << prog->ProgramName()
				<< " 平均时间：" << prog->AverageRunningTime() << "ms" << endl;
			cout << "-----------------暂停，Esc退出，任意键继续--------------------" << endl << endl;
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

