#pragma once
#include <deque>
#include <vector>
#include "statements.hpp"
#define VIRTUAL_MEMORY_SIZE 256

extern deque<statement> buf;
extern int *pMemory;
extern vector<int> print_buffer;
extern void __cdecl global_new_handler();