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
	long hFile = 0; //�ļ����  
	struct _finddata_t fileinfo; //�ļ���Ϣ  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do {
			//�����Ŀ¼,����֮���������,�����б�
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
		fout << "//��������" << endl << "input:" << endl << endl;
	fout << "//�ļ���Ϣ" << endl
		<< "file:" << endl
		<< "//��ȷ�ĳ��� [�������ʱ��]" << endl
		<< force << endl
		<< "//��ĳ��� [�������ʱ��]" << endl
		<< prog << endl
		<< "//��������ļ���ʽ 1����input.txt��output.txt 2����"
		<< prog << ".in��" << prog << ".out" << endl
		<< style << endl;
	fout << "\n//�ȽϷ�ʽ\ncompare:\nnormal\n\
//normal ���������ıȽ�ģʽ��������ĩ�ո����β�س�\n\
//strict �ϸ�Ƚ�ģʽ���������ַ��Ƚ�\n\
//real ʵ���Ƚ�ģʽ��ÿ��ʵ�����бȽϣ�������eps�ڡ�eps��д��real����һ�У�����1e-6��" << endl;
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
// һЩ���������\n\
// ����0~2^63-1������\n\
LL Rand()\n\
{\n\
    return ((LL)(rand() & 7) << 60) | ((LL)rand() << 45) | ((LL)rand() << 30)\n\
        | (rand() << 15) | rand();\n\
}\n\
// ����l~r��int������\n\
int Rand(int l, int r)\n\
{\n\
    if (r < l) swap(l, r);\n\
    return l + Rand() % (r - l + 1);\n\
}\n\
// ����l~r��long long������\n\
LL Rand(LL l, LL r)\n\
{\n\
    if (r < l) swap(l, r);\n\
    return l + Rand() % (r - l + 1);\n\
}\n\
\n\
int main(int argc, char **argv)\n\
{\n\
	// ��Judger�İ����³�ʼ���������\n\
	if (argc > 1) srand(atoi(argv[1]));\n\
		else srand((unsigned)time(0));\n\
	int dataId = 0; // ��dataId������0�����ʾ�����������ɵ�dataId�����ݵ�����\n\
	if (argc > 2) dataId = atoi(argv[2]);\n\
	// �����ļ�\n\
    freopen(\"" << inputFile << "\", \"w\", stdout);\n\
    // �����������Ĵ���\n\
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

	cout << "��ĳ������� " << prog << " �� ����ǣ�ֱ�Ӱ�Enter��������������ĳ�����" << endl;
	string s;
	getline(cin, s);
	if (s != "" && s.find(".exe") == -1) s += ".exe";
	while (s != "" && !ExistFile(s))
	{
		cout << s << "�ƺ������ڣ�����ȷ�Ϻ�����һ�ΰ�" << endl;
		while (getline(cin, s) && s == "");
		if (s.find(".exe") == -1) s += ".exe";
	}
	if (s != "") prog = s.substr(0, s.size() - 4);

	cout << "��ȷ�ĳ������� " << force << " �� ����ǣ�ֱ�Ӱ�Enter��������������ȷ�ĳ�����" << endl;
	getline(cin, s);
	if (s != "" && s.find(".exe") == -1) s += ".exe";
	while (s != "" && !ExistFile(s))
	{
		cout << s << "�ƺ������ڣ�����ȷ�Ϻ�����һ�ΰ�" << endl;
		while (getline(cin, s) && s == "");
		if (s.find(".exe") == -1) s += ".exe";
	}
	if (s != "") force = s.substr(0, s.size() - 4);

	cout << "��������ļ���ʽ��input.txt��output.txt������"
		<< prog << ".in��" << prog << ".out�� ����ǰ�ߣ�������1�����Ǻ��ߣ�������2" << endl
		<< "��ֱ�Ӱ�Enter����Ĭ��Ϊ" << style << endl;
	getline(cin, s);
	if (s != "") style = atoi(s.c_str());

	if (!cg)
	{
		cout << "δ��������ļ����з���" << inGenerName << ".exe" << endl;
		cout << "��Ҫ����һ��ģ���� ��Y�����ǣ�N�����" << endl << endl;
		if (Answer())
		{
			WriteCustomGen(style == 1 ? "input.txt" : prog + ".in");
			cout << "�Ѿ��������ɺ�inputGen.cpp�ˣ����Ժ�������д���������ݵĳ����" << endl << endl;
		}
		else {
			cout << "��ô���Ժ���" << infoFile << "��input:��д���ݵĸ�ʽ��" << endl << endl;
			fout << "//��������" << endl << "input:" << endl << endl;
		}
	}

	fout << "//�ļ���Ϣ" << endl
		<< "file:" << endl
		<< "//��ȷ�ĳ��� [�������ʱ��]" << endl
		<< force << endl
		<< "//��ĳ��� [�������ʱ��]" << endl
		<< prog << endl
		<< "//��������ļ���ʽ 1����input.txt��output.txt 2����"
		<< prog << ".in��" << prog << ".out" << endl
		<< style << endl;

	cout << "Ȼ������Judger��ô�Ƚ���������أ�Judger�����¼��ֱȽ�ģʽ\n\
normal ���������ıȽ�ģʽ��������ĩ�ո����β�س�\n\
strict �ϸ�Ƚ�ģʽ���������ַ��Ƚ�\n\
real ʵ���Ƚ�ģʽ��ÿ��ʵ�����бȽϣ�������eps�ڡ���ѡ��ģʽeps���Ժ��������\n\
�����������ַ����е�һ������ֱ�Ӱ�Enter����Ĭ��Ϊnormal" << endl;
	getline(cin, s);
	if (s != "")
	{
		while (s != "normal" && s != "strict" && s != "real")
		{
			cout << "Judger����ʶ" << s << "����Ƚ�ģʽ�" << endl;
			while (getline(cin, s) && s == "");
		}
	}
	else s = "normal";
	fout << endl << "compare:" << endl << s << endl;
	if (s == "real")
	{
		cout << "������һ��ʵ����ʾ����������" << endl;
		double eps;
		cin >> eps;
		fout << eps << endl;
	}
	fout << "\
//normal ���������ıȽ�ģʽ��������ĩ�ո����β�س�\n\
//strict �ϸ�Ƚ�ģʽ���������ַ��Ƚ�\n\
//real ʵ���Ƚ�ģʽ��ÿ��ʵ�����бȽϣ�������eps�ڡ�eps��д��real����һ�У�����1e-6��" << endl;
	
	fout.close();

	cout << "�õģ��������Ѿ����������ٴμ��" << infoFile 
		<< "�����û�����⣬��������Judger���ɿ�ʼ��" << endl;
}

bool CheckFile(const string &file)
{
	ifstream fin(file);
	if (!fin)
	{
		cout << "ȱʧ�ļ���" << file << endl;
		if (file != infoFile)
			cout << "���ܵ�ԭ�򣺳�����������������ļ���ʽ���ԣ���������ʹ���ļ���������ȡ�" << endl;
		cout << "��������˳�������" << endl;
		WaitAKey();
		exit(0);
	}
	fin.close();
}

void Guide()
{
	cout << "\n\n��Ҫʹ�ý̳������ǣ��밴�ո�����������˳���\n\n";
	if (WaitAKey() != VK_SPACE) return;

	cout << "\
judger��һ�����ڶ��ĵĳ���\n\
judger���е�ʱ�򣬿�����p����ͣ��Esc���˳���ALT+J��С����ָ���\n\
�ĳ���ʱ�����judger������С��״̬����ô���ᵯ������\n\
��ʹ��ǰ������Ҫһ����Ϊ\"" << infoFile << "\"���ļ���\n\
����ļ���Ҫ�����������֣���������(data)���ļ�����(file)��\n\
���б�������ļ����Ʋ��֣�����:\n\n\
file:         ----> ��ʾ�ļ����Ʋ��ֵĿ�ʼ\n\
force 2000    ----> ��ȷ�����������������������ʱ�����ޣ���λms��\n\
kmp           ----> Ҫ���ĵĳ������������û�����ֱ�ʾ������ʱ������\n\
1         ----> ��������ļ�����ʽ��1��ʾ\"input.txt\"\"output.txt\"��\n\
                2��ʾ\"kmp.in\"\"kmp.out\"������ͨOI��ʽ��\n\n\
�������judger����������ɣ������п��ܳ�������Ҫ�ڶ���ǰ�ٿ�һ�¡�\n\
��Esc�˳����������������\n\n";
	WaitAKey();

	cout << "\
���������������ɲ��֡�������ֿ������" << inGenerName << "���������ļ���\n\
��������Լ����������ɳ��򣨷��ڱ�Ŀ¼�£�����Ϊ" << inGenerName << ".exe��������Ժ����ⲿ�֡�\n\
��һ��������˵������һ�����ϲ����⣬��һ�ж���n����������m������������������д��\n\
input:\n\
n(100000) m(100000)\\n\n\
��һ��\"input:\"��ʾ�������ɲ��ֵĿ�ʼ��\
�ڶ����У�n��m�Ǳ���������ֵΪ100000��\n\
'\\n'������Ϊ���з�������Ч��Ϊ�������100000���ÿո�����������С�\n\
�����������У�������������ţ������ַ������������\n\
����������ΪA..Z��a..z�����Һ���Ҫ��ֵ������ᵱ����ͨ�ַ�ֱ�������\n\n\
��������һ��n���ַ�����ʾÿ�����ĳ�ʼ�ַ�����ô��������д��\n\
[n]c('a', 'z') ;\\n\n\
����Ϊѭ����䣬��ʽ��\"[ѭ������(,ѭ������(,��ʼֵ))]ѭ����;\"������Ƕ�ס�\n\
��c��һ������ı���������ר����������ַ��ġ�\n\n\
Ȼ������n-1���ߣ���������д��\n\
[n-1, i]u(1, i) v(i+1)\\n;\n\
������ʽ��i��ѭ����������1��ʼ����Ҳ��������д��[n-1, i, 0]�������i��0��ʼ��\n\n\
��Esc�˳����������������\n\n";
	WaitAKey();
	cout << "\
�����m�����������в���1�ĸ�ʽ��\"1 u v\"����u��v�����㣻����2�ĸ�ʽ��\"2 u v w\"��u��vͬ�ϣ�wΪһ��Ȩֵ��\n\
W$(2*10^18)\n\
[m]{1 u(1, n) v(1, n)|2 u(1, n) v(1, n) w(1, W)}\\n;\n\
��һ���е�'$'��ʾ��������������Ȼ���������ò���W����ʾȨֵ�����ֵ��\n\
�������Եĸ�ֵ���֧�������������+��-��*��/����������%��ȡģ����^���˷�����<��ȡ��Сֵ����>��ȡ�ϴ�ֵ����=���ж��Ƿ���ȣ����Ϊ1������Ϊ0����\
     ����������Ϊlong long���ڡ�\n\
�ڶ��е�ѭ��������һ��ѡ����䣬��ʽ��\"{���1|���2|���3|...}\"�����ʱ�����������еȸ���ѡ��һ����ѡ�����Ҳ����Ƕ�ס�\n\n\
��֮���������������\n\
data:\n\
n(100000) m(100000)\\n\n\
[n]c('a', 'z') ;\\n\n\
[n-1, i]u(1, i) v(i+1)\\n;\n\
W$(2*10^18)\n\
[m]{1 u(1, n) v(1, n)|2 u(1, n) v(1, n) w(1, W)}\\n;\n\
\n";
	WaitAKey();
	cout << "\
Ȼ���Ǹ���JudgerӦ����ô�ȽϵĲ��֡�������ֵĿ�ʼ�Ǳ�ʶ��\"compare:\"��\n\
��һ����һ���ַ�������ֻ�������¼����е�һ����\n\
normal ���������ıȽ�ģʽ��������ĩ�ո����β�س�\n\
strict �ϸ�Ƚ�ģʽ���������ַ��Ƚ�\n\
real ʵ���Ƚ�ģʽ��ÿ��ʵ�����бȽϣ�������eps�ڡ�eps��д��real����һ�У�����1e-6��\n\
�ڶ��и��ݵ�һ�е������д��\n\n";

	cout << "��������˳���Ū��" << infoFile << "���ٴ�����judger����ʹ�á�" << endl;
	WaitAKey();
}

void InitFile(Program *&prog, Program *&force, string &inputFile, string &outputFile)
{
	if (!ExistFile(infoFile))
	{
		cout << "Enter���Զ�����" << infoFile << "���ո�����������򵼣�Esc���˳�" << endl;
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