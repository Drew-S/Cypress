#pragma once

#include <iostream>
#include <string>
#include <regex>

using namespace std;

enum Token {
    KEYWORD,         // import, fn, struct, enum, interface, if, else, switch, case, default, for, return
    TYPE,            // int, float, string, char, bool, IDENT (user types)
    IDENT,           // ^([a-zA-Z]|_[a-zA-Z])[a-zA-Z_0-9]*
    IGNORE,          // _
    COMMENT,         // //.*| /\*(.*|\n)*\*/
    BLOCK_START,     // {
    BLOCK_END,       // }
    ASSIGN,          // =
    ARITH_OP,        // + - / * %
    LOG_OP,          // && || < > <= >= == != !
    BIT_OP,          // & ^ | ~ << >>
    ASSIGN_ARITH_OP, // += -= /= *= ++ --
    ASSIGN_BIT_OP,   // &= ^= |= ~= <<= >>=
    PAR_START,       // (
    PAR_END,         // )
    ARR_START,       // [
    ARR_END,         // ]
    SPEC,            // : -> , ; len print copy append
    INT,             // [0-9]+
    FLOAT,           // [0-9]*\.[0-9]+
    STRING,          // ".*"
    CHAR,            // '.{1}'
    ROOT,            // Special token for root of AST
};

void print_pair(pair<Token, string>);
