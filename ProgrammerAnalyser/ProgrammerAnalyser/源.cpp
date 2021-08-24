#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#define STACKSIZE 5000
using namespace std;

bool flag = true;			//记录程序是否出错
int lev = 0;				//定义层数
int index = 0;				//单词获取的索引
int dx = 3;					//默认变量偏移量，包含SL DL RA


int cx2 = 0;				//记录procedure 的地址回填符号表size域
int cnt = 0;				//统计调用procedure时里面参数个数
int precnt = 0;             //记录之前过程定义时的参数个数

string fname = "";				//记录函数名称



//符号表
struct table {			
	string name ;		//名称
	string kind ;		//类型
	int value ;		    //值
	int level ;			//层数
	int addr;			//地址
	int size;			//记录形参个数
};

//pcode结构体
struct instruction {
	string f;			//伪操作码
	int l;				//层差值
	int a;				//偏移量
};


//存放输入数据
struct strToken {
	string kind_num;		//种别号
	string token;		//单词
	string row;			//行号
};
//创建符号列表
vector<table> table_list;

//存放单个单词
vector<strToken> words;

//存放pcode
vector<instruction> code;

//放入符号列表
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
//查询在符号表中的下标
int findTable(string name)
{
	vector<table>::iterator  it = table_list.begin();
	for (int i = 0; it < table_list.end(); it++,i++)
	{
		if (it->name == name) return i;
	}
	return -1;
}

//出错处理
void Error(string row = "", string des = "")
{
	int row1 = stoi(row);
	flag = false;
	cout << "第 " << row1 + 1<< " 行" << ":" << des << endl;
}

