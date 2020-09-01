#include "head.h"


Expr::Expr() {};


Expr::Expr(int line, int col)
{
    this->line = line;
    this->col = col;
}

Literal::Literal(int num) { this->num = num; }
string Literal::to_string() { return std::to_string(this->num); }
bool Literal::isLiteral(int *num) 
{ 
    (*num) = this->num; 
    return true;
}

OpExpr::OpExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right) {
    this->op = op;
    this->left = move(left);
    this->right = move(right);
}
string OpExpr::to_string() { return "(" + (this->left)->to_string() \
                                    + " " + string(1, this->op) + " "\
                                    + (this->right)->to_string() + ")"; }


 
bool OpExpr::isOpExpr(char *op, Expr **left, Expr **right)
{
    *op = this->op;
    *left = this->left.get();
    *right = this->right.get();
    return true;
}
