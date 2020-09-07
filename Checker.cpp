#include "head.h"


void Checker::report_error(int line, int col, string message)
{
    throw runtime_error("Checker error line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

Declaration *Checker::look_up(Id id, Block *b)
{
    if (!b)
        return nullptr;
    auto it = b->variables.find(id);
    if (it != b->variables.end())
        return it->second;
    else
        return this->look_up(id, b->parent);
}



Type Checker::check_expr(Expr *expr, Block *b)
{
    int num;
    bool bl;
    if (expr->isNumLiteral(&num))
        return Type::Int;
    if (expr->isBoolLiteral(&bl))
        return Type::Bool;

    string name;

    if (expr->isVariable(&name)) {
        Declaration *result = this->look_up(name, b);
        if (!result)
            this->report_error(expr->line, expr->col, "variable hasn't been declared");
        return result->type;
    }
    
    Expr* left;
    Expr* right;
    string op;
    if (expr->isOpExpr(&op, &left, &right)) {
        Type left_type = this->check_expr(left, b);
        Type right_type = this->check_expr(right, b);
        if (left_type != right_type || left_type != Type::Int || right_type != Type::Int)
            this->report_error(expr->line, expr->col, "operands must be int");
        if (op == "<" || op == ">" || op == "=="
            || op == "!=" || op == "<=" || op == ">=")
            return Type::Bool;
        return Type::Int;
    }
    throw runtime_error("Unrecognized expression");
}


void Checker::check_statement(Statement *s, Block *b)
{
    Statement *if_s;
    Statement *else_s;
    Expr *expr;
    Id id;
    Type t;

    if (s->isDeclaration(&t, &id, &expr)) {
        string t_string = TypeConverter::enum_to_string(t);
        Declaration *dec = dynamic_cast<Declaration *>(s);
        if (t != this->check_expr(expr, b))
            this->report_error(expr->line, expr->col, t_string + " expected");
        if (this->look_up(id, b))
            this->report_error(s->line, s->col, "variable has already been declared");
        b->variables.insert({id, dec});
    }else if (s->isAssignment(&id, &expr)) {
        Declaration *result = this->look_up(id, b);
        if (!result)
            this->report_error(s->line, s->col, "variable hasn't been declared");
        string t_string = TypeConverter::enum_to_string(result->type);
        if (result->type != this->check_expr(expr, b))
            this->report_error(expr->line, expr->col, t_string + " expected");
    }else if (s->isPrint(&expr)) {
        this->check_expr(expr, b);
    } else if (s->isIfStatement(&expr, &if_s, &else_s)) {
        if (this->check_expr(expr, b) != Type::Bool)
            this->report_error(expr->line, expr->col, "if statement requires bool");
        this->check_statement(if_s, b);
        if (else_s)
            this->check_statement(else_s, b);
    } else {

        Block *b1 = dynamic_cast<Block *>(s);
        if (b1) {
            b1->parent = b;
            this->check_block(b1);
        } else
            throw runtime_error("Unknown statement type");
    }

}


void Checker::check_block(Block *b)
{

    for (auto &s : b->statements) {
        this->check_statement(s.get(), b);
    }
}

void Checker::check_program(Program *p)
{
    this->check_block(p->block.get());
}


