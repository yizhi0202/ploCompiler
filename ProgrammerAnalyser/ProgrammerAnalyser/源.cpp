#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#define STACKSIZE 5000
using namespace std;

bool flag = true;			//��¼�����Ƿ����
int lev = 0;				//�������
int index = 0;				//���ʻ�ȡ������
int dx = 3;					//Ĭ�ϱ���ƫ����������SL DL RA


int cx2 = 0;				//��¼procedure �ĵ�ַ������ű�size��
int cnt = 0;				//ͳ�Ƶ���procedureʱ�����������
int precnt = 0;             //��¼֮ǰ���̶���ʱ�Ĳ�������

string fname = "";				//��¼��������



//���ű�
struct table {			
	string name ;		//����
	string kind ;		//����
	int value ;		    //ֵ
	int level ;			//����
	int addr;			//��ַ
	int size;			//��¼�βθ���
};

//pcode�ṹ��
struct instruction {
	string f;			//α������
	int l;				//���ֵ
	int a;				//ƫ����
};


//�����������
struct strToken {
	string kind_num;		//�ֱ��
	string token;		//����
	string row;			//�к�
};
//���������б�
vector<table> table_list;

//��ŵ�������
vector<strToken> words;

//���pcode
vector<instruction> code;

//��������б�
void insertTable(string name = "", string kind = "", int  value = 0, int level = 0, int addr = 0, int size = 0)
{
	table t;
	t.name = name;
	t.kind = kind;
	t.value = value;
	t.level = level;
	t.addr = addr;
	t.size = size;
	table_list.push_back(t);
}

void emit(string fade, int leveldiff, int addr)
{
	instruction ins;
	ins.f = fade;
	ins.l = leveldiff;
	ins.a = addr;
	code.push_back(ins);
}
//��ѯ�ڷ��ű��е��±�
int findTable(string name)
{
	vector<table>::iterator  it = table_list.begin();
	for (int i = 0; it < table_list.end(); it++,i++)
	{
		if (it->name == name) return i;
	}
	return -1;
}

//������
void Error(string row = "", string des = "")
{
	int row1 = stoi(row);
	flag = false;
	cout << "�� " << row1 + 1<< " ��" << ":" << des << endl;
}

//��������������С���Ǹ�
int min3(int a, int b, int c)
{
	int min = 0;
	if (a > b)
	{
		 min = b;
	}
	else  min = a;
	if (min > c) return c;
	else return min;
}

float similarity(string s1, string s2)
{
	int len1 = 0;
	int len2 = 0;
	len1 = s1.size();
	len2 = s2.size();
	int maxlen = 0;
	//��ȡ�����ַ���������Ǹ�
	if (len1 > len2) maxlen = len1;
	else maxlen = len2;
	//������ά����
	int **dif;
	dif = new int*[len1 + 1];
	for (int j = 0; j <= len1 + 1; j++)
	{
		dif[j] = new int[len2 + 1];
	}

	//��ʼ������
	for (int i = 0; i <= len1; i++)
	{
		dif[i][0] = i;
	}
	for (int i = 0; i <= len2; i++)
	{
		dif[0][i] = i;
	}
	//��ȡ���㲽��
	int temp;
	for (int i = 1; i <= len1; i++) 
	{
		for (int j = 1; j <= len2; j++)
		{
			if (s1.at(i - 1) == s2.at(j - 1))
			{
				temp = 0;
			}
			else {
				temp = 1;
			}
			//ȡ����ֵ����С��  
			dif[i][j] = min3(dif[i - 1][j - 1] + temp, dif[i][j - 1] + 1,
				dif[i - 1][j] + 1);
		}
	}
	float simi = 1 - (float)dif[len1][len2] / maxlen;
	return simi;
}
//���ļ����е����ݴ���vector�� 
void initWords()
{
	ifstream ifp("result1.txt");
	string temp = "";		//��ŵ�n���������ݵ��м����
	if (!ifp )
	{
		cout << "Cann't open the file !" << endl;
		exit(0);
	}
	strToken str;
	while (!ifp.eof())
	{
		ifp >> temp;
		str.kind_num = temp;
		ifp >> temp;
		str.token = temp;
		ifp >> temp;
		str.row = temp;
		words.push_back(str);
	}
}

strToken getWord()			//���������Լ�����к��ֱ����Ϣ��ȡ
{
	vector<strToken>::iterator it = words.begin();
	it = it + index;
	if (it == words.end()) exit(0);				//������βǿ���˳�
	index++;
	return *it;
}

//����ʽ�������
void block();
void const1();
void body();
void statement();
void expression();
void lexp();
void term();
void factor();


