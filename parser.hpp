#pragma once

#include <string>
#include <vector>

#include "token.hpp"

using namespace std;

enum NodeType {
    FN,
    RET,
    OP,
    REF,
    VAL,
    ASSN,
    CALL,
    ROOT
};

struct Param {
    string type;
    string ident;
    bool pointer;
    NodeType val;
};

struct Node {
    NodeType type;
    string l_type;
    void* val = nullptr;
    string ident;
    string method_of;
    vector<Param> params;
    vector<string> ret;
    vector<Node> block;
    Node* parent;
};

class AST {
private:
    vector<pair<Token, string>> tokens;
    Node root;

    int insert(int, Node*);
    vector<Param> proc_params(int*);
    vector<string> proc_pass_params(int*);
    vector<string> proc_returns(int*);

    void print(Node*, int);

public:
    AST(vector<pair<Token, string>> t);

    void exec();
    bool valid();
    void print();
};
