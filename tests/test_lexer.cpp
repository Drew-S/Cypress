#include "test_lexer.hpp"

#include <regex>
#include <vector>
#include <string>

#include "../src/lexer.hpp"

using namespace std;

#define TABLE vector<pair<string, vector<TokenCap>>>

#define RUN_TEST(table) \
    for (pair<string, vector<TokenCap>> p : table) { \
        vector<TokenCap> o = lex(p.first); \
        if (o.size() != p.second.size()) { \
            CPPUNIT_ASSERT_MESSAGE((string)"Size mismatch: " + p.first + ": " + to_string(o.size()) + " != " + to_string(p.second.size()), o.size() == p.second.size()); \
        } \
        \
        for (int i = 0; i < (int)o.size(); i++) { \
            CPPUNIT_ASSERT_MESSAGE((string)"Tokens do not match: " + o[i].val + ": " + p_token(o[i].token) + " != " + p_token(p.second[i].token), o[i].token == p.second[i].token); \
            CPPUNIT_ASSERT_MESSAGE((string)"Values do not match: " + o[i].val + " != " + p.second[i].val, o[i].val == p.second[i].val); \
            CPPUNIT_ASSERT_MESSAGE((string)"Lines do not match: " + o[i].val + ": " + to_string(o[i].line) + " != " + to_string(p.second[i].line), o[i].line == p.second[i].line); \
            CPPUNIT_ASSERT_MESSAGE((string)"Columns do not match: " + o[i].val + ": " + to_string(o[i].col) + " != " + to_string(p.second[i].col), o[i].col == p.second[i].col); \
        } \ 
    }

void TestLexer::test_comments() {
    TABLE table = {
        { "//single line",     {{ COMMENT,       "//single line",     1, 1  } } },
        { "/*multiline*/",     {{ COMMENT,       "/*multiline*/",     1, 1  } } },
        { "/*\n   multi\n */", {{ COMMENT,       "/*\n   multi\n */", 1, 1  } } },

        { ";//single;",        {{ STATEMENT_END, ";",                 1, 1  },
                                { COMMENT,       "//single;",         1, 2  } } },
        { ";/*mult*/;",        {{ STATEMENT_END, ";",                 1, 1  },
                                { COMMENT,       "/*mult*/",          1, 2  },
                                { STATEMENT_END, ";",                 1, 10 } } },
    };
    RUN_TEST(table);
}

void TestLexer::test_keywords() {
    TABLE table = {
        { "fn",        {{ KEYWORD,       "fn",      1, 1 } } },
        { "import",    {{ KEYWORD,       "import",  1, 1 } } },
        { "for",       {{ KEYWORD,       "for",     1, 1 } } },
        { "if",        {{ KEYWORD,       "if",      1, 1 } } },
        { "else",      {{ KEYWORD,       "else",    1, 1 } } },
        { "switch",    {{ KEYWORD,       "switch",  1, 1 } } },
        { "case",      {{ KEYWORD,       "case",    1, 1 } } },
        { "default",   {{ KEYWORD,       "default", 1, 1 } } },
        { "return",    {{ KEYWORD,       "return",  1, 1 } } },

        { ";fn;",      {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "fn",      1, 2 },
                        { STATEMENT_END, ";",       1, 4 } } },
        { ";import;",  {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "import",  1, 2 },
                        { STATEMENT_END, ";",       1, 8 } } },
        { ";for;",     {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "for",     1, 2 },
                        { STATEMENT_END, ";",       1, 5 } } },
        { ";if;",      {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "if",      1, 2 },
                        { STATEMENT_END, ";",       1, 4 } } },
        { ";else;",    {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "else",    1, 2 },
                        { STATEMENT_END, ";",       1, 6 } } },
        { ";switch;",  {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "switch",  1, 2 },
                        { STATEMENT_END, ";",       1, 8 } } },
        { ";case;",    {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "case",    1, 2 },
                        { STATEMENT_END, ";",       1, 6 } } },
        { ";default;", {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "default", 1, 2 },
                        { STATEMENT_END, ";",       1, 9 } } },
        { ";return;",  {{ STATEMENT_END, ";",       1, 1 },
                        { KEYWORD,       "return",  1, 2 },
                        { STATEMENT_END, ";",       1, 8 } } },
    };
    RUN_TEST(table);
}

