#include "head.h"

Variable::Variable(string name, int line, int col) : Expr(line, col) 
{ 
    this->name = name; 
}


string Variable::to_string() { return this->name; }


bool Variable::isVariable(string *name)
{
    *name = this->name;
    return true;
}
