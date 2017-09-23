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
		if (s == inGenerName + ".exe") { CusGenerExist = true; continue; }
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
		fout << "// ��������" << endl << "input:" << endl << endl;
	fout << "// �ļ���Ϣ" << endl
		<< "file:" << endl
		<< "// ��ȷ�ĳ���" << endl
		<< force << endl
		<< "// ��ĳ���" << endl
		<< prog << endl
		<< "// ��������ļ���ʽ 1����input.txt��output.txt 2����"
		<< prog << ".in��" << prog << ".out" << endl
		<< style << endl;
	fout.close();
}

void WriteCustomGen()
{

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
	if (!cg)
	{
		cout << "δ��������ļ����з���" << inGenerName << ".exe" << endl;
		cout << "��Ҫ����һ��ģ���� ��Y�����ǣ�N�����" << endl << endl;
		if (Answer()) WriteCustomGen();
		else {
			cout << "��ô���Ժ���" << infoFile << "��input:��д���ݵĸ�ʽ��" << endl << endl;
			fout << "// ��������" << endl << "input:" << endl << endl;
		}
	}

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
	if (s != "") prog = s;

	cout << "��ȷ�ĳ������� " << force << " �� ����ǣ�ֱ�Ӱ�Enter��������������ȷ�ĳ�����" << endl;
	getline(cin, s);
	if (s != "" && s.find(".exe") == -1) s += ".exe";
	while (s != "" && !ExistFile(s))
	{
		cout << s << "�ƺ������ڣ�����ȷ�Ϻ�����һ�ΰ�" << endl;
		while (getline(cin, s) && s == "");
		if (s.find(".exe") == -1) s += ".exe";
	}
	if (s != "") force = s;

	cout << "��������ļ���ʽ��input.txt��output.txt������"
		<< prog << ".in��" << prog << ".out�� ����ǰ�ߣ�������1�����Ǻ��ߣ�������2" << endl
		<< "��ֱ�Ӱ�Enter����Ĭ��Ϊ" << style << endl;
	getline(cin, s);
	if (s != "") style = atoi(s.c_str());

	fout << "// �ļ���Ϣ" << endl
		<< "file:" << endl
		<< "// ��ȷ�ĳ��� [�������ʱ��]" << endl
		<< force << endl
		<< "// ��ĳ��� [�������ʱ��]" << endl
		<< prog << endl
		<< "// ��������ļ���ʽ 1����input.txt��output.txt 2����"
		<< prog << ".in��" << prog << ".out" << endl
		<< style << endl;
	fout.close();

	cout << "�õģ����ٴμ��" << infoFile 
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
	cout << "\n\n��Ҫ�״�ʹ���������ǣ��밴�ո�����������˳���\n\n";
	if (getch() != VK_SPACE) return;

	cout << "\
judger��һ�����ڶ��ĵĳ���\n\
judger���е�ʱ�򣬿�����p����ͣ��Esc���˳���ALT+J��С����ָ���\n\
�ĳ���ʱ�����judger������С��״̬����ô���ᵯ������\n\
��ʹ��ǰ������Ҫһ����Ϊ\"JudgeInfo.txt\"���ļ���\n\
����ļ���Ҫ�����������֣���������(data)���ļ�����(file)��\n\
���б�������ļ����Ʋ��֣�����:\n\n\
file:         ----> ��ʾ�ļ����Ʋ��ֵĿ�ʼ\n\
force 2000    ----> ��ȷ�����������������������ʱ�����ޣ���λms��\n\
kmp           ----> Ҫ���ĵĳ������������û�����ֱ�ʾ������ʱ������\n\
1         ----> ��������ļ�����ʽ��1��ʾ\"input.txt\"\"output.txt\"��\n\
                2��ʾ\"kmp.in\"\"kmp.out\"������ͨOI��ʽ��\n\n\
�������judger����������ɣ������п��ܳ���������Ҫ�ڶ���ǰ�ٿ�һ�¡�\n\
��Esc�˳����������������\n\n";
	WaitAKey();

	cout << "\
���������������ɲ��֡�������ֿ������data���������ļ���\n\
��������Լ���data�����ڱ�Ŀ¼�£�����Ϊdata.exe��������Ժ����ⲿ�֡�\n\
��һ��������˵������һ�����ϲ����⣬��һ�ж���n����������m������������������д��\n\
data:\n\
n(100000) m(100000)\\n\n\
��һ��\"data:\"��ʾ�������ɲ��ֵĿ�ʼ��\
�ڶ����У�n��m�Ǳ���������ֵΪ100000��\n\
'\\n'������Ϊ���з�������Ч��Ϊ�������100000���ÿո�����������С�\n\
�����������У�������������ţ������ַ������������\n\
����������ΪA..Z��a..z�����Һ���Ҫ��ֵ������ᵱ����ͨ�ַ�ֱ�������\n\n\
��������һ��n���ַ�����ʾÿ�����ĳ�ʼ�ַ�����ô��������д��\n\
[n]c('a', 'z') ;\\n\n\
����Ϊѭ����䣬��ʽ��\"[ѭ������]ѭ����;\"������Ƕ�ס�\n\
��c��һ������ı���������ר����������ַ��ġ�\n\n\
Ȼ������n-1���ߣ���������д��\n\
[n-1]u(1, i) v(i+1)\\n;\n\
������ʽ��i��ѭ����������1��ʼ��Ƕ��ʱ���ڶ���ѭ����ѭ������Ϊj��������Ϊk��������ơ�\n\n\
��Esc�˳����������������\n\n";
	WaitAKey();
	cout << "\
�����m�����������в���1�ĸ�ʽ��\"1 u v\"����u��v������㣻����2�ĸ�ʽ��\"2 u v w\"��u��vͬ�ϣ�wΪһ��Ȩֵ��\n\
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
[n-1]u(1, i) v(i+1)\\n;\n\
W$(2*10^18)\n\
[m]{1 u(1, n) v(1, n)|2 u(1, n) v(1, n) w(1, W)}\\n;\n\
\n";

	cout << "��������˳���Ū��JudgeInfo.txt���ٴ�����judger����ʹ�á�" << endl;
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