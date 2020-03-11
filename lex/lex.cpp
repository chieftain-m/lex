#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

// 单词种别码, 1-17为关键字种别码
#define CHAR 1
#define SHORT 2
#define INT 3
#define LONG 4
#define FLOAT 5
#define DOUBLE 6
#define CONST 7
#define STATIC 8 
#define IF 9
#define ELSE 10
#define WHILE 11
#define DO 12
#define FOR 13
#define BREAK 14
#define CONTINUE 15
#define VOID 16
#define RETURN 17

// 20为变量名种别码
#define ID 20   

// 30为常量种别码
#define NUM 30    

//31-39为限定符种别码
#define LP 31  // (
#define RP 32  // )
#define LBT 33  // [
#define RBT 34  // ]
#define LBS 35  // {
#define RBS 36  // }
#define COM 37  // ,
#define COL 38  // :
#define SEM 39  // ;

// 40-50为运算符种别码
#define AS 40  // =
#define EQ 41  // ==
#define HV 42  // >
#define LV 43  // <
#define HE 44  // >=
#define LE 45  // <=
#define ADD 46  // + 
#define SUB 47  // - 
#define MUL 48  // * 
#define DIV 49  // / 
#define MOD	50	//%

// -1为无法识别的字符标志码
#define ERROR -1       
int errorNum = 0;  // 记录词法分析错误的个数

// 找到第一个有效字符的位置
int getFirstChar(string str, int begin) {
	while (true) {
		if (str[begin] != ' ')	return begin;
		begin++;
	}
}

// 获取一个单词
string getWord(string str, int begin, int& end) {
	string reg = " +-*/=()[]{},:;"; // 匹配单词的正则表达式
	end = str.find_first_of(reg, begin);
	if (end == -1)	return "";
	if (begin != end)	end--;
	return str.substr(begin, end - begin + 1);
}

//编译预处理，取出无用的字符和注释
void filterSource(char source[], int num){
	char temp[5000];
	int count = 0;
	for (int i = 0; i <= num; i++){
		if (source[i] == '/'&&source[i + 1] == '/'){//单行注释
			while (source[i] != '\n'){
				i++;//向后扫描
			}
		}
		if (source[i] == '/'&&source[i + 1] == '*'){//多行注释
			i += 2;
			while (source[i] != '*' || source[i + 1] != '/'){
				i++;
				if (source[i] == 0){
					printf("注释出错，没有找到 */，程序结束！！！\n");
					exit(0);
				}
			}
			i += 2;//跨过“*/”
		}
		if (source[i] != '\n'&&source[i] != '\t'&&source[i] != '\v'&&source[i] != '\r'){
			temp[count++] = source[i];
		}
	}
	temp[count] = '\0';
	strcpy_s(source, 4096, temp);//产生处理之后的源程序
}

// 判断是不是关键字，是就返回关键字的种别码
int isKey(string str) {
	string keys[17] = { "char", "short", "int", "long", "float", "double", "const", "static", "if", "else", "while",
				"do", "for", "break", "continue", "void", "return" };
	for (unsigned int i = 0; i < 17; i++)
	{
		if (!str.compare(keys[i])) {
			return i + 1;
		}
	}
	return 0;
}

// 判断是不是常量(非负)
bool isNum(string str) {
	int dot = 0; // .的个数
	int notNum = 0; // 不是数字的个数
	for (int i = 0; i < str.length(); i++) {
		if (!(str[i] >= '0' && str[i] <= '9')) {
			notNum++;
			if (notNum > dot + 1) {
				cout << "该常量" << str << "的词法不正确" << endl;
				return false;
			}
			else if (str[i] == '.') {
				dot++;
				if (dot > 1)
				{
					cout << "该常量" << str << "的词法不正确" << endl;
					return false;
				}
			}
			else if ((str[i - 1] >= '0' && str[i - 1] <= '9') && (str[i] == 'E')
				&& (i == str.length() - 1 || (str[i + 1] >= '0' && str[i + 1] <= '9'))) {
				continue;
			}
			else if ((str[i - 1] >= '0' && str[i - 1] <= '9') && (str[i] == 'i')
				&& (i == str.length() - 1 || (str[i + 1] >= '0' && str[i + 1] <= '9'))) {
				continue;
			}
			else {
				cout << "该常量" << str << "的词法不正确";
				return false;
			}
		}
	}
	return true;
}

// 判断是不是字母
bool isLetter(char c) {
	if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z'))
		return true;
	return false;
}

