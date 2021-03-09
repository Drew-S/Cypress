#include <iostream>
#include <stdlib.h>

#include "parser.hpp"

using namespace std;

AST::AST(vector<TokenCap> t): tokens(t) {
    this->root = new Node(&this->tokens, 0, this->tokens.size());
    int i = 0;
    process(this->root, &this->tokens, &i, this->tokens.size());
}
AST::~AST() {
    delete this->root;
}

void AST::print() {
    cout << "ROOT" << endl;
    this->root->print(0);
}
