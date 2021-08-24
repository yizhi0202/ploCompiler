#include<iostream>
#include<map>
#include<algorithm>
#include<string>
#include<fstream>
using namespace std;

string instr = "";		//���뻺����
string token = "";		//�ַ����飬����Ѷ���ĵ��ַ�
char ch = ' ';			//ȫ�ֱ�������ŵ�ǰ������ַ�
int row = 0;			//��¼����λ�õ�����
int col = 0;			//��¼��������
int temp_col = 0;		//��¼����ǰ������
int index = 0;			//��¼�����λ��
bool flag = true;	    //��¼��������ǰ�����û�в�����ĸ

map<string, int> Symbol;		//��ʶ��
map<string, int> Digit;			//����
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

void initReserve()			//��ʼ��������
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

void initOperator()				//��ʼ�������
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

void initBoundary()		//��ʼ�����޷�
{
	Boundary[27] = ",";
	Boundary[28] = "(";
	Boundary[29] = ")";
	Boundary[30] = ";";
	Boundary[31] = "#";
}

void getChar()			//��ȡ�����ַ�����������
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

void getNbc()			//�������ַ�
{
	while ((ch == ' '|| ch == '\n') && index < instr.size())
	{
		getChar();
	}
}	
void concat()			//�����Ѿ�������ַ�����������ķǿ��ַ�
{
	token = token + ch;
}

bool letter()			//�ж���ĸ
{
	if ((ch >= 'A'&& ch <= 'Z')||(ch >= 'a' && ch <= 'z'))
	{
		return true;
	}
	else return false;
}

bool digit()			//�ж�����
{
	if (ch >= '0' && ch <= '9') return true;
	else return false;
}

void retract()			//����һ������λ   ����Ҫ���ǻ��л��˵���һ��
{
	index = index - 1;
	col--;
	if (col == 0)
	{
		row--;
		col = temp_col;
	}
}
 
int reserve()			//ƥ�䱣����
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

string symbol()			    //ƥ���ʶ��
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

string constant()			//ƥ�䳣��
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

Binary error()				//����������
{
	//cout << token << " �÷��ŷǷ�������λ���ڣ�" << row + 1 << "�� " << col - token.length() + 1 << "��" << endl;
	return Binary(row , col-token.length(), "error");
}

Binary LexAnalyze()			//�ʷ���������
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
		while (letter() || digit())			//Ϊ��ĸ��������
		{
			concat();						//׷����token
			getChar();
		}
		retract();							//���˺���
		num = reserve();					//�鿴������
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
		while (digit()||letter())				//Ϊ����
		{
			if (letter()) flag = false;
			concat();
			getChar();
		}
		retract();
		if (flag)
		{
			val = constant();					//�鿴������
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
			return Binary(22, 0, "-");			//����<=����
		}
		else if (ch == '>')
		{
			concat();
			return Binary(25, 0, "-");	//����<>����
		}
		else {
			retract();
			return Binary(21, 0, "-");					//����<����
		}
		
		break;
	case'>':
		getChar();
		if (ch == '=')
		{
			concat();
			return Binary(24, 0, "-");		//����>=����
		}
		else {
			retract();
			return Binary(23, 0, "-");					//����>����
		}
		
		break;
	case':':
		concat();
		getChar();
		if (ch == '=')
		{
			concat();
			return Binary(20, 0, "-");		//����:=����
		}
		else 
		{
			retract();
			error();			//��error����Ϊ�����
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
	//��ʼ��
	initReserve();
	initBoundary();
	initOperator();
	Symbol.clear();
	Digit.clear();
	
	ifstream srcfp("source.txt");		//��ȡԴ���ָ��
	ofstream rsufp("result.txt");		//��ŷ������
	ofstream errfp("error.txt");		//��Ŵ���
	ofstream rsu1fp("result1.txt");		//��Ź��﷨�����ķ������ �����ֱ�� ���� �к�

	if (!srcfp || !rsufp || !errfp||!rsu1fp)
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
		exit(0);
	}
	string in = "";
	index = 0;
	while (!srcfp.eof())
	{
		getline(srcfp,in);
		instr = instr + in +'\n';
	}
	instr.substr(instr.size() - 1);			//ȥ��ĩβ�Ļ��з�
	srcfp.close();		//�رն�ȡ�ļ�ָ��

	Binary word(0, 0, "-");
	rsufp << "\n------------------------ʶ����-----------------------" << endl;
	while (ch != '#')
	{
		cout << "index " << index<<" "<<"instr.length "<<instr.size();
		flag = true;
		word = LexAnalyze();
		cout << word.category << " " << word.index << " " << word.value << endl;
		if (word.value != "error")
		{
			rsufp << "ʶ�𵥴�:\t(" << word.category << "," << word.value << ")" << endl;
			if (word.category > 31) rsu1fp << word.category << " " << word.value << " " << row << endl;
			else if (word.category > 0 && word.category < 16) rsu1fp << word.category << " " << Reserve[word.category] << " " << row << endl;
			else if (word.category > 15 && word.category < 27) rsu1fp << word.category << " " << Operator[word.category] << " " << row << endl;
			else rsu1fp << word.category << " " << Boundary[word.category] << " " <<row<< endl;
		}
		else
		{
			errfp << token << " �÷��ŷǷ�������λ����" << word.category <<"��"<< word.index <<"��"<< endl;
		}
	}
	rsufp.close();
	errfp.close();
	rsu1fp.close();
	return 0;

}


