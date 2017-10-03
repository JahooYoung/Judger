#include "stdafx.h"
#include "fileInfo.h"
#include "program.h"
#include "windowMessage.h"
#include "inputGenerator.h"
#include <windows.h>
#include <conio.h>
#include <direct.h>
#include <io.h>

const string infoFile = "JudgeInfo.txt";

void GetFiles(string path, vector<string>& files)
{
	long hFile = 0; //文件句柄  
	struct _finddata_t fileinfo; //文件信息  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do {
			//如果是目录,迭代之。如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR)) 
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					0; //getFiles(   p.assign(path).append("/").append(fileinfo.name), files   );  
			}
			else {
				files.push_back(p./*assign(path).append("/").*/assign(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void Detect(bool &CusGenerExist, string &spro, string &sforce, int &style)
{
	int i, n = 0, al1 = 0, al2 = 0; 
	string s;
	vector<string> file; 
	ifstream fin;

	style = 1;
	CusGenerExist = false;
	GetFiles(".", file);
	for (i = file.size() - 1; i >= 0; i--)
	{
		s = file[i];
		if (s.find(".in") < s.size() || s.find(".out") < s.size()) style = 2;
		if (s.find(".exe") >= s.size()) continue;
		s = s.substr(0, s.size() - 4);
		if (s == inGenerName) { CusGenerExist = true; continue; }
		if (s == "judger" || s == "Judger") continue;
		if (s.find("force") == (LL)s.size() - 5)
		{
			sforce = s, al2 = 2;
			s = s.substr(0, s.size() - 5);
			fin.open((s + ".exe").c_str());
			if (!fin) continue;
			fin.close();
			spro = s; al1 = 2;
		}
		else if (s.find("ac") == (LL)s.size() - 2)
		{
			sforce = s, al2 = 2;
			s = s.substr(0, s.size() - 2);
			fin.open((s + ".exe").c_str());
			if (!fin) continue;
			fin.close();
			spro = s; al1 = 2;
		}
		else {
			if (!al1 || al1 != 2 && s.size() < spro.size()) spro = s, al1 = 1;
			if (!al2 || al2 != 2 && s.size() > sforce.size()) sforce = s, al2 = 1;
		}
	}
}

void AutoMake()
{
	bool cg; int style;
	string prog, force;
	Detect(cg, prog, force, style);
	ofstream fout(infoFile);
	if (!cg)
		fout << "//生成输入" << endl << "input:" << endl << endl;
	fout << "//文件信息" << endl
		<< "file:" << endl
		<< "//正确的程序 [最大运行时间]" << endl
		<< force << endl
		<< "//你的程序 [最大运行时间]" << endl
		<< prog << endl
		<< "//输入输出文件格式 1代表input.txt和output.txt 2代表"
		<< prog << ".in和" << prog << ".out" << endl
		<< style << endl;
	fout << "\n//比较方式\ncompare:\nnormal\n\
//normal 代表正常的比较模式，忽略行末空格和文尾回车\n\
//strict 严格比较模式，进行逐字符比较\n\
//real 实数比较模式，每个实数进行比较，精度在eps内。eps请写在real的下一行，例如1e-6。" << endl;
	fout.close();
}

void WriteCustomGen(string inputFile)
{
	ofstream fout(inGenerName + ".cpp");
	fout << "\
#include <iostream>\n\
#include <fstream>\n\
#include <algorithm>\n\
#include <cstdio>\n\
#include <cstdlib>\n\
#include <cstring>\n\
#include <ctime>\n\
#include <cmath>\n\
#include <vector>\n\
using namespace std;\n\
typedef long long LL;\n\
\n\
// 一些随机数工具\n\
// 生成0~2^63-1的整数\n\
LL Rand()\n\
{\n\
    return ((LL)(rand() & 7) << 60) | ((LL)rand() << 45) | ((LL)rand() << 30)\n\
        | (rand() << 15) | rand();\n\
}\n\
// 生成l~r的int的整数\n\
int Rand(int l, int r)\n\
{\n\
    if (r < l) swap(l, r);\n\
    return l + Rand() % (r - l + 1);\n\
}\n\
// 生成l~r的long long的整数\n\
LL Rand(LL l, LL r)\n\
{\n\
    if (r < l) swap(l, r);\n\
    return l + Rand() % (r - l + 1);\n\
}\n\
\n\
int main(int argc, char **argv)\n\
{\n\
	// 在Judger的帮助下初始化随机种子\n\
	if (argc > 1) srand(atoi(argv[1]));\n\
		else srand((unsigned)time(0));\n\
	// 设置文件\n\
    freopen(\"" << inputFile << "\", \"w\", stdout);\n\
    // 在下面完成你的代码\n\
    \n\
    return 0;\n\
}\n";
	fout.close();
}

bool Answer()
{
	int key = WaitAKey();
	while (key != 'y' && key != 'n') key = WaitAKey();
	return key == 'y';
}

bool ExistFile(const string &file)
{
	ifstream fin(file);
	if (!fin) return false;
	fin.close();
	return true;
}

void GenerateGuide()
{
	bool cg; int style;
	string prog, force;
	Detect(cg, prog, force, style);
	ofstream fout(infoFile);

	cout << "你的程序名是 " << prog << " 吗？ 如果是，直接按Enter，否则请输入你的程序名" << endl;
	string s;
	getline(cin, s);
	if (s != "" && s.find(".exe") == -1) s += ".exe";
	while (s != "" && !ExistFile(s))
	{
		cout << s << "似乎不存在，请你确认后再填一次吧" << endl;
		while (getline(cin, s) && s == "");
		if (s.find(".exe") == -1) s += ".exe";
	}
	if (s != "") prog = s.substr(0, s.size() - 4);

	cout << "正确的程序名是 " << force << " 吗？ 如果是，直接按Enter，否则请输入正确的程序名" << endl;
	getline(cin, s);
	if (s != "" && s.find(".exe") == -1) s += ".exe";
	while (s != "" && !ExistFile(s))
	{
		cout << s << "似乎不存在，请你确认后再填一次吧" << endl;
		while (getline(cin, s) && s == "");
		if (s.find(".exe") == -1) s += ".exe";
	}
	if (s != "") force = s.substr(0, s.size() - 4);

	cout << "输入输出文件格式是input.txt和output.txt，还是"
		<< prog << ".in和" << prog << ".out？ 若是前者，请输入1，若是后者，请输入2" << endl
		<< "若直接按Enter，则默认为" << style << endl;
	getline(cin, s);
	if (s != "") style = atoi(s.c_str());

	if (!cg)
	{
		cout << "未能在你的文件夹中发现" << inGenerName << ".exe" << endl;
		cout << "需要给你一个模板吗？ 按Y键则是，N键则否" << endl << endl;
		if (Answer())
		{
			WriteCustomGen(style == 1 ? "input.txt" : prog + ".in");
			cout << "已经把你生成好inputGen.cpp了，请稍后在里面写好生成数据的程序吧" << endl << endl;
		}
		else {
			cout << "那么请稍后在" << infoFile << "的input:后写数据的格式吧" << endl << endl;
			fout << "//生成输入" << endl << "input:" << endl << endl;
		}
	}

	fout << "//文件信息" << endl
		<< "file:" << endl
		<< "//正确的程序 [最大运行时间]" << endl
		<< force << endl
		<< "//你的程序 [最大运行时间]" << endl
		<< prog << endl
		<< "//输入输出文件格式 1代表input.txt和output.txt 2代表"
		<< prog << ".in和" << prog << ".out" << endl
		<< style << endl;

	cout << "然后你想Judger怎么比较两个输出呢？Judger有以下几种比较模式\n\
normal 代表正常的比较模式，忽略行末空格和文尾回车\n\
strict 严格比较模式，进行逐字符比较\n\
real 实数比较模式，每个实数进行比较，精度在eps内。若选此模式eps会稍后叫你输入\n\
请输入上面字符串中的一个，若直接按Enter，则默认为normal" << endl;
	getline(cin, s);
	if (s != "")
	{
		while (s != "normal" && s != "strict" && s != "real")
		{
			cout << "Judger不认识" << s << "这个比较模式喔" << endl;
			while (getline(cin, s) && s == "");
		}
	}
	else s = "normal";
	fout << endl << "compare:" << endl << s << endl;
	if (s == "real")
	{
		cout << "请输入一个实数表示最大允许误差" << endl;
		double eps;
		cin >> eps;
		fout << eps << endl;
	}
	fout << "\
//normal 代表正常的比较模式，忽略行末空格和文尾回车\n\
//strict 严格比较模式，进行逐字符比较\n\
//real 实数比较模式，每个实数进行比较，精度在eps内。eps请写在real的下一行，例如1e-6。" << endl;
	
	fout.close();

	cout << "好的！生成向导已经结束！请再次检查" << infoFile 
		<< "，如果没有问题，重新运行Judger即可开始！" << endl;
}

bool CheckFile(const string &file)
{
	ifstream fin(file);
	if (!fin)
	{
		cout << "缺失文件：" << file << endl;
		if (file != infoFile)
			cout << "可能的原因：程序名错误，输入输出文件格式不对，程序忘记使用文件输入输出等。" << endl;
		cout << "按任意键退出。。。" << endl;
		WaitAKey();
		exit(0);
	}
	fin.close();
}

void Guide()
{
	cout << "\n\n需要使用教程吗？如是，请按空格。如否，其他键退出。\n\n";
	if (WaitAKey() != VK_SPACE) return;

	cout << "\
judger是一个用于对拍的程序。\n\
judger运行的时候，可以用p键暂停，Esc键退出，ALT+J最小化或恢复。\n\
拍出错时，如果judger处于最小化状态，那么它会弹出来。\n\
在使用前，他需要一个名为\"" << infoFile << "\"的文件。\n\
这个文件主要包括两个部分：数据生成(data)、文件控制(file)。\n\
其中必须包含文件控制部分，比如:\n\n\
file:         ----> 表示文件控制部分的开始\n\
force 2000    ----> 正确程序名，后面的数字是运行时间上限（单位ms）\n\
kmp           ----> 要对拍的程序名，后面的没有数字表示无运行时间上限\n\
1         ----> 输入输出文件名格式。1表示\"input.txt\"\"output.txt\"。\n\
                2表示\"kmp.in\"\"kmp.out\"这种普通OI格式。\n\n\
这个部分judger会帮助你生成，但是有可能出错，所以要在对拍前再看一下。\n\
按Esc退出，其他任意键继续\n\n";
	WaitAKey();

	cout << "\
接下来是数据生成部分。这个部分可以替代" << inGenerName << "生成输入文件。\n\
如果你有自己的输入生成程序（放在本目录下，命名为" << inGenerName << ".exe），则可以忽略这部分。\n\
用一个例子来说。现有一道树上操作题，第一行读入n（点数）和m（操作数），则这样写：\n\
input:\n\
n(100000) m(100000)\\n\n\
第一行\"input:\"表示数据生成部分的开始；\
第二行中，n、m是变量，被赋值为100000；\n\
'\\n'在这里为换行符，最终效果为输出两个100000，用空格隔开，并换行。\n\
在这种语言中，除变量外和括号，其他字符会照样输出。\n\
变量名可以为A..Z和a..z，并且后面要赋值，否则会当做普通字符直接输出。\n\n\
接下来，一行n个字符，表示每个结点的初始字符。那么可以这样写：\n\
[n]c('a', 'z') ;\\n\n\
上面为循环语句，格式是\"[循环次数(,循环变量(,初始值))]循环体;\"，允许嵌套。\n\
而c是一个特殊的变量，它是专门用来输出字符的。\n\n\
然后输入n-1条边，可以这样写：\n\
[n-1, i]u(1, i) v(i+1)\\n;\n\
其中上式的i是循环变量，从1开始。你也可以这样写：[n-1, i, 0]，这代表i从0开始。\n\n\
按Esc退出，其他任意键继续\n\n";
	WaitAKey();
	cout << "\
最后是m个操作，其中操作1的格式是\"1 u v\"其中u、v代表结点；操作2的格式是\"2 u v w\"，u、v同上，w为一个权值。\n\
W$(2*10^18)\n\
[m]{1 u(1, n) v(1, n)|2 u(1, n) v(1, n) w(1, W)}\\n;\n\
第一行中的'$'表示不输出这个变量，然后我们设置参数W，表示权值的最大值。\n\
这种语言的赋值语句支持如下运算符：+、-、*、/（整除）、%（取模）、^（乘方）、<（取较小值）、>（取较大值）、=（判断是否相等，相等为1，不等为0）。\
     并且运算数为long long以内。\n\
第二行的循环体中有一个选择语句，格式是\"{情况1|情况2|情况3|...}\"，输出时会从所有情况中等概率选择一个。选择语句也允许嵌套。\n\n\
总之，最后它长这样：\n\
data:\n\
n(100000) m(100000)\\n\n\
[n]c('a', 'z') ;\\n\n\
[n-1, i]u(1, i) v(i+1)\\n;\n\
W$(2*10^18)\n\
[m]{1 u(1, n) v(1, n)|2 u(1, n) v(1, n) w(1, W)}\\n;\n\
\n";
	WaitAKey();
	cout << "\
然后是告诉Judger应该怎么比较的部分。这个部分的开始是标识符\"compare:\"。\n\
第一行是一个字符串，它只能是以下几个中的一个：\n\
normal 代表正常的比较模式，忽略行末空格和文尾回车\n\
strict 严格比较模式，进行逐字符比较\n\
real 实数比较模式，每个实数进行比较，精度在eps内。eps请写在real的下一行，例如1e-6。\n\
第二行根据第一行的情况填写。\n\n";

	cout << "按任意键退出。弄好" << infoFile << "后再次运行judger即可使用。" << endl;
	WaitAKey();
}

void InitFile(Program *&prog, Program *&force, string &inputFile, string &outputFile)
{
	if (!ExistFile(infoFile))
	{
		cout << "Enter键自动生成" << infoFile << "，空格键运行生成向导，Esc键退出" << endl;
		if (WaitAKey() == VK_SPACE) GenerateGuide();
		else AutoMake();
		Guide();
		exit(0);
	}

	string s;

	ifstream fin(infoFile);
	while (getline(fin, s) && s != "file:");
	while (getline(fin, s) && s.find("//") == 0);
	//cerr << s << ' ' << s.find("//") << endl;
	int t = inf, sp = s.find(' ');
	if (sp != -1)
	{
		t = atoi(s.substr(sp + 1).c_str());
		s = s.substr(0, sp);
	}
	if ((sp = s.find(".exe")) != -1) s = s.substr(0, sp);
	CheckFile(s + ".exe");
	force = new Program(s, false, t);
	while (getline(fin, s) && s.find("//") == 0);
	t = inf; sp = s.find(' ');
	if (sp != -1)
	{
		t = atoi(s.substr(sp + 1).c_str());
		s = s.substr(0, sp);
	}
	if ((sp = s.find(".exe")) != -1) s = s.substr(0, sp);
	CheckFile(s + ".exe");
	string progName = s;
	prog = new Program(s, false, t);

	while (getline(fin, s) && s.find("//") == 0);
	int style = atoi(s.c_str());
	fin.close();

	switch (style)
	{
	case 1:
		inputFile = "input.txt";
		outputFile = "output.txt";
		break;
	case 2:
		inputFile = progName + ".in";
		outputFile = progName + ".out";
		break;
	}
}