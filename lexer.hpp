#pragma once

#include <vector>
#include <string>

#include "token.hpp"

using namespace std;

vector<pair<Token, string>> lex(string);

vector<pair<Token, string>> lex_file(string);
