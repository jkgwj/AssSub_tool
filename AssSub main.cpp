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

void GetFileDir(string/*目录*/, vector<string>& /*保存位置*/, string /*格式*/);
void ShowFileDir(vector<string> DATA_DIR);
string CL_FileCreate(string DATA_DIR,string DIR);
string CL_FileCreateDump(string DATA_DIR, string DIR);
void CL_FileDelDump(string DATA_DIR);
void Move_FileData(fstream&ioFile,fstream&ioFs);

void RunFsFile(fstream&, AssSubTime& T1, AssSubTime& T2, AssSubTime& T);//第一个文件处理
void RunNextFile(fstream&, AssSubTime& T1, AssSubTime& T2, AssSubTime& T);//下一个文件处理
void RunNextLine(fstream&, AssSubTime& T1, AssSubTime& T2,AssSubTime& T);//文件内部，负责每行处理



//函数内部自动调用
void _SavaTime(int/*数据*/, int/*时间单位信号*/, AssSubTime&/*第一个时间*/, AssSubTime&/*第二个时间*/, int/*写入对象信号*/);
void _MoveFinLinelocation(fstream&);//移动到最后一行开始位置
void _FsGetTimeDataX(fstream& ioFile, string s, AssSubTime& T1, AssSubTime& T2);
void _JmpTimeData(fstream& ioFile, string& s, int& retTellp);
void _MoveSubDataStartLocation(fstream&);//移动到字幕数据块开始位置,并停在时间前面
void _FsGetTimeData(fstream& ioFile ,AssSubTime &T,AssSubTime &T1);
void _GetTimeData(fstream&, AssSubTime &T1, AssSubTime &T2);
void _Read_in(fstream &ioFile, AssSubTime &T1, AssSubTime &T2, AssSubTime &T);

int T_h = 0, T_m = 0;
double T_s = 0.00;
int main(){
   //fstream对象
   fstream ioFile,ioFs;
   //时间对象
   AssSubTime T1, T2, T;
   //目录
   string DIR;

   cout << "请输入目录："; cin >> DIR;
   cout << endl;
   if (!(fs::exists(DIR) && fs::is_directory(DIR))) { cout << "请输入正确的文件目录"<<endl; return -1; }
	 vector<string> DATA_DIR;
	 GetFileDir(DIR, DATA_DIR, ".ass");
	 ShowFileDir(DATA_DIR);
	 cout << endl << "如果什么也没显示，可能目录下没有 .ASS 文件" <<endl;
   //选择要运行的第一个文件，获取T 
    int P;
	 cout << "请选择第一个文件（填前面的序号）： ";
	 cin >> P;
	 P = P - 1;
	 string CL_DIR= CL_FileCreate(DATA_DIR[P], DIR);//最后的输出文件夹
	 ioFile.open(CL_DIR, ios::in | ios::out);
	 if (!ioFile.is_open()) { 
	 cout << "打开文件路径" << DATA_DIR[P] << "失败" << endl;
	 return -1;
	 }
	 RunFsFile(ioFile,T1,  T2, T);
	 ioFile.close();
	 cout << endl << "已关闭上一个文件，并已经获取了位移量，可以执行下一步" << endl
		 << "上一个文件序号是：" << P + 1 << endl;
	 ioFs.open(CL_DIR, ios::app);
	 if (!ioFs.is_open()) {
		 cout << "打开文件路径" << CL_DIR << "失败" << endl;
		 return -1;
	 }
	//选择下一个文件并运行
	 for (int i = 1; i < DATA_DIR.size(); i++)
	 {
		 ShowFileDir(DATA_DIR);
		 cout  << "\n请选择下一个文件（填前面的序号,超出范围可强制退出）： \n";
		 cin >> P;
		 if (P<1 || P>DATA_DIR.size()) { cout << "不符合范围，强制退出\n" ; ioFile.close(); break; }
		 P = P - 1;
		 string Dump_DIR= CL_FileCreateDump(DATA_DIR[P],DIR);
		 ioFile.open(Dump_DIR, ios::in | ios::out);
		 if (!ioFile.is_open()) {
			 cout << "打开文件" << DATA_DIR[P] << "失败" << endl;
			 return -1;
		 }
		 RunNextFile(ioFile, T1, T2, T);
		 Move_FileData(ioFile, ioFs);
		 ioFile.close();
		 CL_FileDelDump(Dump_DIR);
		 cout << endl << "已关闭上一个文件，并已经获取了位移量，可以执行下一步" << endl 
			 <<"上一个文件序号是："<<P+1 << endl
			 << "绝对路径为：" << DATA_DIR[P] << "\n";
	 }
	 cout << "已经帮你结束进程" << endl;
	 ioFs.close();
	 Sleep(50000);
	return 0;
 }

//path/to/stable-diffusion-webui/

