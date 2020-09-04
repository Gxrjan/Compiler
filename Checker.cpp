#include "head.h"


void Checker::report_error(int line, int col, string message)
{
    throw runtime_error("Checker error line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

bool Checker::look_up(Id id, Block *b)
{
    if (!b)
        return false;
    return (b->variables.find(id) != b->variables.end()) || this->look_up(id, b->parent);
}


void Checker::check_expr(Expr *expr, Block *b)
{
    int num;
    if (expr->isNumLiteral(&num))
        return;

    string name;
    if (expr->isVariable(&name)) {
        if (!this->look_up(name, b))
            this->report_error(expr->line, expr->col, "variable hasn't been declared");
    }
    
    Expr* left;
    Expr* right;
    char c;
    if (expr->isOpExpr(&c, &left, &right)) {
        this->check_expr(left, b);
        this->check_expr(right, b);
    }
}


void Checker::check_block(Block *b)
{

    for (auto &s : b->statements) {
        Expr *expr;
        Id id;
        Type t;

        if (s->isDeclaration(&t, &id, &expr)) {
            this->check_expr(expr, b);
            if (this->look_up(id, b))
                this->report_error(s->line, s->col, "variable has already been declared");
            b->variables.insert(id);
        }


        if (s->isAssignment(&id, &expr)) {
            if (!this->look_up(id, b))
                this->report_error(s->line, s->col, "variable hasn't been declared");
            this->check_expr(expr, b);
        }

        if (s->isPrint(&expr)) {
            this->check_expr(expr, b);
        }

        Block *b1 = dynamic_cast<Block *>(s.get());
        if (b1) {
            b1->parent = b;
            this->check_block(b1);
        }

    }
}

void Checker::check_program(Program *p)
{
    this->check_block(p->block.get());
}


