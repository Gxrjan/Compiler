#include "head.h"


Program::Program(vector<unique_ptr<Statement>> statements)
{
    this->statements = statements;
}
string Program::to_string()
{
    string result = "";
    for (long unsigned int i=0;i<this->statements.size();i++)
        result += this->statements[i]->to_string() + "\n";
    return result;
}
