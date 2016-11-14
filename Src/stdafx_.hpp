//这是每一个cpp都必须包含的公有头文件
#pragma once
#pragma warning(disable : 4005)
#pragma warning(disable : 4018)
#pragma warning(disable : 4715)
#include <deque>
#include <vector>
#include "statements.hpp"//包含statement类定义

extern deque<statement> input_buf;//input to analyser.
extern deque<statement> buf;//output from analyser.
extern vector<int> print_buffer;