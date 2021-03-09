#pragma once

#include <string>
#include <vector>

#include "token.hpp"
#include "./nodes/node.hpp"

using namespace std;

class AST {
private:
    vector<TokenCap> tokens;
    Node* root;

public:
    AST(vector<TokenCap>);
    ~AST();

    void print();
};
