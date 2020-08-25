#include "head.h"

Literal::Literal(int num) { this->num = num; }
string Literal::to_string() { return std::to_string(this->num); }

OpExpr::OpExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right) {
    this->op = op;
    this->left = move(left);
    this->right = move(right);
}
string OpExpr::to_string() { return "(" + (this->left)->to_string() \
                                    + " " + string(1, this->op) + " "\
                                    + (this->right)->to_string() + ")"; } 

