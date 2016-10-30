#pragma once
#include <deque>
#include <vector>
#include "statements.hpp"

extern deque<statement> input_buf;//input to analyser.
extern deque<statement> buf;//output from analyser.
extern vector<int> print_buffer;