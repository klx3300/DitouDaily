#2016“亚马逊·种子杯” 初赛试题

by Recolic,klx3300,qzwlecr

--------

##编译运行环境
	archlinux 4.8.4-1-ARCH
	g++ version 6.2.1
###编译方法：
	Makefile

--------

##设计结构与模块说明
程序由三部分组成：主框架、解析器、运算器。主框架负责文本的读入与初步处理，解析器负责文本的解析与语句的处理，运算器负责语句的执行与运算。

###主框架

--------

###解析器
将主框架输入的结果，解析成为运算器可以单步执行的语句。
可能产生以下语句：
- 声明语句（仅1个操作数）
- 赋值语句（最多3个操作数）
- 条件判断语句（仅有1个操作数）
- 条件判断失败语句（没有操作数）
- 跳转语句（没有操作数）
- 作用域打开/关闭语句（没有操作数）
- 输出语句（没有操作数）
解析器内使用栈维护当前作用域状态，使用栈和链表执行解析工作。

--------

###运算器
对于解析器输入的结果，从第一行语句开始运行。
- S_ERROR	报错。
- S_BLANK	跳过。
- S_BLANK_BUT_OUTPUT  输出行号，跳过。
- S_ASSIGN 输出行号，判断是否为声明语句，分割后分别处理，对生成的变量使用map存储。
- S_IF 输出行号。如果IF后变量为真，把下一个作用域后可能存在的ELSE作用域内全部语句置为S_BLANK；如果IF后变量为假，跳过IF作用域。
- S_ELSE 跳过。
- S_FIELD_BEGIN 开始一个新的作用域，作用域计数器自增。
- S_FIELD_END 结束一个作用域，删除所有作用域计数器大于当前值的变量。
- S_GOTO 向前找到上一个作用域内的S_GOTO_DEST，并跳转。
- S_GOTO_DEST 跳过。

--------

##实现功能
###基本功能

###设计优化
- 使用数组动态存储运算过程中的临时变量
- 使用map维护变量的作用域和值
- 没有使用语法树
- 使用运算器内部快速临时变量存储表达式返回值


--------
