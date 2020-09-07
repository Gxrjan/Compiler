#include "head.h"


Statement::Statement() {}

Statement::Statement(int line, int col)
{
    this->line = line;
    this->col = col;
}

Assignment::Assignment(Id id, unique_ptr<Expr> expr, int line, int col)
: Statement(line, col)
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


Declaration::Declaration
(Type t, Id id, unique_ptr<Expr> expr, int line, int col) 
: Statement(line, col)
{
    this->type = t;
    this->id = id;
    this->expr = move(expr);
}

string Declaration::to_string() 
{
    string s = TypeConverter::enum_to_string(this->type);
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



IfStatement::IfStatement(unique_ptr<Expr> cond, unique_ptr<Statement> if_s,
        unique_ptr<Statement> else_s)
{
    this->cond = move(cond);
    this->if_s = move(if_s);
    this->else_s = move(else_s);
}

bool IfStatement::isIfStatement(Expr **cond, Statement **if_s, Statement **else_s)
{
    *cond = this->cond.get();
    *if_s = this->if_s.get();
    *else_s = this->else_s.get();
    return true;
}

string IfStatement::to_string() 
{ 
    string result = "";
    result += "if " + this->cond->to_string() + "" + 
    "\n{\n" + this->if_s->to_string() + "\n}";
    if (this->else_s)
        result += "\n else {\n" + this->else_s->to_string() + "\n}";
    return result;
}
