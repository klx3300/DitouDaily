#pragma once
#include <deque>
#include <vector>
#include <stack>
#include "statements.hpp"

#define cout
#define wcout
#define printf
#define wprintf

extern deque<statement> input_buf;//input to analyser.
extern deque<statement> buf;//output from analyser.
extern vector<int> print_buffer;
extern void __cdecl global_new_handler();
