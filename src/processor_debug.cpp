#include <map>
#include <iostream>
#include <deque>
#include <sstream>
#include <vector>
#include <stack>
#include <memory>
#include <string>
using namespace std; 
typedef enum { S_ERROR = -257, S_BLANK = 0, S_BLANK_BUT_OUTPUT, S_ASSIGN, S_FIELD_BEGIN, S_FIELD_END, S_GOTO, S_GOTO_DEST, S_BREAK, S_IF, S_ELSE, S_ELSE_IF } STATEMENT_T;
class statement
{
public:
	statement() = delete;
	statement(int i, const std::string &s, STATEMENT_T st) : lineNum(i), text(s), cmdType(st) {}
public:
	int lineNum;//�к�Ϊ��ֵʱ��������������к�
	string text;
	STATEMENT_T cmdType;//���ڽ������ʹ���ִ����֮�����ͨѶ�������ظ�����
};
#define VIRTUAL_MEMORY_SIZE 256
using namespace std;
deque<statement> buf;
shared_ptr<int> pMemory;
vector<int> print_buffer;
void __cdecl global_new_handler();
using namespace std;
int tempValue[65535];

map<int,pair<int,int> > allValue;//number cnt value
stringstream ss;//get every thing from the string
int fieldCnt=0;//count how many field it has

void New_Value(int Number)
{
	allValue.insert(make_pair(Number,make_pair(-fieldCnt,0)));
}

int Give_Value(int Number,int Value)
{
	map<int,pair<int,int> >::iterator now=allValue.find(Number);
	if(now==allValue.end())
			return -1;
	(*now).second.second=Value;
	return 0;
}

int Get_Value(int Number)
{
	map<int,pair<int,int> >::iterator now=allValue.find(Number);
	return (*now).second.second;
}

int Get_Value_Double(int Number)
{
	if(Number<0)
		return tempValue[-Number];
	else
		return Get_Value(Number);
}

