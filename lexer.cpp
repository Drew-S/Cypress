#include <fstream>
#include <iostream>
#include <regex>

#include "lexer.hpp"

using namespace std;

/*
 * IS_KEYWORD
 *
 * Checks the string input is a registered keyword, returning a cooresponding boolean.
 *
 * example: IS_KEYWORD("import") -> true
 *          IS_KEYWORD("mything") -> false
 */
static const bool IS_KEYWORD(string in) {
    for (int i = 0; i < 11; i++)
        if (keywords[i] == in)
            return true;

    return false;
}

/*
 * IS_TYPE
 *
 * Checks the string input to see if it a type primitive
 *
 * example: IS_TYPE("int") -> true
 *          IS_TYPE("mything") -> false
 */
static const bool IS_TYPE(string in)  {
    for (int i = 0; i < 5; i++)
        if (primitives[i] == in)
            return true;

    return false;
}

/*
 * lex
 *
 * Lexes a string input source (entire source code) and outputs a list of token tuples (Token, string)
 *
 * Entire string in, entire list out.
 *
 * TODO: Generalize the switch statement as much as possible to enable further editing down the road easier.
 * TODO: Implement a file version of this that is a token stream directly from source code.
 */
vector<TokenCap> lex(string source) {
    vector<TokenCap> out;

    unsigned int c = 1, l = 1;

    // Loops over each and every character
    for (int i = 0; i < (int)source.size(); i++, c++) {
        TokenCap v;

        string str = "";
        str += source[i];
        v.line = l;
        v.col = c;

        // Ignore whitespace
        if (source[i] == ' ' || source[i] == '\t') {
            continue;

        // floating point number
        } else if (str == ".") {
            v.token = Token::FLOAT;
            i++;
            c++;
            while(((string)" \n").find(source[i]) == string::npos && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }

        } else if (str == "\n" || str == ";") {
            c++;
            v.token = Token::STATEMENT_END;
            if (str == "\n") {
                c = 0;
                l++;
            }

        // The character is the start of an IDENT
        } else if (regex_match(str, regex("[_a-zA-Z]"))) {
            if (str == "_" && !regex_match(str + source[i+1], regex("_[_a-zA-Z0-9]")))
                v.token = Token::IGNORE;
            else
                v.token = Token::IDENT;

            i++;
            c++;
            while (((string)" \n(){};.").find(source[i]) == string::npos && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }

            if (source[i] != ' ' || source[i] != '\n') {
                i--;
                c--;
            }

            if (IS_KEYWORD(str))
                v.token = Token::KEYWORD;

            else if (IS_TYPE(str))
                v.token = Token::TYPE;

        // The character is an assign_op
        } else if (str == "=") {
            v.token = Token::ASSIGN;

            if (source[i+1] == '=') {
                v.token = Token::LOG_OP;
                str += source[++i];
                c++;
            }

        // The character is a number
        } else if (regex_match(str, regex("[0-9]"))) {
            // Default to integer
            v.token = Token::INT;
            i++;
            c++;
            while (((string)" \n(){};").find(source[i]) == string::npos && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }

            if (source[i] != ' ' || source[i] != '\n') {
                i--;
                c--;
            }

            // The number is a float
            if (regex_match(str, regex("[0-9]*\\.[0-9]+"))) {
                v.token = Token::FLOAT;
            }

        // The character is the start of a scoped block
        } else if (str == "{") {
            v.token = Token::BLOCK_START;

        // The character is the end of a scoped block
        } else if (str == "}") {
            v.token = Token::BLOCK_END;

        // Character is an arithmetic operater + - * / %
        } else if (regex_match(str, regex("\\+|-|\\*|/|%"))) {
            v.token = Token::ARITH_OP;

            // Arithmetic operater is followed by an =, or is ++ or -- therefore its an arithmetic assignment operator
            if (regex_match(str, regex("\\+|-|\\*|/")) && source[i+1] == '=') {
                i++;
                c++;
                str += '=';
                v.token = Token::ASSIGN_ARITH_OP;

            } else if (str == "+" && source[i+1] == '+') {
                i++;
                c++;
                str += '+';
                v.token = Token::ASSIGN_ARITH_OP;

            } else if (str == "-" && source[i+1] == '-') {
                i++;
                c++;
                str += '-';
                v.token = Token::ASSIGN_ARITH_OP;

            // Comments
            } else if (str == "/" && source[i+1] == '/') {
                i++;
                c++;
                v.token = Token::COMMENT;
                SCAN_TIL(source, i, str, "\n")

            } else if (str == "/" && source[i+1] == '*') {
                v.token = Token::COMMENT;
                str += source[++i];
                i++;
                c+=2;
                while (source[i] != '*') {
                    str += source[i++];
                    c++;
                    if (source[i] == '*') {
                        str += source[i++];
                        c++;
                        if (source[i] == '/') {
                            str += source[i++];
                            c++;
                            break;
                        }
                    }
                }

            // Specials ->
            } else if (str == "-" && source[i+1] == '>') {
                v.token = Token::SPEC;
                i++;
                c++;
                str += '>';
            }

        // Bitwise values & ^ | ~
        } else if (regex_match(str, regex("&|\\^|\\||~"))) {
            v.token = Token::BIT_OP;

            // Bitwise assign operator &= |= ^= ~=
            if (source[i+1] == '=') {
                i++;
                c++;
                str += '=';
                v.token = Token::ASSIGN_BIT_OP;

            // && or || makes this a logical operator
            } else if((str == "&" && source[i+1] == '&') || (str == "|" && source[i+1] == '|')) {
                i++;
                c++;
                str += source[i];
                v.token = Token::LOG_OP;
            }

        // Logical operators < > <= >= != == !
        } else if (regex_match(str, regex("<|>|!"))) {
            v.token = Token::LOG_OP;

            if (source[i+1] == '=') {
                str += source[++i];
                c++;

            // If << or >> its a bit operator
            } else if ((str == "<" && source[i+1] == '<') || (str == ">" && source[i+1] == '>')) {
                v.token = Token::BIT_OP;
                str += source[++i];
                c++;

                // if <<= or >>= its a bit assign operator
                if (source[i+1] == '=') {
                    v.token = Token::ASSIGN_BIT_OP;
                    str += source[++i];
                    c++;
                }
            }

        // Parenthesis start
        } else if (str == "(") {
            v.token = Token::PAR_START;
        
        // Parenthesis end
        } else if (str == ")") {
            v.token = Token::PAR_END;

        // Array start
        } else if (str == "[") {
            v.token = Token::ARR_START;

        // Array end
        } else if (str == "]") {
            v.token = Token::ARR_END;

        // Special operators
        } else if (regex_match(str, regex(":|,"))) {
            v.token = Token::SPEC;

        // Strings
        } else if (str == "\"") {
            v.token = Token::STRING;
            i++;
            c++;
            while (((string)"\"").find(source[i]) != string::npos && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }
            str += '"';
            i++;
            c++;

        // Character
        } else if (str == "'") {
            v.token = Token::CHAR;
            i++;
            c++;
            while (((string)"\"").find(source[i]) != string::npos && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }
            str += '\'';
            i++;
            c++;

        }

        v.val = str;

        out.push_back(v);
    }

    return out;
}

/*
 * lex_file
 *
 * Lexes a source code file by reading entire file into a string and then calling lex
 */
vector<TokenCap> lex_file(string f) {
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