//�������prog
void prog()
{
	strToken str;
	str = getWord();
	if (str.token == "program")
	{
		str = getWord();
		if (str.kind_num == "32")			//��ʶ������Ϊ32
		{
			str = getWord();
			if (str.token == ";")
				block();
			else
			{
				Error(str.row, " ȱ�ٷֺ�;");
				index = index - 1;			//����
				block();
			}
		}
		else 
		{
			Error(str.row, " ȱ�ٳ�������");		//�ж�ǰһ���Ƿ����Ҫ��������������
			if (str.token == ";") block();
			else {
				str = getWord();
				if (str.token == ";") block();
				else {
					Error(str.row, " ȱ�ٷֺ�;");
					index = index - 1;				//������prog���򵥴ʻ���
					block();
				}
			}

		}
	}
	else 
	{
		float simi = similarity(str.token, "program");
		if (simi < 0.3) 
		{
			Error(str.row, "ȱ�ٹؼ���program");
			if (str.kind_num == "32")
			{
				str = getWord();
				if (str.token == ";")
					block();
				else
				{
					Error(str.row, " ȱ�ٷֺ�;");
					index = index - 1;
					block();
				}
			}
			else {
				Error(str.row, "ȱ�ٳ�������");
				if (str.token == ";") block();
				else {
					Error(str.row, "ȱ�ٷֺ�");
					index = index - 1;
					block();
				}
			}
		}
		else 
		{
			Error(str.row, "�ؼ���programƴд����");
			str = getWord();
			if (str.kind_num == "32")
			{
				str = getWord();
				if (str.token == ";")
					block();
				else
				{
					Error(str.row, " ȱ�ٷֺ�;");
					index = index - 1;
					block();
				}
			}
			else {
				Error(str.row, "ȱ�ٳ�������");
				if (str.token == ";") block();
				else {
					Error(str.row, "ȱ�ٷֺ�");
					index = index - 1;
					block();
				}
			}
		}
			
	}
	
}

