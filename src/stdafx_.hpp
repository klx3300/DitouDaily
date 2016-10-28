#pragma once
#include <deque>
#include <vector>
#include <stack>
#include "statements.hpp"

#define cout
#define wcout
#define printf
#define wprintf

extern deque<statement> buf;
extern vector<int> print_buffer;
extern void __cdecl global_new_handler();
