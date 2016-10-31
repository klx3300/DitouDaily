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
#include "stdafx_.hpp"
#include "analyser.hpp"
#include "processor.hpp"

//#define FRM_ERROR(error_code) { cout << "\nSoftware exception occurred at recolic_main():Error code is " << error_code << endl; system("pause"); return error_code; }
//#define FRM_ERROR_(error_code) { cout << "\nSoftware exception occurred at recolic_main.cpp:Error code is " << error_code << endl; system("pause"); exit(error_code); }
//#define ANA_ERROR(error_code) { cout << "\Software exception occurred at analyse_main():Error code is " << error_code << endl; system("pause"); return error_code; }
//#define CPU_ERROR(error_code) { cout << "\Software exception occurred at processor_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define RECOLIC_DEBUG

#ifdef _WIN32
#include <Windows.h>
#endif
using namespace std;

deque<statement> input_buf;
deque<statement> buf;
vector<int> print_buffer;
int *pMemory = nullptr;
//void __cdecl global_new_handler();

namespace recolic_frame
{
	void cut_comment(string *);
	bool is_var_name(const string &);
	size_t format_var_name(string *);
	bool current_line_is_comment = false;
	map<string, int> buf_map;
	std::vector<std::string> DivideString(const std::string &tod);
	string formatH(int i)
	{
		switch (i)
		{
		case S_BLANK:
			return "S_BLANK";
		case S_BLANK_BUT_OUTPUT:
			return "S_BLANK_BUT_OUTPUT";
		case S_ASSIGN:
			return "S_ASSIGN";
		case S_FIELD_BEGIN:
			return "S_FIELD_BEGIN";
		case S_FIELD_END:
			return "S_FIELD_END";
		case S_GOTO:
			return "S_GOTO";
		case S_GOTO_DEST:
			return "S_GOTO_DEST";
		case S_IF:
			return "S_IF";
		case S_ELSE:
			return "S_ELSE";
		case S_ELSE_IF:
			return "S_ELSE_IF";
		case S_BREAK:
			return "S_BREAK";
		case S_ERROR:
			return "S_ERROR";
		default:
			return "ERROR_NOTFOUNT";
		}
	}
	void fuck_do_while();
}

int main()
{
		//set_new_handler(global_new_handler);
		ifstream ifs("input.txt");
		//if (!ifs)
			//FRM_ERROR(3);
		char frm_tmp_buf[256] = { 0 };
		string frm_tmp_buf_;
		ifs.getline(frm_tmp_buf, 256);
		int frm_lineNum = 1;
		while (!ifs.eof())
		{
			frm_tmp_buf_ = frm_tmp_buf;
			recolic_frame::cut_comment(&frm_tmp_buf_);
			recolic_frame::format_var_name(&frm_tmp_buf_);
			input_buf.push_back(statement(frm_lineNum, frm_tmp_buf_, S_ERROR));
			++frm_lineNum;
			ifs.getline(frm_tmp_buf, 256);
		}
		frm_tmp_buf_ = frm_tmp_buf;
		recolic_frame::cut_comment(&frm_tmp_buf_);
		recolic_frame::format_var_name(&frm_tmp_buf_);
		input_buf.push_back(statement(frm_lineNum, frm_tmp_buf_, S_ERROR));
		ifs.close();
		recolic_frame::fuck_do_while();
#ifdef RECOLIC_DEBUG
		for (size_t cter = 0;cter < input_buf.size();++cter)
		{
			cout << input_buf[cter].lineNum << "> " << input_buf[cter].text << endl;
		}
		cout << "********************************\nNow try to launch Compiler...\n" << endl;
#endif
		int analyse_error_code = analyse_main();
#ifdef RECOLIC_DEBUG
		for (size_t cter = 0;cter < buf.size();++cter)
		{
			cout << buf[cter].lineNum << "|" << recolic_frame::formatH(buf[cter].cmdType) << "> " << buf[cter].text << endl;
		}
		cout << "Now try to launch CPU...\n" << endl;
#endif
		//if (analyse_error_code)
		//	ANA_ERROR(analyse_error_code);
		int processor_error_code = processor_main();
		//if (processor_error_code)
		//	CPU_ERROR(processor_error_code);
		{ //Final deal
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
#ifdef RECOLIC_DEBUG
			cout << "\nResult is here:" << too << endl;
#endif
			ofstream os("output.txt", ios::out);
			os << too;
			os.close();
		}
		return 0;
}

void recolic_frame::cut_comment(string *ps)
{
	//先杀跨行注释
	size_t comment_e = string::npos;
	if (recolic_frame::current_line_is_comment)
	{
		comment_e = ps->find("*/");
		if (comment_e == string::npos)
		{
			ps->assign("");
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
	size_t comment_b = ps->find("/*");
	if (comment_b != string::npos)
	{
		comment_e = ps->find("*/", comment_b + 2);
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
		//if (quote_p_ == string::npos)
		//{
		//	FRM_ERROR_(5);
		//}
        quote_p_ = ps->find(',', quote_p_);
        if (quote_p_ == string::npos)
        {
            quote_p_ = ps->find(')', quote_p__);
            //if (quote_p_ == string::npos)
            //	FRM_ERROR_(11);
            ps->erase(ps->begin() + quote_p, ps->begin() + quote_p_);
            goto gt_t_out;
        }
        ps->erase(ps->begin() + quote_p, ps->begin() + quote_p_ + 1);
        goto re_cut_;
	}
    gt_t_out:
size_t qpq = ps->find("//");
	//再杀双杠注释
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
	if (s == "int" || s == "if" || s == "else" || s == "for" || s == "break" || s == "while" || s == "do" || s == "printf" || s == "return")
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
			if (!(markch == ' ' M('\0') M('-') M('*') M('/') M(';') M(',') M('.') M(')') M('(') M('+') M('>') M('<') M('=') M('{') M('}') M('"') M('?') M(':')))
			{
				cp = ps->find(usedHash[cter], cp + 1);
				cpl = cp + usedHash[cter].size();
				cOK = true;
			}
		} while (cOK);
		ps->erase(ps->begin() + cp, ps->begin() + cpl);
		string toi = "_____##@@";
		toi += to_string(buf_map[usedHash[cter]]);
		ps->insert(cp, toi);
	}
	return max_var_num;
}

//void __cdecl global_new_handler()
//{
//    cout << "\nERROR:bad_alloc occurred! You need more memory!" << endl;
//    system("pause");
//    exit(1);
//}

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
			if (ich == ' ' I('-') I('*') I('/') I(';') I(',') I('.') I(')') I('(') I('+') I('>') I('<') I('=') I('{') I('}') I('"') I('?') I(':'))
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
	auto last = remove_if(input_buf.begin(), input_buf.end(), [](const statement &s) -> bool {return s.text.empty();});//remove empty lines.
	input_buf.erase(last, input_buf.end());
	bool must_erase = false;
	bool *pmust_erase = &must_erase;
search_again:
	must_erase = false;
	auto pst = find_if(input_buf.begin(), input_buf.end(), [pmust_erase](statement &s) -> bool {
		size_t pt = s.text.find("do", 0);
		if (pt == string::npos)
			return false;
		if (s.text.find(' ') != string::npos)
		{
			auto last_ = remove_if(s.text.begin(), s.text.end(), [](char ch) -> bool {return ch == ' ';});//remove empty characters.
			s.text.erase(last_, s.text.end());
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
	(pst + 1)->text = "do" + (pst + 1)->text;
	goto search_again;
}
