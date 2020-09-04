#include "head.h"


Expr::Expr() {};


Expr::Expr(int line, int col)
{
    this->line = line;
    this->col = col;
}

NumLiteral::NumLiteral(int num) { this->num = num; }
string NumLiteral::to_string() { return std::to_string(this->num); }
bool NumLiteral::isNumLiteral(int *num) 
{ 
    (*num) = this->num; 
    return true;
}

BoolLiteral::BoolLiteral(bool b) { this->b = b; }
string BoolLiteral::to_string() 
{
    return (this->b) ? "true" : "false"; 
}
bool BoolLiteral::isBoolLiteral(bool *b) 
{ 
    (*b) = this->b; 
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
