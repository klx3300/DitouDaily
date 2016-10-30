#pragma once
#include <string>
using namespace std;

typedef enum { S_ERROR = -257, S_BLANK = 0, S_BLANK_BUT_OUTPUT, S_ASSIGN, S_FIELD_BEGIN, S_FIELD_END, S_GOTO, S_GOTO_DEST, S_IF, S_ELSE, S_ELSE_IF, S_BREAK } STATEMENT_T;
class statement
{
public:
	statement() = delete;
	statement(int i, const string &s, STATEMENT_T st) : lineNum(i), text(s), cmdType(st) {}
public:
	int lineNum;//行号为负值时，代表无需输出行号
	string text;
	STATEMENT_T cmdType;//用于解析器和代码执行器之间进行通讯，减少重复解析
};

