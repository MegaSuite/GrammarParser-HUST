#pragma once

#include<queue>
#include<stack>
#include"tokens_define.h"
#include"structure_define.h"
using namespace std;

#define OK 1
#define ERROR 0

struct print
{
	int indent;						//记录缩进值
	int line_num;					//记录缩进的行号
};

typedef int status;
extern int indent0;					//记录源代码的缩进值
extern queue<print> printList;		//存储各个打印格式单元
extern int w;						//获得GetToken函数的返回值即读入的单词种类编码
extern char kind[100];			    //存取类型关键字
extern char tokenText0[100];		//存取变量名或函数名

status Program(FILE* fp, CTree& T); //语法单位<程序>的子程序
status ExternalDefList(FILE* fp, CTree& T);//语法单位<外部定义序列>的子程序
status ExternalDef(FILE* fp, CTree& T);//语法单位<外部定义>的子程序
status ExternalVariableDef(FILE* fp, CTree& T);//语法单位<外部变量定义>子程序
status VariableList(FILE* fp, CTree& T);//语法单位<变量序列>子程序
status Function(FILE* fp, CTree& T);//语法单位<函数定义>子程序
status ParameterList(FILE* fp, CTree& T);//语法单位<形参序列>子程序
status FormParameterDef(FILE* fp, CTree& T);//语法单位<形参定义>子程序
status ComplexStat(FILE* fp, CTree& T);//语法单位<复合语句>子程序
status LocalVariableList(FILE* fp, CTree& T);//语法单位<局部变量定义序列>子程序
status LocalVariableDef(FILE* fp, CTree& T);//语法单位<局部变量定义>子程序
status StatementList(FILE* fp, CTree& T);//语法单位<语句序列>子程序
status Statement(FILE* fp, CTree& T);//语法单位<语句>子程序
status Expression(FILE* fp, CTree& T, int EndSym);//语法单位<表达式>子程序
char Precedence(const char* a, const char* b);//比较a与b的优先级
status PrintTree(char* data, int indent); //打印缩进
