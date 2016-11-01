#pragma once
#define _UNICODE
#include <string>
#include <iostream>
#ifdef _UNICODE
#define _tstring wstring
#else
#define _tstring string
#endif
using namespace std;

typedef enum { S_ERROR = -257, S_BLANK = 0, S_BLANK_BUT_OUTPUT, S_ASSIGN, S_FIELD_BEGIN, S_FIELD_END, S_GOTO, S_GOTO_DEST, S_IF, S_ELSE, S_ELSE_IF, S_BREAK } STATEMENT_T;
class statement
{
public:
	statement() = delete;
	statement(int i, const _tstring &s, STATEMENT_T st) : lineNum(i), text(s), cmdType(st) {}
public:
	int lineNum;//�к�Ϊ��ֵʱ��������������к�
	_tstring text;
	STATEMENT_T cmdType;//���ڽ������ʹ���ִ����֮�����ͨѶ�������ظ�����
};

