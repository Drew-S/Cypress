#pragma once

#include <vector>
#include <string>

#include "../token.hpp"

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
    string name;
    bool is_pointer = false;
};

class Node {
public:
    vector<TokenCap> *tokens;
    int start;
    int end;
    // vector<NodeError> errs;
    
    vector<Node*> body;

    Node *parent = nullptr;
    Node(vector<TokenCap>*, int, int);
    virtual ~Node();

    //virtual void exec();
    //virtual bool is_valid();
    virtual void print(int);
    // virtual vector<NodeError> errors();
};
/*

[x] Node
[ ] +-- OpNode
[ ] |   +-- ArithOpNode           + - / * %
[ ] |   |   +-- AssnArithOpNode   += -= /= *= ++ --
[ ] |   +-- BitOpNode             ^ ~ & | << >>
[ ] |   |   +-- AssnBitOpNode     ^= ~= &= |= <<= >>=
[ ] |   +-- LogOpNode             && || < > <= >= == != !
[ ] |   +-- FlowOpNode            if else if for switch                                 ...
[ ] +-- IdentNode                 _name Name _0123name Name012                          ...
[ ] |   +-- DefineNode            struct <name> {...} interface <name> {...}            ...
[ ] |   +-- AssignIdentNode       <type>? <ident> = <val> <end_statement>               ...
[ ] |   +-- FnNode                fn <method> <ident>(<params>) -> <returns> { ... }    ...
[ ] |   +-- CallFnNode            <ident>(<params>) <end_statement>                     ...
[ ] |   +-- ReturnNode
[ ] +-- PrimitiveNode
[ ]     +-- IntPrimitiveNode      123                                                   ...
[ ]     +-- FloatPrimitiveNode    123.123 0.123 .123                                    ...
[ ]     +-- StringPrimitiveNode   "string"                                              ...
[ ]     +-- CharPrimitiveNode     'c'                                                   ...
[ ]     +-- BoolPrimitiveNode     true false

*/

class IdentNode : public Node {
public:
    string name;
    bool is_pointer = false;

    IdentNode(vector<TokenCap>*, int, int);

    void print(int);
};

class AssignIdentNode : public IdentNode {
public:
    string type;

    AssignIdentNode(vector<TokenCap>*, int, int);

    void print(int);
};

class FnNode : public IdentNode {
private:
    int proc_params(vector<TokenCap>*, int);
    int proc_returns(vector<TokenCap>*, int);

public:
    bool is_method = false;
    string method_of;
    string method_par;
    vector<Param> params;
    vector<string> returns;

    FnNode(vector<TokenCap>*, int, int);

    void print(int);
};

class CallFnNode : public IdentNode {
public:
    CallFnNode(vector<TokenCap>*, int, int);

    void print(int);
};

class ReturnNode : public Node {
public:
    Node *val;

    ReturnNode(vector<TokenCap>*, int, int);

    void print(int);
};

class PrimitiveNode : public Node {
public:
    PrimitiveNode(vector<TokenCap>*, int, int);
};

class IntPrimitiveNode : public PrimitiveNode {
public:
    int val;

    IntPrimitiveNode(vector<TokenCap>*, int, int);

    void print(int);
};

class OpNode : public Node {
public:
    string name;

    OpNode(vector<TokenCap>*, int, int);

    void print(int);
};

class ArithOpNode : public OpNode {
public:
    ArithOpNode(vector<TokenCap>*, int, int);
};

int collect(vector<TokenCap>*, int*, Token, Token, vector<Token>);

void process(Node*, vector<TokenCap>*, int*, int);

