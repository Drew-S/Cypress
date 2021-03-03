#include "parser.hpp"
#include "lexer.hpp"

int main() {
    AST a(lex("fn main() {\n    print(\"test\")\n}"));
    return 0;
}