void block()
{
	strToken str;
	str = getWord();
	dx = 3;
	int cx1;                //��¼jmpָ������ַ
	int cur_dx;				//��¼��ǰ��ı�������
	float simi = 0;			//��¼��ؼ��ֵ����ƶ�
	cx1 = code.size();		//��¼jmpָ������λ��
	emit("JMP", 0, 0);		//������ת��body����ִ�У������ַ�ȵ���������
	simi = similarity(str.token, "const");
	if (simi > 0.8)
	{
		if (str.token != "const")
		{
			Error(str.row, "�ؼ���constƴд����");
		}
		const1();
		str = getWord();
		while (str.token == ",")
		{
			const1();
			str = getWord();
		}
		if (str.token == ";")
		{
			str = getWord();
		}
		else
		{
			Error(str.row, " ȱ�ٷֺ�;");
		}
	}
	if (str.token == "var")
	{
		str = getWord();
		if (str.kind_num == "32")
		{
			insertTable(str.token, "VARIABLE", 0, lev,dx);
			dx = dx + 1;
			str = getWord();
		}
		else Error(str.row, " var����ȱ�ٱ�ʶ��");
		while (str.token == ",")
		{
			str = getWord();
			if (str.kind_num == "32")
			{
				insertTable(str.token, "VARIABLE", 0, lev,dx);
				dx = dx + 1;
				str = getWord();
			}
			else
			{
				Error(str.row, "����,����ȱ�ٷǱ�ʶ��");
				str = getWord();
			}
		}
		if (str.token == ";") str = getWord();
		else Error(str.row, " ȱ�ٷֺ�;");
	}
	simi = similarity(str.token, "procedure");

	while (simi > 0.8)			//proc->procedure <id>([<id>{,<id>}]);<block>{;<proc>}
	{
		if(str.token != "procedure")
		{
			Error(str.row, "procedureƴд����");
		}
		int count = 0;			//ͳ�Ʋ�������Ȼ�����size��
		str = getWord();
		if (str.kind_num == "32")
		{
			cx2 = table_list.size();			//��¼���ű��д��procedure��λ��
			int addr ;				//��¼���̵ĵ�ַ
			addr = code.size();
			insertTable(str.token, "PROCEDURE", dx, lev,addr);				//procedure valueֵ��洢procedure֮ǰ�����������

			
			str = getWord();
			if (str.token == "(")
			{
				str = getWord();
				if (str.kind_num == "32")
				{
					count = count + 1;		//ÿ�����������һ�������ͼ�һ
					insertTable(str.token, "VARIABLE", 0, lev, dx);
					dx = dx + 1;
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.kind_num == "32")
						{
							count = count + 1;
							insertTable(str.token, "VARIABLE", 0, lev,dx);
							dx = dx + 1;
							str = getWord();
						}
						else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
					}
					if (str.token == ")")
					{
						//��������������size��
						vector<table>::iterator it = table_list.begin();
						it = it + cx2;
						it->size = count;

						str = getWord();
					}
					else
					{
						Error(str.row, "ȱ��������");

						//��������������size��
						vector<table>::iterator it = table_list.begin();
						it = it + cx2;
						it->size = count;

						str = getWord();
					}
				}
				else if (str.token == ")")
				{
					str = getWord();
				}
				else
				{
					Error(str.row, "ȱ��������");
					str = getWord();
				}
			}
			else
			{
				Error(str.row, "ȱ��������");

				if (str.kind_num == "32" || str.token == ")")
				{
					if (str.kind_num == "32")
					{
						count = count + 1;				//����һ�������ͼ�һ
						insertTable(str.token, "VARIABLE", 0, lev, dx);
						dx = dx + 1;

						str = getWord();
						while (str.token == ",")
						{
							str = getWord();
							if (str.kind_num == "32")
							{
								count = count + 1;				//����һ�������ͼ�һ
								insertTable(str.token, "VARIABLE", 0, lev, dx);
								dx = dx + 1;

								str = getWord();
							}
							else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						}
						if (str.token == ")")
						{
							//��������������size��
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;

							str = getWord();

						}
						else
						{
							Error(str.row, "ȱ��������");
							//��������������size��
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;

							str = getWord();
						}
					}
					else if (str.token == ")") str = getWord();
				}
				else
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						count = count + 1;				//����һ�������ͼ�һ
						insertTable(str.token, "VARIABLE", 0, lev, dx);
						dx = dx + 1;

						str = getWord();
						while (str.token == ",")
						{
							str = getWord();
							if (str.kind_num == "32")
							{
								count = count + 1;				//����һ�������ͼ�һ
								insertTable(str.token, "VARIABLE", 0, lev, dx);
								dx = dx + 1;
								str = getWord();
							}
							else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						}
						if (str.token == ")")
						{
							//��������������size��
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;
							str = getWord();
						}
						else
						{
							Error(str.row, "ȱ��������");

							//��������������size��
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;
							str = getWord();
						}
					}
					else if (str.token == ")") str = getWord();
					else
					{
						Error(str.row, "ȱ��������");
						str = getWord();
					}
				}
			}
		}
		else
		{
			Error(str.row, " ȱ�ٺ�������");   
			/* ��Ϊû�к�����������ͳ�Ƶ�countҲû��ָ����size���
						vector<table>::iterator it = table_list.begin();
						it = it + cx2;
						it->size = count;
			*/
			if (str.token == "(")
			{
				str = getWord();
				if (str.kind_num == "32")
				{
					insertTable(str.token, "VARIABLE", 0, lev, dx);
					dx = dx + 1;
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.kind_num == "32")
						{
							insertTable(str.token, "VARIABLE", 0, lev, dx);
							dx = dx + 1;
							str = getWord();
						}
						else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
					}
					if (str.token == ")")
					{
						str = getWord();
					}
					else
					{
						Error(str.row, "ȱ��������");
						str = getWord();
					}
				}
				else if (str.token == ")")
				{
					str = getWord();
				}
				else
				{
					Error(str.row, "ȱ��������");
					str = getWord();
				}
			}
			else
			{

				str = getWord();
				if (str.token == "(")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						
						insertTable(str.token, "VARIABLE", 0, lev, dx);
						dx = dx + 1;
						str = getWord();
						while (str.token == ",")
						{
							str = getWord();
							if (str.kind_num == "32")
							{
								
								insertTable(str.token, "VARIABLE", 0, lev, dx);
								dx = dx + 1;
								str = getWord();
							}
							else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						}
						if (str.token == ")")
						{

							str = getWord();
						}
						else
						{
							Error(str.row, "ȱ��������");

							str = getWord();
						}
					}
					else if (str.token == ")")
					{
						str = getWord();
					}
					else
					{
						Error(str.row, "ȱ��������");
						str = getWord();
					}
				}
				else
				{
					Error(str.row, "ȱ��������");
					if (str.kind_num == "32" || str.token == ")")
					{
						if (str.kind_num == "32")
						{
							insertTable(str.token, "VARIABLE", 0, lev, dx);
							dx = dx + 1;

							str = getWord();
							while (str.token == ",")
							{
								str = getWord();
								if (str.kind_num == "32")
								{
									insertTable(str.token, "VARIABLE", 0, lev, dx);
									dx = dx + 1;

									str = getWord();
								}
								else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
							}
							if (str.token == ")")
							{
								str = getWord();

							}
							else
							{
								Error(str.row, "ȱ��������");

								str = getWord();
							}
						}
						else if (str.token == ")") str = getWord();
					}
					else
					{
						str = getWord();
						if (str.kind_num == "32")
						{
							insertTable(str.token, "VARIABLE", 0, lev, dx);
							dx = dx + 1;

							str = getWord();
							while (str.token == ",")
							{
								str = getWord();
								if (str.kind_num == "32")
								{
									insertTable(str.token, "VARIABLE", 0, lev, dx);
									dx = dx + 1;
									str = getWord();
								}
								else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
							}
							if (str.token == ")")
							{
								str = getWord();
							}
							else
							{
								Error(str.row, "ȱ��������");

								str = getWord();
							}
						}
						else if (str.token == ")") str = getWord();
						else
						{
							Error(str.row, "ȱ��������");
							str = getWord();
						}
					}
				}
			}
		}
		if (str.token == ";") {}
		else
		{
			Error(str.row, "ȱ�ٷֺ�;");
			index = index - 1;
		}
		cur_dx = dx;						//��¼��ǰ��ı�������
		lev = lev + 1;
		block();
		lev = lev - 1;
		dx = cur_dx;			//��ԭ��һ��ı�������
		str = getWord();
		if (str.token == ";")
		{
			str = getWord();
			simi = similarity(str.token, "procedure");	//�������ƶ�
		}
		else break;
			
	}
	index = index - 1;
	vector<instruction>::iterator it = code.begin();
	it = it + cx1;
	it->a = code.size();		//����jmpָ�����
	emit("INT", 0, dx);
	body();
	emit("OPR", 0, 0);			//���̵��ý������ص��õ㲢��ջ
}