void TestLexer::test_primitive_types() {
    TABLE table = {
        { "12",         {{ INT,           "12",     1, 1 } } },
        { "0.13",       {{ FLOAT,         "0.13",   1, 1 } } },
        { ".04",        {{ FLOAT,         ".04",    1, 1 } } },
        { "\"string\"", {{ STRING,        "string", 1, 1 } } },
        { "'c'",        {{ CHAR,          "c",      1, 1 } } },
        { "'char'",     {{ CHAR,          "char",   1, 1 } } },
        { "true",       {{ BOOL,          "true",   1, 1 } } },
        { "false",      {{ BOOL,          "false",  1, 1 } } },

        { ";12;",       {{ STATEMENT_END, ";",      1, 1 },
                         { INT,           "12",     1, 2 }, 
                         { STATEMENT_END, ";",      1, 4 } } },
        { ";0.13;",     {{ STATEMENT_END, ";",      1, 1 },
                         { FLOAT,         "0.13",   1, 2 }, 
                         { STATEMENT_END, ";",      1, 6 } } },
        { ";.04;",      {{ STATEMENT_END, ";",      1, 1 },
                         { FLOAT,         ".04",    1, 2 }, 
                         { STATEMENT_END, ";",      1, 5 } } },
        { ";\"str\";",  {{ STATEMENT_END, ";",      1, 1 },
                         { STRING,        "str",    1, 2 }, 
                         { STATEMENT_END, ";",      1, 7 } } },
        { ";'c';",      {{ STATEMENT_END, ";",      1, 1 },
                         { CHAR,          "c",      1, 2 }, 
                         { STATEMENT_END, ";",      1, 5 } } },
        { ";'char';",   {{ STATEMENT_END, ";",      1, 1 },
                         { CHAR,          "char",   1, 2 }, 
                         { STATEMENT_END, ";",      1, 8 } } },
        { ";true;",     {{ STATEMENT_END, ";",      1, 1 },
                         { BOOL,          "true",   1, 2 }, 
                         { STATEMENT_END, ";",      1, 6 } } },
        { ";false;",    {{ STATEMENT_END, ";",      1, 1 },
                         { BOOL,          "false",  1, 2 }, 
                         { STATEMENT_END, ";",      1, 7 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_identities() {
    TABLE table = {
        { "name",       {{ IDENT,         "name",       1, 1 } } },
        { "_name",      {{ IDENT,         "_name",      1, 1 } } },
        { "Name",       {{ IDENT,         "Name",       1, 1 } } },
        { "NAME",       {{ IDENT,         "NAME",       1, 1 } } },
        { "NameThing",  {{ IDENT,         "NameThing",  1, 1 } } },
        { "_NameThing", {{ IDENT,         "_NameThing", 1, 1 } } },
        { "__thing",    {{ IDENT,         "__thing",    1, 1 } } },
        { "_thing1",    {{ IDENT,         "_thing1",    1, 1 } } },
        { "thing1",     {{ IDENT,         "thing1",     1, 1 } } },
        { "Thing1",     {{ IDENT,         "Thing1",     1, 1 } } },
        { "__0thing",   {{ IDENT,         "__0thing",   1, 1 } } },
        { "i",          {{ IDENT,         "i",          1, 1 } } },

        { ";ident;",    {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "ident",      1, 2 },
                         { STATEMENT_END, ";",          1, 7 } } },
        { ";i;",        {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "i",          1, 2 },
                         { STATEMENT_END, ";",          1, 3 } } },
        { ";_i;",       {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "_i",         1, 2 },
                         { STATEMENT_END, ";",          1, 4 } } },
        { ";_0;",       {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "_0",         1, 2 },
                         { STATEMENT_END, ";",          1, 4 } } },
        { ";_name;",    {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "_name",      1, 2 },
                         { STATEMENT_END, ";",          1, 7 } } },
        { ";_1230;",    {{ STATEMENT_END, ";",          1, 1 },
                         { IDENT,         "_1230",      1, 2 },
                         { STATEMENT_END, ";",          1, 7 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_ignore() {
    TABLE table = {
        { "_",   {{ IGNORE,        "_", 1, 1 } } },

        { ";_;", {{ STATEMENT_END, ";", 1, 1 },
                  { IGNORE,        "_", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_blocks() {
    TABLE table = {
        { "{",   {{ BLOCK_START,   "{", 1, 1 } } },
        { "}",   {{ BLOCK_END,     "}", 1, 1 } } },

        { ";{;", {{ STATEMENT_END, ";", 1, 1 },
                  { BLOCK_START,   "{", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";};", {{ STATEMENT_END, ";", 1, 1 },
                  { BLOCK_END,     "}", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_assign() {
    TABLE table = {
        { "=",   {{ ASSIGN,        "=", 1, 1 } } },

        { ";=;", {{ STATEMENT_END, ";", 1, 1 },
                  { ASSIGN,        "=", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_arithmetic() {
    TABLE table = {
        { "+",   {{ ARITH_OP,      "+", 1, 1 }} },
        { "-",   {{ ARITH_OP,      "-", 1, 1 }} },
        { "/",   {{ ARITH_OP,      "/", 1, 1 }} },
        { "*",   {{ ARITH_OP,      "*", 1, 1 }} },
        { "%",   {{ ARITH_OP,      "%", 1, 1 }} },

        { ";+;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARITH_OP,      "+", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";-;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARITH_OP,      "-", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";/;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARITH_OP,      "/", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";*;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARITH_OP,      "*", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";%;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARITH_OP,      "%", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_logical() {
    TABLE table = {
        { "||",   {{ LOG_OP,       "||",  1, 1 } } },
        { "&&",   {{ LOG_OP,       "&&",  1, 1 } } },
        { "==",   {{ LOG_OP,       "==",  1, 1 } } },
        { "!=",   {{ LOG_OP,       "!=",  1, 1 } } },
        { "<=",   {{ LOG_OP,       "<=",  1, 1 } } },
        { ">=",   {{ LOG_OP,       ">=",  1, 1 } } },
        { "!",    {{ LOG_OP,       "!",   1, 1 } } },
        { "<",    {{ LOG_OP,       "<",   1, 1 } } },
        { ">",    {{ LOG_OP,       ">",   1, 1 } } },

        { ";||;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "||", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";&&;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "&&", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";==;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "==", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";!=;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "!=", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";<=;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "<=", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";>=;", {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        ">=", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";!;",  {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "!",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";<;",  {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        "<",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";>;",  {{ STATEMENT_END, ";",  1, 1 },
                   { LOG_OP,        ">",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_bitwise() {
    TABLE table = {
        { "&",    {{ BIT_OP,        "&",  1, 1 } } },
        { "|",    {{ BIT_OP,        "|",  1, 1 } } },
        { "~",    {{ BIT_OP,        "~",  1, 1 } } },
        { "^",    {{ BIT_OP,        "^",  1, 1 } } },
        { "<<",   {{ BIT_OP,        "<<", 1, 1 } } },
        { ">>",   {{ BIT_OP,        ">>", 1, 1 } } },

        { ";&;",  {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        "&",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";|;",  {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        "|",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";~;",  {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        "~",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";^;",  {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        "^",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
        { ";<<;", {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        "<<", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";>>;", {{ STATEMENT_END, ";",  1, 1 },
                   { BIT_OP,        ">>", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_arithmetic_assign() {
    TABLE table = {
        { "+=",   {{ ASSIGN_ARITH_OP, "+=", 1, 1 } } },
        { "-=",   {{ ASSIGN_ARITH_OP, "-=", 1, 1 } } },
        { "/=",   {{ ASSIGN_ARITH_OP, "/=", 1, 1 } } },
        { "*=",   {{ ASSIGN_ARITH_OP, "*=", 1, 1 } } },
        { "++",   {{ ASSIGN_ARITH_OP, "++", 1, 1 } } },
        { "--",   {{ ASSIGN_ARITH_OP, "--", 1, 1 } } },

        { ";+=;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "+=", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
        { ";-=;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "-=", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
        { ";/=;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "/=", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
        { ";*=;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "*=", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
        { ";++;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "++", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
        { ";--;", {{ STATEMENT_END,   ";",  1, 1 },
                   { ASSIGN_ARITH_OP, "--", 1, 2 },
                   { STATEMENT_END,   ";",  1, 4 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_bitwise_assign() {
    TABLE table = {
        { "&=",    {{ ASSIGN_BIT_OP, "&=",  1, 1 } } },
        { "|=",    {{ ASSIGN_BIT_OP, "|=",  1, 1 } } },
        { "^=",    {{ ASSIGN_BIT_OP, "^=",  1, 1 } } },
        { "~=",    {{ ASSIGN_BIT_OP, "~=",  1, 1 } } },
        { "<<=",   {{ ASSIGN_BIT_OP, "<<=", 1, 1 } } },

        { ";&=;",  {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, "&=",  1, 2 },
                    { STATEMENT_END, ";",   1, 4 } } },
        { ";|=;",  {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, "|=",  1, 2 },
                    { STATEMENT_END, ";",   1, 4 } } },
        { ";^=;",  {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, "^=",  1, 2 },
                    { STATEMENT_END, ";",   1, 4 } } },
        { ";~=;",  {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, "~=",  1, 2 },
                    { STATEMENT_END, ";",   1, 4 } } },
        { ";<<=;", {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, "<<=", 1, 2 },
                    { STATEMENT_END, ";",   1, 5 } } },
        { ";>>=;", {{ STATEMENT_END, ";",   1, 1 },
                    { ASSIGN_BIT_OP, ">>=", 1, 2 },
                    { STATEMENT_END, ";",   1, 5 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_parenthesis() {
    TABLE table = {
        { "(",   {{ PAR_START, "(", 1, 1 } } },
        { ")",   {{ PAR_END,   ")", 1, 1 } } },

        { ";(;", {{ STATEMENT_END, ";", 1, 1 },
                  { PAR_START,     "(", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";);", {{ STATEMENT_END, ";", 1, 1 },
                  { PAR_END,       ")", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_arrays() {
    TABLE table = {
        { "[",   {{ ARR_START, "[", 1, 1 } } },
        { "]",   {{ ARR_END,   "]", 1, 1 } } },

        { ";[;", {{ STATEMENT_END, ";", 1, 1 },
                  { ARR_START,     "[", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
        { ";];", {{ STATEMENT_END, ";", 1, 1 },
                  { ARR_END,       "]", 1, 2 },
                  { STATEMENT_END, ";", 1, 3 } } },
    };
    RUN_TEST(table)
}

void TestLexer::test_specials() {
    TABLE table = {
        { "->",   {{ SPEC,          "->", 1, 1 } } },
        { ",",    {{ SPEC,          ",",  1, 1 } } },

        { ";->;", {{ STATEMENT_END, ";",  1, 1 },
                   { SPEC,          "->", 1, 2 },
                   { STATEMENT_END, ";",  1, 4 } } },
        { ";,;",  {{ STATEMENT_END, ";",  1, 1 },
                   { SPEC,          ",",  1, 2 },
                   { STATEMENT_END, ";",  1, 3 } } },
    };
    RUN_TEST(table)
}
