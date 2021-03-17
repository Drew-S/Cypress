#include <iostream>
#include <stdio.h>

#include "node.hpp"

// Helpers for Printing nodes
// PRE adds spacing in the front for nesting based on the depth of the Node
#define PRE for(int i = 0; i < d; i++) pre += "  "
// BODY loops over the nodes body to ToString the child nodes
#define BODY for(int i = 0; i < (int)this->body.size(); i++) pre += this->body[i]->ToString(d+1)

// Shortcut for dereferencing token pointer
#define T (*t)

static bool IS_ASSIGN(vector<TokenCap> *t, int i) {
    i++;
    if (i >= t->size() || T[i].token == ASSIGN || T[i].token == STATEMENT_END || T[i].token == BLOCK_END)
        return true;

    if (T[i].token == ARITH_OP && T[i].val == "*")
        i++;

    if (i >= t->size() || T[i].token != IDENT)
        return false;

    i++;

    if (i >= t->size() || T[i].token == ASSIGN || T[i].token == STATEMENT_END || T[i].token == BLOCK_END)
        return true;

    return false;
}

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
 * Node::ToString
 *
 * Loops over the body of the nodes ToStringing the children.
 *
 * Each inherited node ToStrings similarly, its own format for its specific use and then its children. Comments for
 *   inherited nodes are omitted.
 */