void const1()
{
	strToken str = getWord();
	string value = str.token;
	int num = 0;
	if (str.kind_num == "32")
	{
		str = getWord();
		if(str.token ==":=")
		{
			str = getWord();
			if (str.kind_num == "33")
			{
				num = stoi(str.token);				//�洢������ֵ
				insertTable(value, "CONST", num, lev);
			}
			else
			{
				Error(str.row, ":=������Ҫ������");
				index = index - 1;
			}
		}
		else
		{
			Error(str.row, "ȱ��:=");
			if (str.kind_num != "33")
			{
				str = getWord();
				if (str.kind_num != "33")
				{
					Error(str.row, ":=������Ҫ������");
					index = index - 1;
				}
				else
				{
					num = stoi(str.token);				//�洢������ֵ
					insertTable(value, "CONST", num, lev);
				}
			}
			else
			{
				num = stoi(str.token);				//�洢������ֵ
				insertTable(value, "CONST", num, lev);
			}
		}
	}
	else
	{
		Error(str.row, "ȱ�ٱ�ʶ��");
		if (str.token == ":=")
		{
			str = getWord();
			if (str.kind_num == "33")
			{
				num = stoi(str.token);				//�洢������ֵ
				insertTable(value, "CONST", num, lev);
			}
			else
			{
				Error(str.row, ":=������Ҫ������");
				index = index - 1;
			}
		}
		else 
		{
			str = getWord();
			if (str.token == ":=")
			{
				str = getWord();
				if (str.kind_num == "33")
				{
					num = stoi(str.token);				//�洢������ֵ
					insertTable(value, "CONST", num, lev);
				}
				else
				{
					Error(str.row, ":=������Ҫ������");
					index = index - 1;
				}
			}
			else 
			{
				Error(str.row, "ȱ��:=");
				if (str.kind_num != "33")
				{
					str = getWord();
					if (str.kind_num != "33")
					{
						Error(str.row, ":=������Ҫ������");
						index = index - 1;
					}
					else 
					{
						num = stoi(str.token);				//�洢������ֵ
						insertTable(value, "CONST", num, lev);
					}
				}
				else 
				{
					num = stoi(str.token);				//�洢������ֵ
					insertTable(value, "CONST", num, lev);
				}
			}
		}
	}
}
void body()
{
	strToken str = getWord();
	if (str.token != "begin")
	{
		Error(str.row, "ȱ��begin");
		index = index - 1;
	}
	statement();
	str = getWord();
	if (str.token == ";" || str.token != "end")
	{
		if (str.token == ";")
		{
			while (str.token == ";")
			{
				statement();
				str = getWord();
			}
		}
		else
		{
			Error(str.row, "ȱ�ٷֺ�;");
			statement();
		}
	}
	if (str.token != "end")
	{
		Error(str.row, "ȱ��end");
		index = index - 1;
	}
}

