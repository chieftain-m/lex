#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;
void filterSource(char source[], int num)//编译预处理，取出无用的字符和注释
{
	char temp[5000];
	int count = 0;
	for (int i = 0; i <= num; i++)
	{
		if (source[i] == '/'&&source[i + 1] == '/')
		{//若为单行注释“//”,则去除注释后面的东西，直至遇到回车换行
			while (source[i] != '\n')
			{
				i++;//向后扫描
			}
		}
		if (source[i] == '/'&&source[i + 1] == '*')
		{//若为多行注释“/* 。。。*/”则去除该内容
			i += 2;
			while (source[i] != '*' || source[i + 1] != '/')
			{
				i++;
				if (source[i] == 0)
				{
					printf("注释出错，没有找到 */，程序结束！！！\n");
					exit(0);
				}
			}
			i += 2;//跨过“*/”
		}
		if (source[i] != '\n'&&source[i] != '\t'&&source[i] != '\v'&&source[i] != '\r')
		{//若出现无用字符，则过滤；否则加载
			temp[count++] = source[i];
		}
	}
	temp[count] = '\0';
	strcpy_s(source, 4096, temp);//产生净化之后的源程序
}

int main()
{
	string filePath;
	cout << "请输入源代码文件路径：";
	cin >> filePath;
	ifstream inputfile(filePath); //构造一个ifstream并打开给定文件
	if (!inputfile)
	{
		cerr << "文件打开失败! " << endl;
		return 0;
	}
	char chars[4096];
	memset(chars, 0, sizeof(chars));
	inputfile.getline(chars, 4096, EOF);
	int num = 0;
	while (chars[num] != 0) {
		num++;
	}
	filterSource(chars, num);
	inputfile.close();//关闭文件
	cout << "预处理后程序如下\n" << chars << endl;
}