# DitouDaily
# FAQ:
http://dian.hust.edu.cn/seedpk/
1 文档输出格式补充说明
输出格式参考样例文件中的output.txt
输出为一行，内容为输入文件C代码运行流程的行号（以空格作为分隔符），末尾不加空格或回车。
例如：行号空格行号空格…..行号。
例如：1 2 3 4 5

2 Q:一行多个语句 那是一个语句输出一次行号 还是只算一次?
答：一行多个语句，只算一次，输出一次行号。

3 Q:是否会出现while(1)?
答：while(1) 或 while(apple) 其中apple=1 属于合法用例，会出现。但不会死循环。

4 Q:代码中的空白符，除了\r\n空格还有别的吗？
答：会出现制表符 \t

5 Q:是否可能出现此类用例 int a = 1; for…..
答：对于所有的输入文件，每行不会超过5个语句，int a = 1; for….. 属于合法用例。

6 Q:会不会复合赋值操作符，+= -= *=
答：赋值语句格式为 ： 变量=值或表达式，表达式不多于10个运算符。不会出现复合赋值。

7 Q:逗号仅出现在声明中？
答：也会出现在printf、循环语句中。但不会超过5个

8 Q：C++标准库
答：C++标准库参考：https://msdn.microsoft.com/zh-cn/library/a7tkse1h.aspx

9 Q：关于顺序结构语句语法的描述
答：
以下[]中的内容重复0次或多次出现。
声明语句： int 变量名[,变量名];
初始化语句：int 变量名=值或表达式[,变量=值或表达式];（只要该行存在声明以及赋值操作，那么属于初始化语句例如 int a,b=3;）
赋值语句：变量名=值或表达式;
输出语句：printf(“xxx is %d[%d]\n”, 变量名或表达式[,变量名或表达式]);
空语句: 空白符号;
以上表达式包含：变量名、十进制整数的运算，运算符只包含+,-,*,\,++,–; 并且一个表达式中不会超过十个运算符。

10 Q:关于for循环语句语法描述
答：
以下[]中的内容重复0次或多次出现。
for(表达式或声明语句[,表达式];条件表达式;表达式[,表达式]){
}
以上条件表达式格式为：表达式 比较符号 表达式;
以上表达式包含：变量名、十进制整数的运算，运算符只包含+,-,*,\,++,–; 并且一个表达式中不会超过十个运算符。
其中表达式或语句可能为 空或\t 且 空或\t 数目不定。
for语句括号中 “,” 出现不超过5个；极端5个”,”的情况 for (int a = 4, b = 3, c = 5, d = 2; a + b*2 < c + d; a = a - b, b++, c--) 并非所有测试用例都是极端情况，会按梯度划分。

11 同一语句都在出现在一行。
例如：
int a,b=10+20;
a=20*10;
if(内容在一行)
for(内容在一行)
while(内容在一行)
其中 “{” “}” 可能跨行。

12 所有表达式运算结果不超过int表示范围。

13 关于注释的分布。
注释不会出现在语句和括号内容中间。但会出现在语句与语句之间。
如下面都属于可能出现的情况：

int apple; //苹果

int apple; /*苹果*/ int orange;

int apple; /*苹
果*/ int orange;

/*苹果*/ apple=1+1; /*苹果*/ orange=apple++; /*苹果*/

承诺以下情况不会出现：
int /*苹果*/apple;

apple=/*苹果*/1+1;

if(/*苹果*/apple>2)

for(/*苹果*/apple=1;apple>2;apple++)

while(/*苹果*/apple–)

14 关于while语句结构
答：
以下[]中的内容重复0次或多次出现。
while(表达式[,表达式]){
}
以上表达式包含：变量名、十进制整数的运算，运算符只包含+,-,*,\,++,–; 并且一个表达式中不会超过十个运算符。
while语句中括号内的“,”数目不超过5个。

15 运算符的优先级
答：参考：http://zh.cppreference.com/w/cpp/language/operator_precedence

16 测试用例中去除前置自增自减运算符。例如++i此类情况。但后置自增自减运算符会出现。如i++;

17 关于if（条件表达式）中条件表达式的格式
答：变量名 比较符号 值（十进制整数）