void statement()      //id  := exp
{
	strToken str = getWord();
	int cx1;			//��¼jmpָ������ַ
	int cx3;            //��¼jpcָ������ַ
	int idx ;				//���ű��ѯ����
	int ffloc = 0;					//����Ѱ���β�λ��formalfactor
	if (str.token == ";")
	{
		Error(str.row, ";�Ƕ����");
		str = getWord();
	}
	if (str.kind_num == "32")
	{
	    idx = findTable(str.token);
		if (idx == -1) Error(str.row, str.token + "δ����");
		else
		{
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			if (it->kind != "VARIABLE")
			{
				Error(str.row, str.token + "���Ǳ��������ܸ�ֵ��");
			}
		}
		str = getWord();
		if (str.token != ":=")
		{
			Error(str.row, "ȱ��:=");
			index = index - 1;
		}
		int slev = 0;		//�洢������ű��еı����Ĳ�����ƫ����
		int saddr = 0;
		expression();
		if (idx != -1)
		{
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			slev = it->level;
			saddr = it->addr;
			emit("STO", lev - slev, saddr);
		}
	}
	else if (str.token == "if")
	{
		lexp();
		str = getWord();
		if (str.token != "then")
		{
			Error(str.row, "ȱ�ٹؼ���then");
			index = index - 1;
		}
		cx3 = code.size();											//��¼jpcָ������ַ
		emit("JPC", 0, 0);
		statement();
		cx1 = code.size();											//��¼jmp�Ļ����ַ
		emit("JMP", 0, 0);											
		vector<instruction>::iterator it = code.begin();            //��ַ���� jpc
		it = it + cx3;
		it->a = code.size();

		it = code.begin();											//����jmp
		it = it + cx1;
		it->a = code.size();
		str = getWord();
		if (str.token == "else")
		{
			cx1 = code.size();
			emit("JMP", 0, 0);
			statement();

			vector<instruction>::iterator it = code.begin();        //��ַ����
			it = it + cx1;
			it->a = code.size();
		}
		else index = index - 1;
	}
	else if (str.token == "while")
	{
		int jump_addr = code.size();				//��¼whileѭ����������ʱ�����
		lexp();
		str = getWord();

		if (str.token != "do")
		{
			Error(str.row, "ȱ��do�ؼ���");
			index = index - 1;
		}

		cx3 = code.size();			//��¼������������ת��ַ���������
		emit("JPC", 0, 0);
		statement();
		emit("JMP", 0, jump_addr);
		vector<instruction>::iterator it = code.begin();			//����
		it = it + cx3;
		it->a = code.size();
	}
	else if (str.token == "call")			//call <id> ([<exp>{,<exp>}])
	{
		str = getWord();
		if (str.kind_num == "32")
		{
			idx = findTable(str.token);
			if (idx == -1)
			{
				Error(str.row, str.token + "����δ���壡");
			}
			else 
			{
				vector<table>::iterator it = table_list.begin();
				it = it + idx;
				if (it->kind == "PROCEDURE")
				{
					precnt = it->size;
					ffloc = it->value;
					//emit("CAL", lev - it->level, it->addr);
				}
				else
				{
					Error(str.row, str.token + "���Ǻ�������");
				}
			}
			str = getWord();

			cnt = 0;						//��ʼͳ�Ƶ��ù�����Ĳ�������
			
			
			if (str.token == "(")
			{
				str = getWord();
				if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
				{
					index = index - 1;
					cnt = cnt + 1;
					expression();
					if(idx != -1)
					{
						vector<table>::iterator it = table_list.begin();
						it = it + idx;
						if (it->kind == "PROCEDURE")
						{
							emit("STO", lev-it->level, ffloc);		//������ʱ��ʵ�θ�ֵ���β�
							ffloc = ffloc + 1;
						}
					}
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
						{
							index = index - 1;
							cnt = cnt + 1;
							expression();
							if (idx != -1)
							{
								vector<table>::iterator it = table_list.begin();
								it = it + idx;
								if (it->kind == "PROCEDURE")
								{
									emit("STO", lev - it->level, ffloc);		//������ʱ��ʵ�θ�ֵ���β�
									ffloc = ffloc + 1;
								}
							}
						}
						str = getWord();
					}
					if (str.token != ")") 
					{
						Error(str.row, "ȱ��������");
						if (cnt != precnt)
						{
							Error(str.row, fname + "����������ƥ�䣡");
						}
						index = index - 1;
					}
				}
				else if (str.token != ")") 
				{
					Error(str.row, "ȱ��������");
					if (cnt != precnt)
					{
						Error(str.row, fname + "����������ƥ�䣡");
					}
					index = index - 1;
				}
				
			}
			else
			{
				Error(str.row, "ȱ��������");

				cnt = 0;
				

				str = getWord();
				if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
				{
					index = index - 1;
					cnt = cnt + 1;
					expression();
					if (idx != -1)
					{
						vector<table>::iterator it = table_list.begin();
						it = it + idx;
						if (it->kind == "PROCEDURE")
						{
							emit("STO", lev - it->level, ffloc);		//������ʱ��ʵ�θ�ֵ���β�
							ffloc = ffloc + 1;
						}
					}
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
						{
							index = index - 1;
							cnt = cnt + 1;
							expression();
							if (idx != -1)
							{
								vector<table>::iterator it = table_list.begin();
								it = it + idx;
								if (it->kind == "PROCEDURE")
								{
									emit("STO", lev - it->level, ffloc);		//������ʱ��ʵ�θ�ֵ���β�
									ffloc = ffloc + 1;
								}
							}
						}
						str = getWord();
					}
					if (str.token != ")")
					{
						Error(str.row, "ȱ��������");
						if (cnt != precnt)
						{
							Error(str.row, fname + "����������ƥ�䣡");
						}
						index = index - 1;
					}
					
				}
				else if (str.token != ")") 
				{
					Error(str.row, "ȱ��������");
					if (cnt != precnt)
					{
						Error(str.row, fname + "����������ƥ�䣡");
					}
					index = index - 1;
				}
				
			}
			if (cnt != precnt)
			{
				Error(str.row, fname + "����������ƥ�䣡");
			}
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			emit("CAL", lev - it->level, it->addr);					//��ֵ��ɺ���ù���
		}
		else
		{
			Error(str.row, "call����ȱ�ٱ�ʶ��");
			str = getWord();
			if (str.token == "(")
			{
				str = getWord();
				if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
				{
					index = index - 1;
					expression();
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
						{
							index = index - 1;
							expression();
						}
						str = getWord();
					}
					if (str.token != ")") 
					{
						Error(str.row, "ȱ��������");
						index = index - 1;
					}
				}
				else if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "ȱ��������");
				str = getWord();
				if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
				{
					index = index - 1;
					expression();
					str = getWord();
					while (str.token == ",")
					{
						str = getWord();
						if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
						{
							index = index - 1;
							expression();
						}
						str = getWord();
					}
					if (str.token != ")")
					{
						Error(str.row, "ȱ��������");
						index = index - 1;
					}
				}
				else if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
		}
	}
	else if (str.token == "read")
	{
		str = getWord();
		if (str.token == "(")
		{
			str = getWord();
			if (str.kind_num == "32")
			{
				idx = findTable(str.token);
				if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
				else 
				{
					vector<table>::iterator it = table_list.begin();
					it = it + idx;
					if (it->kind == "VARIABLE")
					{
						emit("OPR", 0, 16);
						emit("STO", lev - it->level, it->addr);
					}
					else 
					{
						Error(str.row, str.token+"Ϊ�Ǳ�����read�������ź�����Ҫ��������");
					}
				}
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if(str.kind_num == "32") 
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
						else
						{
							vector<table>::iterator it = table_list.begin();
							it = it + idx;
							if (it->kind == "VARIABLE")
							{
								emit("OPR", 0, 16);
								emit("STO", lev - it->level, it->addr);
							}
							else
							{
								Error(str.row, str.token + "Ϊ�Ǳ�����read�������ź�����Ҫ��������");
							}
						}
					}
					else
					{
						Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "�����ź���ȱ�ٱ�ʶ��");
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
						else
						{
							vector<table>::iterator it = table_list.begin();
							it = it + idx;
							if (it->kind == "VARIABLE")
							{
								emit("OPR", 0, 16);
								emit("STO", lev - it->level, it->addr);
							}
							else
							{
								Error(str.row, str.token + "Ϊ�Ǳ�����read�������ź�����Ҫ��������");
							}
						}
					}
					else
					{
						Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						index = index - 1;
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
		}
		else 
		{
			Error(str.row, "ȱ��������");
			str = getWord();
			if (str.kind_num == "32")
			{
				idx = findTable(str.token);
				if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
				else
				{
					vector<table>::iterator it = table_list.begin();
					it = it + idx;
					if (it->kind == "VARIABLE")
					{
						emit("OPR", 0, 16);
						emit("STO", lev - it->level, it->addr);
					}
					else
					{
						Error(str.row, str.token + "Ϊ�Ǳ�����read�������ź�����Ҫ��������");
					}
				}
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
						else
						{
							vector<table>::iterator it = table_list.begin();
							it = it + idx;
							if (it->kind == "VARIABLE")
							{
								emit("OPR", 0, 16);
								emit("STO", lev - it->level, it->addr);
							}
							else
							{
								Error(str.row, str.token + "Ϊ�Ǳ�����read�������ź�����Ҫ��������");
							}
						}
					}
					else Error(str.row, "���ź���ȱ�ٱ�ʶ��");
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "�����ź���ȱ�ٱ�ʶ��");
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "δ���壡"); }
						else
						{
							vector<table>::iterator it = table_list.begin();
							it = it + idx;
							if (it->kind == "VARIABLE")
							{
								emit("OPR", 0, 16);
								emit("STO", lev - it->level, it->addr);
							}
							else
							{
								Error(str.row, str.token + "Ϊ�Ǳ�����read�������ź�����Ҫ��������");
							}
						}
					}
					else
					{
						Error(str.row, "���ź���ȱ�ٱ�ʶ��");
						index = index - 1;
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
		}
	}
	else if (str.token == "write")
	{
		str = getWord();
		if (str.token == "(")
		{
			str = getWord();
			if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
			{
				index = index - 1;
				expression();
				emit("OPR", 0, 14);						//���ջ��ֵ����Ļ
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
					{
						index = index - 1;
						expression();
						emit("OPR", 0, 14);				//���ջ��ֵ����Ļ
					}
					str = getWord();
				}
				emit("OPR", 0, 15);						//�������
				if (str.token != ")")
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
			else if (str.token != ")") 
			{
				Error(str.row, "ȱ��������");
				index = index - 1;
			}
		}
		else
		{
			Error(str.row, "ȱ��������");
			str = getWord();
			if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
			{
				index = index - 1;
				expression();
				emit("OPR", 0, 14);						//���ջ��ֵ����Ļ
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
					{
						index = index - 1;
						expression();
						emit("OPR", 0, 14);						//���ջ��ֵ����Ļ
					}
					str = getWord();
				}
				emit("OPR", 0, 15);						//�������

				if (str.token != ")") 
				{
					Error(str.row, "ȱ��������");
					index = index - 1;
				}
			}
			else if (str.token != ")") 
			{
				Error(str.row, "ȱ��������");
				index = index - 1;
			}
		}
	}
	else 
	{
		body();
    }
}

