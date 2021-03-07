#pragma once

#include <string>

using namespace std;

enum Token {
    KEYWORD,         // import, fn, struct, enum, interface, if, else, switch, case, default, for, return

    TYPE,            // int, float, string, char, bool

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

    SPEC,            // : -> , len print copy append

    INT,             // [0-9]+
    FLOAT,           // [0-9]*\.[0-9]+
    STRING,          // ".*"
    CHAR,            // '.{1}'
    BOOL,            // true|false

    STATEMENT_END,   // ; \n
};

struct TokenCap {
    Token token;
    string val;
    unsigned int line;
    unsigned int col;
};

void print_pair(TokenCap, int);
