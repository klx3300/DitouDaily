#include "analyser.hpp"



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
	int operand;
	char oper;
	item();
};

item::item(){
	type=0;
	operand=0;
	oper=0;
}

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
string low_priority("= == > < >= <= != "); // equate & inequate operators
string mid_priority("+-");
string high_priority("*/");

// operator 'd' indicates -- operator
// operator 'i' indicates ++ operator
// operator 'e' indicates == operator
// operator 'L' indicates >= operator
// operator 'S' indicates <= operator
// operator 'n' indicates != operator

int currIndex=0; // now analysis pointer position
bool tmpvars[256];

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
                    string tmpexpr("_____##@@-");
                    tmpexpr+=itoa(srctmpvar);
                    tmpexpr+=" = - _____##@@";
                    tmpexpr+=itoa(exprlist->get(ed)->item.operand);
                    statement nstate(ln,tmpexpr,S_ASSIGN);
                    
                    exprlist->get(ed)->item.number=-srctmpvar;
                }else{
                    exprlist->get(ed)->item.number=-(exprlist->get(ed)->item.number);
                }
				break;
			case 'd':
				exprlist->get(ed)->item.number=sin(exprlist->get(ed)->item.number);
				break;
			case 'i':
				exprlist->get(ed)->item.number=cos(exprlist->get(ed)->item.number);
				break;
			default:
                
				break;
		}
		exprlist->remove(exprlist->get(start));
		exprlist->remove(exprlist->get(start));
	}else{
		exprlist->remove(exprlist->get(end));
		// check unary
		for(int i=st;i<=ed-1;i++){
			if(exprlist->get(i)->prev->item.type==TYPE_OPER and exprlist->get(i)->item.type==TYPE_OPER and exprlist->get(i)->next->item.type==TYPE_NUM){
				//printf("unary oper:%c\n",exprlist->get(i)->item.oper);
				ed--;
				switch(exprlist->get(i)->item.oper){
					case '-':
						exprlist->get(i+1)->item.number=-(exprlist->get(i+1)->item.number);
						break;
					case 's':
						exprlist->get(i+1)->item.number=sin(exprlist->get(i+1)->item.number);
						break;
					case 'c':
						exprlist->get(i+1)->item.number=cos(exprlist->get(i+1)->item.number);
						break;
					case 't':
						exprlist->get(i+1)->item.number=tan(exprlist->get(i+1)->item.number);
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
			}
		}
		// priority from high to low
		for(int i=st;i<=ed;i++){
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
		}
		for(int i=st;i<=ed;i++){
			if(exprlist->get(i)->item.type==TYPE_OPER and mid_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '*':
						exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)*(exprlist->get(i+1)->item.number);
						break;
					case '/':
						exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)/(exprlist->get(i+1)->item.number);
						break;
					default:
						break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		for(int i=st;i<=ed;i++){
			if(exprlist->get(i)->item.type==TYPE_OPER and low_priority.find(exprlist->get(i)->item.oper)!=std::string::npos){
				ed-=2;
				switch(exprlist->get(i)->item.oper){
					case '+':
						exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
						break;
					case '-':
						exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
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
// calculate the expression
void rcalc(qLinkedList<item> *exprlist){
	
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
			bfcalc(exprlist,firstplace,lastplace);
			//printstack(*exprlist);
			//printf("size:%d\n",exprlist->size());
			firstplace=-1;lastplace=-1;
		}
	}
	// in order to fit in bfcalc() func
	// surround simplest expression with brackets
	item lb,rb;
	lb.type=TYPE_OPER;
	lb.oper='(';
	rb.type=TYPE_OPER;
	rb.oper=')';
	exprlist->addfirst(lb);
	exprlist->addlast(rb);
	bfcalc(exprlist,0,exprlist->size()-1);
}


int analyse_main(){
    // deque<statement> buf is input global var
    // all un-analysed statement is typed as S_ERROR
}
