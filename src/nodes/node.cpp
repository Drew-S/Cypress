#include <iostream>

#include "node.hpp"

// Helpers for Printing nodes
// PRE adds spacing in the front for nesting based on the depth of the Node
#define PRE for(int i = 0; i < d; i++) pre += "    "
// BODY loops over the nodes body to print the child nodes
#define BODY for(int i = 0; i < (int)this->body.size(); i++) this->body[i]->print(d+1)

// TODO: Provide error handling. Invalid syntax should push errors to to a list.
// TODO: Provide valid checks.
// TODO: Implement execution, prevent execution when errors exist
// TODO: Implement parent node pointer assignment
// PLAN: Decide how to handle scope, current design would allow for parent lookup, but a dynamic scope object as we
//       pass down the tree may be preferable
// PLAN: Decide whether to have pointer identifier references (method functions) point to instantiated variable or
//       scan to and update directly

/*
 * find_token
 *
 * Returns the index of the Token `t` is in the list of tokens `ts`, returns -1 if not present.
 */
static int find_token(Token t, vector<Token> ts) {
    for (int i = 0; i < (int)ts.size(); i++)
        if (t == ts[i])
            return i;

    return -1;
}

/*
 * collect
 *
 * Loops over the token list `t` starting from `i` until an end value (token in `end`) is found or the
 *   depth counter is at 0, the depth counter starts at 1 and increases whenever the token `inc` is found
 *   or decresed whenever the token `dec` is found.
 */
int collect(vector<TokenCap> *t, int *i, Token inc, Token dec, vector<Token> end) {
    int d = 1;
    bool f = false;
    while(find_token((*t)[*i].token, end) == -1 && d > 0) {
        if ((*t)[*i].token == inc) {
            d++;
            if (!f) {
                f = true;
                d--;
            }
        }

        if ((*t)[*i].token == dec) {
            d--;
            if (d == 0)
                break;
        }

        if (inc == Token::INVALID && find_token((*t)[*i].token, end) > -1)
            break;

        (*i)++;
    }
    return *i;
}

/*
 * process
 *
 * Loops over the tokens `tokens` starting from `i` and stopping at `stop`. When token matches the start of a
 *   sequence it collects the statement and passes the collection to specific Node type to process, this node
 *   is appended into the current node `cur` body.
 */
void process(Node *cur, vector<TokenCap> *tokens, int *i, int stop) {
    while (*i < stop) {
        switch((*tokens)[*i].token) {
            // All of these should generally be ignored within this loop. Should be captured as part of other
            // statements or simply ignored (STATEMENT_END == ('\n' || ';'), multiple newlines just ignore)
            case Token::INVALID:
            case Token::STATEMENT_END:
            case Token::BLOCK_END:
            case Token::PAR_END:
                break;
            case Token::KEYWORD:
                {
                    // Collect and process entire: `fn [(<m> [*]<method>)] <name>([params...]) [-> returns...] {...}`
                    // into FnNode
                    if ((*tokens)[*i].val == "fn") {
                        int s = *i;
                        int e = collect(tokens, i, Token::BLOCK_START, Token::BLOCK_END, vector<Token>{Token::BLOCK_END});
                        cur->body.push_back(new FnNode(tokens, s, e));

                    // Collect and process entire `return .*`
                    // into ReturnNode
                    } else if ((*tokens)[*i].val == "return") {
                        int s = *i;
                        int e = collect(tokens, i, Token::INVALID, Token::INVALID, vector<Token>{Token::STATEMENT_END, Token::BLOCK_END});
                        cur->body.push_back(new ReturnNode(tokens, s, e));
                    }
                    break;
                }
            case Token::IDENT:
                {
                    // If the next token after an identity is a `(` it is a function call
                    // process: `<name>([params...])`
                    // into a CallFnNode
                    if ((*tokens)[*i + 1].token == Token::PAR_START) {
                        int s = *i;
                        int e = collect(tokens, i, Token::PAR_START, Token::PAR_END, vector<Token>{Token::STATEMENT_END, Token::BLOCK_END, Token::PAR_END});
                        cur->body.push_back(new CallFnNode(tokens, s, e));
                        break;
                    }

                    // Otherwise the ident token is treated as a type for another ident token
                    // <IDENT> <IDENT> -> <TYPE> <IDENT>
                }
            case Token::TYPE:
                {
                    int s = *i;
                    int e = collect(tokens, i, Token::INVALID, Token::INVALID, vector<Token>{Token::STATEMENT_END, Token::BLOCK_END});
                    cur->body.push_back(new AssignIdentNode(tokens, s, e));
                    break;
                }
        }
        (*i)++;
    }
}

