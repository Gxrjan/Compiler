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

CharLiteral::CharLiteral(char c, int line, int col) 
{ 
    this->c = c; 
    this->line = line;
    this->col = col;
}
string CharLiteral::to_string() 
{
    return "'"+string{this->c}+"'"; 
}

StringLiteral::StringLiteral(string s, int line, int col) 
{ 
    this->s = s; 
    this->line = line;
    this->col = col;
}
string StringLiteral::to_string() 
{
    return "\""+this->s+"\""; 
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

ElemAccessExpr::ElemAccessExpr(unique_ptr<Expr> expr, unique_ptr<Expr> index,
                                 int line, int col)
{
    this->expr = move(expr);
    this->index = move(index);
    this->line = line;
    this->col = col;
}

string ElemAccessExpr::to_string()
{
    return this->expr->to_string() + "[" + this->index->to_string() + "]";
}

bool ElemAccessExpr::isElemAccessExpr(Expr **expr, Expr **index) 
{ 
    *expr = this->expr.get();
    *index = this->index.get();
    return true; 
}


LengthExpr::LengthExpr(unique_ptr<Expr> expr, int line, int col)
{
    this->line = line;
    this->col = col;
    this->expr = move(expr);
}

string LengthExpr::to_string() 
{
    return this->expr->to_string() + ".Length";
}

bool LengthExpr::isLengthExpr(Expr **expr)
{
    *expr = this->expr.get();
    return true;
}


TypeCastExpr::TypeCastExpr(Type *type, unique_ptr<Expr> expr, int line, int col)
{
    this->type = type;
    this->line = line;
    this->col = col;
    this->expr = move(expr);
}

string TypeCastExpr::to_string() 
{
    return "("+this->type->to_string()+") "+this->expr->to_string();
}

bool TypeCastExpr::isTypeCastExpr(Type **t, Expr **expr)
{
    *t = this->type;
    *expr = this->expr.get();
    return true;
}
string call_to_string(string prefix, vector<unique_ptr<Expr>> &args) {
    string result = prefix + "(";
    for (auto &a : args)
        result += a->to_string() + ",";
    result.pop_back();
    result += ")";
    return result;
}

SubstrExpr::SubstrExpr(unique_ptr<Expr> expr, vector<unique_ptr<Expr>> args, int line, int col)
{
    this->expr = move(expr);
    this->arguments = move(args);
    this->line = line;
    this->col = col;
}

string SubstrExpr::to_string()
{
    string result = this->expr->to_string() + ".Substring(";
    for (auto &a : this->arguments)
        result += a->to_string() + ",";
    result.pop_back();
    result += ")";
    return result;
}


IntParseExpr::IntParseExpr(vector<unique_ptr<Expr>> args, int line, int col)
{
    this->arguments = move(args);
    this->line = line;
    this->col = col;
}

string IntParseExpr::to_string()
{
    return call_to_string("int.Parse", this->arguments);
}


NewStrExpr::NewStrExpr(vector<unique_ptr<Expr>> arguments)
{
    this->arguments = move(arguments);
}

string NewStrExpr::to_string()
{
    return call_to_string("new string", this->arguments);
}

NullExpr::NullExpr(int line, int col) : Expr(line, col) {}
string NullExpr::to_string() { return "null"; }
    


NewArrExpr::NewArrExpr(Type *type, unique_ptr<Expr> expr, int line, int col) : Expr(line, col)
{
    this->type = type;
    this->expr = move(expr);
}
string NewArrExpr::to_string()
{
    return "new "+this->type->to_string()+" " +this->expr->to_string();
}
