#include <iostream>

#include "parser.hpp"

#define T this->tokens

using namespace std;

AST::AST(vector<pair<Token, string>> t) : tokens(t) {
    this->root.type = NodeType::ROOT;
    for (int i = 0; i < (int)t.size(); i++) {
        i = this->insert(i, &this->root);
        if (i == -1)
            return;
    }
}

void AST::exec() {}

bool AST::valid() {
    return true;
}

vector<Param> AST::proc_params(int *i) {
    vector<Param> pars;
    while (T[*i].first != Token::PAR_END) {
        Param p;

        if (T[*i].first != (Token::INT || Token::STRING || Token::FLOAT || Token::CHAR || Token::IDENT)) {
            cout << "ERR";
            return pars;
        }

        p.type = T[*i].second;
        (*i)++;

        if (T[*i].first == Token::ARITH_OP && T[*i].second == "*") {
            p.pointer = true;
            (*i)++;
        }

        p.ident = T[*i].second;

        (*i)++;
        if (T[*i].first == Token::SPEC && T[*i].second == ",")
            (*i)++;

        pars.push_back(p);
    }
    return pars;
}

vector<string> AST::proc_pass_params(int *i) {
    vector<string> pars;
    while(T[*i].first != Token::PAR_END) {
        if (T[*i].first == Token::SPEC && T[*i].second == ",") {
            (*i)++;
            continue;
        }
        if (T[*i].first != Token::IDENT && T[*i].first != Token::PAR_END) {
            cout << "ERR";
            return pars;
        }
        pars.push_back(T[*i].second);
        (*i)++;
    }
    (*i)++;
    return pars;
}

vector<string> AST::proc_returns(int *i) {
    vector<string> ret;
    while (T[*i].first != Token::BLOCK_START) {
        if (T[*i].first != (Token::INT || Token::STRING || Token::FLOAT || Token::CHAR || Token::IDENT)) {
            cout << "ERR";
            return ret;
        }
        ret.push_back(T[*i].second);
        (*i)++;
        if (T[*i].first == Token::SPEC && T[*i].second == ",")
            (*i)++;
    }
    return ret;
}

int AST::insert(int i, Node *cur) {
    Node n;
    while ((T[i].first == Token::STATEMENT_END || T[i].first == Token::BLOCK_END) && i < (int)T.size())
        i++;

    if (i >= (int)T.size())
        return i;

    switch (T[i].first)  {
        case Token::KEYWORD:
            {
                // Process a function declaration block
                // fn (...method) ident(...params) -> ...return { ...block(RECURSIVE) }
                if (T[i].second == "fn") {
                    n.type = NodeType::FN;

                    // TODO: Method_of func check here

                    if (T[++i].first == Token::IDENT)
                        n.ident = T[i].second;

                    if (T[++i].first != Token::PAR_START) {
                        cout << "ERR " << T[i].second << endl;
                        return -1;
                    }

                    i++;
                    n.params = this->proc_params(&i);
                    i++;

                    if (T[i].first == Token::SPEC && T[i].second == "->") {
                        i++;
                        n.ret = this->proc_returns(&i);
                    }

                    if (T[i].first == Token::BLOCK_START)
                        i = this->insert(++i, &n);

                // Process a return statement
                } else if (T[i].second == "return") {
                    n.type = NodeType::RET;
                    i++;
                    while (!(T[i].first == Token::STATEMENT_END || T[i].first == Token::BLOCK_END) && i < (int)T.size())
                        i = this->insert(i, &n) + 1;
                }
                break;
            }
        case Token::IDENT:
            {
                n.type = NodeType::REF;
                n.ident = T[i].second;
                if (T[i+1].first == Token::PAR_START) {
                    n.type = NodeType::CALL;
                    i += 2;
                    n.ret = this->proc_pass_params(&i);
                }
                break;
            }
        case Token::ARITH_OP:
            {
                n.type = NodeType::OP;
                n.ident = T[i].second;
                if (cur->block.size() == 0) {
                    cout << "ERR";
                    return i;
                }
                n.block.push_back(cur->block.back());
                cur->block.pop_back();
                i++;

                break;
            }
        case Token::INT:
            {
                n.type = NodeType::VAL;
                n.l_type = "int";
                int n_val = stoi(T[i].second);
                n.val = &n_val;
                break;
            }
        case Token::FLOAT:
            {
                n.type = NodeType::VAL;
                n.l_type = "float";
                float n_val = stof(T[i].second);
                n.val = &n_val;
                break;
            }
        case Token::STRING:
            {
                n.type = NodeType::VAL;
                n.l_type = "string";
                string n_val = T[i].second;
                n.val = &n_val;
                break;
            }
        case Token::CHAR:
            {
                n.type = NodeType::VAL;
                n.l_type = "char";
                char n_val = T[i].second[0];
                n.val = &n_val;
                break;
            }
    }
    cur->block.push_back(n);
    n.parent = cur;
    return i;
}

void AST::print(Node *cur, int d) {
    string pre = "";
    for (int i = 0; i < d; i++) {
        pre += "  ";
    }
    pre += "+--";
    switch (cur->type) {
        case NodeType::ROOT:
            pre += " root";
            break;

        case NodeType::FN:
            pre += " fn (" + cur->method_of + ") " + cur->ident + "(";
            for (int i = 0; i < (int)cur->params.size(); i++)
                pre += "" + cur->params[i].type + " " + (cur->params[i].pointer ? "*" : "") + cur->params[i].ident + ", ";

            pre += ")";
            if (cur->ret.size() > 0)
                pre += " -> ";
                for (int i = 0; i < (int)cur->ret.size(); i++)
                    pre += cur->ret[i] + ", ";

            break;

        case NodeType::RET:
            pre += " return";
            break;

        case NodeType::OP:
            pre += " op: " + cur->ident;
            break; 

        case NodeType::REF:
            pre += " ref: " + cur->ident;
            break;

        case NodeType::VAL:
            pre += " val: " + cur->l_type + " ";
            if (cur->l_type == "int")
                pre += *(int*)cur->val;

            if (cur->l_type == "string")
                pre += *(string*)cur->val;

            if (cur->l_type == "float")
                pre += *(float*)cur->val;

            if (cur->l_type == "char")
                pre += *(char*)cur->val;

            break;

        case NodeType::CALL:
            pre += " call: " + cur->ident + "(";
            for (int i = 0; i < (int)cur->ret.size(); i++)
                pre += cur->ret[i] + ", ";

            pre += ")";
            break;
    }
    cout << pre << endl;
    for (int i = 0; i < (int)cur->block.size(); i++) {
        this->print(&cur->block[i], d+1);
    }
}

void AST::print() {
    this->print(&this->root, 0);
}
