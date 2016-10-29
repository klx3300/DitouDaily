#pragma once
#include <deque>
#include <vector>
#include "statements.hpp"

deque<statement> input_buf;//input to analyser.
deque<statement> buf;//output from analyser.
vector<int> print_buffer;