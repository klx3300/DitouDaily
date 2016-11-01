//本头文件用于把程序中所有的字符处理改成宽字符(因为在注意到UTF编码问题的时候我们的代码都已经基本完成，所以不得不使用这种方式来统一调整字符编码方案)
#pragma once
#ifdef _UNICODE
#define RECOLIC_TEXT(txt) L##txt
#else
#define RECOLIC_TEXT(txt) txt
#endif
//检测到Linux下编译wmain出现了鬼畜的错误，只好不用wmain
//#define main wmain
#define string wstring
#define to_string to_wstring
#define stringstream wstringstream
#define istringstream wistringstream
#define ostringstream wostringstream
#define ifstream wifstream
#define ofstream wofstream
#define fstream wfstream
#define char wchar_t
#define cout wcout
#define printf wprintf
#define cerr wcerr
#define cin wcin
#define clog wclog
