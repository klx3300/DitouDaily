#ifdef _UNICODE
#define RECOLIC_TEXT(txt) L##txt
#else
#define RECOLIC_TEXT(txt) txt
#endif
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
