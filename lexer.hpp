#pragma once

#include <vector>
#include <string>

#include "token.hpp"

using namespace std;

string keywords[11] = {
    "import",
    "fn",
    "struct",
    "enum",
    "interface",
    "if",
    "else",
    "switch",
    "case",
    "default",
    "for",
};

string primitives[5] = {
    "int",
    "float",
    "string",
    "char",
    "bool",
};

vector<TokenCap> lex(string);

vector<TokenCap> lex_file(string);
