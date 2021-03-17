#include "test_parser.hpp"

#include <regex>
#include <vector>
#include <string>

#include "../src/parser.hpp"

using namespace std;

#define TABLE vector<pair<string, string>>

#define RUN_TEST(table) \
    for (pair<string, string> p : table) { \
        string v = AST(lex(p.first)).ToString(); \
        CPPUNIT_ASSERT_MESSAGE("Trees do not match: [" + regex_replace(p.second, regex("\n"), "\\n") + "] != [" + regex_replace(v, regex("\n"), "\\n") + "]", v == p.second); \
    }

void TestParser::test_primitive_assign() {
    TABLE table = {
        { "int a = 1", "ROOT\n"
                       "  int a = :\n"
                       "    p_int: 1\n"},

        { "int b = 12;", "ROOT\n"
                         "  int b = :\n"
                         "    p_int: 12\n"},

        { "char c = 'a'", "ROOT\n"
                          "  char c = :\n"
                          "    p_char: a\n"},

        { "float f = 0.12", "ROOT\n"
                            "  float f = :\n"
                            "    p_float: 0.12000000\n"},

        { "float f = .12", "ROOT\n"
                           "  float f = :\n"
                           "    p_float: 0.12000000\n"},

        { "string s = \"str\"", "ROOT\n"
                                "  string s = :\n"
                                "    p_string: str\n"},

        { "bool a = true", "ROOT\n"
                           "  bool a = :\n"
                           "    p_bool: true\n"},

        { "bool a = false", "ROOT\n"
                           "  bool a = :\n"
                           "    p_bool: false\n"},

        { "int a = 1\nint b = 2", "ROOT\n"
                                  "  int a = :\n"
                                  "    p_int: 1\n"
                                  "  int b = :\n"
                                  "    p_int: 2\n"},

        { "int a = 1;int b = 2", "ROOT\n"
                                  "  int a = :\n"
                                  "    p_int: 1\n"
                                  "  int b = :\n"
                                  "    p_int: 2\n"},

        { "int a = 1 + 2", "ROOT\n"
                           "  int a = :\n"
                           "    op: +:\n"
                           "      p_int: 1\n"
                           "      p_int: 2\n"},

        { "int a;", "ROOT\n"
                    "  int a = :\n"
                    "    p_int: 0\n"},

        { "float a;", "ROOT\n"
                      "  float a = :\n"
                      "    p_float: 0.00000000\n"},

        { "string a;", "ROOT\n"
                       "  string a = :\n"
                       "    p_string: \n"},

        { "char a;", "ROOT\n"
                     "  char a = :\n"
                     "    p_char: \n"},

        { "bool a;", "ROOT\n"
                     "  bool a = :\n"
                     "    p_bool: false\n"},
    };
    RUN_TEST(table);
}

void TestParser::test_struct_declaration() {
    TABLE table = {
        { "struct A {\n"
          "    int a\n"
          "    int b\n"
          "}",
          "ROOT\n"
          "  struct A:\n"
          "    int a\n"
          "    int b\n"},

        { "struct A { int a; int b }", "ROOT\n"
                                       "  struct A:\n"
                                       "    int a\n"
                                       "    int b\n"},

        { "struct _Thing {\n"
          "    int a; char b\n"
          "    string c; float d\n"
          "    bool is_thing\n"
          "    myType *ptr\n"
         "}",
         "ROOT\n"
         "  struct _Thing:\n"
         "    int a\n"
         "    char b\n"
         "    string c\n"
         "    float d\n"
         "    bool is_thing\n"
         "    myType *ptr\n"}
    };
    RUN_TEST(table);
}

void TestParser::test_interface_declaration() {
    TABLE table = {
        { "interface A { int go() }", "ROOT\n"
                                   "  interface A:\n"
                                   "    int go()\n"},

        { "interface A {\n"
          "    int add(int, int)\n"
          "}",
          "ROOT\n"
          "  interface A:\n"
          "    int add(int, int, )\n"},

        { "interface A { go(int) }", "ROOT\n"
                                     "  interface A:\n"
                                     "    void go(int, )\n"},

        { "interface A { go(int*) }", "ROOT\n"
                                     "  interface A:\n"
                                     "    void go(int*, )\n"},
    };
    RUN_TEST(table);
}