//======================================================================================================================
// NODE
//======================================================================================================================
/*
 * Node::Node
 *
 * Node stores a pointer to the token list `t`, the starting index for this particular node `s` and the end index
 *   for their tokens `e`
 *
 * All inherited nodes store values the same
 */
Node::Node(vector<TokenCap> *t, int s, int e): tokens(t), start(s), end(e) {}

/*
 * Node::print
 *
 * Loops over the body of the nodes printing the children.
 *
 * Each inherited node prints similarly, its own format for its specific use and then its children. Comments for
 *   inherited nodes are omitted.
 */
void Node::print(int d) {
    BODY;
}

/*
 * Node::~Node()
 *
 * Deletes the children pointers and the parent pointer
 */
Node::~Node() {
    for (Node *n : this->body) {
        delete n;
    }
    this->body.clear();
    if (this->parent != nullptr)
        delete this->parent;
}


//======================================================================================================================
// ReturnNode
//======================================================================================================================
/*
 * ReturnNode::ReturnNode
 *
 * Loops over the collection `return ... <STATEMENT_END>`
 *   creating a body of return values from the statements
 */
ReturnNode::ReturnNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {
    // TODO: Build processing for block at a time `return [block], [block], ...`
    // TODO: Make exhaustive (constants [123, "string", 'char', true, etc], references [i, mything, etc],
    //       functions [myfunc(), myother(123, "string")])
    for (int i = s+1; i < e; i++) {
        if ((*t)[i].token == Token::IDENT) {
            this->body.push_back(new IdentNode(t, i, i));

        } else if ((*t)[i].token == Token::ARITH_OP) {
            if (this->body.size() <= 0) {} // ERROR
            this->body.pop_back();

            this->body.push_back(new ArithOpNode(t, i-1, i+1));
            i++;
        }
    }
}
void ReturnNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- return:";
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// IdentNode
//======================================================================================================================
/*
 * IdentNode::IdentNode
 *
 * Houses a reference to another value. If the first token is *, it is treated as a pointer.
 */
IdentNode::IdentNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {
    int i = s;

    // First token is *, define IdentNode as a pointer variable
    if ((*t)[i].token == Token::ARITH_OP) {
        if ((*t)[i].val == "*") {
            this->is_pointer = true;

        } else {} // ERROR
        i++;
    }
    if ((*t)[i].token != Token::IDENT) {} // ERROR

    // Store reference name
    this->name = (*t)[i].val;
}
void IdentNode::print(int d) {
    string pre;
    PRE;
    pre += (this->is_pointer ? "+-- ident: *" : "+-- ident: ") + this->name;
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// AssignIdentNode
//======================================================================================================================
/*
 * AssignIdentNode::AssignIdentNode
 *
 * An extension of the IdentNode this is the first instance of a reference, it is what stores the current value of
 *   the identity, defines its type, and its default value.
 */
AssignIdentNode::AssignIdentNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s;

    // Sequence does not start with <IDENT> or <TYPE> (eg: `int`, or `mytype`)
    if (!((*t)[i].token == Token::TYPE || (*t)[i].token == Token::IDENT)) {} // ERROR

    if ((*t)[i+1].token == Token::IDENT) {
        this->type = (*t)[i].val;
        i++;
    }

    this->name = (*t)[i].val;

    // Does not follow with an =
    if ((*t)[++i].token != Token::ASSIGN) {} // ERROR


    // Evaluate the right hand side
    switch((*t)[++i].token) {

        // Not a primtitive
        case Token::IDENT:

            // Right hand side is a function
            if((*t)[i+1].token == Token::PAR_START) {
                int s1 = i;
                int e1 = collect(t, &i, Token::PAR_START, Token::PAR_END, vector<Token>{Token::PAR_END});
                this->body.push_back(new CallFnNode(t, s1, e1));

            // Right hand side is a different identity
            } else {
                this->body.push_back(new IdentNode(t, i, i));
            }
            break;

        // Is a primitive Integer
        case Token::INT:
            this->body.push_back(new IntPrimitiveNode(t, i, i));
            break;
    }
}
void AssignIdentNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- " + this->type + " " + this->name + " = :";
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// FnNode
//======================================================================================================================
/*
 * FnNode::FnNode
 *
 * Defines the definition of a function `fn [(method)] <name>([params...]) [-> returns...] {...}`
 */
