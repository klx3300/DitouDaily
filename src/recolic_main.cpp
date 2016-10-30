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

#define FRM_ERROR(error_code) { cout << "\nException occurred at recolic_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define FRM_ERROR_(error_code) { cout << "\nException occurred at recolic_main():Error code is " << error_code << endl; system("pause"); exit(error_code); }
#define ANA_ERROR(error_code) { cout << "\nException occurred at analyse_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define CPU_ERROR(error_code) { cout << "\nException occurred at processor_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define RECOLIC_DEBUG
using namespace std;

deque<statement> input_buf;
deque<statement> buf;
vector<int> print_buffer;
int *pMemory = nullptr;
void __cdecl global_new_handler();

namespace recolic_frame
{
	void cut_comment(string *);
	bool is_var_name(const string &);
	size_t format_var_name(string *);
	bool current_line_is_comment = false;
	map<string, int> buf_map;
	std::vector<std::string> DivideString(const std::string &tod);
}

int main()
{
	set_new_handler(global_new_handler);
	ifstream ifs("D:\\visual studio 2015\\SeedCup2016\\Debug\\input.txt");
	if (!ifs)
		FRM_ERROR(3);
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
#ifdef RECOLIC_DEBUG
	for (size_t cter = 0;cter < input_buf.size();++cter)
	{
		cout << input_buf[cter].lineNum << "> " << input_buf[cter].text << endl;
	}
	cout << "********************************" << endl;
#endif
	int analyse_error_code = analyse_main();
#ifdef RECOLIC_DEBUG
	cout << "**********" << analyse_error_code << endl;
	for (size_t cter = 0;cter < buf.size();++cter)
	{
		cout << buf[cter].lineNum << "|Type=" << buf[cter].cmdType << "> " << buf[cter].text << endl;
	}
#endif
	if (analyse_error_code)
		ANA_ERROR(analyse_error_code);
	cout << "**********" << analyse_error_code << "Now CPU start." << endl;
	int processor_error_code = processor_main();
	if (processor_error_code)
		CPU_ERROR(processor_error_code);
	cout << "**********" << processor_error_code << endl;
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
		cout << endl << "Result is here:" << too << endl;
#endif
		ofstream os("D:\\visual studio 2015\\SeedCup2016\\Debug\\output.txt", ios::out);
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
		if (quote_p_ == string::npos)
		{
			FRM_ERROR_(5);
		}
		else
		{
			quote_p_ = ps->find(',', quote_p_);
			if(quote_p_ == string::npos)
				FRM_ERROR_(11);
			ps->erase(ps->begin() + quote_p, ps->begin() + quote_p_ + 1);
			goto re_cut_;
		}
	}
	//再杀双杠注释
	size_t qpq = ps->find("//");
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
			if (!(markch == ' ' M(0) M('-') M('*') M('/') M(';') M(',') M('.') M(')') M('(') M('+') M('>') M('<') M('=') M('{') M('}') M('"') M('?') M(':')))
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

void __cdecl global_new_handler()
{
	cout << "\nERROR:bad_alloc occurred! You need more memory!" << endl;
	system("pause");
	exit(1);
}

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
	if (thisPos != std::string::npos)
	{
		if (thisPos)
			sbuf.push_back(tod.substr(0, thisPos));
		goto gt_1;
	}
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