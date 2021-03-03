#pragma once

#include <string>
#include <vector>

#include "token.hpp"

using namespace std;

class Node {
private:
    Token type;
    string value;
    vector<Node*> children;

public:
    Node(Token, string);
    ~Node();

    void append(Node*);
};

class AST {
private:
    Node *root;

    void insert(pair<Token, string>);
public:
    AST(vector<pair<Token, string>>);
    ~AST();

    void exec();
    bool valid();

};
