#include <io.h>
#include<fstream>
#include <filesystem>
#include<iostream>
#include <iomanip>
#include <string>
#include <vector>
#include<Windows.h>
#include"AssSubTime.h"

using namespace std;
namespace fs = std::filesystem;

void GetFileDir(string/*Ŀ¼*/, vector<string>& /*����λ��*/, string /*��ʽ*/);
void ShowFileDir(vector<string> DATA_DIR);
string CL_FileCreate(string DATA_DIR,string DIR);
string CL_FileCreateDump(string DATA_DIR, string DIR);
void CL_FileDelDump(string DATA_DIR);
void Move_FileData(fstream&ioFile,fstream&ioFs);

void RunFsFile(fstream&, AssSubTime& T1, AssSubTime& T2, AssSubTime& T);//��һ���ļ�����
void RunNextFile(fstream&, AssSubTime& T1, AssSubTime& T2, AssSubTime& T);//��һ���ļ�����
void RunNextLine(fstream&, AssSubTime& T1, AssSubTime& T2,AssSubTime& T);//�ļ��ڲ�������ÿ�д���



//�����ڲ��Զ�����
void _SavaTime(int/*����*/, int/*ʱ�䵥λ�ź�*/, AssSubTime&/*��һ��ʱ��*/, AssSubTime&/*�ڶ���ʱ��*/, int/*д������ź�*/);
void _MoveFinLinelocation(fstream&);//�ƶ������һ�п�ʼλ��
void _FsGetTimeDataX(fstream& ioFile, string s, AssSubTime& T1, AssSubTime& T2);
void _JmpTimeData(fstream& ioFile, string& s, int& retTellp);
void _MoveSubDataStartLocation(fstream&);//�ƶ�����Ļ���ݿ鿪ʼλ��,��ͣ��ʱ��ǰ��
void _FsGetTimeData(fstream& ioFile ,AssSubTime &T,AssSubTime &T1);
void _GetTimeData(fstream&, AssSubTime &T1, AssSubTime &T2);
void _Read_in(fstream &ioFile, AssSubTime &T1, AssSubTime &T2, AssSubTime &T);

int T_h = 0, T_m = 0;
double T_s = 0.00;
int main(){
   //fstream����
   fstream ioFile,ioFs;
   //ʱ�����
   AssSubTime T1, T2, T;
   //Ŀ¼
   string DIR;

   cout << "������Ŀ¼��"; cin >> DIR;
   cout << endl;
   if (!(fs::exists(DIR) && fs::is_directory(DIR))) { cout << "��������ȷ���ļ�Ŀ¼"<<endl; return -1; }
	 vector<string> DATA_DIR;
	 GetFileDir(DIR, DATA_DIR, ".ass");
	 ShowFileDir(DATA_DIR);
	 cout << endl << "���ʲôҲû��ʾ������Ŀ¼��û�� .ASS �ļ�" <<endl;
   //ѡ��Ҫ���еĵ�һ���ļ�����ȡT 
    int P;
	 cout << "��ѡ���һ���ļ�����ǰ�����ţ��� ";
	 cin >> P;
	 P = P - 1;
	 string CL_DIR= CL_FileCreate(DATA_DIR[P], DIR);//��������ļ���
	 ioFile.open(CL_DIR, ios::in | ios::out);
	 if (!ioFile.is_open()) { 
	 cout << "���ļ�·��" << DATA_DIR[P] << "ʧ��" << endl;
	 return -1;
	 }
	 RunFsFile(ioFile,T1,  T2, T);
	 ioFile.close();
	 cout << endl << "�ѹر���һ���ļ������Ѿ���ȡ��λ����������ִ����һ��" << endl
		 << "��һ���ļ�����ǣ�" << P + 1 << endl;
	 ioFs.open(CL_DIR, ios::app);
	 if (!ioFs.is_open()) {
		 cout << "���ļ�·��" << CL_DIR << "ʧ��" << endl;
		 return -1;
	 }
	//ѡ����һ���ļ�������
	 for (int i = 1; i < DATA_DIR.size(); i++)
	 {
		 ShowFileDir(DATA_DIR);
		 cout  << "\n��ѡ����һ���ļ�����ǰ������,������Χ��ǿ���˳����� \n";
		 cin >> P;
		 if (P<1 || P>DATA_DIR.size()) { cout << "�����Ϸ�Χ��ǿ���˳�\n" ; ioFile.close(); break; }
		 P = P - 1;
		 string Dump_DIR= CL_FileCreateDump(DATA_DIR[P],DIR);
		 ioFile.open(Dump_DIR, ios::in | ios::out);
		 if (!ioFile.is_open()) {
			 cout << "���ļ�" << DATA_DIR[P] << "ʧ��" << endl;
			 return -1;
		 }
		 RunNextFile(ioFile, T1, T2, T);
		 Move_FileData(ioFile, ioFs);
		 ioFile.close();
		 CL_FileDelDump(Dump_DIR);
		 cout << endl << "�ѹر���һ���ļ������Ѿ���ȡ��λ����������ִ����һ��" << endl 
			 <<"��һ���ļ�����ǣ�"<<P+1 << endl
			 << "����·��Ϊ��" << DATA_DIR[P] << "\n";
	 }
	 cout << "�Ѿ������������" << endl;
	 ioFs.close();
	 Sleep(50000);
	return 0;
 }

