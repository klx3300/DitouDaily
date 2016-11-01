#include "analyser.hpp"
#include "w_fix.hpp"//必须最后包含w_fix.hpp

string itos(int i) {
	ostringstream os;
	os << i;
	return os.str();
}

//将数字变量代号转为变量名称
string formatVarName(int varname) {
	return RECOLIC_TEXT("_____##@@") + itos(varname);
}

template<class T>
class qNode {
public:
	qNode();
	qNode(T titem);
	T item;
	qNode<T>* next;
	qNode<T>* prev;

};
template<class T>
qNode<T>::qNode() {
	next = NULL;
	prev = NULL;
}
template<class T>
qNode<T>::qNode(T titem){
	item=titem;
	next=NULL;
	prev=NULL;
}
template <class T>
class qLinkedList {
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
	virtual void addBefore(qNode<T>* pos, qNode<T>* item);
	virtual void addAfter(qNode<T>* pos, qNode<T>* item);
	/*virtual void filter(T key);*/
	virtual void swap(qNode<T>* a, qNode<T>* b);
};
template<class T>
qLinkedList<T>::qLinkedList() {
	first = NULL;
	last = NULL;
}

template<class T>
bool qLinkedList<T>::popable() {
	return (first != NULL);
}

template<class T>
void qLinkedList<T>::addfirst(T i) {
	qNode<T>* origfirst = first;
	first = new qNode<T>();
	if (origfirst != NULL) {
		origfirst->prev = first;
	}
	first->item = i;
	first->next = origfirst;
	if (last == NULL) {
		last = first;
	}
}

template<class T>
void qLinkedList<T>::addlast(T i) {
	qNode<T>* origlast = last;
	last = new qNode<T>();
	if (origlast != NULL) {
		origlast->next = last;
	}
	last->item = i;
	last->prev = origlast;
	if (first == NULL) {
		first = last;
	}
}

template<class T>
T qLinkedList<T>::popfirst() {
	T result = first->item;
	qNode<T>* tmptr = first->next;
	if (tmptr != NULL) {
		tmptr->prev = NULL;
		delete(first);
		first = tmptr;
	}
	else {
		delete(first);
		first = NULL;
		last = NULL;
	}
	return result;
}

template<class T>
T qLinkedList<T>::poplast() {
	T result = last->item;
	qNode<T>* tmptr = last->prev;
	if (tmptr != NULL) {
		tmptr->next = NULL;
		delete(last);
		last = tmptr;
	}
	else {
		delete(last);
		first = NULL;
		last = NULL;
	}
	return result;
}

template <class T>
void qLinkedList<T>::addBefore(qNode<T>* pos, qNode<T>* item) {
	qNode<T>* tmptr = pos->prev;
	pos->prev = item;
	item->prev = tmptr;
	item->next = pos;
	tmptr->next = item;
}

template <class T>
void qLinkedList<T>::addAfter(qNode<T>* pos, qNode<T>* item) {
	qNode<T>* tmptr = pos->next;
	pos->next = item;
	item->prev = pos;
	item->next = tmptr;
	tmptr->prev = item;
}

template <class T>
void qLinkedList<T>::remove(qNode<T>* pos) {
	qNode<T> *prev = pos->prev, *next = pos->next;
	if (first == pos)
		first = pos->next;
	if (last == pos)
		last = pos->prev;
	delete(pos);
	if (prev != NULL)
		prev->next = next;
	if (next != NULL)
		next->prev = prev;
}

