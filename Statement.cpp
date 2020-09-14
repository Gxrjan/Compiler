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




Print::Print(unique_ptr<Expr> expr)
{
    this->expr = move(expr);
}

string Print::to_string() 
{ 
    return "(Print statement: " + this->expr->to_string() + ")"; 
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



Block::Block(vector<unique_ptr<Statement>> statements)
{
    this->statements = move(statements);
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


string IfStatement::to_string() 
{ 
    string result = "";
    result += "if " + this->cond->to_string() + "" + 
    "\n{\n" + this->if_s->to_string() + "\n}";
    if (this->else_s)
        result += "\n else {\n" + this->else_s->to_string() + "\n}";
    return result;
}


WhileStatement::WhileStatement(unique_ptr<Expr> cond, unique_ptr<Statement> statement)
{
    this->cond = move(cond);
    this->statement = move(statement);
}
string WhileStatement::to_string()
{
    return "while (" + this->cond->to_string() + ") {\n" +
            this->statement->to_string() + "\n}";
}


ForStatement::ForStatement(unique_ptr<Declaration> init, unique_ptr<Expr> cond, unique_ptr<Assignment> iter, unique_ptr<Statement> body)
{
    this->init = move(init);
    this->cond = move(cond);
    this->iter = move(iter);
    this->body = move(body);
}
string ForStatement::to_string()
{
    return "for (" + 
        this->init->to_string() + ";" +
        this->cond->to_string() + ";" + 
        this->iter->to_string() + ") {\n" +
            this->body->to_string() + "\n}";
}


BreakStatement::BreakStatement(int line, int col) : Statement(line, col) {}
string BreakStatement::to_string() { return "break"; }
