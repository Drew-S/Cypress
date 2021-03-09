#include <iostream>

#include "node.hpp"

#define PRE for(int i = 0; i < d; i++) pre += "    "
#define BODY for(int i = 0; i < (int)this->body.size(); i++) this->body[i]->print(d+1)

static int find_token(Token t, vector<Token> ts) {
    for (int i = 0; i < (int)ts.size(); i++)
        if (t == ts[i])
            return i;

    return -1;
}

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

void process(Node *cur, vector<TokenCap> *tokens, int *i, int stop) {
    while (*i < stop) {
        switch((*tokens)[*i].token) {
            case Token::INVALID:
            case Token::STATEMENT_END:
            case Token::BLOCK_END:
            case Token::PAR_END:
                break;
            case Token::KEYWORD:
                {
                    if ((*tokens)[*i].val == "fn") {
                        int s = *i;
                        int e = collect(tokens, i, Token::BLOCK_START, Token::BLOCK_END, vector<Token>{Token::BLOCK_END});
                        cur->body.push_back(new FnNode(tokens, s, e));

                    } else if ((*tokens)[*i].val == "return") {
                        int s = *i;
                        int e = collect(tokens, i, Token::INVALID, Token::INVALID, vector<Token>{Token::STATEMENT_END, Token::BLOCK_END});
                        cur->body.push_back(new ReturnNode(tokens, s, e));
                    }
                    break;
                }
            case Token::IDENT:
                {
                    if ((*tokens)[*i + 1].token == Token::PAR_START) {
                        int s = *i;
                        int e = collect(tokens, i, Token::PAR_START, Token::PAR_END, vector<Token>{Token::STATEMENT_END, Token::BLOCK_END, Token::PAR_END});
                        cur->body.push_back(new CallFnNode(tokens, s, e));
                        break;
                    }
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

Node::Node(vector<TokenCap> *t, int s, int e): tokens(t), start(s), end(e) {}
void Node::print(int d) {
    BODY;
}
Node::~Node() {
    for (Node *n : this->body) {
        delete n;
    }
    this->body.clear();
    if (this->parent != nullptr)
        delete this->parent;
}


ReturnNode::ReturnNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {
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


IdentNode::IdentNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {
    int i = s;
    if ((*t)[i].token == Token::ARITH_OP) {
        if ((*t)[i].val == "*") {
            this->is_pointer = true;

        } else {} // ERROR
        i++;
    }
    if ((*t)[i].token != Token::IDENT) {} // ERROR
    this->name = (*t)[i].val;
}
void IdentNode::print(int d) {
    string pre;
    PRE;
    pre += (this->is_pointer ? "+-- ident: *" : "+-- ident: ") + this->name;
    cout << pre << endl;
    BODY;
}


AssignIdentNode::AssignIdentNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s;
    if (!((*t)[i].token == Token::TYPE || (*t)[i].token == Token::IDENT)) {} // ERROR

    if ((*t)[i+1].token == Token::IDENT){
        this->type = (*t)[i].val;
        i++;
    }

    if ((*t)[i].token == Token::IDENT)
        this->name = (*t)[i].val;

    if ((*t)[++i].token != Token::ASSIGN) {} // ERROR

    switch((*t)[++i].token) {
        case Token::IDENT:
            if((*t)[i+1].token == Token::PAR_START) {
                int s1 = i;
                int e1 = collect(t, &i, Token::PAR_START, Token::PAR_END, vector<Token>{Token::PAR_END});
                this->body.push_back(new CallFnNode(t, s1, e1));

            } else {
                this->body.push_back(new IdentNode(t, i, i));
            }
            break;

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


FnNode::FnNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    int i = s + 1;
    // Process function is a method of
    if ((*t)[i].token == Token::PAR_START) {
        this->is_method = true;

        if ((*t)[++i].token != Token::IDENT) {} // ERROR

        this->method_par = (*t)[i].val;

        i++;
        if ((*t)[i].token == Token::ARITH_OP && (*t)[i].val == "*") {
            this->is_pointer = true;
            i++;
        }

        if ((*t)[i].token != Token::IDENT) {} // ERROR

        this->method_of = (*t)[i].val;

        if ((*t)[++i].token != Token::PAR_END) {} // ERROR

        i++;
    }

    if ((*t)[i].token != Token::IDENT) {} // ERROR

    this->name = (*t)[i].val;

    if ((*t)[++i].token != Token::PAR_START) {} // ERROR

    i++;

    i = this->proc_params(t, i);

    if ((*t)[i].token == Token::SPEC && (*t)[i].val == "->")
        i = this->proc_returns(t, ++i);

    process(this, t, &i, e+1);
}
int FnNode::proc_params(vector<TokenCap> *t, int i) {
    while ((*t)[i].token != Token::PAR_END) {
        Param p;

        if (!((*t)[i].token == Token::TYPE || (*t)[i].token == Token::IDENT)) {} // ERROR
        p.type = (*t)[i].val;

        if ((*t)[++i].token == Token::ARITH_OP && (*t)[++i].val == "*") {
            p.is_pointer = true;
            i++;
        }

        if ((*t)[i].token != Token::IDENT) {} // ERROR

        p.name = (*t)[i].val;

        i++;
        if ((*t)[i].token == Token::SPEC) {
            if ((*t)[i].val != ",") {} // ERROR
            else { i++; }
        }
        this->params.push_back(p);
    }
    return ++i;
}
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

OpNode::OpNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {}
void OpNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- op: " + this->name + ":";
    cout << pre << endl;
    BODY;
}

ArithOpNode::ArithOpNode(vector<TokenCap> *t, int s, int e): OpNode(t, s, e) {
    if ((*t)[s+1].token != Token::ARITH_OP) {} // ERROR

    this->name = (*t)[s+1].val;

    switch((*t)[s].token) {
        case Token::IDENT:
            this->body.push_back(new IdentNode(t, s, s));
            break;

        case Token::INT:
            this->body.push_back(new IntPrimitiveNode(t, s, s));
            break;
    }
    switch((*t)[e].token) {
        case Token::IDENT:
            this->body.push_back(new IdentNode(t, e, e));
            break;

        case Token::INT:
            this->body.push_back(new IntPrimitiveNode(t, e, e));
            break;
    }
}

CallFnNode::CallFnNode(vector<TokenCap> *t, int s, int e): IdentNode(t, s, e) {
    for (int i = s; i < e; i++) {
        if ((*t)[i].token != Token::IDENT) {} // ERROR

        this->name = (*t)[i].val;
        if ((*t)[++i].token != Token::PAR_START) {} // ERROR

        if ((*t)[++i].token != Token::IDENT) {} // ERROR

        if ((*t)[i+1].token == Token::PAR_START) {
            int s1 = i;
            int e1 = collect(t, &i, Token::PAR_START, Token::PAR_END, vector<Token>(Token::PAR_END));
            this->body.push_back(new CallFnNode(t, s1, e1));

        } else {
            this->body.push_back(new IdentNode(t, i, i));
        }
    }
}
void CallFnNode::print(int d) {
    string pre;
    PRE;
    pre += "+-- call: " + this->name + ":";
    cout << pre << endl;
    BODY;
}


PrimitiveNode::PrimitiveNode(vector<TokenCap> *t, int s, int e): Node(t, s, e) {}


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
