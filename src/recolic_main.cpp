//This is main frame by Recolic 
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <vector>
#include <iterator>
#include <algorithm>
#include <new>
#include <map>
#include <cctype>
#include <sstream>
#include <ciso646>
#include "utf8.h"//支持UTF8和其他编码的转换
#include "stdafx_.hpp"//公有头文件
#include "analyser.hpp"//定义了解析器的过程
#include "processor.hpp"//定义了模拟CPU的过程

//这里定义了通用的报错格式
#define FRM_ERROR(error_code) { cout << "\nSoftware exception occurred at recolic_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define FRM_ERROR_(error_code) { cout << "\nSoftware exception occurred at recolic_main.cpp:Error code is " << error_code << endl; system("pause"); exit(error_code); }
#define ANA_ERROR(error_code) { cout << "\nSoftware exception occurred at analyse_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define CPU_ERROR(error_code) { cout << "\nSoftware exception occurred at processor_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define RECOLIC_DEBUG//这个开关控制是否输出调试信息

#include "w_fix.hpp"//必须最后包含的头文件
using namespace std;

deque<statement> input_buf;//主框架负责填充这个容器，把稍微格式化的语句传给解析器
deque<statement> buf;//这是解析器处理过的可以由模拟CPU直接运行的语句的容器
vector<int> print_buffer;//这里存放模拟CPU执行的语句顺序(没有除去重复的语句序号)
int *pMemory = nullptr;//指向虚拟内存空间的指针

namespace recolic_frame//定义了只有框架中才用到的方法和变量，习惯性防止冲突
{
	//此处的很多函数的注释直接看main(wmain)函数内的注释
	void cut_comment(string *);
	bool is_var_name(const string &);//判断传入的字符串是否可能是一个有效的变量名(而不是保留字或运算符或常量数)
	size_t format_var_name(string *);
	bool current_line_is_comment = false;//标记当前正在读取的行是否是多行注释的一部分，便于移除多行的注释
	map<string, int> buf_map;//定义了变量名字和虚拟内存地址的映射表
	std::vector<std::string> DivideString(const std::string &tod);//这个方法由一个分割字符串的函数修改而来
	string formatH(int i)//这个函数是为了在调试的时候将数字转换成宏的名字，便于调试
	{
		switch (i)
		{
		case S_BLANK:
			return RECOLIC_TEXT("S_BLANK");
		case S_BLANK_BUT_OUTPUT:
			return RECOLIC_TEXT("S_BLANK_BUT_OUTPUT");
		case S_ASSIGN:
			return RECOLIC_TEXT("S_ASSIGN");
		case S_FIELD_BEGIN:
			return RECOLIC_TEXT("S_FIELD_BEGIN");
		case S_FIELD_END:
			return RECOLIC_TEXT("S_FIELD_END");
		case S_GOTO:
			return RECOLIC_TEXT("S_GOTO");
		case S_GOTO_DEST:
			return RECOLIC_TEXT("S_GOTO_DEST");
		case S_IF:
			return RECOLIC_TEXT("S_IF");
		case S_ELSE:
			return RECOLIC_TEXT("S_ELSE");
		case S_ELSE_IF:
			return RECOLIC_TEXT("S_ELSE_IF");
		case S_BREAK:
			return RECOLIC_TEXT("S_BREAK");
		case S_ERROR:
			return RECOLIC_TEXT("S_ERROR");
		default:
			return RECOLIC_TEXT("ERROR_NOTFOUNT");
		}
	}
	void fuck_do_while();//特殊需求：修改do-while的格式
#undef ifstream
	wstring _ugetline(ifstream &);//临时取消了ifstream到wifstream的宏定义，便于进行ANSI、UTF8、UNICODE的灵活操作
#define ifstream wifstream
}

