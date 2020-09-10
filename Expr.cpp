#include "head.h"


Expr::Expr() {};


Expr::Expr(int line, int col)
{
    this->line = line;
    this->col = col;
}

NumLiteral::NumLiteral(long long num, int line, int col) 
{ 
    this->num = num; 
    this->line = line;
    this->col = col;
}
string NumLiteral::to_string() { return std::to_string(this->num); }
bool NumLiteral::isNumLiteral(long long *num) 
{ 
    (*num) = this->num; 
    return true;
}

BoolLiteral::BoolLiteral(bool b, int line, int col) 
{ 
    this->b = b; 
    this->line = line;
    this->col = col;
}
string BoolLiteral::to_string() 
{
    return (this->b) ? "true" : "false"; 
}
bool BoolLiteral::isBoolLiteral(bool *b) 
{ 
    (*b) = this->b; 
    return true;
}


OpExpr::OpExpr
(string op, unique_ptr<Expr> left, unique_ptr<Expr> right, int line, int col) 
: Expr(line, col)
{
    this->op = op;
    this->left = move(left);
    this->right = move(right);
}
string OpExpr::to_string() { return "(" + (this->left)->to_string() \
                                    + " " + this->op + " "\
                                    + (this->right)->to_string() + ")"; }


 
bool OpExpr::isOpExpr(string *op, Expr **left, Expr **right)
{
    *op = this->op;
    *left = this->left.get();
    *right = this->right.get();
    return true;
}