int processor_main()//If no error occurred, return 0.
{
	for(deque<statement>::iterator now_it=buf.begin();now_it!=buf.end();++now_it)
	{
		statement now_state=(*now_it);
		switch(now_state.cmdType)
		{
			case S_ERROR:
			{
				return S_ERROR;
			}
			case S_BLANK:
			{
				break;
			}
			case S_BLANK_BUT_OUTPUT:
			{
				print_buffer.push_back(now_state.lineNum);
				break;
			}
			case S_GOTO_DEST:
			{
				break;
			}
			case S_FIELD_BEGIN:
			{
				++fieldCnt;
				break;
			}
			case S_FIELD_END:
			{
				--fieldCnt;
				map<int,pair<int,int> >::iterator tmp;
				for(map<int,pair<int,int> >::iterator now=allValue.begin();now!=allValue.end();)
				{
					if((*now).second.first<-fieldCnt)
					{
						tmp=now;
						++now;
						allValue.erase(tmp);
					}
					else	
						++now;
				}
				break;
			}
			case S_GOTO:
			{
				int Loc_fieldCnt=fieldCnt;
				for(deque<statement>::iterator now_it_tmp=now_it;now_it_tmp!=buf.begin();--now_it_tmp)
				{
					if((*now_it_tmp).cmdType==S_FIELD_BEGIN)
						++fieldCnt;
					if((*now_it_tmp).cmdType==S_FIELD_END)
						--fieldCnt;
					if((*now_it_tmp).cmdType==S_GOTO_DEST&&Loc_fieldCnt==fieldCnt)
					{
						now_it=now_it_tmp;
						break;
					}
				}
			}
			case S_ASSIGN:
			{
				string everyMember[4];
				int tmpValue[4];
				if(now_state.lineNum>0)
					print_buffer.push_back(now_state.lineNum);
				ss.clear();
				ss<<now_state.text;
				ss>>everyMember[0];
				if(everyMember[0]=="int")
				{
					ss>>everyMember[1];
					ss.clear();
					ss<<everyMember[1].substr(9);
					ss>>tmpValue[0];
					//cout<<tmpValue<<endl;
					New_Value(tmpValue[0]);
				}
				else
				{
					ss>>everyMember[1];
					ss>>everyMember[1];
					if(ss.eof())
					{
						ss.clear();
						ss<<everyMember[0].substr(9);
						ss>>tmpValue[0];
						if(everyMember[1][0]=='_')
						{
							ss.clear();
							ss<<everyMember[1].substr(9);
							ss>>tmpValue[1];
						}
						else
						{
							ss.clear();
							ss<<everyMember[1];
							ss>>tmpValue[1];
						}
						if(tmpValue[0]<0)
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1]);
						else
							Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1]));
					}
					else
					{
						ss>>everyMember[2];
						ss>>everyMember[3];
						ss.clear();
						ss<<everyMember[0].substr(9);
						ss>>tmpValue[0];
						if(everyMember[1][0]=='_')
						{
							ss.clear();
							ss<<everyMember[1].substr(9);
							ss>>tmpValue[1];
						}
						else
						{
							ss.clear();
							ss<<everyMember[1];
							ss>>tmpValue[1];
						}
						if(everyMember[3][0]=='_')
						{
							ss.clear();
							ss<<everyMember[3].substr(9);
							ss>>tmpValue[3];
						}
						else
						{
							ss.clear();
							ss<<everyMember[3];
							ss>>tmpValue[3];
						}
						if(tmpValue[0]<0)
						{
							if(everyMember[2]=="+")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])+((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="-")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])-((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="*")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])*((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="/")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])/((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]==">")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])>((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="<")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])<((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]==">=")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])>=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="<=")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])<=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="!=")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])!=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
							else if(everyMember[2]=="==")
								tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])==((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						}
						else
						{
							if(everyMember[2]=="+")
								Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])+((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
							else if(everyMember[2]=="-")
								Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])-((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
							else if(everyMember[2]=="*")
								Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])*((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
							else if(everyMember[2]=="/")
								Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])/((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
						}
					}
				}
				break;
			}
			case S_BREAK:
			{
				if(now_state.lineNum>0)
					print_buffer.push_back(now_state.lineNum);
				while((*now_it).cmdType!=S_GOTO)
				{
					++now_it;
					if((*now_it).cmdType==S_FIELD_BEGIN)
						++fieldCnt;
					if((*now_it).cmdType==S_FIELD_END)
					{
						--fieldCnt;
						map<int,pair<int,int> >::iterator tmp;
						for(map<int,pair<int,int> >::iterator now=allValue.begin();now!=allValue.end();)
						{
							if((*now).second.first<-fieldCnt)
							{
								tmp=now;
								++now;
								allValue.erase(tmp);
							}
							else	
								++now;
						}
					}
				}
				break;
			}
			case S_IF:
			{
				string everyMember;
				bool flag;
				int tmpValue;
				if(now_state.lineNum>0)
					print_buffer.push_back(now_state.lineNum);
				ss.clear();
				ss<<now_state.text;
				ss>>everyMember;
				if(everyMember[0]=='_')
				{
					ss.clear();
					ss<<everyMember.substr(9);
					ss>>tmpValue;
					if(tmpValue>0)
						flag=(Get_Value(tmpValue)!=0);
					else
						flag=(tempValue[-tmpValue]!=0);
				}
				else
				{
					ss.clear();
					ss<<everyMember;
					ss>>tmpValue;
					flag=(tmpValue!=0);
				}
				if(!flag)
				{
					int Loc_fieldCnt=fieldCnt;
					while(fieldCnt>Loc_fieldCnt-1)
					{
						++now_it;
						if((*now_it).cmdType==S_FIELD_BEGIN)
							++fieldCnt;
						if((*now_it).cmdType==S_FIELD_END)
						{
							--fieldCnt;
							map<int,pair<int,int> >::iterator tmp;
							for(map<int,pair<int,int> >::iterator now=allValue.begin();now!=allValue.end();)
							{
								if((*now).second.first<-fieldCnt)
								{
									tmp=now;
									++now;
									allValue.erase(tmp);
								}
								else	
									++now;
							}
						}
					}
				}
			}
			case S_ELSE:
			{
				if(now_state.lineNum>0)
					print_buffer.push_back(now_state.lineNum);
				break;
			}
		}
	}
	return 0;
}

int main()
{
	buf.push_back(statement(1,"int _____@@##1",S_ASSIGN));
	buf.push_back(statement(2,"_____@@##1 = 1",S_ASSIGN));
	buf.push_back(statement(3,"_____@@##-1 = 2 + 1",S_ASSIGN));
	buf.push_back(statement(4,"_____@@##1 = _____@@##-1 + _____@@##1",S_ASSIGN));
	processor_main();
	//printf("%d\n",Get_Value(1));
	//printf("%d\n",Get_Value_Double(-1));
	for(int i=0;i<print_buffer.size();++i)
		printf("%d\n",print_buffer[i]);
	return 0;
}