#include<iostream>
#include<string>
using namespace std;

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
int main()
{
	cout << "please input two string:" << endl;
	cout << "now, input the first one:" << endl;
	string str1, str2;
	getline(cin,str1);
	cout << "now , input the second one:" << endl;
	getline(cin, str2);
	float simi = similarity(str1, str2);
	simi = simi * 100;
	cout.setf(ios::fixed);
	cout.precision(1);
	cout << str1 << " and" << " " << str2 << " similarity is " << simi <<"%"<< endl;
	return 0;
	
}