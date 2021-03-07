#include <iostream>
#include <stdlib.h>

#include "parser.hpp"

#define T this->tokens

using namespace std;

/*
 * AST
 *
 * Create an abstract syntax tree from a tokenlist
 */
AST::AST(vector<pair<Token, string>> t) : tokens(t) {
    this->root.type = NodeType::ROOT;
    for (int i = 0; i < (int)t.size(); i++) {
        i = this->insert(i, &this->root);
        if (i == -1)
            return;
    }
}

/*
 * exec
 *
 * Executes the AST running the code
 */
void AST::exec() {}

/*
 * valid
 *
 * Returns whether the code is valid or not
 */
bool AST::valid() {
    return true;
}

/*
 * Errors
 *
 * TODO: Implement
 *
 * function to return a recording of errors, what, where, and why
 */

/*
 * proc_params
 *
 * Processes parameters inside a function declaration (...)
 */
vector<Param> AST::proc_params(int *i) {
    vector<Param> pars;
    while (T[*i].first != Token::PAR_END) {
        Param p;

        if (T[*i].first != Token::TYPE && T[*i].first != Token::IDENT) {
            cout << "param not a type" << endl;
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

/*
 * proc_pass_params
 *
 * Processes parameters inside a function call (...)
 */
vector<string> AST::proc_pass_params(int *i) {
    vector<string> pars;
    while(T[*i].first != Token::PAR_END) {
        if (T[*i].first == Token::SPEC && T[*i].second == ",") {
            (*i)++;
            continue;
        }
        if (T[*i].first != Token::INT &&
            T[*i].first != Token::STRING &&
            T[*i].first != Token::FLOAT &&
            T[*i].first != Token::CHAR &&
            T[*i].first != Token::IDENT) {
            cout << "passed value not valid" << endl;
            return pars;
        }
        pars.push_back(T[*i].second);
        (*i)++;
    }
    (*i)++;
    return pars;
}

/*
 * proc_returns
 *
 * Processes return statements into a list -> ... {
 */
vector<string> AST::proc_returns(int *i) {
    vector<string> ret;
    while (T[*i].first != Token::BLOCK_START) {
        if (T[*i].first != (Token::INT || Token::STRING || Token::FLOAT || Token::CHAR || Token::IDENT)) {
            cout << "return is not a valid type" << endl;
            return ret;
        }
        ret.push_back(T[*i].second);
        (*i)++;
        if (T[*i].first == Token::SPEC && T[*i].second == ",")
            (*i)++;
    }
    return ret;
}

/*
 * what_type
 *
 * returns the string type of the token, defaults to nil
 */
string AST::what_type(Token t) {
    switch(t) {
        case Token::INT:
            return "int";
        case Token::CHAR:
            return "char";
        case Token::STRING:
            return "string";
        case Token::FLOAT:
            return "float";
        default:
            return "nil";
    }
}

/*
 * insert
 *
 * Insert a token into the tree recursively
 */
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
                        cout << "unexpected token, expected '('" << endl;
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
                        while (T[i].first != Token::BLOCK_END && i < (int)T.size())
                            i = this->insert(++i, &n);

                // Process a return statement
                } else if (T[i].second == "return") {
                    n.type = NodeType::RET;
                    i++;
                    while (T[i].first != Token::STATEMENT_END && T[i].first != Token::BLOCK_END && i < (int)T.size())
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

                } else if (T[i+1].first == Token::ASSIGN) {
                    n.type = NodeType::REF_UP;
                    i += 2;
                    i = this->insert(i, &n) + 1;
                }
                break;
            }
        case Token::ARITH_OP:
            {
                n.type = NodeType::OP;
                n.ident = T[i].second;
                if (cur->block.size() == 0) {
                    cout << "operator has no left hand side value" << endl;
                    return i;
                }
                n.block.push_back(cur->block.back());
                cur->block.pop_back();
                i = this->insert(++i, &n) + 1;
                break;
            }
        case Token::INT:
            {
                n.type = NodeType::VAL;
                n.l_type = "int";
                int *n_val = (int*)malloc(sizeof(int));
                *n_val = stoi(T[i].second);
                n.val = (void*)n_val;
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
        case Token::TYPE:
            n.type = NodeType::ASSN;
            n.l_type = T[i].second;
            i++;
            if (T[i].first == Token::ARITH_OP && T[i].second == "*") {
                n.pointer = true;
                i++;
            }

            if (T[i].first != Token::IDENT) {
                cout << "unexpected token, expected identity" << endl;
                return -1;
            }
            n.ident = T[i].second;

            if (T[++i].first != Token::ASSIGN) {
                cout << "Expected assignment operator '='" << endl;
                return -1;
            }
            i++;

            if (this->what_type(T[i].first) != n.l_type) {
                cout << "Assigned type mismatch, expected " << n.l_type << endl;
                return -1;
            }
            switch(T[i].first) {
                case Token::INT:
                    {
                        int *n_val = (int*)malloc(sizeof(int));
                        *n_val = stoi(T[i].second);
                        n.val = (void*)n_val;
                        break;
                    }
                case Token::FLOAT:
                    {
                        float n_val = stof(T[i].second);
                        n.val = &n_val;
                        break;    
                    }
                case Token::STRING:
                    {
                        string n_val = T[i].second;
                        n.val = &n_val;
                        break;
                    }
                case Token::CHAR:
                    {
                        char n_val = T[i].second[0];
                        n.val = &n_val;
                        break;
                    }
            }
            if (T[++i].first != Token::STATEMENT_END) {
                cout << "Expected statement end ';' or '\\n'";
                return -1;
            }
            break;
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

        case NodeType::REF_UP:
            pre += " ref: " + cur->ident + " = ";
            break;

        case NodeType::VAL:
            pre += " val: " + cur->l_type + " ";
            if (cur->l_type == "int")
                pre += to_string(*(int*)cur->val);

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

        case NodeType::ASSN:
            pre += " declare: " + cur->l_type + " " + cur->ident + " = ";
            if (cur->l_type == "string")
                pre += *(string*)cur->val;

            else if (cur->l_type == "char")
                pre += *(char*)cur->val;

            else if (cur->l_type == "int")
                pre += to_string(*(int*)cur->val);

            else if (cur->l_type == "float")
                pre += *(float*)cur->val;
    }
    cout << pre << endl;
    for (int i = 0; i < (int)cur->block.size(); i++) {
        this->print(&cur->block[i], d+1);
    }
}

void AST::print() {
    this->print(&this->root, 0);
}
