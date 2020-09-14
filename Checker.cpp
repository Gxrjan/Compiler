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
    long long num;
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

    if (dynamic_cast<CharLiteral *>(expr))
        return Type::Char;
    
    Expr* left;
    Expr* right;
    string op;
    if (expr->isOpExpr(&op, &left, &right)) {
        Type left_type = this->check_expr(left, b);
        Type right_type = this->check_expr(right, b);
        
        if (op == "&&" || op == "||") {
            if (left_type != Type::Bool || right_type != Type::Bool)
                this->report_error(expr->line, expr->col, "operands must be bool");
            return Type::Bool;
        } else if (op == "==" || op == "!=") {
            if (left_type == Type::Bool || right_type == Type::Bool)
                this->report_error(expr->line, expr->col, "operands must be int or char");
            return Type::Bool;
                
        } else if (op == "<" || op == ">" || op == "<=" || op == ">="){
            if (left_type == Type::Bool || right_type == Type::Bool)
                this->report_error(expr->line, expr->col, "operands must be int or char");
            return Type::Bool;
        } else { 
            if (left_type == Type::Bool || right_type == Type::Bool)
                this->report_error(expr->line, expr->col, "operands must int or char");
            return Type::Int;
        }
    }
    throw runtime_error("Unrecognized expression");
}


void Checker::check_declaration(Declaration *dec, Block *b)
{
    string t_string = TypeConverter::enum_to_string(dec->type);
    if (dec->type != this->check_expr(dec->expr.get(), b))
        this->report_error(dec->expr->line, dec->expr->col, t_string + " expected");
    if (this->look_up(dec->id, b))
        this->report_error(dec->line, dec->col, "variable has already been declared");
    b->variables.insert({dec->id, dec});
}


void Checker::check_assignment(Assignment *asgn, Block *b)
{
    Declaration *result = this->look_up(asgn->id, b);
    if (!result)
        this->report_error(asgn->line, asgn->col, "variable hasn't been declared");
    string t_string = TypeConverter::enum_to_string(result->type);
    if (result->type != this->check_expr(asgn->expr.get(), b))
        this->report_error(asgn->expr->line, asgn->expr->col, t_string + " expected");
}

void Checker::check_if_statement(IfStatement *st, Block *b, bool in_loop)
{
    if (this->check_expr(st->cond.get(), b) != Type::Bool)
        this->report_error(st->cond->line, st->cond->col, "if statement requires bool");
    this->check_statement(st->if_s.get(), b, in_loop);
    if (st->else_s)
        this->check_statement(st->else_s.get(), b, in_loop);
}

void Checker::check_while_statement(WhileStatement *st, Block *b)
{
    if (this->check_expr(st->cond.get(), b) != Type::Bool)
        this->report_error(st->cond->line, st->cond->col, "while statement requires bool");
    this->check_statement(st->statement.get(), b, true);
}

void Checker::check_for_statement(ForStatement *for_s, Block *b)
{
    // Checking init
    this->check_declaration(for_s->init.get(), b);
    
    // Checking cond
    if (this->check_expr(for_s->cond.get(), b) != Type::Bool)
        this->report_error(for_s->cond->line, 
                           for_s->cond->col,
                           "Bool expression expected");

    // Checking iter
    this->check_assignment(for_s->iter.get(), b);

    // Checking body
    this->check_statement(for_s->body.get(), b, true);

    // Removing id after for statement is over
    b->variables.erase(for_s->init->id);
}

void Checker::check_statement(Statement *s, Block *b, bool in_loop)
{
    if (auto dec = dynamic_cast<Declaration *>(s)) {
        this->check_declaration(dec, b);
    }else if (auto asgn = dynamic_cast<Assignment *>(s)) {
        this->check_assignment(asgn, b);
    }else if (auto print = dynamic_cast<Print *>(s)) {
        this->check_expr(print->expr.get(), b);
    } else if (auto st = dynamic_cast<IfStatement *>(s)) {
        this->check_if_statement(st, b, in_loop);
    } else if (auto st = dynamic_cast<WhileStatement *>(s)) {
        this->check_while_statement(st, b); 
    } else if (auto for_s = dynamic_cast<ForStatement *>(s)) {
        this->check_for_statement(for_s, b);
    } else if (auto br = dynamic_cast<BreakStatement *>(s)) { 
        if (!in_loop)
            this->report_error(br->line, br->col, "break outside of loop");
    } else {
        Block *b1 = dynamic_cast<Block *>(s);
        if (b1) {
            b1->parent = b;
            this->check_block(b1, in_loop);
        } else
            throw runtime_error("Unknown statement type");
    }

}


void Checker::check_block(Block *b, bool in_loop)
{

    for (auto &s : b->statements) {
        this->check_statement(s.get(), b, in_loop);
    }
}

void Checker::check_program(Program *p)
{
    this->check_block(p->block.get(), false);
}