void GetFileDir(string path, vector<string>& files, string fileType)
{
	intptr_t  hFile = 0;// 文件句柄
	struct _finddata_t fileInfo;// 文件信息
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + fileType).c_str(), &fileInfo)) != -1) {
		do {
			// 保存文件的全路径
			files.push_back(p.assign(path).append("\\").append(fileInfo.name));
		} 
		while (_findnext(hFile, &fileInfo) == 0);  //寻找下一个，成功返回0，否则-1

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
	string retDIR = DIR + "\\合成后的文件.ass";
	int i=0;
	if (fs::exists(retDIR)) {
		cout << "发现存在上一次的工作文件，输入1则删去，请输入："; cin >> i;
		if (i != 1) { cout << "请先删去或重命名原来的工作文件" << endl; return 0; }
		fs::remove(retDIR);
	}
	fs::copy(DATA_DIR, retDIR);
	return retDIR;
}
string CL_FileCreateDump(string DATA_DIR, string DIR) {
	string retDIR = DIR + "\\dump.ass";
	int i = 0;
	if (fs::exists(retDIR)) {
		cout << "发现存在上一次的工作文件，输入1则删去，请输入："; cin >> i;
		if (i != 1) { cout << "请先删去或重命名原来的工作文件" << endl; return 0; }
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
	cout << "数据移动成功" << endl;
}

void RunFsFile(fstream& ioFile, AssSubTime& T1, AssSubTime& T2, AssSubTime& T) {
	_MoveFinLinelocation(ioFile);
	cout << "已到达末端最后一行开头"<<endl;
	_FsGetTimeData(ioFile, T, T1);
}
void RunNextFile(fstream& ioFile, AssSubTime& T1, AssSubTime& T2, AssSubTime& T) {
	_MoveSubDataStartLocation(ioFile);
	cout << "已经移动到字幕数据位置" << endl;
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

	cout << "已经获取一行的时间" << endl;
	_Read_in(ioFile,T1,T2,T);
	} 
	ioFile.seekp(beline+1);
	getline(ioFile, s);
	i = s.find(",", 0);
	ioFile.seekg(beline+i + 2);//跳回时间前，准备读取和写入
	getline(ioFile, s);
	_FsGetTimeDataX(ioFile,s, T1, T);
	cout << "下一个文件偏移量为：";
	T.show();
	cout <<endl<< "该文件已完成" << endl;
}


void _MoveSubDataStartLocation(fstream &ioFile) {
	ioFile.seekg(ios::beg);
	// 查找字幕数据块
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
	cout << "位置：" << ioFile.tellg() << endl;
	cout << "检查是否正确为最后行的数据，继续请输入1，退出请输入0：";
	cin >> X ;
	cout << endl;
	if (X == 1)
	{
		i = S.find(",", 0);
		ioFile.seekg(before + i + 1);//跳到时间前，准备写入
		getline(ioFile, S);
		_FsGetTimeDataX(ioFile, S, T1, T);
	}
	else { cout << "你选择了不继续，或者出现错误" << endl; exit(-1); }
	cout << "已经获取的位移量为：";
	T.show();
}
void _GetTimeData(fstream& ioFile,AssSubTime &T1, AssSubTime &T2 ) {
	string s; 
	int retTellp= ioFile.tellg();
	getline(ioFile, s);
	if (isdigit(s[0])) {//数字时
		retTellp = ioFile.tellg();
	}
	else _JmpTimeData(ioFile, s, retTellp);
	//cout << s << endl;
	int sum = 0, i = 0 ,BOOL=0,X=1;
	while (i < 21)
	{
		
		while (isdigit(s[i])&&s[i]!=':')//数字就进入循环
		{
			sum = sum * 10 + s[i++] - '0';//转整形
			BOOL++;
		}
		
		//浮点数标记
		if (s[i] != '.') {
			_SavaTime(sum,BOOL, T1, T2,X);
			BOOL = 0;
			sum = 0;
		}
		//非数字时
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
	cout << "写入成功" << endl;
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
	// cout << "文件末端位置：" << ioFile.tellg() << endl;
	 for (int i = 0; i < 2; i++)
	 {
		 // 查看前一个字符是否为回车符
		 while (ioFile.peek() != '\n')
		 {
			 ioFile.seekg(-1, ioFile.cur);
		 }
		 // 走到这里表示跳过一行了
		 ioFile.seekg(-1, ioFile.cur);
	 }
	 ioFile.seekg(3, ioFile.cur);
}
void _FsGetTimeDataX(fstream& ioFile,string s,AssSubTime&T1, AssSubTime& T) {
	int sum = 0, i = 10, BOOL = 0, X = 2,retTellp=ioFile.tellp();
	cout << s << endl;
	while (i < 21)
	{	
		while (isdigit(s[i]) && s[i] != ':')//数字就进入循环
		{
			sum = sum * 10 + s[i++] - '0';//转整形
			BOOL++;
		}

		//浮点数标记
		if (s[i] != '.') {
			_SavaTime(sum, BOOL, T1,T, X);
			BOOL = 0;
			sum = 0;
		}
		//非数字时
		while (!isdigit(s[i])) {
			i++;
		}
		
	}
}
void _JmpTimeData(fstream& ioFile,string& s,int& retTellp) {
	int i = 0;
	string st;
	i = s.find(",", 0);
	ioFile.seekg(retTellp + i + 1);//跳到时间数据前
	retTellp = ioFile.tellp();
	getline(ioFile, s);
	cout << s << endl;
}