//本头文件定义了非常重要的statement类
#pragma once
#define _UNICODE
#include <string>
#include <iostream>
#ifdef _UNICODE
#define _tstring wstring
#else
#define _tstring string
#endif
using namespace std;//由于工程足够小，在头文件中包含这一条语句是比较方便的

typedef enum { S_ERROR = -257, S_BLANK = 0, S_BLANK_BUT_OUTPUT, S_ASSIGN, S_FIELD_BEGIN, S_FIELD_END, S_GOTO, S_GOTO_DEST, S_IF, S_ELSE, S_BREAK } STATEMENT_T;
class statement
{
public:
	statement() = delete;//防止不带参数的直接构造造成问题
	statement(int i, const _tstring &s, STATEMENT_T st) : lineNum(i), text(s), cmdType(st) {}
public:
	int lineNum;//这个语句在原始文本中的行号。解析器可以把这一项用负数填充来表示模拟CPU无需为其输出行号
	_tstring text;
	STATEMENT_T cmdType;//表示了这一条语句的类型，用于简化模拟CPU的操作
};
