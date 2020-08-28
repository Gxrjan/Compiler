#include "head.h"

Program::Program(unique_ptr<Statement> statement, unique_ptr<Program> next)
{
    this->statement = move(statement);
    this->next = move(next); 
}
string Program::to_string()
{
    if (this->next)
        return "(Prog: " + this->statement->to_string() + " : " + this->next->to_string() + ")";
    else
        return "(Prog: " + this->statement->to_string() + ")";
}
