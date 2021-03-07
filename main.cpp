#include <string>
#include <vector>

#include "parser.hpp"
#include "lexer.hpp"
#include "token.hpp"

using namespace std;

int main() {
    vector<TokenCap> l = lex_file("token_base.jo");
    //for (int i = 0; i < (int)l.size(); i++) {
        //print_pair(l[i], i);
    //}
    AST a(l);
    a.print();
    //a.exec();
    return 0;
}
