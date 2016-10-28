//This is main frame by Recolic 
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <vector>
#include <iterator>
#include <algorithm>
#include <new>
#include "stdafx_.hpp"
#include "analyser.hpp"
#include "processor.hpp"

#define FRM_ERROR(error_code) { cout << "\nException occurred at recolic_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define ANA_ERROR(error_code) { cout << "\nException occurred at analyse_main():Error code is " << error_code << endl; system("pause"); return error_code; }
#define CPU_ERROR(error_code) { cout << "\nException occurred at processor_main():Error code is " << error_code << endl; system("pause"); return error_code; }
using namespace std;

deque<statement> buf;
vector<int> print_buffer;
shared_ptr<int> pMemory;

namespace recolic_frame
{
	void cut_comment(string *);
	bool is_var_name(const string &);
	size_t format_var_name();
	void cut_dup_ele_in_print();
}



int main()
{
	set_new_handler(global_new_handler);
	ifstream ifs("input.txt");
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
		buf.push_back(statement(frm_lineNum, frm_tmp_buf_, S_ERROR));
		++frm_lineNum;
		ifs.getline(frm_tmp_buf, 256);
	}
	size_t frm_memory_size = recolic_frame::format_var_name();
	pMemory = new int[VIRTUAL_MEMORY_SIZE] {};
	int analyse_error_code = analyse_main();
	if (analyse_error_code)
		ANA_ERROR(analyse_error_code);
	int processor_error_code = processor_main();
	if (processor_error_code)
		CPU_ERROR(processor_error_code);
	recolic_frame::cut_dup_ele_in_print();
	ostream_iterator<int> oi(cout, " ");
	copy(print_buffer.cbegin(), print_buffer.cend(), oi);
	return 0;
}

void recolic_frame::cut_comment(string *ps)
{
	//传入一个文本行，杀死注释和\t，杀死双引号内的东西
}

bool recolic_frame::is_var_name(const string &s_)
{
	//判断输入的字符串是否是一个合法的变量名
	string s = s_;
	remove_if(s.begin(), s.end(), [](char ch) -> bool {return ch == ' ';});//去空格
	//首尾去运算符
	if (s == "int" || s == "if" || s == "else" || s == "for" || s == "break" || s == "while" || s == "do" || s == "printf" || s == "return")
		return false;
	//断言不全为数字
	//断言所有字符合适
	return true;
}

size_t recolic_frame::format_var_name()
{
	return size_t();
}

void recolic_frame::cut_dup_ele_in_print()
{

}

void __cdecl global_new_handler()
{
	cout << "\nERROR:bad_alloc occurred! You need more memory!"<< endl;
	system("pause");
	exit(1);
}
