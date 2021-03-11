#include <fstream>
#include <iostream>
#include <regex>

#include "lexer.hpp"

using namespace std;

static const string KEYWORDS[12] = {
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
    "return",
};

static const string PRIMITIVES[5] = {
    "int",
    "float",
    "string",
    "char",
    "bool",
};

/*
 * IS_KEYWORD
 *
 * Checks the string input is a registered keyword, returning a cooresponding boolean.
 *
 * example: IS_KEYWORD("import") -> true
 *          IS_KEYWORD("mything") -> false
 */
static const bool IS_KEYWORD(string in) {
    for (int i = 0; i < 12; i++)
        if (KEYWORDS[i] == in)
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
        if (PRIMITIVES[i] == in)
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
            v.token = FLOAT;
            i++;
            c++;
            while(regex_match(str + source[i], regex("\\.[0-9]+")) && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }
            i--;
            c--;

        } else if (str == "\n" || str == ";") {
            v.token = STATEMENT_END;
            if (str == "\n") {
                c = 0;
                l++;
            }

        // The character is the start of an IDENT
        } else if (regex_match(str, regex("[_a-zA-Z]"))) {
            if (str == "_" && !regex_match(str + source[i+1], regex("_[_a-zA-Z0-9]")))
                v.token = IGNORE;
            else
                v.token = IDENT;

            i++;
            c++;
            while (regex_match(str + source[i], regex("[_a-zA-Z0-9]+")) && i < (int)source.size()) {
                str += source[i++];
                c++;
            }

            if (source[i] != ' ' || source[i] != '\n') {
                i--;
                c--;
            }

            if (IS_KEYWORD(str))
                v.token = KEYWORD;

            else if (IS_TYPE(str))
                v.token = TYPE;

            else if (str == "true" || str == "false")
                v.token = BOOL;

        // The character is an assign_op
        } else if (str == "=") {
            v.token = ASSIGN;

            if (source[i+1] == '=') {
                v.token = LOG_OP;
                str += source[++i];
                c++;
            }

        // The character is a number
        } else if (regex_match(str, regex("[0-9]"))) {
            // Default to integer
            v.token = INT;
            i++;
            c++;
            while (regex_match(str + source[i], regex("[0-9\\.]+")) && i < (int)source.size()) {
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
                v.token = FLOAT;
            }

        // The character is the start of a scoped block
        } else if (str == "{") {
            v.token = BLOCK_START;

        // The character is the end of a scoped block
        } else if (str == "}") {
            v.token = BLOCK_END;

        // Character is an arithmetic operater + - * / %
        } else if (regex_match(str, regex("\\+|-|\\*|/|%"))) {
            v.token = ARITH_OP;

            // Arithmetic operater is followed by an =, or is ++ or -- therefore its an arithmetic assignment operator
            if (regex_match(str, regex("\\+|-|\\*|/")) && source[i+1] == '=') {
                i++;
                c++;
                str += '=';
                v.token = ASSIGN_ARITH_OP;

            } else if (str == "+" && source[i+1] == '+') {
                i++;
                c++;
                str += '+';
                v.token = ASSIGN_ARITH_OP;

            } else if (str == "-" && source[i+1] == '-') {
                i++;
                c++;
                str += '-';
                v.token = ASSIGN_ARITH_OP;

            // Comments
            } else if (str == "/" && source[i+1] == '/') {
                i++;
                c++;
                v.token = COMMENT;
                while (((string)"\n").find(source[i]) == string::npos && i < (int)source.size()) {
                    str += source[i];
                    i++;
                    c++;
                }
                c = 0;
                l++;


            } else if (str == "/" && source[i+1] == '*') {
                v.token = COMMENT;
                str += source[++i];
                i++;
                c+=2;
                while (source[i] != '*') {
                    str += source[i++];
                    c++;
                    if (source[i] == '\n') {
                        c = 0;
                        l++;
                    }
                    if (source[i] == '*') {
                        str += source[i++];
                        c++;
                        if (source[i] == '/') {
                            str += source[i];
                            break;
                        }
                    }
                }

            // Specials ->
            } else if (str == "-" && source[i+1] == '>') {
                v.token = SPEC;
                i++;
                c++;
                str += '>';
            }

        // Bitwise values & ^ | ~
        } else if (regex_match(str, regex("&|\\^|\\||~"))) {
            v.token = BIT_OP;

            // Bitwise assign operator &= |= ^= ~=
            if (source[i+1] == '=') {
                i++;
                c++;
                str += '=';
                v.token = ASSIGN_BIT_OP;

            // && or || makes this a logical operator
            } else if((str == "&" && source[i+1] == '&') || (str == "|" && source[i+1] == '|')) {
                i++;
                c++;
                str += source[i];
                v.token = LOG_OP;
            }

        // Logical operators < > <= >= != == !
        } else if (regex_match(str, regex("<|>|!"))) {
            v.token = LOG_OP;

            if (source[i+1] == '=') {
                str += source[++i];
                c++;

            // If << or >> its a bit operator
            } else if ((str == "<" && source[i+1] == '<') || (str == ">" && source[i+1] == '>')) {
                v.token = BIT_OP;
                str += source[++i];
                c++;

                // if <<= or >>= its a bit assign operator
                if (source[i+1] == '=') {
                    v.token = ASSIGN_BIT_OP;
                    str += source[++i];
                    c++;
                }
            }

        // Parenthesis start
        } else if (str == "(") {
            v.token = PAR_START;
        
        // Parenthesis end
        } else if (str == ")") {
            v.token = PAR_END;

        // Array start
        } else if (str == "[") {
            v.token = ARR_START;

        // Array end
        } else if (str == "]") {
            v.token = ARR_END;

        // Special operators
        } else if (regex_match(str, regex(":|,"))) {
            v.token = SPEC;

        // Strings
        } else if (str == "\"") {
            v.token = STRING;
            i++;
            c++;
            str = "";
            while (source[i] != '"' && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }

        // Character
        } else if (str == "'") {
            v.token = CHAR;
            i++;
            c++;
            str = "";
            while (source[i] != '\'' && i < (int)source.size()) {
                str += source[i];
                i++;
                c++;
            }

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
