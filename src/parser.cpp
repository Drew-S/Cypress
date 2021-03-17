#include <iostream>
#include <stdlib.h>

#include "parser.hpp"

using namespace std;

AST::AST(vector<TokenCap> t): tokens(t) {
    this->root = new Node(&this->tokens, 0, this->tokens.size());
    int i = 0;
    this->root->parse(this->root, &this->tokens, &i, this->tokens.size());
}
AST::~AST() {
    delete this->root;
}

void AST::print() {
    cout << this->ToString();
}

string AST::ToString() {
    return "ROOT\n" + this->root->ToString(0);
}
