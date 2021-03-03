#include "parser.hpp"

using namespace std;

Node::Node(Token t, string v): type(t), value(v) {}
Node::~Node() {}

void Node::append(Node* n) {
    this->children.push_back(n);
}


AST::AST(vector<pair<Token, string>> tokens) {
    this->root = new Node(Token::ROOT, "");
    for (int i = 1; i < (int)tokens.size(); i++) {
        this->insert(tokens[i]);
    }
}

AST::~AST() {
    delete this->root;
}

void AST::exec() {}

bool AST::valid() {
    return true;
}

void AST::insert(pair<Token, string> t) {

}
