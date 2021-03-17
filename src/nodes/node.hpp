#pragma once

#include <vector>
#include <string>

#include "../token.hpp"

using namespace std;

/*
 * Node
 *
 * Base node class for all parts of the AST.
 */
class Node {
public:
    vector<TokenCap> *tokens;  // Pointer to token list
    int start;                 // Start index of tokens for Node section
    int end;                   // End index of tokens for Node section
    // vector<NodeError> errs;

    vector<Node*> body;        // Children nodes 

    Node *parent = nullptr;    // Pointer to parent node


    Node(vector<TokenCap>*, int, int);
    virtual ~Node();

    //virtual void exec();
    //virtual bool is_valid();
    virtual string ToString(int);
    // virtual vector<NodeError> errors();

    int collect_block(vector<TokenCap>*, int*, bool is_par); // collect .*{} or .*()
    int collect_statement(vector<TokenCap>*, int*); // collect .*(\n|;|\)|})

    void parse(Node*, vector<TokenCap>*, int*, int);
};

/*

[-] Node
[-] +-- OpNode
[-] |   +-- ArithOpNode           + - / * %
[ ] |   |   +-- AssnArithOpNode   += -= /= *= ++ --
[ ] |   +-- BitOpNode             ^ ~ & | << >>
[ ] |   |   +-- AssnBitOpNode     ^= ~= &= |= <<= >>=
[ ] |   +-- LogOpNode             && || < > <= >= == != !
[ ] |   +-- FlowOpNode            if else if for switch                                 ...
[-] +-- IdentNode                 _name Name _0123name Name012                          ...
[ ] |   +-- DefineIdentNode       struct <name> {...} interface <name> {...}            ...
[x] |   +-- AssignIdentNode       <type>? <ident> = <val> <end_statement>               ...
[x] |   +-- FnNode                fn <method> <ident>(<params>) -> <returns> { ... }    ...
[x] |   +-- CallFnNode            <ident>(<params>) <end_statement>                     ...
[x] |   +-- ReturnNode
[-] +-- PrimitiveNode
[x]     +-- IntPrimitiveNode      123                                                   ...
[ ]     +-- FloatPrimitiveNode    123.123 0.123 .123                                    ...
[ ]     +-- StringPrimitiveNode   "string"                                              ...
[ ]     +-- CharPrimitiveNode     'c'                                                   ...
[ ]     +-- BoolPrimitiveNode     true false

*/

/*
 * IdentNode
 *
 * Identity node is a node for housing variables simply.
 *
 * For example, calling a function with a passed parameter `myfunc(i)`, `i` is the identity (a reference)
 */
class IdentNode : public Node {
public:
    string name;
    bool is_pointer = false;

    IdentNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * AssignIdentNode
 *
 * This is an extension ot the IdentNode where we assign a value to an identity `<type> i = <val>`
 */
class AssignIdentNode : public IdentNode {
public:
    string type;

    AssignIdentNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * Param
 *
 * Structure for housing functions passed parameter types and scope names
 */
struct Param {
    string type;
    string name;
    bool is_pointer = false;
};

/*
 * FnNode
 *
 * Node that houses a function declaration `fn [(method)] myfunc([params]) [-> returns] {...}`
 *
 * Houses params that it requires, houses what it returns, the body of the function, and what object it belongs to
 *   (go-like attachment `func (m *MyStruct) myfunc(...) {...}`)
 */
class FnNode : public IdentNode {
private:
    int proc_params(vector<TokenCap>*, int);  // Proceses contents inside (...) into param list
    int proc_returns(vector<TokenCap>*, int); // Processes contents inside -> ... { into return list

public:
    bool is_method = false;   // Flag whether function is a method
    string method_of;         // What this function belongs to
    string method_par;        // The name of the scoped variable for the method (same as param)
    vector<Param> params;     // List of passed param names and types
    vector<string> returns;   // List of returning elements

    FnNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * CallFnNode
 *
 * This is a reference node for a function call to a function defined in FnNode
 */
class CallFnNode : public IdentNode {
private:
    int proc_params(vector<TokenCap>*, int);

public:
    CallFnNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * ReturnNode
 *
 * This is a simple Node special for return statements, its body are each their own return evaluators
 */
class ReturnNode : public Node {
public:
    ReturnNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * PrimitiveNode
 *
 * The primitive node is a base type for all other basic values types (char, string, int, float, bool)
 */
class PrimitiveNode : public Node {
public:
    PrimitiveNode(vector<TokenCap>*, int, int);
};

/*
 * IntPrimtitiveNode
 *
 * The integer primitive node type.
 */
class IntPrimitiveNode : public PrimitiveNode {
public:
    int val = 0;

    IntPrimitiveNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * CharPrimtitiveNode
 *
 * The character primitive node type.
 */
class CharPrimitiveNode : public PrimitiveNode {
public:
    bool set = false;
    char val;

    CharPrimitiveNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * StringPrimtitiveNode
 *
 * The string primitive node type.
 */
class StringPrimitiveNode : public PrimitiveNode {
public:
    string val = "";

    StringPrimitiveNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * FloatPrimtitiveNode
 *
 * The float primitive node type.
 */
class FloatPrimitiveNode : public PrimitiveNode {
public:
    float val = 0.0;

    FloatPrimitiveNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * BoolPrimtitiveNode
 *
 * The boolean primitive node type.
 */
class BoolPrimitiveNode : public PrimitiveNode {
public:
    bool val = false;

    BoolPrimitiveNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * OpNode
 *
 * Operations node, this is a base class all other operation types are inherited from.
 *
 * Arithmetic, Bitwise, Logic, and assignment variants
 */
class OpNode : public Node {
public:
    string name;

    OpNode(vector<TokenCap>*, int, int);

    string ToString(int);
};

/*
 * ArithOpNode
 *
 * Handles arithmetic operations. + - / * %
 */
class ArithOpNode : public OpNode {
public:
    ArithOpNode(vector<TokenCap>*, int, int);
};

/*
 * Field
 *
 * The fields of the struct/interface (like params, but with fn support)
 */
struct Field {
    string type;
    string name;
    bool is_pointer = false;
    vector<Param> params;
};

/*
 * DefineIdentNode
 *
 * Definitions for structs and interfaces
 */
class DefineIdentNode : public IdentNode {
private:
    void process_block(vector<TokenCap>*, int, int);
    string string_fields(int);

public:
    bool interface = false;
    vector<Field> fields;
    DefineIdentNode(vector<TokenCap>*, int, int);

    string ToString(int);
};