int main()
{
#undef ifstream
	ifstream ifs("input.txt");//临时取消了ifstream到wifstream的宏定义，便于进行ANSI、UTF8、UNICODE的灵活操作
#define ifstream wifstream
	if (!ifs)//文件打开失败？
		FRM_ERROR(3);
	string frm_tmp_buf_;//实际上是wstring,详情看w_fix.hpp
	int frm_lineNum = 1;
	while (!ifs.eof())
	{
		frm_tmp_buf_ = recolic_frame::_ugetline(ifs);//读到一行
		recolic_frame::cut_comment(&frm_tmp_buf_);//把注释砍掉(包括单行，多行，行内，\t，\r，且会把printf第一个参数直接击杀)
		recolic_frame::format_var_name(&frm_tmp_buf_);//把各种变量名字统一格式化为 _____##@@ 加上一个数字
		input_buf.push_back(statement(frm_lineNum, frm_tmp_buf_, S_ERROR));//把处理的差不多的这行文本写入input_buf，准备传给解析器，原始行号在这时和语句绑定
		++frm_lineNum;
	}
	ifs.close();//打开的文件要关闭
	recolic_frame::fuck_do_while();//把解析器的do-while的一点问题解决，详见本函数的内容的注释
#ifdef RECOLIC_DEBUG//输出调试信息
	for (size_t cter = 0;cter < input_buf.size();++cter)
	{
		cout << input_buf[cter].lineNum << RECOLIC_TEXT("> ") << input_buf[cter].text << endl;
	}
	cout << RECOLIC_TEXT("********************************\nNow try to launch Compiler...\n") << endl;
#endif
	int analyse_error_code = analyse_main();//调用解析器回调函数并取得错误码
#ifdef RECOLIC_DEBUG//输出调试信息
	for (size_t cter = 0;cter < buf.size();++cter)
	{
		cout << buf[cter].lineNum << RECOLIC_TEXT("|") << recolic_frame::formatH(buf[cter].cmdType) << RECOLIC_TEXT("> ") << buf[cter].text << endl;
	}
	cout << RECOLIC_TEXT("Now try to launch CPU...\n") << endl;
#endif
	if (analyse_error_code)
		ANA_ERROR(analyse_error_code);
	int processor_error_code = processor_main();//调用虚拟CPU回调函数并取得错误码
	if (processor_error_code)
		CPU_ERROR(processor_error_code);
	{ //先去掉连续重复的行号
		int last_success = -1;
		stringstream ss;
		for (size_t cter = 0;cter < print_buffer.size();++cter)
		{
			if (print_buffer[cter] != last_success)
			{
				last_success = print_buffer[cter];
				ss << last_success << ' ';
			}
		}
		string too = ss.str();
		too = too.substr(0, too.size() - 1);
#ifdef RECOLIC_DEBUG//输出调试信息
		cout << RECOLIC_TEXT("\nResult is here:") << too << endl;
#endif
		ofstream os("output.txt", ios::out);//输出结果
		os << too;
		os.close();
	}//程序不暂停
	return 0;
}

