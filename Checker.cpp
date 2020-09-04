#include "head.h"


void Checker::report_error(int line, int col, string message)
{
    throw runtime_error("Checker error line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

Declaration *Checker::look_up(Declaration *dec, Block *b)
{
    if (!b)
        return nullptr;
    auto it = b->variables.find(dec);
    if (it != b->variables.end())
        return *it;
    else
        return this->look_up(dec, b->parent);
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
        Declaration to_find{Type::Bool, name, nullptr, 0, 0 };
        Declaration *result = this->look_up(&to_find, b);
        if (!result)
            this->report_error(expr->line, expr->col, "variable hasn't been declared");
        return result->type;
    }
    
    Expr* left;
    Type left_type;
    Expr* right;
    Type right_type;
    char c;
    if (expr->isOpExpr(&c, &left, &right)) {
        left_type = this->check_expr(left, b);
        right_type = this->check_expr(right, b);
        if (left_type != right_type)
            this->report_error(expr->line, expr->col, "operands must be int");
        return left_type;
    }
    return Type::Int;
}


void Checker::check_block(Block *b)
{

    for (auto &s : b->statements) {
        Expr *expr;
        Id id;
        Type t;

        if (s->isDeclaration(&t, &id, &expr)) {
            string t_string = (t == Type::Int) ? "int" : "bool";
            Declaration *dec = dynamic_cast<Declaration *>(s.get());
            if (t != this->check_expr(expr, b))
                this->report_error(expr->line, expr->col, t_string + " expected");
            if (this->look_up(dec, b))
                this->report_error(s->line, s->col, "variable has already been declared");
            b->variables.insert(dec);
        }


        if (s->isAssignment(&id, &expr)) {
            Declaration to_find{Type::Bool, id, nullptr, 0, 0 };
            Declaration *result = this->look_up(&to_find, b);
            if (!result)
                this->report_error(s->line, s->col, "variable hasn't been declared");
            string t_string = (result->type == Type::Int) ? "int" : "bool";
            if (result->type !=this->check_expr(expr, b))
                this->report_error(expr->line, expr->col, t_string + " expected");
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


