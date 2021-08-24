#include<iostream>
#include<map>
#include<algorithm>
#include<string>
#include<fstream>
using namespace std;

string instr = "";		//输入缓冲区
string token = "";		//字符数组，存放已读入的的字符
char ch = ' ';			//全局变量，存放当前读入的字符
int row = 0;			//记录出错位置的行数
int col = 0;			//记录出错列数
int temp_col = 0;		//记录回退前的列数
int index = 0;			//记录读入的位置
bool flag = true;	    //记录数字在最前面后有没有掺杂字母

map<string, int> Symbol;		//标识符
map<string, int> Digit;			//数字
map<string, int>::iterator ite;

const int len = 16;
string Reserve[len];
string Boundary[3 * len];
string Operator[2 * len];

struct Binary {
	Binary(int c, int i, string v = "-")
	{
		category = c;
		index = i;
		value = v;
	}
	int category = 0;
	int index = 0;
	string value = "-";
};

void initReserve()			//初始化保留字
{
	Reserve[1] = "program";
	Reserve[2] = "const";
	Reserve[3] = "var";
	Reserve[4] = "procedure";
	Reserve[5] = "begin";
	Reserve[6] = "end";
	Reserve[7] = "if";
	Reserve[8] = "while";
	Reserve[9] = "call";
	Reserve[10] = "read";
	Reserve[11] = "write";
	Reserve[12] = "odd";
	Reserve[13] = "do";
	Reserve[14] = "then";
	Reserve[15] = "else";
}

void initOperator()				//初始化运算符
{
	Operator[16] = "+";
	Operator[17] = "-";
	Operator[18] = "*";
	Operator[19] = "/";
	Operator[20] = ":=";
	Operator[21] = "<";
	Operator[22] = "<=";
	Operator[23] = ">";
	Operator[24] = ">=";
	Operator[25] = "<>";
	Operator[26] = "=";
}

void initBoundary()		//初始化界限符
{
	Boundary[27] = ",";
	Boundary[28] = "(";
	Boundary[29] = ")";
	Boundary[30] = ";";
	Boundary[31] = "#";
}

void getChar()			//读取输入字符，更新行数
{
	ch = instr[index];
	index = index + 1;
	col++;
	if (ch == '\n')
	{
		row++;
		temp_col = col - 1;
		col = 0;
	}
}

void getNbc()			//跳过空字符
{
	while ((ch == ' '|| ch == '\n') && index < instr.size())
	{
		getChar();
	}
}	
void concat()			//连接已经输入的字符和现在输入的非空字符
{
	token = token + ch;
}

bool letter()			//判断字母
{
	if ((ch >= 'A'&& ch <= 'Z')||(ch >= 'a' && ch <= 'z'))
	{
		return true;
	}
	else return false;
}

bool digit()			//判断数字
{
	if (ch >= '0' && ch <= '9') return true;
	else return false;
}

void retract()			//回退一个输入位   是需要考虑换行回退到上一行
{
	index = index - 1;
	col--;
	if (col == 0)
	{
		row--;
		col = temp_col;
	}
}
 
int reserve()			//匹配保留字
{
	for (int i = 1; i < len; ++i)
	{
		if (Reserve[i] == token)
		{
			return i;
		}
	}
	return -1;
}

string symbol()			    //匹配标识符
{
	ite = Symbol.find(token);
	if (ite != Symbol.end())
	{
		return ite->first;
	}
	else {
		Symbol[token] = Symbol.size();
		return token;
	}
}

string constant()			//匹配常数
{
	ite = Digit.find(token);
	if (ite != Digit.end())
	{
		return ite->first;
	}
	else {
		Digit[token] = Digit.size();
		return token;
	}
}

Binary error()				//出错报警处理
{
	//cout << token << " 该符号非法，错误位置在：" << row + 1 << "行 " << col - token.length() + 1 << "列" << endl;
	return Binary(row , col-token.length(), "error");
}

