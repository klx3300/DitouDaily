#include "analyser.hpp"

string itos(int i){
	ostringstream os;
	os<<i;
	return os.str();
}

string formatVarName(int varname){
	return "_____##@@"+itos(varname);
}

template<class T>
class qNode{
	public:
		qNode();
		T item;
		qNode<T>* next;
		qNode<T>* prev;
		
};
template<class T>
qNode<T>::qNode(){
	next=NULL;
	prev=NULL;
}
template <class T>
class qLinkedList{
	public:
	qLinkedList();
	qLinkedList(qLinkedList<T> src);
	qNode<T>* first;
	qNode<T>* last;
	virtual T popfirst();
	virtual T poplast();
	virtual qNode<T>* get(int index);
	virtual int size();
	virtual void addfirst(T i);
	virtual void addlast(T i);
	virtual bool popable();
	virtual void removeAfter(qNode<T>* i);
	virtual void remove(qNode<T>* pos);
	virtual void addBefore(qNode<T>* pos,qNode<T>* item);
	virtual void addAfter(qNode<T>* pos,qNode<T>* item);
	/*virtual void filter(T key);*/
	virtual void swap(qNode<T>* a,qNode<T>* b);
};
template<class T>
qLinkedList<T>::qLinkedList(){
	first=NULL;
	last=NULL;
}

template<class T>
qLinkedList<T>::qLinkedList(qLinkedList<T> src){
	for(int i=0;src.get(i)!=NULL;i++){
		addlast(src.get(i));
	}
}

template<class T>
bool qLinkedList<T>::popable(){
	return (first!=NULL);
}

template<class T>
void qLinkedList<T>::addfirst(T i){
	qNode<T>* origfirst=first;
	first=new qNode<T>();
	if(origfirst!=NULL){
		origfirst->prev=first;
	}
	first->item=i;
	first->next=origfirst;	
	if(last==NULL){
		last=first;
	}
}

template<class T>
void qLinkedList<T>::addlast(T i){
	qNode<T>* origlast=last;
	last=new qNode<T>();
	if(origlast!=NULL){
		origlast->next=last;
	}
	last->item=i;
	last->prev=origlast;
	if(first==NULL){
		first=last;
	}
}

template<class T>
T qLinkedList<T>::popfirst(){
	T result=first->item;
	qNode<T>* tmptr=first->next;
	if(tmptr!=NULL){
		tmptr->prev=NULL;
		delete(first);
		first=tmptr;
	}else{
		delete(first);
		first=NULL;
		last=NULL;
	}
	return result;
}

template<class T>
T qLinkedList<T>::poplast(){
	T result=last->item;
	qNode<T>* tmptr=last->prev;
	if(tmptr!=NULL){
		tmptr->next=NULL;
		delete(last);
		last=tmptr;
	}else{
		delete(last);
		first=NULL;
		last=NULL;
	}
	return result;
}

template <class T>
void qLinkedList<T>::addBefore(qNode<T>* pos,qNode<T>* item){
	qNode<T>* tmptr=pos->prev;
	pos->prev=item;
	item->prev=tmptr;
	item->next=pos;
	tmptr->next=item;
}

template <class T>
void qLinkedList<T>::addAfter(qNode<T>* pos,qNode<T>* item){
	qNode<T>* tmptr=pos->next;
	pos->next=item;
	item->prev=pos;
	item->next=tmptr;
	tmptr->prev=item;
}

template <class T>
void qLinkedList<T>::remove(qNode<T>* pos){
	qNode<T> *prev=pos->prev,*next=pos->next;
	if(first==pos)
		first=pos->next;
	if(last==pos)
		last=pos->prev;
	delete(pos);
	if(prev!=NULL)
		prev->next=next;
	if(next!=NULL)
		next->prev=prev;
}

template <class T>
void qLinkedList<T>::removeAfter(qNode<T>* i){
	qNode<T>* tmptr=i->next,*delptr=NULL;
	while(tmptr!=NULL){
		delptr=tmptr;
		tmptr=tmptr->next;
		delete(delptr);
	}
}

/*template <class T>
void qLinkedList<T>::filter(T key){
	qNode<T>* tmptr=first;
	while(tmptr!=NULL){
		while(tmptr->next!=NULL and tmptr->next->item==key){
			remove(tmptr->next);
		}
		tmptr=tmptr->next;
	}
}*/

