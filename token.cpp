#include "token.hpp"

// Prints the pair token for debug purposes "TYPE: content"
void print_pair(pair<Token, string> p) {
    string pre;
    switch (p.first) {
        case Token::IDENT:
            pre = "IDENT          : ";
            break;
        case Token::ASSIGN:
            pre = "ASSIGN         : ";
            break;
        case Token::INT:
            pre = "INT            : ";
            break;
        case Token::KEYWORD:
            pre = "KEYWORD        : ";
            break;
        case Token::TYPE:
            pre = "TYPE           : ";
            break;
        case Token::IGNORE:
            pre = "IGNORE         : ";
            break;
        case Token::BLOCK_START:
            pre = "BLOCK_START    : ";
            break;
        case Token::BLOCK_END:
            pre = "BLOCK_END      : ";
            break;
        case Token::ARITH_OP:
            pre = "ARITH_OP       : ";
            break;
        case Token::ASSIGN_ARITH_OP:
            pre = "ARITH_ASSIGN_OP: ";
            break;
        case Token::BIT_OP:
            pre = "BIT_OP         : ";
            break;
        case Token::ASSIGN_BIT_OP:
            pre = "ASSIGN_BIT_OP  : ";
            break;
        case Token::LOG_OP:
            pre = "LOG_OP         : ";
            break;
        case Token::PAR_START:
            pre = "PAR_START      : ";
            break;
        case Token::PAR_END:
            pre = "PAR_END        : ";
            break;
        case Token::ARR_START:
            pre = "ARR_START      : ";
            break;
        case Token::ARR_END:
            pre = "ARR_END        : ";
            break;
        case Token::COMMENT:
            pre = "COMMENT        : ";
            break;
        case Token::FLOAT:
            pre = "FLOAT          : ";
            break;
        case Token::SPEC:
            pre = "SPEC           : ";
            break;
        case Token::STRING:
            pre = "STRING         : ";
            break;
        case Token::CHAR:
            pre = "CHAR           : ";
            break;
        case Token::ROOT:
            break;
    }

    cout << pre << regex_replace(p.second, regex("\\n"), "\\n") << endl;
};