void expression()
{
	strToken str;
	str = getWord();
	if (str.token == "+" || str.token == "-")
	{
		if (str.token == "-") emit("OPR", 0, 1);			//ȡ������
		term();
	}
	else 
	{
		index = index - 1;
		term();
	}
	str = getWord();
	while (str.token == "+" || str.token == "-")
	{
		term();
		if (str.token == "+") emit("OPR", 0, 2);
		else emit("OPR", 0, 3);
		str = getWord();
	}
	index = index - 1;
}

void term()
{
	factor();
	strToken str;
	str = getWord();
	while (str.token == "*" || str.token == "/")
	{
		factor();
		if (str.token == "*") emit("OPR", 0, 4);
		else emit("OPR", 0,5);
		str = getWord();
	}
	index = index - 1;
}
void factor()
{
	int idx ;				//���ű��ѯ����
	strToken str;
	str = getWord();
	if (str.token == "(")
	{
		expression();
		str = getWord();
		if (str.token != ")")
		{
			Error(str.row, "ȱ��������");
			index = index - 1;
		}
	}
	else if (str.kind_num == "32")
	{
		idx = findTable(str.token);
		if (idx == -1) Error(str.row, str.token + "δ���壡");
		else 
		{
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			if (it->kind == "CONST") emit("LIT", 0, it->value);
			else if (it->kind == "VARIABLE") emit("LOD", lev - it->level, it->addr);
			else if (it->kind == "PROCEDURE") Error(str.row, str.token + "Ϊ������,����");
		}
		
	}
	else if (str.kind_num == "33")
	{
		int num = stoi(str.token);
		emit("LIT", 0, num);
	}
	else {
		Error(str.row, str.token + "���ڴ˴�����ע������Ӧ���ǳ��������Ǳ�ʶ�������Ǳ��ʽ��");
	}
}