template <class T>
void qLinkedList<T>::swap(qNode<T>* a,qNode<T>* b){
	T tmp=a->item;
	a->item=b->item;
	b->item=tmp;
}
template <class T>
qNode<T>* qLinkedList<T>::get(int index){
	qNode<T>* target=first;
	for(int i=0;i<index;i++){
		target=target->next;
	}
	return target;
}
template<class T>
int qLinkedList<T>::size(){
	int i=0;
	qNode<T>* ptr=first;
	if(ptr!=NULL){
		for(i=1;ptr->next!=NULL;ptr=ptr->next){
			i++;
		}
	}
	return i;
}
class item{
	public:
	int type;
	int number;
	char oper;
	item();
};

item::item(){
	type=0;
	number=0;
	oper=0;
}
//void bfcalc(qLinkedList<item> *exprlist,int start,int end,int lnnumber);

void clearTempVarFlags(){
    for(int i<0;i<256;i++){
        tmpvars[i]=false;
    }
}
int allocTempVar(){
    // search for avaliable tmpvar
    for(int i<0;i<256;i++){
        if(tmpvars[i]==false){
            tmpvars[i]=true;
            return i;
        }
    }
    return -1;
}
void freeTempVar(int i){
    tmpvars[i]=false;
}


int goto_level=-1;

int allocGotoTag(){
	return goto_level--;
}

void resetGotoTag(){
	goto_level=-1;
}

void defineStatement(int ln,int varname){
	string tmpstr("");
	tmpstr+="int _____##@@";
	tmpstr+=itos(varname);
	statement s(ln,tmpstr,S_ASSIGN);
	buf.insert(currIndex++,s);
}

void assignStatement(int ln,int varname,string simplest){
	string tmpstr("_____##@@"+itos(varname)+" = "+simplest);
	statement s(ln,tmpstr,S_ASSIGN);
	buf.insert(currIndex++,s);
}

void fieldStatement(int ln,STATEMENT_T type){
	statement s(ln,"",type);
	buf.insert(currIndex++,s);
}

void gotoStatement(int ln){
	statement s(ln,"",S_GOTO);
	buf.insert(currIndex++,s);
}

void gotodestStatement(int ln){
	statement s(ln,"",S_GOTO_DEST);
	buf.insert(currIndex++,s);
}

void ifStatement(int ln,int varname){
	statement s(ln,formatVarName(varname),S_IF);
	buf.insert(currIndex++,s);
}

void elseStatement(int ln){
	statement s(ln,"",S_ELSE);
	buf.insert(currIndex++,s);
}

void printStatement(int ln){
	statement s(ln,"",S_BLANK_BUT_OUTPUT);
	buf.insert(currIndex++,s);
};

void breakStatement(int ln){
	statement s(ln,"",S_BREAK);
	buf.insert(currIndex++,s);
};


/* imzhwk created analyser
 * cmdType对应text中的内容如下：
 S_ERROR 无意义
 S_BLANK,无意义
 S_BLANK_BUT_OUTPUT,无意义
 S_ASSIGN,完整的一行语句字符串，不含逗号分号
 S_FIELD_BEGIN,无意义
 S_FIELD_END,无意义
 S_GOTO,目标GOTO_DEST结构中那个特殊的lineNum的值(必须为负)
 S_GOTO_DEST,无意义
 S_IF,if括号内的那个式子
 S_ELSE_IF,if括号内的那个式子
 S_ELSE,无意义
 */


const int TYPE_OPERATOR=1;
const int TYPE_OPERAND=2;
const int TYPE_OPERAND_VAR=3;

/*提醒：有含义的预留关键词有
int if else for break while do \r\n \t printf return
+ - * / ++ --
> < = == >= <= !=
, ; "" '' ( ) //  /.../ { }
_____##@@...*/

string last_priority(",");
string low_priority("= e > < L S n "); // equate & inequate operators
string mid_priority("+-");
string high_priority("*/");

// operator 'd' indicates -- operator
// operator 'i' indicates ++ operator
// operator 'e' indicates == operator
// operator 'L' indicates >= operator
// operator 'S' indicates <= operator
// operator 'n' indicates != operator

int currIndex=0; // now analysis pointer position
bool tmpvars[65536];

