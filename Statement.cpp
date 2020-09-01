#include "head.h"

Assignment::Assignment(unique_ptr<Token> id, unique_ptr<Expr> expr)
{
    this->id = move(id);
    this->expr = move(expr);
}

string Assignment::to_string() { 
    return "(Assignment: " + this->id->to_string() +
            " = " + this->expr->to_string() + ")"; 
}

bool Assignment::isAssignment(IdToken** id, Expr** expr)
{
    *id = (IdToken*)this->id.get();
    *expr = this->expr.get();
    return true;
}



Print::Print(unique_ptr<Expr> expr)
{
    this->expr = move(expr);
}

string Print::to_string() 
{ 
    return "(Print statement: " + this->expr->to_string() + ")"; 
}

bool Print::isPrint(Expr** expr)
{
    *expr = this->expr.get();
    return true;
}
