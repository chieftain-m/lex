#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;
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
}