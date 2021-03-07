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
    REF_UP,
    VAL,
    ASSN,
    CALL,
    ROOT
};

struct Param {
    string type;
    string ident;
    bool pointer = false;
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
    bool pointer;
    Node* parent;
};

//class BaseNode {
//public:
    //BaseNode *parent;
    //BaseNode(vector<pair<Token, string>>*, int*);
        
    //virtual void exec();
//};

//class FnNode : public BaseNode {
//public:
    //string ident;
    //string method_of;
    //vector<Param> params;
    //vector<string> ret;
    //vector<BaseNode> block;

    //FnNode(vector<pair<Token, string>>*, int*);

    //void exec();
//};

class AST {
private:
    vector<pair<Token, string>> tokens;
    Node root;

    int insert(int, Node*);
    vector<Param> proc_params(int*);
    vector<string> proc_pass_params(int*);
    vector<string> proc_returns(int*);
    string what_type(Token);

    void print(Node*, int);

public:
    AST(vector<pair<Token, string>> t);

    void exec();
    bool valid();
    void print();
};
