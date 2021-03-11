#include <iostream>
#include <regex>
#include <stdio.h>

#include "token.hpp"

using namespace std;

// Prints the pair token for debug purposes "TYPE: content"
/*
 * print_pair
 *
 * Takes a pair of token string and prints it to the terminal. Used for debuging purposes
 */
void print_pair(TokenCap p, int i) {
    char buf[4];
    string pre;
    sprintf(buf, "%04d", i);
    pre += buf;
    pre += " ";
    sprintf(buf, "%03d", p.line);
    pre += buf;
    pre += ":";
    sprintf(buf, "%03d", p.col);
    pre += buf;
    pre += " ";

    switch (p.token) {
        case IDENT:
            pre += "IDENT          : ";
            break;
           
        case ASSIGN:
            pre += "ASSIGN         : ";
            break;
           
        case INT:
            pre += "INT            : ";
            break;
           
        case KEYWORD:
            pre += "KEYWORD        : ";
            break;
           
        case TYPE:
            pre += "TYPE           : ";
            break;
           
        case IGNORE:
            pre += "IGNORE         : ";
            break;
           
        case BLOCK_START:
            pre += "BLOCK_START    : ";
            break;
           
        case BLOCK_END:
            pre += "BLOCK_END      : ";
            break;
           
        case ARITH_OP:
            pre += "ARITH_OP       : ";
            break;
           
        case ASSIGN_ARITH_OP:
            pre += "ARITH_ASSIGN_OP: ";
            break;
           
        case BIT_OP:
            pre += "BIT_OP         : ";
            break;
           
        case ASSIGN_BIT_OP:
            pre += "ASSIGN_BIT_OP  : ";
            break;
           
        case LOG_OP:
            pre += "LOG_OP         : ";
            break;
           
        case PAR_START:
            pre += "PAR_START      : ";
            break;
           
        case PAR_END:
            pre += "PAR_END        : ";
            break;
           
        case ARR_START:
            pre += "ARR_START      : ";
            break;
           
        case ARR_END:
            pre += "ARR_END        : ";
            break;
           
        case COMMENT:
            pre += "COMMENT        : ";
            break;
           
        case FLOAT:
            pre += "FLOAT          : ";
            break;
           
        case SPEC:
            pre += "SPEC           : ";
            break;
           
        case STRING:
            pre += "STRING         : ";
            break;
           
        case CHAR:
            pre += "CHAR           : ";
            break;
           
        case STATEMENT_END:
            pre += "STATEMENT_END  : ";
            break;

        case BOOL:
            pre += "BOOL           : ";
            break;

        case INVALID:
            pre += "INVALID        : ";
            break;
    }

    cout << pre << regex_replace(p.val, regex("\\n"), "\\n") << endl;
};

string p_token(Token t) {
    switch (t) {
        case IDENT:
            return "IDENT";
        case ASSIGN:
            return "ASSIGN";
        case INT:
            return "INT";
        case KEYWORD:
            return "KEYWORD";
        case TYPE:
            return "TYPE";
        case IGNORE:
            return "IGNORE";
        case BLOCK_START:
            return "BLOCK_START";
        case BLOCK_END:
            return "BLOCK_END";
        case ARITH_OP:
            return "ARITH_OP";
        case ASSIGN_ARITH_OP:
            return "ASSIGN_ARITH_OP";
        case BIT_OP:
            return "BIT_OP";
        case ASSIGN_BIT_OP:
            return "ASSIGN_BIT_OP";
        case LOG_OP:
            return "LOG_OP";
        case PAR_START:
            return "PAR_START";
        case PAR_END:
            return "PAR_END";
        case ARR_START:
            return "ARR_START";
        case ARR_END:
            return "ARR_END";
        case COMMENT:
            return "COMMENT";
        case FLOAT:
            return "FLOAT";
        case SPEC:
            return "SPEC";
        case STRING:
            return "STRING";
        case CHAR:
            return "CHAR";
        case STATEMENT_END:
            return "STATEMENT_END";
        case BOOL:
            return "BOOL";
        case INVALID:
            return "INVALID";
            
    }
};
