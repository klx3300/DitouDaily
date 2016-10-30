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
	int lineNum;
	string text;
	STATEMENT_T cmdType;
};
#define VIRTUAL_MEMORY_SIZE 256
using namespace std;
deque<statement> buf;
shared_ptr<int> pMemory;
vector<int> print_buffer;

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
        allValue.insert(make_pair(Number,make_pair(-fieldCnt,Value)));
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

int Calc(deque<statement>::iterator &now_it)
{
    //cout<<(*now_it).text<<endl;
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
					--fieldCnt;
				if((*now_it_tmp).cmdType==S_FIELD_END)
					++fieldCnt;
				if((*now_it_tmp).cmdType==S_GOTO_DEST&&Loc_fieldCnt==fieldCnt)
				{
					now_it=now_it_tmp;
					break;
				}
			}
			fieldCnt=Loc_fieldCnt;
			break;
		}
		case S_ASSIGN:
		{
			string everyMember[4];
			int tmpValue[4];
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
                if(now_state.lineNum>0)
				print_buffer.push_back(now_state.lineNum);
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
            if(now_state.lineNum>0)
                print_buffer.push_back(now_state.lineNum);
			string everyMember;
			bool flag;
			int tmpValue;
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
				break;
			}
			else
            {
                int Loc_fieldCnt=fieldCnt-1;
                deque<statement>::iterator Loc_now_it=now_it;
                while(fieldCnt!=Loc_fieldCnt)
                {
                    ++now_it;
					if((*now_it).cmdType==S_FIELD_BEGIN)
						++fieldCnt;
					if((*now_it).cmdType==S_FIELD_END)
						--fieldCnt;
                }
                ++now_it;
                if((*now_it).cmdType!=S_FIELD_BEGIN)
                {
                    fieldCnt=Loc_fieldCnt+1;
                    now_it=Loc_now_it;
                    break;
                }
                ++now_it;
                ++fieldCnt;
                if((*now_it).cmdType!=S_ELSE)
                {
                    fieldCnt=Loc_fieldCnt+1;
                    now_it=Loc_now_it;
                    break;
                }
                while(fieldCnt!=Loc_fieldCnt)
                {
                    ++now_it;
                    if((*now_it).cmdType!=S_FIELD_BEGIN&&(*now_it).cmdType!=S_FIELD_END)
                        (*now_it).cmdType=S_BLANK,(*now_it).lineNum=-1;
                    else
                        if((*now_it).cmdType==S_FIELD_BEGIN)
                            ++fieldCnt;
                        else
                            --fieldCnt;
                }
                fieldCnt=Loc_fieldCnt+1;
                now_it=Loc_now_it;
                break;
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


int processor_main()//If no error occurred, return 0.
{
	for(deque<statement>::iterator now_it=buf.begin();now_it!=buf.end();++now_it)
		Calc(now_it);
	return 0;
}


int main()
{
    buf.clear();
	/*buf.push_back(statement(1,"int _____@@##1",S_ASSIGN));
    buf.push_back(statement(2,"",S_FIELD_BEGIN));
    buf.push_back(statement(3,"1",S_IF));
    buf.push_back(statement(4,"",S_FIELD_BEGIN));
    buf.push_back(statement(5,"",S_FIELD_END));
    buf.push_back(statement(6,"",S_FIELD_END));
    buf.push_back(statement(7,"",S_FIELD_BEGIN));
    buf.push_back(statement(8,"",S_ELSE));
    buf.push_back(statement(9,"",S_FIELD_BEGIN));
    buf.push_back(statement(10,"_____@@##1 = 1",S_ASSIGN));
    buf.push_back(statement(11,"",S_FIELD_END));
    buf.push_back(statement(12,"",S_FIELD_END));*/
	buf.push_back(statement(1,"int _____@@##1",S_ASSIGN));
	buf.push_back(statement(2,"int _____@@##2",S_ASSIGN));
    buf.push_back(statement(3,"int _____@@##3",S_ASSIGN));
    buf.push_back(statement(3,"_____##@@3 = 1",S_ASSIGN));
    buf.push_back(statement(4,"_____##@@2 = 5",S_ASSIGN));
    buf.push_back(statement(5,"_____##@@1 = 1",S_ASSIGN));
    buf.push_back(statement(6,"_____##@@-3 = _____##@@1 + 1",S_ASSIGN));
    buf.push_back(statement(6,"_____##@@1 = _____##@@-3",S_ASSIGN));
    buf.push_back(statement(7,"",S_FIELD_BEGIN));
    buf.push_back(statement(7,"_____##@@4 = 0",S_ASSIGN));
	buf.push_back(statement(7," _____##@@-998 = 0",S_ASSIGN));
    buf.push_back(statement(7,"",S_GOTO_DEST));
	buf.push_back(statement(7,"",S_FIELD_BEGIN));
    buf.push_back(statement(7,"_____##@@-998",S_IF));
    buf.push_back(statement(7,"_____##@@-1 = _____##@@4",S_ASSIGN));
    buf.push_back(statement(7,"_____##@@4 = _____##@@4 + 1",S_ASSIGN));
    buf.push_back(statement(7,"",S_FIELD_END));
    buf.push_back(statement(7,"_____##@@-998 = 1",S_ASSIGN));
    buf.push_back(statement(7,"_____##@@-2 = _____##@@4 < 5",S_ASSIGN));
    buf.push_back(statement(7,"_____##@@-2",S_IF));
    buf.push_back(statement(8,"",S_FIELD_BEGIN));
    buf.push_back(statement(9,"",S_BLANK_BUT_OUTPUT));
    buf.push_back(statement(10,"",S_FIELD_END));
    buf.push_back(statement(10,"",S_GOTO));
    buf.push_back(statement(10,"",S_FIELD_END));
    buf.push_back(statement(11,"",S_BLANK_BUT_OUTPUT));
    buf.push_back(statement(11,"_____##@@-3 = _____##@@2",S_ASSIGN));
    buf.push_back(statement(11,"_____##@@2 = _____##@@2 + 1",S_ASSIGN));
    buf.push_back(statement(12,"",S_FIELD_BEGIN));
    buf.push_back(statement(12,"_____##@@2",S_IF));
    buf.push_back(statement(13,"",S_BLANK_BUT_OUTPUT));
    buf.push_back(statement(13,"",S_FIELD_END));
    processor_main();
	//printf("%d\n",Get_Value(1));
	//printf("%d\n",Get_Value_Double(-1));
	for(int i=0;i<print_buffer.size();++i)
		printf("%d\n",print_buffer[i]);
	return 0;
}
