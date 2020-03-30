#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stack>
using namespace std;

ifstream origin_input;//源文件
ifstream grammar_input;//文法
ofstream output_result;//输出

char chars[4096];
char state[100];
char start;
char final[100];
int len_state = 0;
int len_final = 0;
bool isfinal[150];
int hang = 1;//行数
int begin = 0, end = 0; //单词的第一个和最后一个位置
struct NFA_set//nfa结构体
{
	char set[100] = { '#' };//下一个状态
	int len = 0;//输入同一字符的转移状态数量
};
NFA_set moves[100][100];//nfa图
NFA_set new_set[100];
int num_new_set = 0;
int dfa[150][150];
/*是否是数字*/
bool isInteger(char a)
{
	if (a >= '0' && a <= '9') {
		return true;
	}
	return false;
}
/*是否是字母*/
bool isLetter(char c) {
	if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z')) {
		return true;
	}
	return false;
}
/*是否是关键字*/
int isKey(string str) {
	string keyword[15] = { "char","int","long","float","double","char",
		"string","if","else","for","while","main","return","void","bool" };
	for (int i = 0; i < 15; i++)
	{
		if (!str.compare(keyword[i])) {
			return i + 1;
		}
	}
	return 0;
}
/*是否是运算符*/
bool isOperator(char a)
{
	char Operator[20] = { '+','-','*','/','!','%','~','&','|','^','=','<','>' };
	for (int i = 0; i < 13; i++)
	{
		if (Operator[i] == a)
			return true;
	}
	return false;
}
/*是否是界符*/
bool isDelimiter(char a)
{
	char Delimiter[20] = { ',','(',')','{','}',';' };
	for (int i = 0; i < 7; i++)
	{
		if (Delimiter[i] == a)
			return true;
	}
	return false;
}
/*是否已经在状态集中*/
bool is_in_state(char a)
{
	for (int i = 0; i < len_state; i++)
	{
		if (a == state[i])
			return true;
	}
	return false;
}
/*是否是终结符*/
bool is_in_final(char a)
{
	for (int i = 0; i < len_final; i++)
	{
		if (a == final[i])
			return true;
	}
	return false;
}
/*是否已经在子集中*/
bool is_in_set(char a, NFA_set temp)
{
	for (int i = 0; i < temp.len; i++)
	{
		if (a == temp.set[i])
			return true;
	}
	return false;
}
/*利用输入文法创建NFA*/
void createNFA()
{
	int N;//文法个数
	bool flag = true;   //是不是第一个
	char ch;    //用来读 文法左边
	char point;//用来读中间的->
	string str;    //用来读 文法 右边
	grammar_input >> N;
	while (N--)
	{
		grammar_input >> ch >> point >> point >> str;
		if (flag)//仅在第一个时执行
		{
			start = ch;
			flag = false;
		}
		if (!is_in_state(ch))//左侧非终结符是否已经在状态集中
		{
			state[len_state++] = ch;
		}

		if (!is_in_final(str[0]))//是否是终结符
		{
			final[len_final++] = str[0];
		}
		if (str.size() > 1)//若右侧大于一个字符
		{
			moves[ch][str[0]].set[moves[ch][str[0]].len++] = str[1];//由一个终结符和一个非终结符构成路径
		}
		else
		{
			moves[ch][str[0]].set[moves[ch][str[0]].len++] = 'Y';  //仅有一个终结符，为终态
		}
	}
}
/*和已有的newset有没有重复的，有就返回重复的编号*/
int is_in_newset(NFA_set temp)
{
	bool flag[100];
	bool flag1;
	for (int i = 0; i < temp.len; i++)
	{
		flag[i] = false;
	}
	for (int i = 0; i < num_new_set; i++)
	{
		for (int k = 0; k < temp.len; k++)
		{
			for (int j = 0; j < new_set[i].len; j++)
			{
				if (temp.set[k] == new_set[i].set[j])
				{
					flag[k] = true;
				}
			}
		}
		flag1 = true;
		for (int m = 0; m < temp.len; ++m)
		{
			if (flag[m] == false)
			{
				flag1 = false;
				break;
			}
		}
		if (flag1 == true)
			return i;
		for (int m = 0; m < temp.len; ++m)
		{
			flag[m] = false;
		}
	}
	return -1;
}
/*得到第一个完整的子集 */
void get_Closure(NFA_set &temp)
{
	for (int i = 0; i < temp.len; i++)
	{
		for (int j = 0; j < moves[temp.set[i]]['@'].len; j++)
		{
			if (!is_in_set(moves[temp.set[i]]['@'].set[j], temp))//是否已经在栈中
			{
				temp.set[temp.len++] = moves[temp.set[i]]['@'].set[j];
			}
		}
	}
}
/*判断是否是终态*/
bool Is_contained_Y(NFA_set temp)
{
	for (int i = 0; i < temp.len; i++)
	{
		if (temp.set[i] == 'Y')
			return true;
	}
	return false;
}
/*将NFA转化为DFA*/
void NFA_to_DFA()
{
	num_new_set = 0;
	NFA_set work_set;
	NFA_set worked_set;
	work_set.set[work_set.len++] = start;//输入第一个状态
	worked_set.len = 0;
	stack<NFA_set> s; //做一个NFA_set类型的栈
	get_Closure(work_set);//第一次计算closure
	s.push(work_set);//第一次入栈
	new_set[num_new_set++] = work_set;
	for (int i = 0; i < 150; i++)
	{
		for (int j = 0; j < 150; j++)
		{
			dfa[i][j] = '-1';
		}
	}
	for (int i = 0; i < 150; i++)
		isfinal[i] = false;
	if (Is_contained_Y(work_set))
		isfinal[num_new_set - 1] = true;
	while (!s.empty())
	{
		work_set = s.top();//栈顶元素
		s.pop();//弹出一个
		for (int i = 0; i < len_final; i++)
		{
			for (int j = 0; j < work_set.len; j++)
			{
				for (int k = 0; k < moves[work_set.set[j]][final[i]].len; k++)
				{
					if (moves[work_set.set[j]][final[i]].set[k] != '#' && moves[work_set.set[j]][final[i]].set[k] != 'Y' && !is_in_set(moves[work_set.set[j]][final[i]].set[k], worked_set))//不在表中且不为终结
					{
						worked_set.set[worked_set.len++] = moves[work_set.set[j]][final[i]].set[k];//加入表
					}
					if (moves[work_set.set[j]][final[i]].set[k] == 'Y' && !is_in_set(moves[work_set.set[j]][final[i]].set[k], worked_set))//不在表中且为终结
					{
						worked_set.set[worked_set.len++] = 'Y';    //用Y表示终态
					}
				}
			}
			get_Closure(worked_set);
			if (worked_set.len > 0 && is_in_newset(worked_set) == -1)
			{
				dfa[num_new_set - 1][final[i]] = num_new_set;
				s.push(worked_set);
				new_set[num_new_set++] = worked_set;
				if (Is_contained_Y(worked_set))
				{
					isfinal[num_new_set - 1] = true;
				}
			}
			if (worked_set.len > 0 && is_in_newset(worked_set) > -1 && final[i] != '@')
			{
				dfa[is_in_newset(work_set)][final[i]] = is_in_newset(worked_set);
			}
			worked_set.len = 0;
		}
	}
}
/*判断是否满足DFA*/
bool DFA(string str)
{
	char now_state = 0;
	for (int i = 0; i < str.size(); i++)
	{
		now_state = dfa[now_state][str[i]];
		if (now_state == -1)
			return false;
	}
	if (isfinal[now_state] == true)
		return true;
	return false;
}
/*过滤源代码*/
void filterSource(char source[], int num) {
	char temp[5000];
	int count = 0;
	for (int i = 0; i <= num; i++) {
		if (source[i] == '/'&&source[i + 1] == '/') {//单行注释
			while (source[i] != '\n') {
				i++;//向后扫描
			}
		}
		if (source[i] == '/'&&source[i + 1] == '*') {//多行注释
			i += 2;
			while (source[i] != '*' || source[i + 1] != '/') {
				i++;
				if (source[i] == 0) {
					printf("注释出错，没有找到 */，程序结束！！！\n");
					exit(0);
				}
			}
			i += 2;//跨过“*/”
		}
		if (source[i] != '\t'&&source[i] != '\v'&&source[i] != '\r') {
			temp[count++] = source[i];
		}
	}
	temp[count] = '\0';
	strcpy_s(source, 4096, temp);//产生处理之后的源程序
}
/*获取单词*/
string getWord(string str, int begin, int& end) {
	string reg = " */=()[]{},:;"; // 匹配单词的正则表达式
	end = str.find_first_of(reg, begin);
	if (end == -1)	return "";
	if (begin != end)	end--;
	return str.substr(begin, end - begin + 1);
}
/*获取第一个字符*/
int getFirstChar(string str, int begin) {
	while (true) {
		if (str[begin] != ' '&&str[begin] != '\n')	return begin;
		else if (str[begin] == '\n') {
			hang++;
		}
		begin++;
	}
}
/*扫描单词*/
void scan(vector<string> str, vector<int> line) {
	for (int i = 0; i < str.size(); i++) {
		if (isInteger(str[i][0])) {
			if (DFA(str[i]))
			{
				cout << setw(10) << str[i] << " " << setw(10) << "常量" << setw(10) << line[i] << endl;
				output_result << 3;
			}
			else
			{
				cout << setw(10) << str[i] << " " << setw(10) << "出错，不是常量" << setw(10) << line[i] << endl;
			}
		}
		if (isLetter(str[i][0])) {
			if (isKey(str[i]))
			{
				cout << setw(10) << str[i] << " " << setw(10) << "关键字" << setw(10) << line[i] << endl;
				//output_result << f(str);
			}
			else
			{
				if (DFA(str[i]))
				{
					cout << setw(10) << str[i] << " " << setw(10) << "标识符" << setw(10) << line[i] << endl;
					output_result << 2;
				}
				else
				{
					cout << setw(10) << str[i] << " " << setw(10) << "出错，不是标识符" << setw(10) << line[i] << endl;
				}
			}
		}
		if (isDelimiter(str[i][0]))
		{
			cout << setw(10) << str[i] << " " << setw(10) << "界符" << setw(10) << line[i] << endl;
			output_result << str[i];
		}
		if (isOperator(str[i][0]))
		{
			if (isOperator(str[i + 1][0]))
			{
				cout << setw(9) << str[i] << str[i + 1] << " " << setw(10) << "运算符" << setw(10) << line[i] << endl;
				i++;
				//output_result<<4;
			}
			else
			{
				cout << setw(10) << str[i] << " " << setw(10) << "运算符" << setw(10) << line[i] << endl;
				output_result << str[i];
			}
		}
	}
}
/*打开文件及初始化*/
void init() {
	origin_input.open("d:\\1\\词法分析_源程序.txt");
	if (origin_input) {
		cout << "源程序文件打开成功! " << endl;
	}
	else {
		cerr << "源程序文件打开失败! " << endl;
		exit(0);
	}
	grammar_input.open("D:\\1\\词法分析_文法.txt");
	if (grammar_input) {
		cout << "文法文件打开成功! " << endl;
	}
	else {
		cerr << "文法文件打开失败! " << endl;
		exit(0);
	}
	output_result.open("D:\\1\\output.txt");
	if (!output_result) {
		cerr << "输出文法文件打开失败! " << endl;
		exit(0);
	}
	memset(chars, 0, sizeof(chars));
}
/*关闭文件*/
void endf() {
	origin_input.close();
	grammar_input.close();
	output_result.close();
}
int main() {
	init();
	origin_input.getline(chars, 4096, EOF);
	int num = 0;
	while (chars[num] != 0)
	{
		num++;
	}
	filterSource(chars, num);//预处理
	//提取单词
	int begin = 0, end = 0; //单词的第一个和最后一个字符
	vector<string> array;//保存单词
	vector<int>	line_Num;//行号
	while (1)
	{
		begin = getFirstChar(chars, begin);//获取第一个字符的位置
		string word = getWord(chars, begin, end);//获取单词
		if (end == -1)	break;//读取完成
		array.push_back(word);
		line_Num.push_back(hang);
		begin = end + 1;
	}
	createNFA();
	NFA_to_DFA();
	scan(array, line_Num);
	endf();
	return 0;
}