string Node::ToString(int d) {
    string pre;
    BODY;
    return pre;
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

/*
 * Node::parse
 *
 * Parses the tokens from start to end into nodes
 */

void Node::parse(Node *parent, vector<TokenCap> *t, int *start, int end) {
    for (int i = *start; i <= end; i++) {
        if (i >= t->size())
            return;
        switch(T[i].token) {
            case STATEMENT_END:
            case PAR_END:
            case BLOCK_END:
                break;

            case KEYWORD:
                if (T[i].val == "fn") {
                    int sub_start = i;
                    int sub_end = Node::collect_block(t, &i, false);
                    parent->body.push_back(new FnNode(t, sub_start, sub_end));

                } else if (T[i].val == "struct" || T[i].val == "interface") {
                    int sub_start = i;
                    int sub_end = Node::collect_block(t, &i, false);
                    parent->body.push_back(new DefineIdentNode(t, sub_start, sub_end));
                }
                break;
            case IDENT:
                if (!IS_ASSIGN(t, i)) {
                    if (T[i+1].token == PAR_START) {
                        int sub_start = i;
                        int sub_end = Node::collect_block(t, &i, true);
                        parent->body.push_back(new CallFnNode(t, sub_start, sub_end));

                    } else if (T[i+1].token == ARITH_OP) {
                        int sub_start = i;
                        int sub_end = Node::collect_statement(t, &i);
                        parent->body.push_back(new ArithOpNode(t, sub_start, sub_end));
                    }
                    break;
                }
            case TYPE:
                if (IS_ASSIGN(t, i)) {
                    int sub_start = i;
                    int sub_end = Node::collect_statement(t, &i);
                    parent->body.push_back(new AssignIdentNode(t, sub_start, sub_end));

                } else if (T[i+1].token == PAR_START) {
                    // Type cast: int(thing)
                }
                break;
            case INT:
                if (T[i+1].token == ARITH_OP) {
                    parent->body.push_back(new ArithOpNode(t, i, i+2));
                    i += 2;

                } else {
                    parent->body.push_back(new IntPrimitiveNode(t, i, i));
                }
                break;
            case FLOAT:
                parent->body.push_back(new FloatPrimitiveNode(t, i, i));
                break;
            case CHAR:
                parent->body.push_back(new CharPrimitiveNode(t, i, i));
                break;
            case STRING:
                parent->body.push_back(new StringPrimitiveNode(t, i, i));
                break;
            case BOOL:
                parent->body.push_back(new BoolPrimitiveNode(t, i, i));
                break;
        }
    }
}

/*
 * Node::collect_block
 *
 * Get ending index for a block statement (is_par for collecting (...) instead of {...})
 */
int Node::collect_block(vector<TokenCap> *t, int* i, bool is_par) {
    int d = 0;
    if (is_par) {
        while(*i < (int)(*t).size()) {
            if ((*t)[*i].token == PAR_START) {
                d++;

            } else if ((*t)[*i].token == PAR_END) {
                d--;
                if (d <= 0)
                    break;
            }
            (*i)++;
        }
    } else {
        while(*i < (int)(*t).size()) {
            if ((*t)[*i].token == BLOCK_START) {
                d++;

            } else if ((*t)[*i].token == BLOCK_END) {
                d--;
                if (d <= 0)
                    break;
            }
            (*i)++;
        }
    }
    return *i;
}

/*
 * Node::collect_statement
 *
 * Get ending index for a line
 */
int Node::collect_statement(vector<TokenCap> *t, int* i) {
    int pd = 0;
    int bd = 0;
    while (*i < (int)(*t).size()) {
        if (T[*i].token == PAR_START)
            pd++;
        else if (T[*i].token == PAR_END)
            pd--;

        if (T[*i].token == BLOCK_START)
            bd++;
        else if (T[*i].token == BLOCK_END)
            bd--;

        if (pd <= 0 && bd <= 0 && (T[*i].token == STATEMENT_END || T[*i].token == PAR_END || T[*i].token == BLOCK_END))
            break;

        (*i)++;
    }
    return *i;
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
        if ((*t)[i].token == IDENT) {
            this->body.push_back(new IdentNode(t, i, i));

        } else if ((*t)[i].token == ARITH_OP) {
            if (this->body.size() <= 0) {} // ERROR
            this->body.pop_back();

            this->body.push_back(new ArithOpNode(t, i-1, i+1));
            i++;
        }
    }
}
string ReturnNode::ToString(int d) {
    string pre;
    PRE;
    pre += "return:\n";
    BODY;
    return pre;
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
    if ((*t)[i].token == ARITH_OP) {
        if ((*t)[i].val == "*") {
            this->is_pointer = true;

        } else {} // ERROR
        i++;
    }
    if ((*t)[i].token != IDENT) {} // ERROR

    // Store reference name
    this->name = (*t)[i].val;
}
string IdentNode::ToString(int d) {
    string pre;
    PRE;
    pre += (this->is_pointer ? "ident: *" : "ident: ") + this->name + "\n";
    BODY;
    return pre;
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
    if (!((*t)[i].token == TYPE || (*t)[i].token == IDENT)) {} // ERROR

    if ((*t)[i+1].token == IDENT) {
        this->type = (*t)[i].val;
        i++;
    }

    this->name = (*t)[i].val;

    i++;

    // Does not follow with an =
    if ((*t)[i].token != ASSIGN) {
        if (this->type == "int")
            this->body.push_back(new IntPrimitiveNode(t, i-1, i-1));

        else if (this->type == "float")
            this->body.push_back(new FloatPrimitiveNode(t, i-1, i-1));

        else if (this->type == "char")
            this->body.push_back(new CharPrimitiveNode(t, i-1, i-1));

        else if (this->type == "string")
            this->body.push_back(new StringPrimitiveNode(t, i-1, i-1));

        else if (this->type == "bool")
            this->body.push_back(new BoolPrimitiveNode(t, i-1, i-1));

    } else {
        i++;
        Node::parse(this, t, &i, e);
    }
}
string AssignIdentNode::ToString(int d) {
    string pre;
    PRE;
    pre += "" + this->type + " " + this->name + " = :\n";
    BODY;
    return pre;
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
    if ((*t)[i].token == PAR_START) {
        // Set is method
        this->is_method = true;

        // Following token needs to be a parameter identity
        if ((*t)[++i].token != IDENT) {} // ERROR

        this->method_par = (*t)[i].val;

        // If followed by `*` it is a pointer to an object
        i++;
        if ((*t)[i].token == ARITH_OP && (*t)[i].val == "*") {
            this->is_pointer = true;
            i++;
        }

        // Needs to be follwed by the identity of the defined object
        if ((*t)[i].token != IDENT || (*t)[i].token != TYPE) {} // ERROR

        this->method_of = (*t)[i].val;

        // Need a closing parenthesis
        if ((*t)[++i].token != PAR_END) {} // ERROR

        i++;
    }

    // Need a name of the function
    if ((*t)[i].token != IDENT) {} // ERROR

    this->name = (*t)[i].val;

    // Need a set of parenthesis for a function
    if ((*t)[++i].token != PAR_START) {} // ERROR

    i++;

    // Process values in the (...) into a parameter list
    i = this->proc_params(t, i);

    // If followed by -> process return values
    if ((*t)[i].token == SPEC && (*t)[i].val == "->")
        i = this->proc_returns(t, ++i);

    // Process the body of a function
    Node::parse(this, t, &i, e+1);
}
/*
 * FnNode::proc_params
 *
 * Processes a list tokens between (...) into a parameter list storing the type and scope reference value
 */