FnNode::FnNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s + 1;

    // fn is followed by a ( it is a method of function, process as so
    if ((*t)[i].token == Token::PAR_START) {
        // Set is method
        this->is_method = true;

        // Following token needs to be a parameter identity
        if ((*t)[++i].token != Token::IDENT) {} // ERROR

        this->method_par = (*t)[i].val;

        // If followed by `*` it is a pointer to an object
        i++;
        if ((*t)[i].token == Token::ARITH_OP && (*t)[i].val == "*") {
            this->is_pointer = true;
            i++;
        }

        // Needs to be follwed by the identity of the defined object
        if ((*t)[i].token != Token::IDENT || (*t)[i].token != Token::TYPE) {} // ERROR

        this->method_of = (*t)[i].val;

        // Need a closing parenthesis
        if ((*t)[++i].token != Token::PAR_END) {} // ERROR

        i++;
    }

    // Need a name of the function
    if ((*t)[i].token != Token::IDENT) {} // ERROR

    this->name = (*t)[i].val;

    // Need a set of parenthesis for a function
    if ((*t)[++i].token != Token::PAR_START) {} // ERROR

    i++;

    // Process values in the (...) into a parameter list
    i = this->proc_params(t, i);

    // If followed by -> process return values
    if ((*t)[i].token == Token::SPEC && (*t)[i].val == "->")
        i = this->proc_returns(t, ++i);

    // Process the body of a function
    process(this, t, &i, e+1);
}
/*
 * FnNode::proc_params
 *
 * Processes a list tokens between (...) into a parameter list storing the type and scope reference value
 */
int FnNode::proc_params(vector<TokenCap> *t, int i) {
    while ((*t)[i].token != Token::PAR_END) {
        Param p;

        // Record the type of the token
        if (!((*t)[i].token == Token::TYPE || (*t)[i].token == Token::IDENT)) {} // ERROR
        p.type = (*t)[i].val;

        // If followed by * it is a pointer reference
        if ((*t)[++i].token == Token::ARITH_OP && (*t)[++i].val == "*") {
            p.is_pointer = true;
            i++;
        }

        // Must be followed by an identifier
        if ((*t)[i].token != Token::IDENT) {} // ERROR

        p.name = (*t)[i].val;

        i++;

        // Each , we increment to process the next set
        if ((*t)[i].token == Token::SPEC) {
            if ((*t)[i].val != ",") {} // ERROR
            else { i++; }
        }
        this->params.push_back(p);
    }
    return ++i;
}
/*
 * FnNode::proc_returns
 *
 * Processes values between -> ... { into a return value list, list is a vector of string of the types returned
 */