void recolic_frame::cut_comment(string *ps)
{
	//先杀跨行注释
	size_t comment_e = string::npos;
	if (recolic_frame::current_line_is_comment)
	{
		comment_e = ps->find(RECOLIC_TEXT("*/"));
		if (comment_e == string::npos)
		{
			ps->assign(RECOLIC_TEXT(""));
			return;
		}
		else
		{
			*ps = ps->substr(comment_e + 2);
			recolic_frame::current_line_is_comment = false;
			//Do not return!
		}
	}
re_cut:
	//再杀行内注释(考虑跨行)
	size_t comment_b = ps->find(RECOLIC_TEXT("/*"));
	if (comment_b != string::npos)
	{
		comment_e = ps->find(RECOLIC_TEXT("*/"), comment_b + 2);
		if (comment_e == string::npos)
		{
			recolic_frame::current_line_is_comment = true;
			*ps = ps->substr(0, comment_b);
			return;
		}
		else
		{
			ps->erase(ps->begin() + comment_b, ps->begin() + comment_e + 2);
			goto re_cut;
		}
	}
re_cut_:
	//再杀双引号内容
	size_t quote_p = ps->find('"');
	if (quote_p != string::npos)
	{
		size_t quote_p_ = ps->find('"', quote_p + 1);
		const size_t quote_p__ = quote_p_;
		if (quote_p_ == string::npos)
		{
			FRM_ERROR_(5);
		}
		else
		{
			quote_p_ = ps->find(',', quote_p_);
			if (quote_p_ == string::npos)
			{
				quote_p_ = ps->find(')', quote_p__);
				if (quote_p_ == string::npos)
					FRM_ERROR_(11);
				ps->erase(ps->begin() + quote_p, ps->begin() + quote_p_);
				goto gt_t_out;
			}
			ps->erase(ps->begin() + quote_p, ps->begin() + quote_p_ + 1);
			goto re_cut_;
		}
	}
gt_t_out:
	//再杀双杠注释
	size_t qpq = ps->find(RECOLIC_TEXT("//"));
	*ps = ps->substr(0, qpq);
	//杀光\t\r完事
	auto last_it = remove_if(ps->begin(), ps->end(), [](char ch) -> bool { return ch == '\t' || ch == '\r'; });
	ps->erase(last_it, ps->end());
	return;
}

bool recolic_frame::is_var_name(const string &s)
{
	//判断输入的字符串是否是一个合法的变量名
	if (s.empty())
		return false;
	if (s == RECOLIC_TEXT("int") || s == RECOLIC_TEXT("if") || s == RECOLIC_TEXT("else") || s == RECOLIC_TEXT("for")|| s == RECOLIC_TEXT("break") || s == RECOLIC_TEXT("while") || 
		s == RECOLIC_TEXT("do") || s == RECOLIC_TEXT("printf") || s == RECOLIC_TEXT("return"))
		return false;
	if (isdigit(s[0]))
		return false;
	bool all_num = true;
	for (char chi : s)
	{
		if (isalpha(chi) || chi == '_')
			all_num = false;
		else if (isdigit(chi))
			;
		else
			return false;
	}
	if (all_num)
		return false;
	else
		return true;
}

#define M(ch) ||markch==ch
size_t recolic_frame::format_var_name(string *ps)
{
	//以运算符和空格为界。
	//先构建Hash表
	vector<string> usedHash;
	auto rets = DivideString(*ps);
	static int max_var_num = 0;
	for (size_t cter = 0;cter < rets.size();++cter)
	{
		const string &ts = rets[cter];
		if (is_var_name(ts))
		{
			try {
				buf_map.at(ts);
			}
			catch (out_of_range &)
			{
				++max_var_num;
				buf_map[ts] = max_var_num;
			}
			usedHash.push_back(ts);
		}
	}
	//再根据Hash表替换
	for (size_t cter = 0;cter < usedHash.size();++cter)
	{
		size_t cp = ps->find(usedHash[cter]);
		size_t cpl = cp + usedHash[cter].size();
		bool cOK = false;
		do
		{
			cOK = false;
			char markch = (*ps)[cpl];
			char markch_ = (*ps)[(cp ? cp - 1 : cp)];
			if (!((markch == ' ' M('\0') M('-') M('*') M('/') M(';') M(',') M('.') M(')') M('(') M('+') M('>') M('<') M('=') M('{') M('}') M('"') M('?') M(':') M('!')) && 
				(cp ? (markch_ == ' ' M('\0') M('-') M('*') M('/') M(';') M(',') M('.') M(')') M('(') M('+') M('>') M('<') M('=') M('{') M('}') M('"') M('?') M(':') M('!')) : true  )
				))
			{
				cp = ps->find(usedHash[cter], cp + 1);
				cpl = cp + usedHash[cter].size();
				cOK = true;
			}

		} while (cOK);
		ps->erase(ps->begin() + cp, ps->begin() + cpl);
		string toi = RECOLIC_TEXT("_____##@@");
		toi += to_string(buf_map[usedHash[cter]]);
		ps->insert(cp, toi);
	}
	return max_var_num;
}