//bracket-free part
void bfcalc(qLinkedList<item> *exprlist,int start,int end,int lnnumber){
	// income example:( expr... )
	int st=start+1,ed=end-1,ln=lnnumber;
	// firstly:check unary operator
	if(end-start==2){// (x) pattern example:sin(x)
        // simply de-bracket
		exprlist->remove(exprlist->get(end));
		exprlist->remove(exprlist->get(start));
	}else if(end-start==3){// (-x) pattern
		exprlist->remove(exprlist->get(end));
		switch(exprlist->get(st)->item.oper){
			case '-':
				// check variable?
                if(exprlist->get(ed)->item.type==TYPE_OPERAND_VAR){
                    // i cant read virtual memory directly,so use tempvars.
                    int srctmpvar=allocTempVar();
                    assignStatement(ln,-srctmpvar,"- "+formatVarName(exprlist->get(ed)->item.number));
                    exprlist->get(ed)->item.number=-srctmpvar;
                }else{
                    exprlist->get(ed)->item.number=-(exprlist->get(ed)->item.number);
                }
				break;
			case 'd':
				if(exprlist->get(ed)->item.type==TYPE_OPERAND_VAR){
					int srctmpvar=allocTempVar();
					assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(ed)->item.number));
					assignStatement(ln,exprlist->get(ed)->item.number,formatVarName(exprlist->get(ed)->item.number)+" - 1");
					exprlist->get(ed)->item.number=-srctmpvar;
				}else{
					// using -- on an instant value is illegal.
					// in this case will throw an SyntaxErrorException.
					// waiting to communicate with lbsjj and modify the mainframe
				}
				break;
			case 'i':
				if(exprlist->get(ed)->item.type==TYPE_OPERAND_VAR){
					int srctmpvar=allocTempVar();
					assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(ed)->item.number));
					assignStatement(ln,exprlist->get(ed)->item.number,formatVarName(exprlist->get(ed)->item.number)+" + 1");
					exprlist->get(ed)->item.number=-srctmpvar;
				}else{
					// using -- on an instant value is illegal.
					// in this case will throw an SyntaxErrorException.
					// waiting to communicate with lbsjj and modify the mainframe
				}
			default:
                
				break;
		}
		exprlist->remove(exprlist->get(start));
		exprlist->remove(exprlist->get(start));
	}else{
		exprlist->remove(exprlist->get(end));
		// check unary
		for(int i=st;i<=ed-1;i++){

			if(exprlist->get(i)->prev->item.type==TYPE_OPER and exprlist->get(i)->item.type==TYPE_OPER and (exprlist->get(i)->next->item.type==TYPE_OPERAND or exprlist->get(i)->next->item.type==TYPE_OPERAND_VAR)){
				//printf("unary oper:%c\n",exprlist->get(i)->item.oper);
				ed--;
				switch(exprlist->get(i)->item.oper){
					case '-':
						//exprlist->get(i+1)->item.number=-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,"- "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i+1)->item.number=-srctmpvar;
						}else{
							exprlist->get(i+1)->item.number=-(exprlist->get(i+1)->item.number);
						}
						break;
					case 'd':
						if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i+1)->item.number));
							assignStatement(ln,exprlist->get(i+1)->item.number,formatVarName(exprlist->get(i+1)->item.number)+" - 1");
							exprlist->get(i+1)->item.number=-srctmpvar;
						}else{
							// SyntaxErrorException
						}
						break;
					case 'i':
						if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i+1)->item.number));
							assignStatement(ln,exprlist->get(i+1)->item.number,formatVarName(exprlist->get(i+1)->item.number)+" + 1");
							exprlist->get(i+1)->item.number=-srctmpvar;
						}else{
							// SyntaxErrorException
						}
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
			}
		}
		// priority from high to lowest
		/*
		 * for(int i=st;i<=ed;i++){
			if(exprlist->get(i)->item.type==TYPE_OPER and high_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '&':
						exprlist->get(i-1)->item.number=((int)(exprlist->get(i-1)->item.number))&((int)(exprlist->get(i+1)->item.number));
						break;
					case '|':
						exprlist->get(i-1)->item.number=((int)(exprlist->get(i-1)->item.number))|((int)(exprlist->get(i+1)->item.number));
						break;
					case '^':
						exprlist->get(i-1)->item.number=pow((exprlist->get(i-1)->item.number),(exprlist->get(i+1)->item.number));
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}*/
		for(int i=st;i<=ed;i++){//high
			if(exprlist->get(i)->item.type==TYPE_OPER and high_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '*':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)*(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" * "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" * "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" * "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)*(exprlist->get(i+1)->item.number);
						}
						break;
					case '/':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)/(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" / "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" / "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" / "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)/(exprlist->get(i+1)->item.number);
						}
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		for(int i=st;i<=ed;i++){//mid
			if(exprlist->get(i)->item.type==TYPE_OPER and mid_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '+':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" + "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" + "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" + "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
						}
						break;
					case '-':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" - "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" - "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" - "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						}
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		for(int i=st;i<=ed;i++){//low
			if(exprlist->get(i)->item.type==TYPE_OPER and low_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '=':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							assignStatement(ln,exprlist->get(i-1)->item.number,formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=exprlist->get(i+1)->item.number;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							assignStatement(ln,exprlist->get(i-1)->item.number,itos(exprlist->get(i+1)->item.number);
							exprlist->get(i-1)->item.number=exprlist->get(i+1)->item.number;
                            exprlist->get(i-1)->item.type=TYPE_OPERAND;
						}else{
							// SyntaxErrorException
						}
						break;
					case 'e':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" == "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" == "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" == "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							int srctmpvar=allocTempVar();
                            assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" == "+itos(exprlist->get(i+1)->item.number));
                            exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}
						break;
                    case 'L':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" >= "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" >= "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" >= "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							int srctmpvar=allocTempVar();
                            assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" >= "+itos(exprlist->get(i+1)->item.number));
                            exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}
						break;
                    case 'S':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" <= "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" <= "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" <= "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							int srctmpvar=allocTempVar();
                            assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" <= "+itos(exprlist->get(i+1)->item.number));
                            exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}
						break;
                    case 'n':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
						if(exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR && exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" != "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if (exprlist->get(i-1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,formatVarName(exprlist->get(i-1)->item.number)+" != "+itos(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
						}else if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
							int srctmpvar=allocTempVar();
							assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" != "+formatVarName(exprlist->get(i+1)->item.number));
							exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// all operands are instant value!
							// that's simple!
							int srctmpvar=allocTempVar();
                            assignStatement(ln,-srctmpvar,itos(exprlist->get(i-1)->item.number)+" != "+itos(exprlist->get(i+1)->item.number));
                            exprlist->get(i-1)->item.number=-srctmpvar;
							exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		for(int i=st;i<=ed;i++){//lowest
			if(exprlist->get(i)->item.type==TYPE_OPER and last_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case ',':
						//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
						if(exprlist->get(i+1)->item.type==TYPE_OPERAND_VAR){
                            exprlist->get(i-1)->item.number=(exprlist->get(i+1)->item.number);
                            exprlist->get(i-1)->item.type=TYPE_OPERAND_VAR;
						}else{
							// right operands are instant value!
							// that's simple!
							exprlist->get(i-1)->item.number=(exprlist->get(i+1)->item.number);
                            exprlist->get(i-1)->item.type=TYPE_OPERAND;
						}
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		exprlist->remove(exprlist->get(start));
		//printstack(*exprlist);
		//printf("size:%d\n",exprlist->size());
	}
}


void debracket_process(qLinkedList<item> *exprlist,int ln){
	bool havebrackets=true;
	int firstplace=-1;
	int lastplace=-1;
	while(havebrackets){
		//printstack(*exprlist);
		//printf("size:%d\n",exprlist->size());
		havebrackets=false;
		//check brackets
		for(int i=0;i<exprlist->size();i++){
			if(exprlist->get(i)->item.type==TYPE_OPER and exprlist->get(i)->item.oper=='('){
				havebrackets=true;
				firstplace=i;
			}
			if(exprlist->get(i)->item.type==TYPE_OPER and exprlist->get(i)->item.oper==')'){
				havebrackets=true;
				lastplace=i;
				break;
			}
		}
		if(havebrackets==true){
			//printf("have brackets:from %d to %d\n",firstplace,lastplace);
			bfcalc(exprlist,firstplace,lastplace,ln);
			//printstack(*exprlist);
			//printf("size:%d\n",exprlist->size());
			firstplace=-1;lastplace=-1;
		}
	}
}

void rebracket_process(qLinkedList<item> *exprlist,int ln){
	item lb,rb;
	lb.type=TYPE_OPER;
	lb.oper='(';
	rb.type=TYPE_OPER;
	rb.oper=')';
	exprlist->addfirst(lb);
	exprlist->addlast(rb);
	bfcalc(exprlist,0,exprlist->size()-1,ln);
}
// having the brackets part
// operator D -> int
// operator J -> if
// operator W -> while loop
// operator F -> for loop
// operator P -> printf
// operator E -> else
// operator B -> break
// operator o -> do
// when sent in,';' in the end of statement is removed!

// true: didn't close normally(using '{' & '}')
const int FIELD_NORMAL_TRUE=0,FIELD_NORMAL_FALSE=1,FIELD_LOOP_TRUE=2,FIELD_LOOP_FALSE=3,FIELD_DLOOP=4;

qLinkedList<int> fieldStack;

void rcalc(qLinkedList<item> *exprlist,int ln){
	/* possible sing_line expressions are:
     * D varname = expression(or just close)
     * W expression
     * F expression ; expression ; expression
     * P
	 * J expression
     * a=b like:not have startup operators
	 * } FIELD_END
     */
    // detect startup operators
    if(exprlist->first->item.type==TYPE_OPER){
		switch(exprlist->first->item.oper){
			case 'D':
				defineStatement(ln,exprlist->get(1)->item.number);
				exprlist->popfirst();
				debracket_process(exprlist);
				rebracket_process(exprlist);
				break;
			case 'J':
				exprlist->popfirst();
				fieldStatement(ln,S_FIELD_BEGIN);
				if(exprlist->last->item.oper=='{'){
					exprlist->poplast();
					debracket_process(exprlist);
					rebracket_process(exprlist);
					ifStatement(ln,exprlist->first->item.number);
					fieldStack.addfirst(FIELD_NORMAL_FALSE);
				}else{
					debracket_process(exprlist);
					rebracket_process(exprlist);
					ifStatement(ln,exprlist->first->item.number);
					fieldStack.addfirst(FIELD_NORMAL_TRUE);
				}
				break;
			case 'W':
				if(fieldStack.first->item!=FIELD_DLOOP){
					// not a do-while loop!
					exprlist->popfirst();
					fieldStatement(ln,S_FIELD_BEGIN);
					if(exprlist->last->item.oper=='{'){
						exprlist->poplast();
						gotodestStatement(ln);
						debracket_process(exprlist);
						rebracket_process(exprlist);
						ifStatement(ln,exprlist->first->item.number);
						fieldStack.addfirst(FIELD_LOOP_FALSE);
					}else{
						gotodestStatement(ln);
						debracket_process(exprlist);
						rebracket_process(exprlist);
						ifStatement(ln,exprlist->first->item.number);
						fieldStack.addfirst(FIELD_LOOP_TRUE);
					}
				}else{
					// is a do-while loop!
					fieldStack.popfirst();
					exprlist->popfirst();
					debracket_process(exprlist);
					rebracket_process(exprlist);
					ifStatement(ln,exprlist->first->item.number);
					gotoStatement(ln);
					fieldStatement(ln,S_FIELD_END);
				}
				break;
			case 'F':
				// divide the whole statement.
				exprlist->popfirst();
				qLinkedList<item> initexpr,condexpr,iterexpr;
				for(int i=1;exprlist->get(i)->item.oper!=';';exprlist.popfirst()){
					initexpr.addlast(exprlist->get(i)->item);
				}
				exprlist->popfirst();
				for(int i=1;exprlist->get(i)->item.oper!=';';exprlist.popfirst()){
					condexpr.addlast(exprlist->get(i)->item);
				}
				exprlist->popfirst();
				for(int i=1;exprlist->get(i)->item.oper!=')';exprlist.popfirst()){
					iterexpr.addlast(exprlist->get(i)->item);
				}
				// qLinkedList<item> cpcondexpr(condexpr);
				// these statements divided suc.
				fieldStatement(ln,S_FIELD_BEGIN);
				if(exprlist->last->item.oper=='{'){
					exprlist->poplast();
					debracket_process(&initexpr);
					rebracket_process(&initexpr);
					assignStatement(ln,-998,"0");
					gotodestStatement(ln);
					fieldStatement(ln,S_FIELD_BEGIN);
					ifStatement(ln,-998);
					debracket_process(&iterexpr);
					rebracket_process(&iterexpr);
					fieldStatement(ln,S_FIELD_END);
					assignStatement(ln,-998,"1");
					debracket_process(&condexpr);
					rebracket_process(&condexpr);
					ifStatement(ln,condexpr.first->item.number);
					fieldStack.addfirst(FIELD_LOOP_FALSE);
				}else{
					debracket_process(&initexpr);
					rebracket_process(&initexpr);
					assignStatement(ln,-998,"0");
					gotodestStatement(ln);
					fieldStatement(ln,S_FIELD_BEGIN);
					ifStatement(ln,-998);
					debracket_process(&iterexpr);
					rebracket_process(&iterexpr);
					fieldStatement(ln,S_FIELD_END);
					assignStatement(ln,-998,"1");
					debracket_process(&condexpr);
					rebracket_process(&condexpr);
					ifStatement(ln,condexpr.first->item.number);
					fieldStack.addfirst(FIELD_LOOP_TRUE);
				}
				break;
			case 'P':
				printStatement(ln);
				break;
			case 'E':
				elseStatement(ln);
				fieldStatement(ln,S_FIELD_BEGIN);
				fieldStack.addfirst(FIELD_NORMAL_TRUE);
				break;
			case 'B':
				breakStatement(ln);
				break;
			case 'o':
				fieldStack.addfirst(FIELD_DLOOP);
				fieldStatement(ln,S_FIELD_BEGIN);
				gotodestStatement(ln);
				break;
			case '{':
				fieldStack.addfirst(FIELD_NORMAL_FALSE);
				fieldStatement(ln,S_FIELD_BEGIN);
				break;
			case '}':
					//close field
					if(fieldStack.first->item==FIELD_NORMAL_FALSE){
						// didn't close field normally!
						// close it and pop the stack.
						fieldStatement(ln,S_FIELD_END);
						fieldStack.popfirst();
						while(fieldStack.first->item==FIELD_NORMAL_TRUE){
							fieldStatement(ln,S_FIELD_END);
							fieldStack.popfirst();
						}
					}else if(fieldStack.first->item==FIELD_LOOP_FALSE){
						// for loops
						gotoStatement(ln);
						fieldStatement(ln,S_FIELD_END);
						fieldStack.popfirst();
						while(fieldStack.first->item==FIELD_NORMAL_TRUE){
							gotoStatement(ln);
							fieldStatement(ln,S_FIELD_END);
							fieldStack.popfirst();
						}
					}
				break;
			default:
				// that's a statement that have no meanings!
				// but we still have to execute it due to the fucking requirements!
				debracket_process(exprlist);
				rebracket_process(exprlist);
				// check if last didn't use } to close field.
				// true: didn't close normally(using '{' & '}')
				// and commit close.
				if(fieldStack.first->item==FIELD_NORMAL_TRUE){
					// didn't close field normally!
					// close it and pop the stack.
					while(fieldStack.first->item==FIELD_NORMAL_TRUE){
						fieldStatement(ln,S_FIELD_END);
						fieldStack.popfirst();
					}
				}else if(fieldStack.first->item==FIELD_LOOP_TRUE){
					// for loops
					while(fieldStack.first->item==FIELD_NORMAL_TRUE){
						gotoStatement(ln);
						fieldStatement(ln,S_FIELD_END);
						fieldStack.popfirst();
					}
				}
				break;
		}
    }else{
        // that indicates it's an very simple assign statement(but may have brackets!).
        debracket_process(exprlist);
		rebracket_process(exprlist);
		// check if last didn't use } to close field.
		// true: didn't close normally(using '{' & '}')
		// and commit close.
		if(fieldStack.first->item==FIELD_NORMAL_TRUE){
			// didn't close field normally!
			// close it and pop the stack.
			while(fieldStack.first->item==FIELD_NORMAL_TRUE){
				fieldStatement(ln,S_FIELD_END);
				fieldStack.popfirst();
			}
		}else if(fieldStack.first->item==FIELD_LOOP_TRUE){
			// for loops
			while(fieldStack.first->item==FIELD_NORMAL_TRUE){
				gotoStatement(ln);
				fieldStatement(ln,S_FIELD_END);
				fieldStack.popfirst();
			}
		}
    }
	
}

// convert deque<statement> into qLinkedLists and send to rcalc()

// when detected "for" loop, ignore next 2 ';'s (not to close statement too early.
// when meet { or } , close statement immediately.the close result must include { or } itself.
// when meet "else" , close statement immediately.

void genExpr(){
	
}

int analyse_main(){
    // deque<statement> buf is input global var
    // all un-analysed statement is typed as S_ERROR
}