Binary LexAnalyze()			//词法分析函数
{
	token = "";
	getChar();
	getNbc();
	string val;
	int num = -1;
	switch (ch) {
	case'a':
	case'b':
	case'c':
	case'd':
	case'e':
	case'f':
	case'g':
	case'h':
	case'i':
	case'j':
	case'k':
	case'l':
	case'm':
	case'n':
	case'o':
	case'p':
	case'r':
	case's':
	case't':
	case'u':
	case'v':
	case'w':
	case'x':
	case'y':
	case'z':
	case'A':
	case'B':
	case'C':
	case'D':
	case'E':
	case'F':
	case'G':
	case'H':
	case'I':
	case'J':
	case'K':
	case'L':
	case'M':
	case'N':
	case'O':
	case'P':
	case'Q':
	case'R':
	case'S':
	case'T':
	case'U':
	case'V':
	case'W':
	case'X':
	case'Y':
	case'Z':
		while (letter() || digit())			//为字母或者数字
		{
			concat();						//追加至token
			getChar();
		}
		retract();							//回退函数
		num = reserve();					//查看保留字
		if (num != -1)
		{
			return Binary(num, 0);
		}
		else {
			val = symbol();
			return Binary(32, Symbol[val], val);
		}
	
		break;
	case'0':
	case'1':
	case'2':
	case'3':
	case'4':
	case'5':
	case'6':
	case'7':
	case'8':
	case'9':
		while (digit()||letter())				//为数字
		{
			if (letter()) flag = false;
			concat();
			getChar();
		}
		retract();
		if (flag)
		{
			val = constant();					//查看常数表
			return Binary(33, Digit[val], val);
		}
		else
		{
			error();
		}
		break;
	case'<':
		getChar();
		if (ch == '=')
		{
			concat();
			return Binary(22, 0, "-");			//返回<=符号
		}
		else if (ch == '>')
		{
			concat();
			return Binary(25, 0, "-");	//返回<>符号
		}
		else {
			retract();
			return Binary(21, 0, "-");					//返回<符号
		}
		
		break;
	case'>':
		getChar();
		if (ch == '=')
		{
			concat();
			return Binary(24, 0, "-");		//返回>=符号
		}
		else {
			retract();
			return Binary(23, 0, "-");					//返回>符号
		}
		
		break;
	case':':
		concat();
		getChar();
		if (ch == '=')
		{
			concat();
			return Binary(20, 0, "-");		//返回:=符号
		}
		else 
		{
			retract();
			error();			//将error定义为出错符
		}
		
		break;
	case'=':
		return Binary(26, 0, "-");
		break;
	case'+':
		return Binary(16, 0, "-");
		
		break;
	case'-':
		return Binary(17, 0, "-");
		
		break;
	case'*':
		return Binary(18, 0, "-");
		
		break;
	case'/':
		return Binary(19, 0, "-");
	
		break;
	case',':
		return Binary(27, 0, "-");
		
		break;
	case'(':
		return Binary(28, 0, "-");
		
		break;
	case')':
		return Binary(29, 0, "-");
		
		break;
	case';':
		return Binary(30, 0, "-");
		
		break;
	case'#':
		return Binary(31, 0, "-");
		break;
	default:
		concat();
		while ( ch != '#')
		{
			concat();
			getChar();
			if (ch == ' ' || ch == '\n')
				break;
		}
		retract();
		error();
	}
}

int main()
{
	//初始化
	initReserve();
	initBoundary();
	initOperator();
	Symbol.clear();
	Digit.clear();
	
	ifstream srcfp("source.txt");		//读取源码的指针
	ofstream rsufp("result.txt");		//存放分析结果
	ofstream errfp("error.txt");		//存放错误
	ofstream rsu1fp("result1.txt");		//存放供语法分析的分析结果 包括种别号 单词 行号

	if (!srcfp || !rsufp || !errfp||!rsu1fp)
	{
		cout << "文件打开失败！" << endl;
		exit(0);
	}
	string in = "";
	index = 0;
	while (!srcfp.eof())
	{
		getline(srcfp,in);
		instr = instr + in +'\n';
	}
	instr.substr(instr.size() - 1);			//去除末尾的换行符
	srcfp.close();		//关闭读取文件指针

	Binary word(0, 0, "-");
	rsufp << "\n------------------------识别结果-----------------------" << endl;
	while (ch != '#')
	{
		cout << "index " << index<<" "<<"instr.length "<<instr.size();
		flag = true;
		word = LexAnalyze();
		cout << word.category << " " << word.index << " " << word.value << endl;
		if (word.value != "error")
		{
			rsufp << "识别单词:\t(" << word.category << "," << word.value << ")" << endl;
			if (word.category > 31) rsu1fp << word.category << " " << word.value << " " << row << endl;
			else if (word.category > 0 && word.category < 16) rsu1fp << word.category << " " << Reserve[word.category] << " " << row << endl;
			else if (word.category > 15 && word.category < 27) rsu1fp << word.category << " " << Operator[word.category] << " " << row << endl;
			else rsu1fp << word.category << " " << Boundary[word.category] << " " <<row<< endl;
		}
		else
		{
			errfp << token << " 该符号非法，错误位置在" << word.category <<"行"<< word.index <<"列"<< endl;
		}
	}
	rsufp.close();
	errfp.close();
	rsu1fp.close();
	return 0;

}


