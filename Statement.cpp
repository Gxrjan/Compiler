#include "head.h"

Assignment::Assignment(Id id, unique_ptr<Expr> expr)
{
    this->id = id;
    this->expr = move(expr);
}

string Assignment::to_string() { 
    return "(Assignment: " + this->id +
            " = " + this->expr->to_string() + ")"; 
}

bool Assignment::isAssignment(Id* id, Expr** expr)
{
    *id = this->id;
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


Declaration::Declaration(Type t, Id id, unique_ptr<Expr> expr)
{
    this->type = t;
    this->id = id;
    this->expr = move(expr);
}

string Declaration::to_string() 
{
    string s;
    switch (this->type) {
        case Type::Bool:
            s = "bool";
            break;
        case Type::Int:
            s = "int";
            break;
        default:
            throw runtime_error("Unknown Type");
            break;
    }
    return "(Declaration statement: " + s + " " + this->id + " = " +
            this->expr->to_string() + ")";
}

bool Declaration::isDeclaration(Type *t, Id *id, Expr **expr)
{
    *t = this->type;
    *id = this->id;
    *expr = this->expr.get();
    return true;
}


Block::Block(vector<unique_ptr<Statement>> statements)
{
    this->statements = move(statements);
}

bool Block::isBlock(vector<Statement*> statements)
{
    for (auto &s : this->statements)
        statements.push_back(s.get());
    return true;
}


string Block::to_string()
{
    string result = "{ Block start\n";
    for (auto &s:this->statements)
        result += s->to_string() + "\n";
    return result + "} Block ends";
}