int FnNode::proc_params(vector<TokenCap> *t, int i) {
    while ((*t)[i].token != PAR_END) {
        Param p;

        // Record the type of the token
        if (!((*t)[i].token == TYPE || (*t)[i].token == IDENT)) {} // ERROR
        p.type = (*t)[i].val;

        // If followed by * it is a pointer reference
        if ((*t)[++i].token == ARITH_OP && (*t)[++i].val == "*") {
            p.is_pointer = true;
            i++;
        }

        // Must be followed by an identifier
        if ((*t)[i].token != IDENT) {} // ERROR

        p.name = (*t)[i].val;

        i++;

        // Each , we increment to process the next set
        if ((*t)[i].token == SPEC) {
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
    while((*t)[i].token != BLOCK_START) {
        if (!((*t)[i].token == IDENT || (*t)[i].token == TYPE)) {} // ERROR
        this->returns.push_back((*t)[i].val);

        if ((*t)[++i].token == SPEC) {
            if ((*t)[i].val != ",") {} // ERROR
            else { i++; }
        }
    }
    return ++i;
}
string FnNode::ToString(int d) {
    string pre;
    PRE;
    pre += "fn "
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
    pre += ":\n";
    BODY;
    return pre;
}


//======================================================================================================================
// OpNode
//======================================================================================================================
/*
 * OpNode::OpNode
 *
 * Base class for handling operations * -, etc.
 *
 * OpNode::ToString handles ToStringing for inherited Nodes.
 */
OpNode::OpNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {}
string OpNode::ToString(int d) {
    string pre;
    PRE;
    pre += "op: " + this->name + ":\n";
    BODY;
    return pre;
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
    if ((*t)[s+1].token != ARITH_OP) {} // ERROR

    this->name = (*t)[s+1].val;

    // Set the left hand side of the value
    switch((*t)[s].token) {
        // Value reference
        case IDENT:
            this->body.push_back(new IdentNode(t, s, s));
            break;

        // Primitive type Integers
        case INT:
            this->body.push_back(new IntPrimitiveNode(t, s, s));
            break;

        // TODO: Implement other primitives
    }

    // Set the right hand side
    switch((*t)[e].token) {
        // Value reference
        case IDENT:
            this->body.push_back(new IdentNode(t, e, e));
            break;

        // Primitive type Integers
        case INT:
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
    if ((*t)[i].token != IDENT) {} // ERROR

    this->name = (*t)[i].val;
    if ((*t)[++i].token != PAR_START) {} // ERROR

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
    while((*t)[i].token != PAR_END) {
        if ((*t)[i].token != IDENT) {} // ERROR

        if ((*t)[i+1].token == PAR_START) {
            int s = i;
            int e = Node::collect_block(t, &i, true);
            this->body.push_back(new CallFnNode(t, s, e));

        } else {
            this->body.push_back(new IdentNode(t, i, i));
        }
        i++;
    }
    return i;
}
string CallFnNode::ToString(int d) {
    string pre;
    PRE;
    pre += "call: " + this->name + ":\n";
    BODY;
    return pre;
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
    if ((*t)[s].token == IDENT) return;

    if ((*t)[s].token == INT) {
        this->val = stoi((*t)[s].val);

    } else {} // ERROR
}
string IntPrimitiveNode::ToString(int d) {
    string pre;
    PRE;
    char buf[64];
    sprintf(buf, "%d", this->val);
    pre += ((string)"p_int: ") + buf + "\n";
    return pre;
}


//======================================================================================================================
// FloatPrimitiveNode
//======================================================================================================================
/*
 * FloatPrimitiveNode::FloatPrimitiveNode
 *
 * Handles a single token for float values. Simply a storage container for the value.
 */
FloatPrimitiveNode::FloatPrimitiveNode(vector<TokenCap> *t, int s, int e): PrimitiveNode(t, s, e) {
    if ((*t)[s].token == IDENT) return;

    if ((*t)[s].token == FLOAT) {
        this->val = stof((*t)[s].val);

    } else {} // ERROR
}
string FloatPrimitiveNode::ToString(int d) {
    string pre;
    PRE;
    char buf[64];
    sprintf(buf, "%0.8f", this->val);
    pre += ((string)"p_float: ") + buf + "\n";
    return pre;
}


//======================================================================================================================
// CharPrimitiveNode
//======================================================================================================================
/*
 * CharPrimitiveNode::CharPrimitiveNode
 *
 * Handles a single token for character values. Simply a storage container for the value.
 */
CharPrimitiveNode::CharPrimitiveNode(vector<TokenCap> *t, int s, int e): PrimitiveNode(t, s, e) {
    if ((*t)[s].token == IDENT) return;

    if ((*t)[s].token == CHAR) {
        this->set = true;
        this->val = (*t)[s].val[0];

    } else {} // ERROR
}
string CharPrimitiveNode::ToString(int d) {
    string pre;
    PRE;
    pre += ((string)"p_char: ") + (this->set ? (string)"" + this->val : (string)"") + "\n";
    return pre;
}


//======================================================================================================================
// StringPrimitiveNode
//======================================================================================================================
/*
 * StringPrimitiveNode::StringPrimitiveNode
 *
 * Handles a single token for string values. Simply a storage container for the value.
 */
StringPrimitiveNode::StringPrimitiveNode(vector<TokenCap> *t, int s, int e): PrimitiveNode(t, s, e) {
    if ((*t)[s].token == IDENT) return;

    if ((*t)[s].token == STRING) {
        this->val = (*t)[s].val;

    } else {} // ERROR
}
string StringPrimitiveNode::ToString(int d) {
    string pre;
    PRE;
    pre += "p_string: " + this->val + "\n";
    return pre;
}


//======================================================================================================================
// BoolPrimitiveNode
//======================================================================================================================
/*
 * BoolPrimitiveNode::BoolPrimitiveNode
 *
 * Handles a single token for boolean values. Simply a storage container for the value.
 */
BoolPrimitiveNode::BoolPrimitiveNode(vector<TokenCap> *t, int s, int e): PrimitiveNode(t, s, e) {
    if ((*t)[s].token == IDENT) return;

    if ((*t)[s].token == BOOL) {
        if ((*t)[s].val == "true")
            this->val = true;
        else
            this->val = false;

    } else {} // ERROR
}
string BoolPrimitiveNode::ToString(int d) {
    string pre;
    PRE;
    pre += ((string)"p_bool: ") + (this->val ? "true\n" : "false\n");
    return pre;
}


//======================================================================================================================
// DefineIdentNode
//======================================================================================================================
/*
 * DefineIdentNode::DefineIdentNode
 *
 * Stores a defined structure or interface for custom types
 */
DefineIdentNode::DefineIdentNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s;

    if (T[i].token != KEYWORD) {} // ERROR

    if (T[i].val == "interface")
        this->interface = true;

    if (T[++i].token != IDENT) {} // ERROR

    this->name = T[i].val;

    i++;
    if (T[i].token != BLOCK_START) {} // ERROR

    this->process_block(t, i, e);
}
/*
 * DefineIdentNode::process_block
 *
 * Processes the block statement for interface/struct {...} into a set of fields
 */
void DefineIdentNode::process_block(vector<TokenCap> *t, int s, int e) {
    int i = s;

    if (T[i].token == BLOCK_START)
        i++;

    if (T[i].token == STATEMENT_END)
        i++;

    if (this->interface)
        while (i < e) {
            Field f;
            if (T[i+1].token == PAR_START) {
                if (T[i].token != IDENT) {} // ERROR
                f.type = "void";

            } else {
                if (T[i].token != IDENT && T[i].token != TYPE) {} // ERROR
                f.type = T[i].val;
                i++;
            }

            if (T[i].token != IDENT) {} // ERROR

            f.name = T[i].val;
            i += 2;
            while (T[i].token != PAR_END) {
                Param p;
                if (T[i].token != IDENT && T[i].token != TYPE) {} // ERROR
                p.type = T[i].val;
                i++;

                if (T[i].token == ARITH_OP && T[i].val == "*") {
                    p.is_pointer = true;
                    i++;
                }

                if(T[i].token == SPEC) {
                    if (T[i].val != ",") {} // ERROR
                    i++;
                }

                f.params.push_back(p);
            }
            i += 2;
            this->fields.push_back(f);
        }
    else
        while (i < e) {
            Field f;
            if (T[i].token != IDENT && T[i].token != TYPE) {} // ERROR
            f.type = T[i].val;
            i++;
            if (T[i].token == ARITH_OP && T[i].val == "*") {
                f.is_pointer = true;
                i++;
            }
            if (T[i].token != IDENT) {} // ERROR
            f.name = T[i].val;
            i += 2;
            this->fields.push_back(f);
        }
}
/*
 * DefineIdentNode::string_fields
 *
 * lists each field under struct
 */
string DefineIdentNode::string_fields(int d) {
    string pre;
    for (int i = 0; i < (int)this->fields.size(); i++) {
        PRE;
        pre += this->fields[i].type + " ";
        if (this->fields[i].is_pointer)
            pre += "*";

        pre += this->fields[i].name;
        if (this->interface) {
            pre += "(";
            if (this->fields[i].params.size() > 0) {
                for (int j = 0; j < (int)this->fields[i].params.size(); j++) {
                    pre += this->fields[i].params[j].type;
                    if (this->fields[i].params[j].is_pointer)
                        pre += "*";
                    pre += this->fields[i].params[j].name + ", ";
                }
            }
            pre += ")";
        }
        pre += "\n";
    }
    return pre;
}
string DefineIdentNode::ToString(int d) {
    string pre;
    PRE;
    if (this->interface)
        pre += "interface ";

    else
        pre += "struct ";

    pre += this->name + ":\n";
    pre += this->string_fields(d+1);
    return pre;
}