int FnNode::proc_returns(vector<TokenCap> *t, int i) {
    while((*t)[i].token != Token::BLOCK_START) {
        if (!((*t)[i].token == Token::IDENT || (*t)[i].token == Token::TYPE)) {} // ERROR
        this->returns.push_back((*t)[i].val);

        if ((*t)[++i].token == Token::SPEC) {
            if ((*t)[i].val != ",") {} // ERROR
            else { i++; }
        }
    }
    return ++i;
}
void FnNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- fn "
        + (this->is_method ? ("(" + this->method_par + (this->is_pointer ? "*" : "") + this->method_of + ")") : "")
        + " " + this->name + "(";

    for (int i = 0; i < (int)this->params.size(); i++)
        pre += this->params[i].type + " " + this->params[i].name + ", ";

    pre += ")";
    
    if (this->returns.size() > 0) {
        pre += " -> ";
        for (int i = 0; i < (int)this->returns.size(); i++)
            pre += this->returns[i] + ", ";
    }
    pre += ":";
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// OpNode
//======================================================================================================================
/*
 * OpNode::OpNode
 *
 * Base class for handling operations * -, etc.
 *
 * OpNode::print handles printing for inherited Nodes.
 */
OpNode::OpNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {}
void OpNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- op: " + this->name + ":";
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// ArithOpNode
//======================================================================================================================
/*
 * ArithOpNode::ArithOpNode
 *
 * Handles operators: + - / * %
 *
 * Children body element must be exactly two elements `<left> <op> <right>`
 */
ArithOpNode::ArithOpNode(vector<TokenCap> *t, int s, int e): OpNode(t, s, e) {
    if ((*t)[s+1].token != Token::ARITH_OP) {} // ERROR

    this->name = (*t)[s+1].val;

    // Set the left hand side of the value
    switch((*t)[s].token) {
        // Value reference
        case Token::IDENT:
            this->body.push_back(new IdentNode(t, s, s));
            break;

        // Primitive type Integers
        case Token::INT:
            this->body.push_back(new IntPrimitiveNode(t, s, s));
            break;

        // TODO: Implement other primitives
    }

    // Set the right hand side
    switch((*t)[e].token) {
        // Value reference
        case Token::IDENT:
            this->body.push_back(new IdentNode(t, e, e));
            break;

        // Primitive type Integers
        case Token::INT:
            this->body.push_back(new IntPrimitiveNode(t, e, e));
            break;

        // TODO: Implement other primitives
    }
}


//======================================================================================================================
// CallFnNode
//======================================================================================================================
/*
 * CallFnNode
 *
 * Identity reference for funciton being called.
 *
 * <ident> = myfunc(...)
 */
CallFnNode::CallFnNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s;
    if ((*t)[i].token != Token::IDENT) {} // ERROR

    this->name = (*t)[i].val;
    if ((*t)[++i].token != Token::PAR_START) {} // ERROR

    // Process each param in the (...)
    i = this->proc_params(t, ++i);
}
/*
 * CallFnNode::proc_params
 *
 * Processes passed values in (...) to store into a body statement
 */
int CallFnNode::proc_params(vector<TokenCap>* t, int i) {
    // TODO: Handle pointer dereference
    // TODO: Handle operations
    while((*t)[i].token != Token::PAR_END) {
        if ((*t)[i].token != Token::IDENT) {} // ERROR

        if ((*t)[i+1].token == Token::PAR_START) {
            int s = i;
            int e = collect(t, &i, Token::PAR_START, Token::PAR_END, vector<Token>{Token::PAR_END});
            this->body.push_back(new CallFnNode(t, s, e));

        } else {
            this->body.push_back(new IdentNode(t, i, i));
        }
        i++;
    }
    return i;
}
void CallFnNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- call: " + this->name + ":";
    cout << pre << endl;
    BODY;
}


//======================================================================================================================
// PrimitiveNode
//======================================================================================================================
PrimitiveNode::PrimitiveNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {}


//======================================================================================================================
// IntPrimitiveNode
//======================================================================================================================
/*
 * IntPrimitiveNode::IntPrimitiveNode
 *
 * Handles a single token for integer values. Simply a storage container for the value.
 */
IntPrimitiveNode::IntPrimitiveNode(vector<TokenCap> *t, int s, int e): PrimitiveNode(t, s, e) {
    if ((*t)[s].token != Token::INT) {} // ERROR
    this->val = stoi((*t)[s].val);
}
void IntPrimitiveNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- p_int: " + to_string(this->val);
    cout << pre << endl;
}