/*void __cdecl global_new_handler()
{
cout << "\nERROR:bad_alloc occurred! You need more memory!" << endl;
system("pause");
exit(1);
}*/

#define I(ch) ||ich==ch
std::vector<std::string> recolic_frame::DivideString(const std::string &tod)
{
	if (tod.empty())
		return vector<string>();
	auto o_find = [](const string &istr, size_t ioffset = 0) -> size_t {
		size_t cter = ioffset;
		for (;cter < istr.size();++cter)
		{
			char ich = istr[cter];
			if (ich == ' ' I('-') I('*') I('/') I(';') I(',') I('.') I(')') I('(') I('+') I('>') I('<') I('=') I('{') I('}') I('"') I('?') I(':') I('!'))
				break;
		}
		if (cter == istr.size())
			return string::npos;
		else
			return cter;
	};
	size_t lastPos = 0;
	size_t thisPos = o_find(tod);
	std::vector<std::string> sbuf;
	if (thisPos == string::npos)
		return vector<string>{tod};
	if (thisPos)
		sbuf.push_back(tod.substr(0, thisPos));
	goto gt_1;
	do {
		if (thisPos - lastPos - 1)
			sbuf.push_back(tod.substr(lastPos + 1, thisPos - lastPos - 1));
	gt_1:
		lastPos = thisPos;
		thisPos = o_find(tod, lastPos + 1);
	} while (thisPos != std::string::npos);
	if (lastPos + 1 != tod.size())
		sbuf.push_back(tod.substr(lastPos + 1));
	return sbuf;
}


void recolic_frame::fuck_do_while()
{
	//把所有的do自动和下一行(或者隔几个回车)的左花括号连接起来
	auto last = remove_if(input_buf.begin(), input_buf.end(), [](const statement &s) -> bool {return s.text.empty();});//remove empty lines.
	input_buf.erase(last, input_buf.end());
	bool must_erase = false;
	bool *pmust_erase = &must_erase;
search_again:
	must_erase = false;
	auto pst = find_if(input_buf.begin(), input_buf.end(), [pmust_erase](statement &s) -> bool {
		size_t pt = s.text.find(RECOLIC_TEXT("do"), 0);
		if (pt == string::npos)
			return false;
		if (s.text.find(' ') != string::npos)
		{
			auto last_ = remove_if(s.text.begin(), s.text.end(), [](char ch) -> bool {return ch == ' ';});//remove empty characters.
			s.text.erase(last_, s.text.end());
			pt = s.text.find(RECOLIC_TEXT("do"), 0);
		}
		if (s.text.size() > pt + 2)
		{
			if (s.text[pt + 2] == '{')
				return false;
		}
		*pmust_erase = true;
		return true;
	});//寻找非法do语句
	if (!must_erase)
		return;
	pst->text.erase(pst->text.end() - 2, pst->text.end());
	(pst + 1)->text = RECOLIC_TEXT("do") + (pst + 1)->text;
	goto search_again;
}

#undef ifstream
#undef string
#undef char
//临时取消了ifstream到wifstream的宏定义，便于进行ANSI、UTF8、UNICODE的灵活操作
wstring recolic_frame::_ugetline(ifstream &is)
{
	char buf[256] = { 0 };
	string sbuf;
	wstring wsbuf;
	is.getline(buf, 256);
	sbuf = buf;
	utf8::utf8to16(sbuf.begin(), sbuf.end(), back_inserter(wsbuf));
	if (!wsbuf.empty())
	{
		if (wsbuf[0] == static_cast<wchar_t>(65279))
			wsbuf.erase(0, 1);
	}
	return wsbuf;
}
#define ifstream wifstream
