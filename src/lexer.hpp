#pragma once

#include <vector>
#include <string>

#include "token.hpp"

using namespace std;

vector<TokenCap> lex(string);

vector<TokenCap> lex_file(string);
