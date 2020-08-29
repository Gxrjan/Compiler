#include "head.h"


Program::Program(vector<unique_ptr<Statement>> statements)
{
    //for (long unsigned int i=0;i<statements->size();i++)
    //    this->statements.push_back(move((*statements)[i]));
    this->statements = move(statements);
}
string Program::to_string()
{
    string result = "";
    for (auto &s:this->statements)
        result += s->to_string() + "\n";
    return result;
}
