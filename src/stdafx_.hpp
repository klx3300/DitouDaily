#pragma once
#include <deque>
#include <vector>
#include <stack>
#include <memory>
#include "statements.hpp"
#define VIRTUAL_MEMORY_SIZE 256

extern deque<statement> buf;
extern shared_ptr<int> pMemory;
extern vector<int> print_buffer;
extern void __cdecl global_new_handler();
