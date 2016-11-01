#include "processor.hpp"
#include "w_fix.hpp"

int tempValue[65535];//存储不需要声明的运算过程中的临时变量
map<int,pair<int,int> > allValue;//存储需要声明的变量
stringstream ss;
int fieldCnt=0;//作用域计数器

void New_Value(int Number)//声明变量并赋值为0
{
    map<int,pair<int,int> >::iterator now=allValue.find(Number);
    if(now!=allValue.end()&&(*now).second.first!=-fieldCnt)
        return;
	allValue.insert(make_pair(Number,make_pair(-fieldCnt,0)));
}

int Give_Value(int Number,int Value)//变量赋值
{
	map<int,pair<int,int> >::iterator now=allValue.find(Number);
	if(now==allValue.end())
        allValue.insert(make_pair(Number,make_pair(-fieldCnt,Value)));
    else
        (*now).second.second=Value;
	return 0;
}

int Get_Value(int Number)//获取变量值
{
	map<int,pair<int,int> >::iterator now=allValue.find(Number);
	return (*now).second.second;
}

int Get_Value_Double(int Number)//获取两种变量值
{
	if(Number<0)
		return tempValue[-Number];
	else
		return Get_Value(Number);
}

int Calc(deque<statement>::iterator &now_it)
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
		case S_FIELD_BEGIN://if else for while 或者 do-while 的作用域开始
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
			}//删除所有该作用域中的变量
			break;
		}
		case S_GOTO:
		{
			int Loc_fieldCnt=fieldCnt;
			for(deque<statement>::iterator now_it_tmp=now_it;now_it_tmp!=buf.begin();--now_it_tmp)//find S_GOTO_DEST in the same field
			{
 				if((*now_it_tmp).cmdType==S_FIELD_BEGIN)
					--fieldCnt;
				if((*now_it_tmp).cmdType==S_FIELD_END)
					++fieldCnt;
				if((*now_it_tmp).cmdType==S_GOTO_DEST&&fieldCnt==Loc_fieldCnt-1)
				{
					now_it=now_it_tmp;
                    			fieldCnt=Loc_fieldCnt;
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
			//使用stringstream获取变量
			ss<<now_state.text;
			ss>>everyMember[0];
			if(everyMember[0]==RECOLIC_TEXT("int"))
			{
				ss>>everyMember[1];
				ss.clear();
				ss<<everyMember[1].substr(9);
				ss>>tmpValue[0];
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
						if(everyMember[2]== RECOLIC_TEXT("+"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])+((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("-"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])-((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("*"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])*((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("/"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])/((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT(">"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])>((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("<"))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])<((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT(">="))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])>=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("<="))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])<=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("!="))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])!=((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
						else if(everyMember[2]== RECOLIC_TEXT("=="))
							tempValue[-tmpValue[0]]=((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])==((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]);
					}
					else
					{
						if(everyMember[2]== RECOLIC_TEXT("+"))
							Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])+((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
						else if(everyMember[2]== RECOLIC_TEXT("-"))
							Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])-((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
						else if(everyMember[2]== RECOLIC_TEXT("*"))
							Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])*((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
						else if(everyMember[2]== RECOLIC_TEXT("/"))
							Give_Value(tmpValue[0],((everyMember[1][0]=='_')?Get_Value_Double(tmpValue[1]):tmpValue[1])/((everyMember[3][0]=='_')?Get_Value_Double(tmpValue[3]):tmpValue[3]));
					}
				}
			}
			break;
		}
		case S_BREAK:
		{
            		//向后找到合适的FIELD_END实现break
			if(now_state.lineNum>0)
				print_buffer.push_back(now_state.lineNum);
			while((*now_it).cmdType!=S_GOTO)
			{
				++now_it;
				if((*now_it).cmdType==S_FIELD_BEGIN)
					++fieldCnt;
				if((*now_it).cmdType==S_FIELD_END)
				{
					//删除作用域内变量
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
			if(!flag)//如果IF内变量为假
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
			else//如果为真
            		{
				int Loc_fieldCnt=fieldCnt-1;
				deque<statement>::iterator Loc_now_it=now_it;
				while(fieldCnt!=Loc_fieldCnt&&now_it!=buf.end())
				{
					++now_it;
					if((*now_it).cmdType==S_FIELD_BEGIN)
						++fieldCnt;
					if((*now_it).cmdType==S_FIELD_END)
						--fieldCnt;
                		}
				if((*now_it).cmdType!=S_FIELD_END)
				{
				    fieldCnt=Loc_fieldCnt+1;
				    now_it=Loc_now_it;
				    break;
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
				        (*now_it).cmdType=S_BLANK,(*now_it).lineNum=-1;//全部置为S_BLANK
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
			break;
		}
	}
}


int processor_main()
{
	for(deque<statement>::iterator now_it=buf.begin();now_it!=buf.end();++now_it)
        	Calc(now_it);
    	return 0;
}

