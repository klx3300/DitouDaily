#include "processor.hpp"

int tempValue[65535];

map<int,pair<int,int> > allValue;//number cnt value
stringstream ss;//get every thing from the string
int fieldCnt=0;//count how many field it has

void New_Value(int Number)
{
    map<int,pair<int,int> >::iterator now=allValue.find(Number);
    if(now!=allValue.end()&&(*now).second.first!=-fieldCnt)
        return;
	allValue.insert(make_pair(Number,make_pair(-fieldCnt,0)));
}

int Give_Value(int Number,int Value)
{
	map<int,pair<int,int> >::iterator now=allValue.find(Number);
	if(now==allValue.end())
        allValue.insert(make_pair(Number,make_pair(-fieldCnt,Value)));
    else
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
            //printf("fieldbegin\n");
			break;
		}
		case S_FIELD_END:
		{
			--fieldCnt;
			map<int,pair<int,int> >::iterator tmp;
            //printf("fieldend\n");
			for(map<int,pair<int,int> >::iterator now=allValue.begin();now!=allValue.end();)
			{
                //printf("erasefor\n");
                //printf("%d %d %d\n",(*now).first,(*now).second.first,(*now).second.second);
				if((*now).second.first<-fieldCnt)
				{
					tmp=now;
					++now;
                    //printf("erase\n");
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
                //printf("%d::::%d %d %s\n",fieldCnt,(*now_it_tmp).cmdType,(*now_it_tmp).lineNum,(*now_it_tmp).text.c_str());
				if((*now_it_tmp).cmdType==S_FIELD_BEGIN)
					--fieldCnt;
				if((*now_it_tmp).cmdType==S_FIELD_END)
					++fieldCnt;
				if((*now_it_tmp).cmdType==S_GOTO_DEST&&Loc_fieldCnt==fieldCnt)
				{
					now_it=now_it_tmp;
                    puts("");
					break;
				}
			}
			puts("");
			fieldCnt=Loc_fieldCnt;
			break;
		}
		case S_ASSIGN:
		{
            //printf("%d %d %s\n",(*now_it).cmdType,(*now_it).lineNum,(*now_it).text.c_str());
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
                //printf("go into else\n");
                int Loc_fieldCnt=fieldCnt-1;
                deque<statement>::iterator Loc_now_it=now_it;
                while(fieldCnt!=Loc_fieldCnt&&now_it!=buf.end())
                {
                    //printf("gogogo\n");
                    //printf("%d %d %d\n",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                    ++now_it;
					if((*now_it).cmdType==S_FIELD_BEGIN)
						++fieldCnt;
					if((*now_it).cmdType==S_FIELD_END)
						--fieldCnt;
                }
                //printf("%d %d %d\n",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                if((*now_it).cmdType!=S_FIELD_END)
                {
                    //printf("go away1\n");
                    //printf("%d %d %d",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                    fieldCnt=Loc_fieldCnt+1;
                    now_it=Loc_now_it;
                    break;
                }
                ++now_it;
                //printf("%d %d %d\n",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                if((*now_it).cmdType!=S_FIELD_BEGIN)
                {
                    //printf("go away2\n");
                    //printf("%d %d %d",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                    fieldCnt=Loc_fieldCnt+1;
                    now_it=Loc_now_it;
                    break;
                }
                //printf("\n %d \n",fieldCnt);
                ++now_it;
                ++fieldCnt;
                if((*now_it).cmdType!=S_ELSE)
                {
                    //printf("go away3\n");
                    fieldCnt=Loc_fieldCnt+1;
                    now_it=Loc_now_it;
                    break;
                }
                //printf("%d\n",fieldCnt);
                while(fieldCnt!=Loc_fieldCnt)
                {
                    //printf("%d %d\n",fieldCnt,Loc_fieldCnt);
                    ++now_it;
                    //printf("cout:%d %d %d\n",fieldCnt,(*now_it).cmdType,(*now_it).lineNum);
                    if((*now_it).cmdType!=S_FIELD_BEGIN&&(*now_it).cmdType!=S_FIELD_END)
                        (*now_it).cmdType=S_BLANK,(*now_it).lineNum=-1;
                    else
                        if((*now_it).cmdType==S_FIELD_BEGIN)
                            ++fieldCnt;
                        else
                            --fieldCnt;
                }//printf("k\n");
                fieldCnt=Loc_fieldCnt+1;
                now_it=Loc_now_it;
                break;
            }
		}
		case S_ELSE:
		{
			//if(now_state.lineNum>0)
			//	print_buffer.push_back(now_state.lineNum);
			break;
		}
	}
}


int processor_main()//If no error occurred, return 0.
{
    int cnt=0;
	for(deque<statement>::iterator now_it=buf.begin();now_it!=buf.end();++now_it)
    {
        //if(++cnt>100)
        //   break;
        //if((*now_it).cmdType==S_IF)
        //{
            //printf("%s\n",(*now_it).text.c_str());
        //    for(map<int,pair<int,int> >::iterator it=allValue.begin();it!=allValue.end();++it)
        //        printf("%d %d %d\n",(*it).first,(*it).second.first,(*it).second.second);
        //    puts("");
        //}
       // printf("%d %d %s\n",(*now_it).cmdType,(*now_it).lineNum,(*now_it).text.c_str());
		Calc(now_it);
    }
    return 0;
}

