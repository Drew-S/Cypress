#pragma once

#include <vector>
#include <string>

#include "token.hpp"

using namespace std;

// TODO: Convert token tuple (Token, string) into struct (Token, string, int,  int)
//                                                       (token, value,  line, col)
vector<pair<Token, string>> lex(string);

vector<pair<Token, string>> lex_file(string);