void lexp()
{
	strToken str = getWord();
	if (str.token == "odd")
	{
		expression();
		emit("OPR", 0, 6);
	}
	else
	{
		index = index - 1;
		expression();
		str = getWord();
		if (str.token != "=" && str.token != "<>" && str.token != "<" && str.token != ">" && str.token != "<=" && str.token != ">=")
		{
			Error(str.row, "ȱ�ٱȽ������");
			index = index - 1;
		}
		expression();
		if (str.token == "=") emit("OPR", 0, 8);
		else if (str.token == "<>") emit("OPR", 0, 9);
		else if (str.token == "<") emit("OPR", 0, 10);
		else if (str.token == ">=") emit("OPR", 0, 11);
		else if (str.token == ">") emit("OPR", 0, 12);
		else if (str.token == "<=") emit("OPR", 0, 13);
	}
}

int base(int stack[],int currentlevel, int leveldiff)			//����¼��ַ
{
	int b = currentlevel;
	while (leveldiff--)
	{
		b = stack[b];
	}
	return b;
}

void interpret()
{
	int pc = 0;			//��������� program counter
	vector<instruction>::iterator it = code.begin();
	vector<instruction>::iterator end = code.end();
	int stack[STACKSIZE];
	int top = 0;        //ջ���Ĵ���
	int b = 0;			//ջ�׼Ĵ���
	instruction i;		//ָ��Ĵ���

	cout << "Begin excuting PL/0 program." << endl;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;
	do {
		it = code.begin();
		it = it + pc;
		if (it < end)
		{
			i.f = it->f;
			i.l = it->l;
			i.a = it->a;
		}
		else break;
		pc = pc + 1;
		if (i.f == "LIT")
		{
			stack[++top] = i.a;
		}
		else if (i.f == "OPR")
		{
			if (i.a == 0)			//���̵��ý���������
			{
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
			}
			else if (i.a == 1)		//ȡ������
			{
				stack[top] = -stack[top];
			}
			else if (i.a == 2)		//�ӷ�����
			{
				top--;
				stack[top] += stack[top + 1];
			}
			else if (i.a == 3)		//��������
			{
				top--;
				stack[top] -= stack[top + 1];
			}
			else if (i.a == 4)		//�˷�����
			{
				top--;
				stack[top] *= stack[top + 1];
			}
			else if (i.a == 5)		//��������
			{
				if (stack[top] == 0)
				{
					cout << "���������ĸ����Ϊ�㣡" << endl;
					cout << "������ֹ��" << endl;
					exit(1);
				}
				else
				{
					top--;
					stack[top] /= stack[top + 1];
				}
			}
			else if (i.a == 6)		//�ж���ż��
			{
				stack[top] %= 2;
			}
			else if (i.a == 8)		//�ж��Ƿ����
			{
				top--;
				stack[top] = (stack[top] == stack[top + 1]);
			}
			else if (i.a == 9)		//�ж��Ƿ񲻵�
			{
				top--;
				stack[top] = (stack[top] != stack[top + 1]);
			}
			else if (i.a == 10)		//�ж��Ƿ�С��
			{
				top--;
				stack[top] = (stack[top] < stack[top + 1]);
			}
			else if (i.a == 11)		//�ж��Ƿ���ڵ���
			{
				top--;
				stack[top] = (stack[top] >= stack[top + 1]);
			}
			else if (i.a == 12)		//�ж��Ƿ����
			{
				top--;
				stack[top] = (stack[top] > stack[top + 1]);
			}
			else if (i.a == 13)		//�ж��Ƿ�С�ڵ���
			{
				top--;
				stack[top] = (stack[top] <= stack[top + 1]);
			}
			else if (i.a == 14)		//���ջ��ֵ����Ļ
			{
				cout << stack[top];
			}
			else if (i.a == 15)    //��Ļ�������
			{
				cout << endl;
			}
			else if (i.a == 16)		//���ն�����һ����������ջ��
			{
				int data;
				cout << "please input a integer:";
				cin >> data;
				stack[++top] = data;
			}
		}
		else if (i.f == "LOD")
		{
			stack[++top] = stack[base(stack, b, i.l) + i.a];
		}
		else if (i.f == "STO")
		{
			stack[base(stack, b, i.l) + i.a] = stack[top];
			top--;
		}
		else if (i.f == "CAL")
		{
			stack[top + 1] = base(stack, b, i.l);
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
		}
		else if (i.f == "INT")
		{
			top = top + i.a;
		}
		else if (i.f == "JMP")
		{
			pc = i.a;
		}
		else if (i.f == "JPC")
		{
			if (stack[top] == 0)
			{
				pc = i.a;
			}
			top--;
			
		}
	} while(pc);
	cout << "End executing PL/0 program." << endl;
}

void GrammarAnalyser() 
{
	initWords();
	prog();
}

int main()
{
	GrammarAnalyser();
	if (flag)
	{
		interpret();
		vector<instruction>::iterator it = code.begin();
		vector<instruction>::iterator end = code.end();
		int cal = 0;
		for (; it < end; ++it)
		{
			cout << it->f << " " << it->l << " " << it->a << endl;
			cal++;
		}
		cout << "The num of instructions is " << cal << endl;
	}
	return 0;
}