template <class T>
void qLinkedList<T>::removeAfter(qNode<T>* i) {
	qNode<T>* tmptr = i->next, *delptr = NULL;
	while (tmptr != NULL) {
		delptr = tmptr;
		tmptr = tmptr->next;
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
void qLinkedList<T>::swap(qNode<T>* a, qNode<T>* b) {
	T tmp = a->item;
	a->item = b->item;
	b->item = tmp;
}
template <class T>
qNode<T>* qLinkedList<T>::get(int index) {
	qNode<T>* target = first;
	for (int i = 0;i<index;i++) {
		target = target->next;
	}
	return target;
}
template<class T>
int qLinkedList<T>::size() {
	int i = 0;
	qNode<T>* ptr = first;
	if (ptr != NULL) {
		for (i = 1;ptr->next != NULL;ptr = ptr->next) {
			i++;
		}
	}
	return i;
}
class item {
public:
	int type;
	int number;
	char oper;
	item();
};

item::item() {
	type = 0;
	number = 0;
	oper = 0;
}
//void bfcalc(qLinkedList<item> *exprlist,int start,int end,int lnnumber);
int currIndex = 0; // 解析器正在解析的index
bool tmpvars[65536];// processor临时变量占用状态

qLinkedList<bool *> vstack;//作用域变量状态栈（按代号存储）

//新作用域入栈
void vstack_new_field(){
	bool *variables=new bool[65536];
	for(int i=0;i<65536;i++){
		variables[i]=false;
	}
	vstack.addfirst(variables);
};

//作用域结束出栈
void vstack_pop_field(){
	delete[] vstack.first->item;
	vstack.popfirst();
};

//清除processor临时变量状态
void clearTempVarFlags() {
	for (int i = 0;i<256;i++) {
		tmpvars[i] = false;
	}
}

//申请processor临时变量
int allocTempVar() {
	// search for avaliable tmpvar
	for (int i = 1;i<256;i++) {
		if (tmpvars[i] == false) {
			tmpvars[i] = true;
			return i;
		}
	}
	return -1;
}

//释放已经使用的processor临时变量
void freeTempVar(int i) {
	tmpvars[i] = false;
}

//当前GOTO语句在栈中位置
int goto_level = -1;

//增加一层GOTO
int allocGotoTag() {
	return goto_level--;
}

//重置GOTO位置
void resetGotoTag() {
	goto_level = -1;
}

/*定义变量。
 *首先检测该变量是否首次定义，避免重复定义。
 */
void defineStatement(int ln, int varname) {
	if(varname<0){
		string tmpstr(RECOLIC_TEXT(""));
		tmpstr += RECOLIC_TEXT("int _____##@@");
		tmpstr += itos(varname);
		statement s(ln, tmpstr, S_ASSIGN);
		buf.push_back(s);
	}else{
		if(vstack.first->item[varname]==false){
			string tmpstr(RECOLIC_TEXT(""));
			tmpstr += RECOLIC_TEXT("int _____##@@");
			tmpstr += itos(varname);
			statement s(ln, tmpstr, S_ASSIGN);
			buf.push_back(s);
			vstack.first->item[varname]=true;
		}else{
			// defined do nothing.
		}
	}
}


//赋值语句。
void assignStatement(int ln, int varname, string simplest) {
	string tmpstr(RECOLIC_TEXT("_____##@@") + itos(varname) + RECOLIC_TEXT(" = ") + simplest);
	statement s(ln, tmpstr, S_ASSIGN);
	buf.push_back(s);
}
//作用域语句
void fieldStatement(int ln, STATEMENT_T type) {
	if(type==S_FIELD_BEGIN){
		vstack_new_field();
	}else{
		vstack_pop_field();
	}
	statement s(ln, RECOLIC_TEXT(""), type);
	buf.push_back(s);
}
// 跳转语句
void gotoStatement(int ln) {
	statement s(ln, RECOLIC_TEXT(""), S_GOTO);
	buf.push_back(s);
}
// 跳转语句的目标语句
void gotodestStatement(int ln) {
	statement s(ln, RECOLIC_TEXT(""), S_GOTO_DEST);
	buf.push_back(s);
}
// 条件判断语句
void ifStatement(int ln, string expr) {
	statement s(ln, expr, S_IF);
	buf.push_back(s);
}
// ELSE语句
void elseStatement(int ln) {
	statement s(ln, RECOLIC_TEXT(""), S_ELSE);
	buf.push_back(s);
}
// 输出语句
void printStatement(int ln) {
	statement s(ln, RECOLIC_TEXT(""), S_BLANK_BUT_OUTPUT);
	buf.push_back(s);
};
// 跳出循环语句
void breakStatement(int ln) {
	statement s(ln, RECOLIC_TEXT(""), S_BREAK);
	buf.push_back(s);
};

//item类型可能取值
const int TYPE_OPER = 1;//运算符
const int TYPE_OPERAND = 2;//立即数
const int TYPE_OPERAND_VAR = 3;//变量

string last_priority(RECOLIC_TEXT(","));//最低优先级
string low_priority(RECOLIC_TEXT("e > < L S n ")); // 以此类推
string mid_priority(RECOLIC_TEXT("+-"));
string high_priority(RECOLIC_TEXT("*/"));

// 运算符对应表
// operator 'd' indicates -- operator
// operator 'i' indicates ++ operator
// operator 'e' indicates == operator
// operator 'L' indicates >= operator
// operator 'S' indicates <= operator
// operator 'n' indicates != operator

// 调试输出
void printstack(qLinkedList<item> exprlist) {
	for (int i = 0;i<exprlist.size();i++) {
		switch (exprlist.get(i)->item.type) {
		case TYPE_OPERAND:
			printf(RECOLIC_TEXT("%d"), exprlist.get(i)->item.number);
			break;
		case TYPE_OPERAND_VAR:
			printf(RECOLIC_TEXT("_____##@@%d"), exprlist.get(i)->item.number);
			break;
		case TYPE_OPER:
			printf(RECOLIC_TEXT("%c"), exprlist.get(i)->item.oper);
			break;
		default:
			printf(RECOLIC_TEXT("[UNDEF]"));
		}
	}
	printf(RECOLIC_TEXT("\n"));
}


//解析无括号语句
void bfcalc(qLinkedList<item> *exprlist, int start, int end, int lnnumber) {
	// income example:( expr... )
	int st = start + 1, ed = end - 1, ln = lnnumber;
	// firstly:check unary operator
	if (end - start == 2) {// (x) pattern example:sin(x)
						   // simply de-bracket
		exprlist->remove(exprlist->get(end));
		exprlist->remove(exprlist->get(start));
	}
	else if (end - start == 3) {// (-x) pattern && (x-) pattern
		exprlist->remove(exprlist->get(end));
		if(exprlist->get(st)->item.type == TYPE_OPER){
			switch (exprlist->get(st)->item.oper) {
			case '-':
				// check variable?
				if (exprlist->get(ed)->item.type == TYPE_OPERAND_VAR) {
					// i cant read virtual memory directly,so use tempvars.
					int srctmpvar = allocTempVar();
					assignStatement(ln, -srctmpvar, RECOLIC_TEXT("- ") + formatVarName(exprlist->get(ed)->item.number));
					exprlist->get(ed)->item.number = -srctmpvar;
				}
				else {
					exprlist->get(ed)->item.number = -(exprlist->get(ed)->item.number);
				}
				break;
			default:
	
				break;
			}
			exprlist->remove(exprlist->get(start));
			exprlist->remove(exprlist->get(start));
		}else{
			switch (exprlist->get(ed)->item.oper) {
			case 'd':
				if (exprlist->get(st)->item.type == TYPE_OPERAND_VAR) {
					int srctmpvar = allocTempVar();
					assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(st)->item.number));
					assignStatement(ln, exprlist->get(st)->item.number, formatVarName(exprlist->get(st)->item.number) + RECOLIC_TEXT(" - 1"));
					exprlist->get(st)->item.number = -srctmpvar;
				}
				else {
					// using -- on an instant value is illegal.
					// in this case will throw an SyntaxErrorException.
					// waiting to communicate with lbsjj and modify the mainframe
				}
				break;
			case 'i':
				if (exprlist->get(st)->item.type == TYPE_OPERAND_VAR) {
					int srctmpvar = allocTempVar();
					assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(st)->item.number));
					assignStatement(ln, exprlist->get(st)->item.number, formatVarName(exprlist->get(st)->item.number) + RECOLIC_TEXT(" + 1"));
					exprlist->get(st)->item.number = -srctmpvar;
				}
				else {
					// using -- on an instant value is illegal.
					// in this case will throw an SyntaxErrorException.
					// waiting to communicate with lbsjj and modify the mainframe
				}
			default:
	
				break;
			}
			exprlist->remove(exprlist->get(end-1));
			exprlist->remove(exprlist->get(start));
		}
		
	}
	else {
		//exprlist->remove(exprlist->get(end));
		// check unary
		for (int i = st;i <= ed;i++) {
			/*if(exprlist->get(i)->item.type==TYPE_OPER)
				printf(RECOLIC_TEXT("PROC OPER:%c\n"),exprlist->get(i)->item.oper);
			else
				printf(RECOLIC_TEXT("PROC NUMB:%d\n"),exprlist->get(i)->item.number);*/
			if (exprlist->get(i)->prev->item.type == TYPE_OPERAND_VAR and exprlist->get(i)->item.type == TYPE_OPER and (exprlist->get(i)->next->item.type != TYPE_OPERAND and exprlist->get(i)->next->item.type != TYPE_OPERAND_VAR)) {
				//printf("unary oper:%c\n",exprlist->get(i)->item.oper);
				
				switch (exprlist->get(i)->item.oper) {
				case 'd':
					ed--;
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number));
						assignStatement(ln, exprlist->get(i - 1)->item.number, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" - 1"));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else {
						// SyntaxErrorException
					}
					exprlist->remove(exprlist->get(i));
					break;
				case 'i':
					ed--;
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number));
						assignStatement(ln, exprlist->get(i - 1)->item.number, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" + 1"));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else {
						// SyntaxErrorException
					}
					exprlist->remove(exprlist->get(i));
					break;
				default:
					
					break;
				}
				//exprlist->remove(exprlist->get(i));
				//exprlist->remove(exprlist->get(ed+1));
			}
			if (exprlist->get(i)->prev->item.type == TYPE_OPER and exprlist->get(i)->item.type == TYPE_OPER and (exprlist->get(i)->next->item.type == TYPE_OPERAND or exprlist->get(i)->next->item.type == TYPE_OPERAND_VAR)) {
				//printf("unary oper:%c\n",exprlist->get(i)->item.oper);
				
				switch (exprlist->get(i)->item.oper) {
				case '-':
					ed--;
					//exprlist->get(i+1)->item.number=-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, RECOLIC_TEXT("- ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i + 1)->item.number = -srctmpvar;
					}
					else {
						exprlist->get(i + 1)->item.number = -(exprlist->get(i + 1)->item.number);
					}
					exprlist->remove(exprlist->get(i));
					break;
				default:
					
					break;
				}
				printf(RECOLIC_TEXT("ax:"));
				printstack(*exprlist);
				
			}
			
		}
		exprlist->remove(exprlist->get(ed+1));
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
		for (int i = st;i <= ed;i++) {//high
			if (exprlist->get(i)->item.type == TYPE_OPER and high_priority.find(exprlist->get(i)->item.oper) != std::string::npos) {
				ed -= 2;
				switch (exprlist->get(i)->item.oper) {
				case '*':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)*(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" * ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" * ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" * ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						exprlist->get(i - 1)->item.number = (exprlist->get(i - 1)->item.number)*(exprlist->get(i + 1)->item.number);
					}
					break;
				case '/':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)/(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" / ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" / ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" / ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						exprlist->get(i - 1)->item.number = (exprlist->get(i - 1)->item.number) / (exprlist->get(i + 1)->item.number);
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
		printf(RECOLIC_TEXT("HIG END\n"));
		printstack(*exprlist);
		for (int i = st;i <= ed;i++) {//mid
			if (exprlist->get(i)->item.type == TYPE_OPER and mid_priority.find(exprlist->get(i)->item.oper) != std::string::npos) {
				ed -= 2;
				switch (exprlist->get(i)->item.oper) {
				case '+':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" + ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" + ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" + ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that'sexprlist->popfirst() simple!
						exprlist->get(i - 1)->item.number = (exprlist->get(i - 1)->item.number) + (exprlist->get(i + 1)->item.number);
					}
					break;
				case '-':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" - ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" - ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" - ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						exprlist->get(i - 1)->item.number = (exprlist->get(i - 1)->item.number) - (exprlist->get(i + 1)->item.number);
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
		//printstack(*exprlist);
		printf(RECOLIC_TEXT("MID END\n"));
		printstack(*exprlist);
		
		
		for (int i = st;i <= ed;i++) {//low
			if (exprlist->get(i)->item.type == TYPE_OPER and low_priority.find(exprlist->get(i)->item.oper) != std::string::npos) {
				ed -= 2;
				switch (exprlist->get(i)->item.oper) {
				/*case '=':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						assignStatement(ln, exprlist->get(i - 1)->item.number, formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = exprlist->get(i + 1)->item.number;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						assignStatement(ln, exprlist->get(i - 1)->item.number, itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = exprlist->get(i + 1)->item.number;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND;
					}
					else {
						// SyntaxErrorException
					}
					//printstack(*exprlist);
					break;*/
				case 'e':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" == ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" == ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" == ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" == ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					break;
				case 'L':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" >= ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" >= ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" >= ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" >= ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					break;
				case 'S':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" <= ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" <= ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" <= ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" <= ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					break;
				case 'n':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" != ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" != ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" != ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" != ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					break;
				default:
					if(exprlist->get(i)->item.oper==',')
						break;
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)-(exprlist->get(i+1)->item.number);
					if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" ")+exprlist->get(i)->item.oper+ RECOLIC_TEXT(" ") + formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, formatVarName(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" ")+exprlist->get(i)->item.oper+ RECOLIC_TEXT(" ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
					}
					else if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" ")+exprlist->get(i)->item.oper+ RECOLIC_TEXT(" ")+ formatVarName(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// all operands are instant value!
						// that's simple!
						int srctmpvar = allocTempVar();
						assignStatement(ln, -srctmpvar, itos(exprlist->get(i - 1)->item.number) + RECOLIC_TEXT(" ")+exprlist->get(i)->item.oper+ RECOLIC_TEXT(" ") + itos(exprlist->get(i + 1)->item.number));
						exprlist->get(i - 1)->item.number = -srctmpvar;
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					break;
					break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}
		printf(RECOLIC_TEXT("LOW PR\n"));
		printstack(*exprlist);
		// low reverse: = operator
		
		for(int i=ed-1;i>=st;i--){//low reverse
			if(exprlist->get(i)->next!=NULL and exprlist->get(i)->item.type == TYPE_OPER and exprlist->get(i)->item.oper=='='){
				// '=' operator detected.
				ed-=2;
				if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR && exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
					assignStatement(ln, exprlist->get(i - 1)->item.number, formatVarName(exprlist->get(i + 1)->item.number));
					exprlist->get(i - 1)->item.number = exprlist->get(i + 1)->item.number;
				}
				else if (exprlist->get(i - 1)->item.type == TYPE_OPERAND_VAR) {
					assignStatement(ln, exprlist->get(i - 1)->item.number, itos(exprlist->get(i + 1)->item.number));
					exprlist->get(i - 1)->item.number = exprlist->get(i + 1)->item.number;
					exprlist->get(i - 1)->item.type = TYPE_OPERAND;
				}
				else {
						// SyntaxErrorException
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
			}
			printf(RECOLIC_TEXT("LP\n"));
		}
		
		printf(RECOLIC_TEXT("LP EXIT\n"));
		/*for (int i = st;i <= ed;i++) {//lowest
			if (exprlist->get(i)->item.type == TYPE_OPER and last_priority.find(exprlist->get(i)->item.oper) != std::string::npos) {
				ed -= 2;
				switch (exprlist->get(i)->item.oper) {
				case ',':
					//exprlist->get(i-1)->item.number=(exprlist->get(i-1)->item.number)+(exprlist->get(i+1)->item.number);
					if (exprlist->get(i + 1)->item.type == TYPE_OPERAND_VAR) {
						exprlist->get(i - 1)->item.number = (exprlist->get(i + 1)->item.number);
						exprlist->get(i - 1)->item.type = TYPE_OPERAND_VAR;
					}
					else {
						// right operands are instant value!
						// that's simple!
						exprlist->get(i - 1)->item.number = (exprlist->get(i + 1)->item.number);
						exprlist->get(i - 1)->item.type = TYPE_OPERAND;
					}
					break;
				default:
					break;
				}
				exprlist->remove(exprlist->get(i));
				exprlist->remove(exprlist->get(i));
				i--;
			}
		}*/
		exprlist->remove(exprlist->get(start));
		//printstack(*exprlist);
		//printf("size:%d\n",exprlist->size());
	}
}

//去除语句中所有括号
void debracket_process(qLinkedList<item> *exprlist, int ln) {
	bool havebrackets = true;
	int firstplace = -1;
	int lastplace = -1;
	while (havebrackets) {
		//printstack(*exprlist);
		//printf("size:%d\n",exprlist->size());
		havebrackets = false;
		//check brackets
		for (int i = 0;i<exprlist->size();i++) {
			if (exprlist->get(i)->item.type == TYPE_OPER and exprlist->get(i)->item.oper == '(') {
				havebrackets = true;
				firstplace = i;
			}
			if (exprlist->get(i)->item.type == TYPE_OPER and exprlist->get(i)->item.oper == ')') {
				havebrackets = true;
				lastplace = i;
				break;
			}
		}
		if (havebrackets == true) {
			//printf("have brackets:from %d to %d\n",firstplace,lastplace);
			bfcalc(exprlist, firstplace, lastplace, ln);
			//printstack(*exprlist);
			//printf("size:%d\n",exprlist->size());
			firstplace = -1;lastplace = -1;
		}
	}
}
// 在语句两端加上括号方便bfcalc()解析
void rebracket_process(qLinkedList<item> *exprlist, int ln) {
	item lb, rb;
	lb.type = TYPE_OPER;
	lb.oper = '(';
	rb.type = TYPE_OPER;
	rb.oper = ')';
	exprlist->addfirst(lb);
	exprlist->addlast(rb);
	bfcalc(exprlist, 0, exprlist->size() - 1, ln);
}
// 逗号运算符解析
void commacalc(qLinkedList<item> *exprlist,int ln){
	printf(RECOLIC_TEXT("COMMA START\n"));
	for(int i=0;exprlist->get(i)!=NULL;i++){
		if(exprlist->get(i)->item.type==TYPE_OPER && exprlist->get(i)->item.oper==','){
			item lb, rb;
			lb.type = TYPE_OPER;
			lb.oper = '(';
			rb.type = TYPE_OPER;
			rb.oper = ')';
			exprlist->addAfter(exprlist->get(i),new qNode<item>(lb));
			exprlist->addBefore(exprlist->get(i),new qNode<item>(rb));
			i++;
		}
	}
	item lb, rb;
	lb.type = TYPE_OPER;
	lb.oper = '(';
	rb.type = TYPE_OPER;
	rb.oper = ')';
	exprlist->addfirst(lb);
	exprlist->addlast(rb);
	debracket_process(exprlist,ln);
};

// 保留字对应表
// operator D -> int
// operator J -> if
// operator W -> while loop
// operator F -> for loop
// operator P -> printf
// operator E -> else
// operator B -> break
// operator o -> do
// when sent in,';' in the end of statement is removed!

// 作用域栈元素类型
const int FIELD_NORMAL_TRUE = 0, FIELD_NORMAL_FALSE = 1, FIELD_LOOP_TRUE = 2, FIELD_LOOP_FALSE = 3, FIELD_DLOOP = 4,FIELD_ELSE=5;
// 作用域栈
qLinkedList<int> fieldStack;
// 检测作用域是否正常关闭，若没有则关闭
void check_stack_closure_normal(int ln,bool FLAG_CHECKELSE=true){
	printf(RECOLIC_TEXT("LN>%d invoked close stack\n"),ln);
	
	if (fieldStack.popable() && fieldStack.first->item == FIELD_NORMAL_TRUE) {
		// didn't close field normally!
		// close it and pop the stack.
		fieldStatement(ln, S_FIELD_END);
		fieldStack.popfirst();
		while(FLAG_CHECKELSE && fieldStack.popable() && fieldStack.first->item == FIELD_ELSE){
			fieldStatement(ln, S_FIELD_END);
			fieldStack.popfirst();
		}
		while (fieldStack.popable() && (fieldStack.first->item == FIELD_NORMAL_TRUE or fieldStack.first->item == FIELD_LOOP_TRUE)) {
			switch(fieldStack.first->item){
				case FIELD_NORMAL_TRUE:
					fieldStatement(ln, S_FIELD_END);
					fieldStack.popfirst();
				break;
				case FIELD_LOOP_TRUE:
					gotoStatement(ln);
					fieldStatement(ln, S_FIELD_END);
					fieldStack.popfirst();
				break;
				default:
					
					break;
			}
		}
		
	}else if(fieldStack.popable() && fieldStack.first->item == FIELD_ELSE){
		while(FLAG_CHECKELSE && fieldStack.popable() && fieldStack.first->item == FIELD_ELSE){
			fieldStatement(ln, S_FIELD_END);
			fieldStack.popfirst();
		}
	}
	else if (fieldStack.popable() && fieldStack.first->item == FIELD_LOOP_TRUE) {
		// for loops
		while (fieldStack.popable() && (fieldStack.first->item == FIELD_NORMAL_TRUE or fieldStack.first->item == FIELD_LOOP_TRUE)) {
			switch(fieldStack.first->item){
				case FIELD_NORMAL_TRUE:
					fieldStatement(ln, S_FIELD_END);
					fieldStack.popfirst();
				break;
				case FIELD_LOOP_TRUE:
					gotoStatement(ln);
					fieldStatement(ln, S_FIELD_END);
					fieldStack.popfirst();
				break;
				default:
					
					break;
			}
			
		}
	}
}

//带关键字表达式处理
void rcalc(qLinkedList<item> *exprlist, int ln) {
	/* 可能存在的所有关键字形式（大略版）:
	* D varname = expression(or just close)
	* W expression
	* F expression ; expression ; expression
	* P
	* J expression
	* a=b like:not have startup operators
	* } FIELD_END
	*/
	// detect startup operators
	if (exprlist->first->item.type == TYPE_OPER) {
		switch (exprlist->first->item.oper) {
		case 'D':
			for(int i=0;exprlist->get(i)!=NULL;i++){
				if(exprlist->get(i)->item.type==TYPE_OPERAND_VAR){
					if(exprlist->get(i+1)!=NULL && exprlist->get(i+1)->item.type==TYPE_OPER && exprlist->get(i+1)->item.oper=='=')
						defineStatement(ln, exprlist->get(i)->item.number);
				}
			}
			exprlist->popfirst();
			printstack(*exprlist);
			commacalc(exprlist,ln);
			//if(FLAG_CHECKCLOSE)
				check_stack_closure_normal(ln);
			break;
		case 'J':
			exprlist->popfirst();
			fieldStatement(ln, S_FIELD_BEGIN);
			if (exprlist->last->item.oper == '{') {
				exprlist->poplast();
				commacalc(exprlist,ln);
				if(exprlist->first->item.type==TYPE_OPERAND){
					ifStatement(ln, itos(exprlist->first->item.number));
				}else{
					ifStatement(ln, formatVarName(exprlist->first->item.number));
				}
				fieldStack.addfirst(FIELD_NORMAL_FALSE);
			}
			else {
				commacalc(exprlist, ln);
				if(exprlist->first->item.type==TYPE_OPERAND){
					ifStatement(ln, itos(exprlist->first->item.number));
				}else{
					ifStatement(ln, formatVarName(exprlist->first->item.number));
				}
				fieldStack.addfirst(FIELD_NORMAL_TRUE);
			}
			fieldStatement(ln, S_FIELD_BEGIN);
			fieldStack.addfirst(FIELD_NORMAL_TRUE);
			break;
		case 'W':
			if (!fieldStack.popable() || fieldStack.first->item != FIELD_DLOOP) {
				// not a do-while loop!
				fieldStatement(ln, S_FIELD_BEGIN);
				if (exprlist->last->item.oper == '{') {
					exprlist->poplast();
					gotodestStatement(ln);
					commacalc(exprlist, ln);
					fieldStack.addfirst(FIELD_NORMAL_FALSE);
					fieldStatement(ln,S_FIELD_BEGIN);
					if(exprlist->first->item.type==TYPE_OPERAND){
						ifStatement(ln, itos(exprlist->first->item.number));
					}else{
						ifStatement(ln, formatVarName(exprlist->first->item.number));
					}
					fieldStack.addfirst(FIELD_LOOP_FALSE);
				}
				else {
					gotodestStatement(ln);
					commacalc(exprlist, ln);
					fieldStack.addfirst(FIELD_NORMAL_TRUE);
					fieldStatement(ln,S_FIELD_BEGIN);
					if(exprlist->first->item.type==TYPE_OPERAND){
						ifStatement(ln, itos(exprlist->first->item.number));
					}else{
						ifStatement(ln, formatVarName(exprlist->first->item.number));
					}
					fieldStack.addfirst(FIELD_LOOP_TRUE);
				}
			}
			else {
				// is a do-while loop!
				fieldStack.popfirst();
				exprlist->popfirst();
				commacalc(exprlist, ln);
				if(exprlist->first->item.type==TYPE_OPERAND){
					ifStatement(ln, itos(exprlist->first->item.number));
				}else{
					ifStatement(ln, formatVarName(exprlist->first->item.number));
				}
				gotoStatement(ln);
				fieldStatement(ln, S_FIELD_END);
			}
			break;
		case 'F':
		{// fix the cross-initialization problem.
		 // divide the whole statement.
			exprlist->popfirst();
			qLinkedList<item> initexpr, condexpr, iterexpr;
			for (int i = 1;exprlist->get(i)->item.oper != ';';exprlist->popfirst()) {
				initexpr.addlast(exprlist->get(i)->item);
			}
			exprlist->popfirst();
			//printstack(*exprlist);
			for (int i = 1;exprlist->get(i)->item.oper != ';';exprlist->popfirst()) {
				condexpr.addlast(exprlist->get(i)->item);
			}
			if(!condexpr.popable()){
				item it;
				it.type = TYPE_OPERAND;
				it.number=1;
				condexpr.addlast(it);
			}
			//printstack(condexpr);
			exprlist->popfirst();
			for (int i = 1;exprlist->get(i)->next != NULL;exprlist->popfirst()) {
				iterexpr.addlast(exprlist->get(i)->item);
			}
			//printstack(initexpr);
			// qLinkedList<item> cpcondexpr(condexpr);
			// these statements divided suc.
			fieldStatement(ln, S_FIELD_BEGIN);
			if (exprlist->last->item.oper == '{') {
				exprlist->poplast();
				if(initexpr.popable()){
					rcalc(&initexpr,ln);
				}
				assignStatement(ln, -998, RECOLIC_TEXT("0"));
				gotodestStatement(ln);
				fieldStatement(ln, S_FIELD_BEGIN);
				ifStatement(ln, formatVarName(-998));
				commacalc(&iterexpr, ln);
				fieldStatement(ln, S_FIELD_END);
				assignStatement(ln, -998, RECOLIC_TEXT("1"));
				commacalc(&condexpr, ln);
				//printstack(initexpr);
				
				fieldStatement(ln,S_FIELD_BEGIN);
				if(condexpr.first->item.type==TYPE_OPERAND){
					ifStatement(ln, itos(condexpr.first->item.number));
				}else{
					ifStatement(ln, formatVarName(condexpr.first->item.number));
				}
				fieldStack.addfirst(FIELD_NORMAL_FALSE);
				fieldStack.addfirst(FIELD_LOOP_TRUE);
			}
			else {
				if(initexpr.popable()){
					for(int i=0;initexpr.get(i)!=NULL;i++){
						if(initexpr.first->item.type==TYPE_OPER && initexpr.first->item.oper=='D' && initexpr.get(i)->item.type==TYPE_OPERAND_VAR){
							if(initexpr.get(i+1)!=NULL && initexpr.get(i+1)->item.type==TYPE_OPER && initexpr.get(i+1)->item.oper=='='){
								//printf(RECOLIC_TEXT("DEFINE!\n"));
								defineStatement(ln, initexpr.get(i)->item.number);
							}
						}
					}
					initexpr.popfirst();
					//printstack(initexpr);
					commacalc(&initexpr, ln);
				}
				assignStatement(ln, -998, RECOLIC_TEXT("0"));
				gotodestStatement(ln);
				fieldStatement(ln, S_FIELD_BEGIN);
				ifStatement(ln,formatVarName(-998));
				commacalc(&iterexpr, ln);
				fieldStatement(ln, S_FIELD_END);
				assignStatement(ln, -998, RECOLIC_TEXT("1"));
				commacalc(&condexpr, ln);
				
				fieldStatement(ln,S_FIELD_BEGIN);
				if(condexpr.first->item.type==TYPE_OPERAND){
					ifStatement(ln, itos(condexpr.first->item.number));
				}else{
					ifStatement(ln, formatVarName(condexpr.first->item.number));
				}
				fieldStack.addfirst(FIELD_NORMAL_TRUE);
				fieldStack.addfirst(FIELD_LOOP_TRUE);
			}
		}
		break;
		case 'P':
			
			printStatement(ln);
			//if(FLAG_CHECKCLOSE)
				//check_stack_closure_normal(ln);
			break;
		case 'E':
			fieldStatement(ln, S_FIELD_BEGIN);
			fieldStack.addfirst(FIELD_ELSE);
			elseStatement(ln);
			
			break;
		case 'B':
			breakStatement(ln);
			//if(FLAG_CHECKCLOSE)
				check_stack_closure_normal(ln);
			break;
		case 'o':
			fieldStack.addfirst(FIELD_DLOOP);
			fieldStatement(ln, S_FIELD_BEGIN);
			gotodestStatement(ln);
			break;
		case '{':
			fieldStack.addfirst(FIELD_NORMAL_FALSE);
			fieldStatement(ln, S_FIELD_BEGIN);
			break;
		case '}':
			//close field
			if (fieldStack.popable() && fieldStack.first->item == FIELD_NORMAL_FALSE) {
				// didn't close field normally!
				// close it and pop the stack.
				fieldStatement(ln, S_FIELD_END);
				fieldStack.popfirst();
				while(fieldStack.popable() && fieldStack.first->item == FIELD_ELSE){
					fieldStatement(ln, S_FIELD_END);
					fieldStack.popfirst();
				}
				while (fieldStack.popable() && (fieldStack.first->item == FIELD_NORMAL_TRUE or fieldStack.first->item == FIELD_LOOP_TRUE)) {
					switch(fieldStack.first->item){
						case FIELD_NORMAL_TRUE:
							fieldStatement(ln, S_FIELD_END);
							fieldStack.popfirst();
						break;
						case FIELD_LOOP_TRUE:
							gotoStatement(ln);
							fieldStatement(ln, S_FIELD_END);
							fieldStack.popfirst();
						break;
						default:
							
							break;
					}
					
				}
			}
			else if (fieldStack.popable() && fieldStack.first->item == FIELD_LOOP_FALSE) {
				// for loops
				gotoStatement(ln);
				fieldStatement(ln, S_FIELD_END);
				fieldStack.popfirst();
				while (fieldStack.popable() && (fieldStack.first->item == FIELD_NORMAL_TRUE or fieldStack.first->item == FIELD_LOOP_TRUE)) {
					switch(fieldStack.first->item){
						case FIELD_NORMAL_TRUE:
							fieldStatement(ln, S_FIELD_END);
							fieldStack.popfirst();
						break;
						case FIELD_LOOP_TRUE:
							gotoStatement(ln);
							fieldStatement(ln, S_FIELD_END);
							fieldStack.popfirst();
						break;
						default:
							
							break;
					}
					
				}
			}
			break;
		default:
			// that's a statement that have no meanings!
			// but we still have to execute it due to the fucking requirements!
			commacalc(exprlist, ln);
			//if(FLAG_CHECKCLOSE)
				check_stack_closure_normal(ln);
			// check if last didn't use } to close field.
			// true: didn't close normally(using '{' & '}')
			// and commit close.
			
			break;
		}
	}
	else {
		// that indicates it's an very simple assign statement(but may have brackets!).
		commacalc(exprlist, ln);
		// check if last didn't use } to close field.
		// true: didn't close normally(using '{' & '}')
		// and commit close.
		//if(FLAG_CHECKCLOSE)
			check_stack_closure_normal(ln);
	}
	clearTempVarFlags();
}
//将字符串转为整数
int qatoi(string str) {
	if (str[0] == '.')
		str = RECOLIC_TEXT("0") + str;
	if (str[str.size() - 1] == '.')
		str += RECOLIC_TEXT("0");
	return stoi(str);
}

//有效数字字符
string valid_numbers(RECOLIC_TEXT("1234567890"));

//括号栈类型
const int BRACKET_FLAG_NOLOOP = -1, BRACKET_FLAG_LOOP = 0, BRACKET_FLAG = 1;

// 将 deque<statement> 转换为解析器标准解析类型 qLinkedLists 并传给 rcalc()

// when detected any cond or loop statements,use bracket-stack to check the close of this statement.
// when meet { or } , close statement immediately.the close result must include { or } itself.
// when meet "else" , close statement immediately.

void genExpr() {
	vstack_new_field();
	// convert deque input_buf into a single string.
	string expr(RECOLIC_TEXT(""));
	int ln = input_buf[0].lineNum;
	int input_iterator = 0;
	expr = input_buf[input_iterator].text;
	char readbuffer = 0;
	int for_comment = 0;
	string numberbuffer;
	qLinkedList<item> *exprlist = new qLinkedList<item>();
	qLinkedList<int> bracketstack;
	bracketstack.addfirst(BRACKET_FLAG_NOLOOP);
	bool flag_exprclosed = false;
	// read all valid sing-line expr to an linkedlist.
	// and sent to rcalc immediately.
	// in case all var names are replaced,
	// check start-up char directly.
	for (int i = 0;i<expr.size();i++) {
		readbuffer = expr[i];
		//printf(RECOLIC_TEXT("READ CHAR: %c\n"),readbuffer);
		if (readbuffer != 0 and readbuffer != ' ') {
			if (valid_numbers.find(readbuffer) != string::npos) {
				if (expr[i - 1] == '@') {
					// case VARIABLE
					numberbuffer += RECOLIC_TEXT("@");
				}
				numberbuffer += readbuffer;
			}
			else {
				//reached an operator
				//close numberbuffer
				if (numberbuffer != RECOLIC_TEXT("")) {
					item it;
					if (numberbuffer[0] == '@') {
						numberbuffer.erase(0, 1);
						it.type = TYPE_OPERAND_VAR;
					}
					else {
						it.type = TYPE_OPERAND;
					}
					it.number = qatoi(numberbuffer);
					//printf("num:%.2lf\n",it.number);
					exprlist->addlast(it);
					numberbuffer = RECOLIC_TEXT("");
				}
				if (readbuffer == 'i' and expr[i + 1] == 'n') {
					i += 2;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'D';
					exprlist->addlast(it);
				}
				else if (readbuffer == 'i' and expr[i + 1] == 'f') {
					i += 1;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'J';
					exprlist->addlast(it);
					bracketstack.last->item=BRACKET_FLAG_LOOP;
				}
				else if (readbuffer == 'w') {
					i += 4;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'W';
					exprlist->addlast(it);
					bracketstack.last->item = BRACKET_FLAG_LOOP;
				}
				else if (readbuffer == 'f') {
					for_comment = 2;
					i += 2;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'F';
					exprlist->addlast(it);
					bracketstack.last->item = BRACKET_FLAG_LOOP;
				}
				else if (readbuffer == 'd') {
					i += 1;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'o';
					exprlist->addlast(it);
				}
				else if (readbuffer == 'p') {
					i += 5;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'P';
					exprlist->addlast(it);
					flag_exprclosed = true;
					printf(RECOLIC_TEXT("EXPR CLOSE.\n"));
				}
				else if (readbuffer == 'e') {
					i += 3;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'E';
					exprlist->addlast(it);
					flag_exprclosed = true;
					printf(RECOLIC_TEXT("EXPR CLOSE.\n"));
				}
				else if (readbuffer == 'b') {
					i += 4;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'B';
					exprlist->addlast(it);
				}
				else if (readbuffer == '(') {
					item it;
					it.type = TYPE_OPER;
					it.oper = readbuffer;
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
					if (bracketstack.first->item == BRACKET_FLAG or bracketstack.first->item == BRACKET_FLAG_LOOP) {
						bracketstack.addfirst(BRACKET_FLAG);
					}
				}
				else if (readbuffer == ')') {
					item it;
					it.type = TYPE_OPER;
					it.oper = readbuffer;
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
					if (bracketstack.first->item == BRACKET_FLAG) {
						bracketstack.popfirst();
						if (bracketstack.first->item == BRACKET_FLAG_LOOP) {
							bracketstack.first->item = BRACKET_FLAG_NOLOOP;
							flag_exprclosed = true;
							printf(RECOLIC_TEXT("EXPR CLOSE.\n"));
						}
					}
				}
				else if (readbuffer == '{' or readbuffer == '}') {
					item it;
					it.type = TYPE_OPER;
					it.oper = readbuffer;
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
					flag_exprclosed = true;
					printf(RECOLIC_TEXT("EXPR CLOSE.\n"));
				}
				else if (readbuffer == ';') {
					if (for_comment>0) {
						for_comment--;
						item it;
						it.type = TYPE_OPER;
						it.oper = readbuffer;
						//printf("oper:%c\n",readbuffer);
						exprlist->addlast(it);
					}
					else {
						flag_exprclosed = true;
						printf(RECOLIC_TEXT("EXPR CLOSE.\n"));
					}
				}
				else if (readbuffer == '+' and expr[i + 1] == '+') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'i';
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '-' and expr[i + 1] == '-') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'd';
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '>' and expr[i + 1] == '=') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'L';;
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '<' and expr[i + 1] == '=') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'S';
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '=' and expr[i + 1] == '=') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'e';
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '!' and expr[i + 1] == '=') {
					i++;
					item it;
					it.type = TYPE_OPER;
					it.oper = 'n';
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
				else if (readbuffer == '_' or readbuffer == '#' or readbuffer == '@') {
					// do nothing.
				}
				else {
					item it;
					it.type = TYPE_OPER;
					it.oper = readbuffer;
					//printf("oper:%c\n",readbuffer);
					exprlist->addlast(it);
				}
			}
			//if statement closed,sent to rcalc.
			if (flag_exprclosed) {
				printstack(*exprlist);
				if(!exprlist->popable()){
					item it;
					it.type = TYPE_OPERAND;
					it.number=1;
					exprlist->addlast(it);
				}
				rcalc(exprlist, ln);
				delete exprlist;
				exprlist = new qLinkedList<item>();
				flag_exprclosed=false;
			}
			
		}
		// force close all unclosed statemnts and fields
		if (i + 1 >= expr.size() and input_iterator + 1<input_buf.size()) {
			do{
				printf(RECOLIC_TEXT("NEXT LN\n"));
				printf(input_buf[input_iterator+1].text.c_str());
				printf(RECOLIC_TEXT("\n"));
				input_iterator++;
				expr = input_buf[input_iterator].text;
				ln = input_buf[input_iterator].lineNum;
				i = -1;
			}while(expr.empty());
		}
	}
	/*// close numberbuffer
	if(numberbuffer!=""){
	item it;
	if(numberbuffer[0]=='@'){
	numberbuffer.erase(0,1);
	it.type=TYPE_OPERAND_VAR
	}else{
	it.type=TYPE_OPERAND;
	}
	it.number=qatof(numberbuffer);
	//printf("num:%.2lf\n",it.number);
	exprlist.addlast(it);
	numberbuffer="";
	}
	rcalc(&exprlist);*/
	{
		item it;
		it.type = TYPE_OPERAND;
		it.number=1;
		exprlist->addlast(it);
		rcalc(exprlist, ln);
	}
	vstack_pop_field();
}

// 主框架入口函数
int analyse_main() {
	// deque<statement> buf is input global var
	// all un-analysed statement is typed as S_ERROR
	genExpr();
	return 0;
}