// 词法分析函数
vector<pair<int, string> > analyse(vector<string> strs) {
	vector<pair<int, string> > vec;
	for (unsigned int i = 0; i < strs.size(); i++) {
		if (strs[i].size() == 1) {
			if (strs[i] == "=") {
				if (strs[i + 1] == "=") {
					vec.push_back(pair<int, string>(EQ, "=="));
					i++;
				}
				else {
					vec.push_back(pair<int, string>(AS, "="));
				}
			}
			else if (strs[i] == ">") {
				if (strs[i + 1] == "=") {
					vec.push_back(pair<int, string>(HE, ">="));
					i++;
				}
				else {
					vec.push_back(pair<int, string>(HV, ">"));
				}
			}
			else if (strs[i] == "<") {
				if (strs[i + 1] == "=") {
					vec.push_back(pair<int, string>(LE, "<="));
					i++;
				}
				else {
					vec.push_back(pair<int, string>(LV, "<"));
				}
			}
			else if (strs[i] == "+") {
				if ((strs[i - 1] == "=" || strs[i - 1] == "(") && isNum(strs[i + 1])) {//有符号常量（正数）
					vec.push_back(pair<int, string>(NUM, strs[i].append(strs[++i])));
				}
				else {//+
					vec.push_back(pair<int, string>(ADD, "+"));
				}
			}
			else if (strs[i] == "-") {
				if ((strs[i - 1] == "=" || strs[i - 1] == "(") && isNum(strs[i + 1])) {//有符号常量（负数）
					vec.push_back(pair<int, string>(NUM, strs[i].append(strs[++i])));
				}
				else {//-
					vec.push_back(pair<int, string>(SUB, "-"));
				}
			}
			else if (strs[i] == "*") {//*
				vec.push_back(pair<int, string>(MUL, "*"));
			}
			else if (strs[i] == "/") {///
				vec.push_back(pair<int, string>(DIV, "/"));
			}
			else if (strs[i] == "%") {///
				vec.push_back(pair<int, string>(MOD, "%"));
			}
			else if (strs[i] == "(") {//(
				vec.push_back(pair<int, string>(LP, "("));
			}
			else if (strs[i] == ")") {//)
				vec.push_back(pair<int, string>(RP, ")"));
			}
			else if (strs[i] == "[") {//[
				vec.push_back(pair<int, string>(LBT, "["));
			}
			else if (strs[i] == "]") {//]
				vec.push_back(pair<int, string>(RBT, "]"));
			}
			else if (strs[i] == "{") {//{
				vec.push_back(pair<int, string>(LBS, "{"));
			}
			else if (strs[i] == "}") {//}
				vec.push_back(pair<int, string>(RBS, "}"));
			}
			else if (strs[i] == ",") {//,
				vec.push_back(pair<int, string>(COM, ","));
			}
			else if (strs[i] == ":") {//:
				vec.push_back(pair<int, string>(COL, ":"));
			}
			else if (strs[i] == ";") {//;
				vec.push_back(pair<int, string>(SEM, ";"));
			}
			else if (strs[i][0] >= '0'&&strs[i][0] <= '9') {//一位数字常量
				vec.push_back(pair<int, string>(NUM, strs[i]));
			}
			else if (isLetter(strs[i][0])) {//一位字母变量名
				vec.push_back(pair<int, string>(ID, strs[i]));
			}
			else {//无法识别的字符
				vec.push_back(pair<int, string>(ERROR, strs[i]));
				errorNum++;
			}
		}
		else if (strs[i][0] >= '0'&&strs[i][0] <= '9' || strs[i][0] == '.') {//单词长度大于1，判断是不是常量
			if (!isNum(strs[i])) {
				errorNum++;
				vec.push_back(pair<int, string>(ERROR, strs[i]));
			}
			else if ((strs[i + 1][0] == '+' || strs[i + 1][0] == '-') && isNum(strs[i + 2])) {
				vec.push_back(pair<int, string>(NUM, strs[i] + strs[i + 1] + strs[i + 2]));
				i = i + 2;
			}
			else if ((strs[i + 1][0] == '+' || strs[i + 1][0] == '-') && isNum(strs[i + 2])) {
				vec.push_back(pair<int, string>(NUM, strs[i] + strs[i + 1] + strs[i + 2]));
				i = i + 2;
			}
			else {//无符号常量
				vec.push_back(pair<int, string>(NUM, strs[i]));
			}
		}
		else if (isKey(strs[i])) {//是否为关键字
			vec.push_back(pair<int, string>(isKey(strs[i]), strs[i]));
		}
		else if (isLetter(strs[i][0]) || strs[i][0] == '_') {//是否为变量名
			vec.push_back(pair<int, string>(ID, strs[i]));
		}
		else {//无法识别的单词
			vec.push_back(pair<int, string>(ERROR, strs[i]));
			errorNum++;
		}
	}
	return vec;
}

int main()
{
	//导入文件
	string filePath;
	cout << "请输入源代码文件路径：";
	cin >> filePath;
	filePath = "d:\\input.txt";
	ifstream inputfile(filePath); //构造一个ifstream并打开给定文件
	if (!inputfile){
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
	inputfile.close();//关闭文件
	//预处理
	filterSource(chars, num);
	cout << "预处理后程序如下\n" << chars << endl;
	
	//提取单词
	int begin = 0, end = 0; //单词的第一个和最后一个位置
	vector<string> array;
	do {
		begin = getFirstChar(chars, begin);
		string word = getWord(chars, begin, end);
		if (end == -1)	break;
		if (word.compare(" "))	array.push_back(word);
		begin = end + 1;
	} while (true);
	for (int i = 0; i < array.size(); i++) {
		cout << array[i] << endl;
	}
	vector< pair<int, string> > result = analyse(array);
	cout << "\n词法分析结果：\n< 类别 , 内容 >" << endl;
	for (unsigned int i = 0; i < result.size(); i++) {
		if (result[i].first > 0 && result[i].first < 20) {
			cout << "< 关键字 " << result[i].second << " >" << endl;
		}
		else if (result[i].first == 20) {
			cout << "< 标识符 " << result[i].second << " >" << endl;;
		}
		else if (result[i].first == 30) {
			cout << "< 常量 " << result[i].second << " >" << endl;
		}
		else if (result[i].first > 30 && result[i].first <= 39) {
			cout << "< 限定符 " << result[i].second << " >" << endl;
		}
		else if (result[i].first > 39 && result[i].first <= 50) {
			cout << "< 运算符 " << result[i].second << " >" << endl;
		}
		else if (result[i].first == -1) {
			cout << "< 无法识别的符号 " << result[i].second << " >" << endl;
		}
	}
	cout << "词法分析结束，有" << errorNum << "个无法识别的符号" << endl;
}