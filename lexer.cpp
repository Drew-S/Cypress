#include <fstream>
#include <iostream>
#include <regex>

#include "lexer.hpp"

#define SCAN_TIL(source, i, str, t, n) \
    while (source[i] != t && source[i] != n && i < (int)source.size()) { \
        str += source[i]; \
        i++; \
    }

using namespace std;

// Checks if an IDENT is a keyword
static const bool IS_KEYWORD(string in) {
    if (
        in == "import" ||
        in == "fn" ||
        in == "struct" ||
        in == "enum" ||
        in == "interface" ||
        in == "if" ||
        in == "else" ||
        in == "switch" ||
        in == "case" ||
        in == "default" ||
        in == "for" ||
        in == "return"
    )
        return true;

    return false;
}

// Checks if an IDENT is a type
static const bool IS_TYPE(string in)  {
    if (
        in == "int" ||
        in == "float" ||
        in == "string" ||
        in == "char" ||
        in == "bool"
    )
        return true;

    return false;
}

// Lexer to convert a string input to a token list
vector<pair<Token, string>> lex(string source) {
    vector<pair<Token, string>> out;

    // Loops over each and every character
    for (int i = 0; i < (int)source.size(); i++) {
        pair<Token, string> v;

        string str = "";
        str += source[i];

        // Ignore whitespace
        if (source[i] == ' ' || source[i] == '\n' || source[i] == '\t') {
            continue;

        // floating point number
        } else if (str == ".") {
            v.first = Token::FLOAT;
            i++;
            SCAN_TIL(source, i, str, ' ', '\n')

        // The character is the start of an IDENT
        } else if (regex_match(str, regex("[_a-zA-Z]"))) {
            if (str == "_" && !regex_match(str + source[i+1], regex("_[_a-zA-Z0-9]")))
                v.first = Token::IGNORE;
            else
                v.first = Token::IDENT;

            i++;
            SCAN_TIL(source, i, str, ' ', '\n')

            if (IS_KEYWORD(str))
                v.first = Token::KEYWORD;

            else if (IS_TYPE(str))
                v.first = Token::TYPE;

        // The character is an assign_op
        } else if (str == "=") {
            v.first = Token::ASSIGN;

            if (source[i+1] == '=') {
                v.first = Token::LOG_OP;
                str += source[++i];
            }

        // The character is a number
        } else if (regex_match(str, regex("[0-9]"))) {
            // Default to integer
            v.first = Token::INT;
            i++;
            SCAN_TIL(source, i, str, ' ', '\n')

            // The number is a float
            if (regex_match(str, regex("[0-9]*\\.[0-9]+"))) {
                v.first = Token::FLOAT;
            }

        // The character is the start of a scoped block
        } else if (str == "{") {
            v.first = Token::BLOCK_START;

        // The character is the end of a scoped block
        } else if (str == "}") {
            v.first = Token::BLOCK_END;

        // Character is an arithmetic operater + - * / %
        } else if (regex_match(str, regex("\\+|-|\\*|/|%"))) {
            v.first = Token::ARITH_OP;

            // Arithmetic operater is followed by an =, or is ++ or -- therefore its an arithmetic assignment operator
            if (regex_match(str, regex("\\+|-|\\*|/")) && source[i+1] == '=') {
                i++;
                str += '=';
                v.first = Token::ASSIGN_ARITH_OP;

            } else if (str == "+" && source[i+1] == '+') {
                i++;
                str += '+';
                v.first = Token::ASSIGN_ARITH_OP;

            } else if (str == "-" && source[i+1] == '-') {
                i++;
                str += '-';
                v.first = Token::ASSIGN_ARITH_OP;

            // Comments
            } else if (str == "/" && source[i+1] == '/') {
                i++;
                v.first = Token::COMMENT;
                SCAN_TIL(source, i, str, '\n', '\n')

            } else if (str == "/" && source[i+1] == '*') {
                v.first = Token::COMMENT;
                str += source[++i];
                i++;
                while (source[i] != '*') {
                    str += source[i++];
                    if (source[i] == '*') {
                        str += source[i++];
                        if (source[i] == '/') {
                            str += source[i++];
                            break;
                        }
                    }
                }

            // Specials ->
            } else if (str == "-" && source[i+1] == '>') {
                v.first = Token::SPEC;
                i++;
                str += '>';
            }

        // Bitwise values & ^ | ~
        } else if (regex_match(str, regex("&|\\^|\\||~"))) {
            v.first = Token::BIT_OP;

            // Bitwise assign operator &= |= ^= ~=
            if (source[i+1] == '=') {
                i++;
                str += '=';
                v.first = Token::ASSIGN_BIT_OP;

            // && or || makes this a logical operator
            } else if((str == "&" && source[i+1] == '&') || (str == "|" && source[i+1] == '|')) {
                i++;
                str += source[i];
                v.first = Token::LOG_OP;
            }

        // Logical operators < > <= >= != == !
        } else if (regex_match(str, regex("<|>|!"))) {
            v.first = Token::LOG_OP;

            if (source[i+1] == '=') {
                str += source[++i];

            // If << or >> its a bit operator
            } else if ((str == "<" && source[i+1] == '<') || (str == ">" && source[i+1] == '>')) {
                v.first = Token::BIT_OP;
                str += source[++i];

                // if <<= or >>= its a bit assign operator
                if (source[i+1] == '=') {
                    v.first = Token::ASSIGN_BIT_OP;
                    str += source[++i];
                }
            }

        // Parenthesis start
        } else if (str == "(") {
            v.first = Token::PAR_START;
        
        // Parenthesis end
        } else if (str == ")") {
            v.first = Token::PAR_END;

        // Array start
        } else if (str == "[") {
            v.first = Token::ARR_START;

        // Array end
        } else if (str == "]") {
            v.first = Token::ARR_END;

        // Special operators
        } else if (regex_match(str, regex(":|;|,"))) {
            v.first = Token::SPEC;

        // Strings
        } else if (str == "\"") {
            v.first = Token::STRING;
            i++;
            SCAN_TIL(source, i, str, '"', '"')
            str += '"';
            i++;

        // Character
        } else if (str == "'") {
            v.first = Token::CHAR;
            i++;
            SCAN_TIL(source, i, str, '\'', '\'')
            str += '\'';
            i++;
        }

        v.second = str;

        out.push_back(v);
    }

    return out;
}

vector<pair<Token, string>> lex_file(string f) {
    fstream file;
    file.open(f);

    string copy, tmp;

    while(getline(file, tmp)) {
        copy += tmp;
        copy += '\n';
    }

    file.close();

    return lex(copy);
}
