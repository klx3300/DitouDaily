#pragma once
#include <deque>
#include <vector>
#include <stack>
#include "statements.hpp"

extern deque<statement> buf;
extern vector<int> print_buffer;
extern void __cdecl global_new_handler();