//path/to/stable-diffusion-webui/

void GetFileDir(string path, vector<string>& files, string fileType)
{
	intptr_t  hFile = 0;// �ļ����
	struct _finddata_t fileInfo;// �ļ���Ϣ
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + fileType).c_str(), &fileInfo)) != -1) {
		do {
			// �����ļ���ȫ·��
			files.push_back(p.assign(path).append("\\").append(fileInfo.name));
		} 
		while (_findnext(hFile, &fileInfo) == 0);  //Ѱ����һ�����ɹ�����0������-1

		_findclose(hFile);
	}
}
void ShowFileDir(vector<string> DATA_DIR) {
 for (int i = 0; i < DATA_DIR.size(); ++i)
	{
		cout <<i+1<<":    " << DATA_DIR[i] << endl;
	}
}
string CL_FileCreate(string DATA_DIR,string DIR) {
	string retDIR = DIR + "\\�ϳɺ���ļ�.ass";
	int i=0;
	if (fs::exists(retDIR)) {
		cout << "���ִ�����һ�εĹ����ļ�������1��ɾȥ�������룺"; cin >> i;
		if (i != 1) { cout << "����ɾȥ��������ԭ���Ĺ����ļ�" << endl; return 0; }
		fs::remove(retDIR);
	}
	fs::copy(DATA_DIR, retDIR);
	return retDIR;
}
string CL_FileCreateDump(string DATA_DIR, string DIR) {
	string retDIR = DIR + "\\dump.ass";
	int i = 0;
	if (fs::exists(retDIR)) {
		cout << "���ִ�����һ�εĹ����ļ�������1��ɾȥ�������룺"; cin >> i;
		if (i != 1) { cout << "����ɾȥ��������ԭ���Ĺ����ļ�" << endl; return 0; }
		fs::remove(retDIR);
	}
	fs::copy(DATA_DIR, retDIR);
	return retDIR;
}
void CL_FileDelDump(string DATA_DIR) {
	fs::remove(DATA_DIR);
}
void Move_FileData(fstream& ioFile, fstream& ioFs) {
	string str;
	ioFile.seekp(ios::beg);
	do {
		getline(ioFile, str);
	} while (str != "[Events]");
	ioFile.seekp(ioFile.tellg());
	int i = 0, before = 0;
	getline(ioFile, str);
	before = ioFile.tellp();
	while (1)
	{
		if (ioFile.peek() == 'D' || ioFile.peek() == 'C')break;
		else ioFile.seekp(1, ios::cur);
	}
	ioFile.seekp(before);
	while (getline(ioFile,str))
	{
		ioFs << str << "\n";
	}
	cout << "�����ƶ��ɹ�" << endl;
}