//返回三个数中最小的那个
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
	//获取两个字符串中最长的那个
	if (len1 > len2) maxlen = len1;
	else maxlen = len2;
	//建立二维数组
	int **dif;
	dif = new int*[len1 + 1];
	for (int j = 0; j <= len1 + 1; j++)
	{
		dif[j] = new int[len2 + 1];
	}

	//初始化数组
	for (int i = 0; i <= len1; i++)
	{
		dif[i][0] = i;
	}
	for (int i = 0; i <= len2; i++)
	{
		dif[0][i] = i;
	}
	//获取运算步骤
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
			//取三个值中最小的  
			dif[i][j] = min3(dif[i - 1][j - 1] + temp, dif[i][j - 1] + 1,
				dif[i - 1][j] + 1);
		}
	}
	float simi = 1 - (float)dif[len1][len2] / maxlen;
	return simi;
}
//将文件夹中的数据存入vector中 
void initWords()
{
	ifstream ifp("result1.txt");
	string temp = "";		//存放第n块读入的数据的中间变量
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

strToken getWord()			//单个单词以及相关行号种别号信息读取
{
	vector<strToken>::iterator it = words.begin();
	it = it + index;
	if (it == words.end()) exit(0);				//读到结尾强制退出
	index++;
	return *it;
}

//产生式调用入口
void block();
void const1();
void body();
void statement();
void expression();
void lexp();
void term();
void factor();


//程序入口prog
void prog()
{
	strToken str;
	str = getWord();
	if (str.token == "program")
	{
		str = getWord();
		if (str.kind_num == "32")			//标识符编码为32
		{
			str = getWord();
			if (str.token == ";")
				block();
			else
			{
				Error(str.row, " 缺少分号;");
				index = index - 1;			//回退
				block();
			}
		}
		else 
		{
			Error(str.row, " 缺少程序名称");		//判断前一个是否符合要求，若不是则跳过
			if (str.token == ";") block();
			else {
				str = getWord();
				if (str.token == ";") block();
				else {
					Error(str.row, " 缺少分号;");
					index = index - 1;				//读到非prog区域单词回退
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
			Error(str.row, "缺少关键字program");
			if (str.kind_num == "32")
			{
				str = getWord();
				if (str.token == ";")
					block();
				else
				{
					Error(str.row, " 缺少分号;");
					index = index - 1;
					block();
				}
			}
			else {
				Error(str.row, "缺少程序名称");
				if (str.token == ";") block();
				else {
					Error(str.row, "缺少分号");
					index = index - 1;
					block();
				}
			}
		}
		else 
		{
			Error(str.row, "关键字program拼写错误");
			str = getWord();
			if (str.kind_num == "32")
			{
				str = getWord();
				if (str.token == ";")
					block();
				else
				{
					Error(str.row, " 缺少分号;");
					index = index - 1;
					block();
				}
			}
			else {
				Error(str.row, "缺少程序名称");
				if (str.token == ";") block();
				else {
					Error(str.row, "缺少分号");
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
	int cx1;                //记录jmp指令回填地址
	int cur_dx;				//记录当前层的变量个数
	float simi = 0;			//记录与关键字的相似度
	cx1 = code.size();		//记录jmp指令回填的位置
	emit("JMP", 0, 0);		//方便跳转到body部分执行，具体地址等到算出后回填
	simi = similarity(str.token, "const");
	if (simi > 0.8)
	{
		if (str.token != "const")
		{
			Error(str.row, "关键字const拼写错误");
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
			Error(str.row, " 缺少分号;");
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
		else Error(str.row, " var后面缺少标识符");
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
				Error(str.row, "逗号,后面缺少非标识符");
				str = getWord();
			}
		}
		if (str.token == ";") str = getWord();
		else Error(str.row, " 缺少分号;");
	}
	simi = similarity(str.token, "procedure");

	while (simi > 0.8)			//proc->procedure <id>([<id>{,<id>}]);<block>{;<proc>}
	{
		if(str.token != "procedure")
		{
			Error(str.row, "procedure拼写错误");
		}
		int count = 0;			//统计参数个数然后存入size域
		str = getWord();
		if (str.kind_num == "32")
		{
			cx2 = table_list.size();			//记录符号表中存放procedure的位置
			int addr ;				//记录过程的地址
			addr = code.size();
			insertTable(str.token, "PROCEDURE", dx, lev,addr);				//procedure value值域存储procedure之前本层变量个数

			
			str = getWord();
			if (str.token == "(")
			{
				str = getWord();
				if (str.kind_num == "32")
				{
					count = count + 1;		//每遇到括号里的一个变量就加一
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
						else Error(str.row, "逗号后面缺少标识符");
					}
					if (str.token == ")")
					{
						//将参数个数填入size域
						vector<table>::iterator it = table_list.begin();
						it = it + cx2;
						it->size = count;

						str = getWord();
					}
					else
					{
						Error(str.row, "缺少右括号");

						//将参数个数填入size域
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
					Error(str.row, "缺少右括号");
					str = getWord();
				}
			}
			else
			{
				Error(str.row, "缺少左括号");

				if (str.kind_num == "32" || str.token == ")")
				{
					if (str.kind_num == "32")
					{
						count = count + 1;				//遇到一个变量就加一
						insertTable(str.token, "VARIABLE", 0, lev, dx);
						dx = dx + 1;

						str = getWord();
						while (str.token == ",")
						{
							str = getWord();
							if (str.kind_num == "32")
							{
								count = count + 1;				//遇到一个变量就加一
								insertTable(str.token, "VARIABLE", 0, lev, dx);
								dx = dx + 1;

								str = getWord();
							}
							else Error(str.row, "逗号后面缺少标识符");
						}
						if (str.token == ")")
						{
							//将参数个数填入size域
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;

							str = getWord();

						}
						else
						{
							Error(str.row, "缺少右括号");
							//将参数个数填入size域
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
						count = count + 1;				//遇到一个变量就加一
						insertTable(str.token, "VARIABLE", 0, lev, dx);
						dx = dx + 1;

						str = getWord();
						while (str.token == ",")
						{
							str = getWord();
							if (str.kind_num == "32")
							{
								count = count + 1;				//遇到一个变量就加一
								insertTable(str.token, "VARIABLE", 0, lev, dx);
								dx = dx + 1;
								str = getWord();
							}
							else Error(str.row, "逗号后面缺少标识符");
						}
						if (str.token == ")")
						{
							//将参数个数填入size域
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;
							str = getWord();
						}
						else
						{
							Error(str.row, "缺少右括号");

							//将参数个数填入size域
							vector<table>::iterator it = table_list.begin();
							it = it + cx2;
							it->size = count;
							str = getWord();
						}
					}
					else if (str.token == ")") str = getWord();
					else
					{
						Error(str.row, "缺少右括号");
						str = getWord();
					}
				}
			}
		}
		else
		{
			Error(str.row, " 缺少函数名称");   
			/* 因为没有函数名称所以统计的count也没有指定的size存放
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
						else Error(str.row, "逗号后面缺少标识符");
					}
					if (str.token == ")")
					{
						str = getWord();
					}
					else
					{
						Error(str.row, "缺少右括号");
						str = getWord();
					}
				}
				else if (str.token == ")")
				{
					str = getWord();
				}
				else
				{
					Error(str.row, "缺少右括号");
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
							else Error(str.row, "逗号后面缺少标识符");
						}
						if (str.token == ")")
						{

							str = getWord();
						}
						else
						{
							Error(str.row, "缺少右括号");

							str = getWord();
						}
					}
					else if (str.token == ")")
					{
						str = getWord();
					}
					else
					{
						Error(str.row, "缺少右括号");
						str = getWord();
					}
				}
				else
				{
					Error(str.row, "缺少左括号");
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
								else Error(str.row, "逗号后面缺少标识符");
							}
							if (str.token == ")")
							{
								str = getWord();

							}
							else
							{
								Error(str.row, "缺少右括号");

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
								else Error(str.row, "逗号后面缺少标识符");
							}
							if (str.token == ")")
							{
								str = getWord();
							}
							else
							{
								Error(str.row, "缺少右括号");

								str = getWord();
							}
						}
						else if (str.token == ")") str = getWord();
						else
						{
							Error(str.row, "缺少右括号");
							str = getWord();
						}
					}
				}
			}
		}
		if (str.token == ";") {}
		else
		{
			Error(str.row, "缺少分号;");
			index = index - 1;
		}
		cur_dx = dx;						//记录当前层的变量个数
		lev = lev + 1;
		block();
		lev = lev - 1;
		dx = cur_dx;			//还原上一层的变量个数
		str = getWord();
		if (str.token == ";")
		{
			str = getWord();
			simi = similarity(str.token, "procedure");	//更新相似度
		}
		else break;
			
	}
	index = index - 1;
	vector<instruction>::iterator it = code.begin();
	it = it + cx1;
	it->a = code.size();		//回填jmp指令入口
	emit("INT", 0, dx);
	body();
	emit("OPR", 0, 0);			//过程调用结束返回调用点并退栈
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
				num = stoi(str.token);				//存储的整数值
				insertTable(value, "CONST", num, lev);
			}
			else
			{
				Error(str.row, ":=后面需要跟整数");
				index = index - 1;
			}
		}
		else
		{
			Error(str.row, "缺少:=");
			if (str.kind_num != "33")
			{
				str = getWord();
				if (str.kind_num != "33")
				{
					Error(str.row, ":=后面需要跟整数");
					index = index - 1;
				}
				else
				{
					num = stoi(str.token);				//存储的整数值
					insertTable(value, "CONST", num, lev);
				}
			}
			else
			{
				num = stoi(str.token);				//存储的整数值
				insertTable(value, "CONST", num, lev);
			}
		}
	}
	else
	{
		Error(str.row, "缺少标识符");
		if (str.token == ":=")
		{
			str = getWord();
			if (str.kind_num == "33")
			{
				num = stoi(str.token);				//存储的整数值
				insertTable(value, "CONST", num, lev);
			}
			else
			{
				Error(str.row, ":=后面需要跟整数");
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
					num = stoi(str.token);				//存储的整数值
					insertTable(value, "CONST", num, lev);
				}
				else
				{
					Error(str.row, ":=后面需要跟整数");
					index = index - 1;
				}
			}
			else 
			{
				Error(str.row, "缺少:=");
				if (str.kind_num != "33")
				{
					str = getWord();
					if (str.kind_num != "33")
					{
						Error(str.row, ":=后面需要跟整数");
						index = index - 1;
					}
					else 
					{
						num = stoi(str.token);				//存储的整数值
						insertTable(value, "CONST", num, lev);
					}
				}
				else 
				{
					num = stoi(str.token);				//存储的整数值
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
		Error(str.row, "缺少begin");
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
			Error(str.row, "缺少分号;");
			statement();
		}
	}
	if (str.token != "end")
	{
		Error(str.row, "缺少end");
		index = index - 1;
	}
}

void statement()      //id  := exp
{
	strToken str = getWord();
	int cx1;			//记录jmp指令回填地址
	int cx3;            //记录jpc指令回填地址
	int idx ;				//符号表查询索引
	int ffloc = 0;					//用来寻找形参位置formalfactor
	if (str.token == ";")
	{
		Error(str.row, ";是多余的");
		str = getWord();
	}
	if (str.kind_num == "32")
	{
	    idx = findTable(str.token);
		if (idx == -1) Error(str.row, str.token + "未定义");
		else
		{
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			if (it->kind != "VARIABLE")
			{
				Error(str.row, str.token + "不是变量，不能赋值！");
			}
		}
		str = getWord();
		if (str.token != ":=")
		{
			Error(str.row, "缺少:=");
			index = index - 1;
		}
		int slev = 0;		//存储放入符号表中的变量的层数和偏移量
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
			Error(str.row, "缺少关键字then");
			index = index - 1;
		}
		cx3 = code.size();											//记录jpc指令回填地址
		emit("JPC", 0, 0);
		statement();
		cx1 = code.size();											//记录jmp的回填地址
		emit("JMP", 0, 0);											
		vector<instruction>::iterator it = code.begin();            //地址回填 jpc
		it = it + cx3;
		it->a = code.size();

		it = code.begin();											//回填jmp
		it = it + cx1;
		it->a = code.size();
		str = getWord();
		if (str.token == "else")
		{
			cx1 = code.size();
			emit("JMP", 0, 0);
			statement();

			vector<instruction>::iterator it = code.begin();        //地址回填
			it = it + cx1;
			it->a = code.size();
		}
		else index = index - 1;
	}
	else if (str.token == "while")
	{
		int jump_addr = code.size();				//记录while循环条件成立时的入口
		lexp();
		str = getWord();

		if (str.token != "do")
		{
			Error(str.row, "缺少do关键字");
			index = index - 1;
		}

		cx3 = code.size();			//记录条件不成立跳转地址，后面回填
		emit("JPC", 0, 0);
		statement();
		emit("JMP", 0, jump_addr);
		vector<instruction>::iterator it = code.begin();			//回填
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
				Error(str.row, str.token + "函数未定义！");
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
					Error(str.row, str.token + "不是函数名！");
				}
			}
			str = getWord();

			cnt = 0;						//开始统计调用过程里的参数个数
			
			
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
							emit("STO", lev-it->level, ffloc);		//将调用时的实参赋值给形参
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
									emit("STO", lev - it->level, ffloc);		//将调用时的实参赋值给形参
									ffloc = ffloc + 1;
								}
							}
						}
						str = getWord();
					}
					if (str.token != ")") 
					{
						Error(str.row, "缺少右括号");
						if (cnt != precnt)
						{
							Error(str.row, fname + "参数个数不匹配！");
						}
						index = index - 1;
					}
				}
				else if (str.token != ")") 
				{
					Error(str.row, "缺少右括号");
					if (cnt != precnt)
					{
						Error(str.row, fname + "参数个数不匹配！");
					}
					index = index - 1;
				}
				
			}
			else
			{
				Error(str.row, "缺少左括号");

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
							emit("STO", lev - it->level, ffloc);		//将调用时的实参赋值给形参
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
									emit("STO", lev - it->level, ffloc);		//将调用时的实参赋值给形参
									ffloc = ffloc + 1;
								}
							}
						}
						str = getWord();
					}
					if (str.token != ")")
					{
						Error(str.row, "缺少右括号");
						if (cnt != precnt)
						{
							Error(str.row, fname + "参数个数不匹配！");
						}
						index = index - 1;
					}
					
				}
				else if (str.token != ")") 
				{
					Error(str.row, "缺少右括号");
					if (cnt != precnt)
					{
						Error(str.row, fname + "参数个数不匹配！");
					}
					index = index - 1;
				}
				
			}
			if (cnt != precnt)
			{
				Error(str.row, fname + "参数个数不匹配！");
			}
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			emit("CAL", lev - it->level, it->addr);					//赋值完成后调用过程
		}
		else
		{
			Error(str.row, "call后面缺少标识符");
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
						Error(str.row, "缺少右括号");
						index = index - 1;
					}
				}
				else if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "缺少左括号");
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
						Error(str.row, "缺少右括号");
						index = index - 1;
					}
				}
				else if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
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
				if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
						Error(str.row, str.token+"为非变量，read函数括号后面需要跟变量！");
					}
				}
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if(str.kind_num == "32") 
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
								Error(str.row, str.token + "为非变量，read函数括号后面需要跟变量！");
							}
						}
					}
					else
					{
						Error(str.row, "逗号后面缺少标识符");
						
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "左括号后面缺少标识符");
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
								Error(str.row, str.token + "为非变量，read函数括号后面需要跟变量！");
							}
						}
					}
					else
					{
						Error(str.row, "逗号后面缺少标识符");
						index = index - 1;
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
		}
		else 
		{
			Error(str.row, "缺少左括号");
			str = getWord();
			if (str.kind_num == "32")
			{
				idx = findTable(str.token);
				if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
						Error(str.row, str.token + "为非变量，read函数括号后面需要跟变量！");
					}
				}
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
								Error(str.row, str.token + "为非变量，read函数括号后面需要跟变量！");
							}
						}
					}
					else Error(str.row, "逗号后面缺少标识符");
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
			else
			{
				Error(str.row, "左括号后面缺少标识符");
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.kind_num == "32")
					{
						idx = findTable(str.token);
						if (idx == -1) { Error(str.row, str.token + "未定义！"); }
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
								Error(str.row, str.token + "为非变量，read函数括号后面需要跟变量！");
							}
						}
					}
					else
					{
						Error(str.row, "逗号后面缺少标识符");
						index = index - 1;
					}
					str = getWord();
				}
				if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
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
				emit("OPR", 0, 14);						//输出栈顶值到屏幕
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
					{
						index = index - 1;
						expression();
						emit("OPR", 0, 14);				//输出栈顶值到屏幕
					}
					str = getWord();
				}
				emit("OPR", 0, 15);						//输出换行
				if (str.token != ")")
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
			else if (str.token != ")") 
			{
				Error(str.row, "缺少右括号");
				index = index - 1;
			}
		}
		else
		{
			Error(str.row, "缺少左括号");
			str = getWord();
			if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
			{
				index = index - 1;
				expression();
				emit("OPR", 0, 14);						//输出栈顶值到屏幕
				str = getWord();
				while (str.token == ",")
				{
					str = getWord();
					if (str.token == "+" || str.token == "-" || str.token == "(" || str.kind_num == "32" || str.kind_num == "33")
					{
						index = index - 1;
						expression();
						emit("OPR", 0, 14);						//输出栈顶值到屏幕
					}
					str = getWord();
				}
				emit("OPR", 0, 15);						//输出换行

				if (str.token != ")") 
				{
					Error(str.row, "缺少右括号");
					index = index - 1;
				}
			}
			else if (str.token != ")") 
			{
				Error(str.row, "缺少右括号");
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
		if (str.token == "-") emit("OPR", 0, 1);			//取反操作
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
	int idx ;				//符号表查询索引
	strToken str;
	str = getWord();
	if (str.token == "(")
	{
		expression();
		str = getWord();
		if (str.token != ")")
		{
			Error(str.row, "缺少右括号");
			index = index - 1;
		}
	}
	else if (str.kind_num == "32")
	{
		idx = findTable(str.token);
		if (idx == -1) Error(str.row, str.token + "未定义！");
		else 
		{
			vector<table>::iterator it = table_list.begin();
			it = it + idx;
			if (it->kind == "CONST") emit("LIT", 0, it->value);
			else if (it->kind == "VARIABLE") emit("LOD", lev - it->level, it->addr);
			else if (it->kind == "PROCEDURE") Error(str.row, str.token + "为函数名,出错！");
		}
		
	}
	else if (str.kind_num == "33")
	{
		int num = stoi(str.token);
		emit("LIT", 0, num);
	}
	else {
		Error(str.row, str.token + "放在此处有误，注意这里应该是常数或者是标识符或者是表达式！");
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
			Error(str.row, "缺少比较运算符");
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

int base(int stack[],int currentlevel, int leveldiff)			//求活动记录基址
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
	int pc = 0;			//程序计数器 program counter
	vector<instruction>::iterator it = code.begin();
	vector<instruction>::iterator end = code.end();
	int stack[STACKSIZE];
	int top = 0;        //栈顶寄存器
	int b = 0;			//栈底寄存器
	instruction i;		//指令寄存器

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
			if (i.a == 0)			//过程调用结束，回退
			{
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
			}
			else if (i.a == 1)		//取反操作
			{
				stack[top] = -stack[top];
			}
			else if (i.a == 2)		//加法操作
			{
				top--;
				stack[top] += stack[top + 1];
			}
			else if (i.a == 3)		//减法操作
			{
				top--;
				stack[top] -= stack[top + 1];
			}
			else if (i.a == 4)		//乘法操作
			{
				top--;
				stack[top] *= stack[top + 1];
			}
			else if (i.a == 5)		//除法操作
			{
				if (stack[top] == 0)
				{
					cout << "除法运算分母不能为零！" << endl;
					cout << "程序终止！" << endl;
					exit(1);
				}
				else
				{
					top--;
					stack[top] /= stack[top + 1];
				}
			}
			else if (i.a == 6)		//判断奇偶性
			{
				stack[top] %= 2;
			}
			else if (i.a == 8)		//判断是否相等
			{
				top--;
				stack[top] = (stack[top] == stack[top + 1]);
			}
			else if (i.a == 9)		//判断是否不等
			{
				top--;
				stack[top] = (stack[top] != stack[top + 1]);
			}
			else if (i.a == 10)		//判断是否小于
			{
				top--;
				stack[top] = (stack[top] < stack[top + 1]);
			}
			else if (i.a == 11)		//判断是否大于等于
			{
				top--;
				stack[top] = (stack[top] >= stack[top + 1]);
			}
			else if (i.a == 12)		//判断是否大于
			{
				top--;
				stack[top] = (stack[top] > stack[top + 1]);
			}
			else if (i.a == 13)		//判断是否小于等于
			{
				top--;
				stack[top] = (stack[top] <= stack[top + 1]);
			}
			else if (i.a == 14)		//输出栈顶值至屏幕
			{
				cout << stack[top];
			}
			else if (i.a == 15)    //屏幕输出换行
			{
				cout << endl;
			}
			else if (i.a == 16)		//从终端输入一个整数放入栈顶
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