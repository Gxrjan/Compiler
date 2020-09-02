#include "head.h"


void Checker::report_error(int line, int col, string message)
{
    throw runtime_error("Checker error line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

void Checker::check_expr(Expr *expr)
{
    int num;
    if (expr->isLiteral(&num))
        return;

    string name;
    if (expr->isVariable(&name)) {
        if (this->variables.find(name) == this->variables.end())
            this->report_error(expr->line, expr->col, "Variable not declared");
    }
    
    Expr* left;
    Expr* right;
    char c;
    if (expr->isOpExpr(&c, &left, &right)) {
        this->check_expr(left);
        this->check_expr(right);
    }
}


void Checker::check_block(Block *b)
{
    for (auto &s : b->statements) {
        string name;
        Expr *expr;
        Id id;
        if (s->isPrint(&expr)) {
            this->check_expr(expr);
        }

        if (s->isAssignment(&id, &expr)) {
            this->check_expr(expr);
            this->variables.insert(id);
        }

    }
}

void Checker::check_program(Program *p)
{
    this->check_block(p->block.get());
}


