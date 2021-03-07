#include <string>
#include <vector>

#include "parser.hpp"
#include "lexer.hpp"
#include "token.hpp"

using namespace std;

int main() {
    vector<pair<Token, string>> l = lex("fn double(int i) -> int {\n    return i *2\n}\n\nfn main() {int i = 12\n\ni = double(i)\n\nprintln(i)\n}");
    //for (int i = 0; i < (int)l.size(); i++) {
        //print_pair(l[i]);
    //}
    AST a(l);
    //a.print();
    //a.exec();
    return 0;
}
