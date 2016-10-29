#pragma once
#include <deque>
#include <vector>

#include "statements.hpp"

using namespace std;
deque<statement> input_buf;//input to analyser.
deque<statement> buf;//output from analyser.
vector<int> print_buffer;