void RunFsFile(fstream& ioFile, AssSubTime& T1, AssSubTime& T2, AssSubTime& T) {
	_MoveFinLinelocation(ioFile);
	cout << "�ѵ���ĩ�����һ�п�ͷ"<<endl;
	_FsGetTimeData(ioFile, T, T1);
}
void RunNextFile(fstream& ioFile, AssSubTime& T1, AssSubTime& T2, AssSubTime& T) {
	_MoveSubDataStartLocation(ioFile);
	cout << "�Ѿ��ƶ�����Ļ����λ��" << endl;
	RunNextLine(ioFile,  T1,  T2, T);
}
void RunNextLine(fstream&ioFile, AssSubTime& T1, AssSubTime& T2,AssSubTime& T) {
	string s;
	
	int beline=0,i=0,End=0;
	beline = ioFile.tellp();
	ioFile.seekp(0, ios::end);
	End = ioFile.tellp();
	ioFile.seekp(beline);
	while (getline(ioFile, s))
	{
	if (ioFile.tellp()>=(End-6))
	break;
	beline = ioFile.tellp();
    _GetTimeData(ioFile, T1, T2);

	cout << "�Ѿ���ȡһ�е�ʱ��" << endl;
	_Read_in(ioFile,T1,T2,T);
	} 
	ioFile.seekp(beline+1);
	getline(ioFile, s);
	i = s.find(",", 0);
	ioFile.seekg(beline+i + 2);//����ʱ��ǰ��׼����ȡ��д��
	getline(ioFile, s);
	_FsGetTimeDataX(ioFile,s, T1, T);
	cout << "��һ���ļ�ƫ����Ϊ��";
	T.show();
	cout <<endl<< "���ļ������" << endl;
}


void _MoveSubDataStartLocation(fstream &ioFile) {
	ioFile.seekg(ios::beg);
	// ������Ļ���ݿ�
	 string str;
	 do { 
     getline(ioFile, str);
	 //cout << str << "\n";
	 } while (str != "[Events]"); 
	 ioFile.seekp(ioFile.tellg());
	 int i = 0,before=0;
	 before = ioFile.tellp();
	 getline(ioFile, str);
		 while (1)
		 {
			 if (ioFile.peek() == 'D' || ioFile.peek() == 'C')break;
			 else ioFile.seekp(1, ios::cur);
		 }
		 ioFile.seekp(before);
		 getline(ioFile, str);
		 i=str.find(",", 0);

	 ioFile.seekg(before+i+1);
}
void _FsGetTimeData(fstream& ioFile,AssSubTime &T, AssSubTime &T1) {
	string S;
	int i = 0, before = 0,X=0;
	_MoveFinLinelocation(ioFile);
	before = ioFile.tellp();
	ioFile.seekg(ioFile.tellg());
	
	getline(ioFile, S);
	cout << S << endl;
	cout << "λ�ã�" << ioFile.tellg() << endl;
	cout << "����Ƿ���ȷΪ����е����ݣ�����������1���˳�������0��";
	cin >> X ;
	cout << endl;
	if (X == 1)
	{
		i = S.find(",", 0);
		ioFile.seekg(before + i + 1);//����ʱ��ǰ��׼��д��
		getline(ioFile, S);
		_FsGetTimeDataX(ioFile, S, T1, T);
	}
	else { cout << "��ѡ���˲����������߳��ִ���" << endl; exit(-1); }
	cout << "�Ѿ���ȡ��λ����Ϊ��";
	T.show();
}
void _GetTimeData(fstream& ioFile,AssSubTime &T1, AssSubTime &T2 ) {
	string s; 
	int retTellp= ioFile.tellg();
	getline(ioFile, s);
	if (isdigit(s[0])) {//����ʱ
		retTellp = ioFile.tellg();
	}
	else _JmpTimeData(ioFile, s, retTellp);
	//cout << s << endl;
	int sum = 0, i = 0 ,BOOL=0,X=1;
	while (i < 21)
	{
		
		while (isdigit(s[i])&&s[i]!=':')//���־ͽ���ѭ��
		{
			sum = sum * 10 + s[i++] - '0';//ת����
			BOOL++;
		}
		
		//���������
		if (s[i] != '.') {
			_SavaTime(sum,BOOL, T1, T2,X);
			BOOL = 0;
			sum = 0;
		}
		//������ʱ
		while (!isdigit(s[i])) {
			if (s[i]==',')X=2;
			i++;
		}
	}
	ioFile.seekp(retTellp);
}
void _Read_in(fstream &ioFile,AssSubTime &T1, AssSubTime &T2, AssSubTime &T) {
	AssSubTime T3=T1+T;
	ioFile.seekg(ioFile.tellp());
	ioFile << setw(1) << setfill('0') << T3.get_hour()+T_h
		<< ":" << setw(2) << setfill('0') << T3.get_minute()+T_m
		<< ":" << fixed << setw(5) << setfill('0') << setprecision(2) << T3.get_sec()+T_s ;
	ioFile << ",";
	T3 = T2 + T;
	ioFile << setw(1) << setfill('0') << T3.get_hour()+T_h
		<< ":" << setw(2) << setfill('0') << T3.get_minute()+T_m
		<< ":" << fixed << setw(5) << setfill('0') << setprecision(2) << T3.get_sec()+T_s;
	cout << "д��ɹ�" << endl;
	/*string S;
	ioFile.seekg(ioFile.tellp());
	getline(ioFile, S);
	cout << endl << S << endl;*/
}
void _SavaTime(int data, int timeBool, AssSubTime& T1, AssSubTime& T2, int X) {
	if (X == 1) {
		switch (timeBool)
		{
		case 1:
			T1.set_time_hour(data);
			break;
		case 2:
			T1.set_time_minute(data);
			break;
		case 4:
			double D = (double)data;
			D = D / 100.00;
			T1.set_time_sec(D);
		}
	}
	else {
		switch (timeBool)
		{
		case 1:
			T2.set_time_hour(data);
			break;
		case 2:
			T2.set_time_minute(data);
			break;
		case 4:
			double D = (double)data;
			D = D / 100.00;
			T2.set_time_sec(D);
		}
	}
}
void _MoveFinLinelocation(fstream& ioFile) {
	 ioFile.seekg(-2,ios::end);
	// cout << "�ļ�ĩ��λ�ã�" << ioFile.tellg() << endl;
	 for (int i = 0; i < 2; i++)
	 {
		 // �鿴ǰһ���ַ��Ƿ�Ϊ�س���
		 while (ioFile.peek() != '\n')
		 {
			 ioFile.seekg(-1, ioFile.cur);
		 }
		 // �ߵ������ʾ����һ����
		 ioFile.seekg(-1, ioFile.cur);
	 }
	 ioFile.seekg(3, ioFile.cur);
}
void _FsGetTimeDataX(fstream& ioFile,string s,AssSubTime&T1, AssSubTime& T) {
	int sum = 0, i = 10, BOOL = 0, X = 2,retTellp=ioFile.tellp();
	cout << s << endl;
	while (i < 21)
	{	
		while (isdigit(s[i]) && s[i] != ':')//���־ͽ���ѭ��
		{
			sum = sum * 10 + s[i++] - '0';//ת����
			BOOL++;
		}

		//���������
		if (s[i] != '.') {
			_SavaTime(sum, BOOL, T1,T, X);
			BOOL = 0;
			sum = 0;
		}
		//������ʱ
		while (!isdigit(s[i])) {
			i++;
		}
		
	}
}
void _JmpTimeData(fstream& ioFile,string& s,int& retTellp) {
	int i = 0;
	string st;
	i = s.find(",", 0);
	ioFile.seekg(retTellp + i + 1);//����ʱ������ǰ
	retTellp = ioFile.tellp();
	getline(ioFile, s);
	cout << s << endl;
}