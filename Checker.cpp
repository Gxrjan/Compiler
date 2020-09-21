#include "head.h"


void Checker::expect_type(Expr *e, Block *b, Type *t)
{
    if (this->check_expr(e, b) != t)
        this->report_error(e->line, e->col,
        TypeConverter::enum_to_string(t)+" expected");
}

bool Checker::convertible_to_int(Type *type)
{
    return (type == &Int || type == &Char);
}

Type *Checker::check_compatability(OpExpr *expr, Block *b)
{
    Type *left_t = this->check_expr(expr->left.get(), b);
    Type *right_t = this->check_expr(expr->right.get(), b);
    
    switch (TypeConverter::string_to_operation(expr->op)) {
        case Operation::Add:
            if (!(
                (left_t == &String && right_t == &String) ||
                (left_t == &String && this->convertible_to_int(right_t)) ||
                (this->convertible_to_int(left_t) && right_t == &String) ||
                (this->convertible_to_int(left_t) && this->convertible_to_int(right_t))
                )) 
                this->report_error(expr->line, expr->col, "invalid operand types");
            return (left_t == &String || right_t == &String) ? &String : &Int;
            break;
        case Operation::Sub:
        case Operation::Mul:
        case Operation::Div:
        case Operation::Mod:
            if (!this->convertible_to_int(left_t) ||
                !this->convertible_to_int(right_t))
                this->report_error(expr->line, expr->col, "invalid operand types");
            return &Int;
            break;
        case Operation::L:
        case Operation::G:
        case Operation::Le:
        case Operation::Ge:
            if (!this->convertible_to_int(left_t) ||
                !this->convertible_to_int(right_t))
                this->report_error(expr->line, expr->col, "invalid operand types");
            return &Bool;
            break;
        case Operation::E: 
        case Operation::Ne:
            if (!((left_t == right_t) || 
                (this->convertible_to_int(left_t) && this->convertible_to_int(right_t))))
                this->report_error(expr->line, expr->col, "invalid operand types");
            return &Bool;
            break;
        case Operation::And:
        case Operation::Or:
            if (left_t != &Bool || right_t != &Bool)
                this->report_error(expr->line, expr->col, "invalid operand types");
            return &Bool;
            break;
        default:
            this->report_error(expr->line, expr->col, "Unrecognized operation");
            break;
    }
    throw runtime_error("For GCC");
}

void Checker::verify_assignment(Declaration *dec, Expr *expr, Block *b)
{
    string t_string = TypeConverter::enum_to_string(dec->type);
    Type *expr_type = this->check_expr(expr, b);
    if (dec->type == &Int) {
        if (!this->convertible_to_int(expr_type))
            this->report_error(expr->line, expr->col, "int or char expected");
    } else if (dec->type == &String) {
        if (expr_type != &String && expr_type != &Empty)
            this->report_error(expr->line, expr->col, "string or null expected");
    } else
        this->expect_type(dec->expr.get(), b, dec->type);
}

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


Type *Checker::check_variable(Variable *var, Block *b)
{
        Declaration *result = this->look_up(var->name, b);
        if (!result)
            this->report_error(var->line, var->col, "variable hasn't been declared");
        return result->type;
}

Type *Checker::check_elem_access_expr(ElemAccessExpr *expr, Block *b)
{
    Type *left_type = this->check_expr(expr->expr.get(), b);
    Type *right_type = this->check_expr(expr->index.get(), b);
    if (left_type != &String)
        this->report_error(expr->expr->line, expr->expr->col, "[] operator is only available for strings for now");
    if (!this->convertible_to_int(right_type))
        this->report_error(expr->index->line, expr->index->col, "[] accepts only int or char");
    return &Char;
}

Type *Checker::check_length_expr(LengthExpr *expr, Block *b)
{
    if (this->check_expr(expr->expr.get(), b) != &String)
        this->report_error(expr->expr->line, expr->expr->col, "Length function call is only applicable to strings");
    return &Int;
}

Type *Checker::check_type_cast_expr(TypeCastExpr *expr, Block *b)
{
    if (!this->convertible_to_int(expr->type))
        this->report_error(expr->line, expr->col, "You can type cast to char or int");
    if (!this->convertible_to_int(this->check_expr(expr->expr.get(), b)))
        this->report_error(expr->expr->line, expr->expr->col, "Type casts are only supported for ints and chars");
    return expr->type;
}


Type *Checker::check_substr_expr(SubstrExpr *e, Block *b)
{
    if (this->check_expr(e->expr.get(), b) != &String)
        this->report_error(e->line, e->col, "Substring is supported only for strings");
    if (e->arguments.size() < 1 || e->arguments.size() > 2)
        this->report_error(e->line, e->col, "wrong number of arguments");
    for (auto &a : e->arguments)
        if (!this->convertible_to_int(this->check_expr(a.get(), b)))
            this->report_error(a->line, a->col, "must be int or char");
    return &String;
}

Type *Checker::check_int_parse_expr(IntParseExpr *e, Block *b)
{
    if (e->arguments.size() != 1)
        this->report_error(e->line, e->col, "wrong number of arguments");
    this->expect_type(e->arguments[0].get(), b, &String);
    return &Int;
}

Type *Checker::check_new_str_expr(NewStrExpr *e, Block *b)
{
    if (e->arguments.size() != 2)
        this->report_error(e->line, e->col, "wrong number of arguments");
    this->expect_type(e->arguments[0].get(), b, &Char);
    if (!this->convertible_to_int(
            this->check_expr(e->arguments[1].get(), b)))
            this->report_error(e->arguments[1]->line,
                               e->arguments[1]->col,
                               "int or char expected");
    return &String;
}

Type *Checker::check_expr(Expr *expr, Block *b)
{
    return expr->type = this->check_expr_type(expr, b);
}

Type *Checker::check_expr_type(Expr *expr, Block *b)
{
    if (dynamic_cast<NumLiteral *>(expr))
        return &Int;
    if (dynamic_cast<BoolLiteral *>(expr))
        return &Bool;

    if (auto var = dynamic_cast<Variable *>(expr))
        return this->check_variable(var, b);

    if (dynamic_cast<CharLiteral *>(expr))
        return &Char;
    if (dynamic_cast<StringLiteral *>(expr))
        return &String;
    
    if (auto e = dynamic_cast<OpExpr *>(expr))
        return this->check_compatability(e, b);
    
    if (auto e = dynamic_cast<ElemAccessExpr *>(expr))
        return this->check_elem_access_expr(e, b);

    if (auto e = dynamic_cast<LengthExpr *>(expr))
        return this->check_length_expr(e, b);
    
    if (auto e = dynamic_cast<TypeCastExpr *>(expr))
        return this->check_type_cast_expr(e, b);

    if (auto e = dynamic_cast<SubstrExpr *>(expr))
        return this->check_substr_expr(e, b);

    if (auto e = dynamic_cast<IntParseExpr *>(expr))
        return this->check_int_parse_expr(e, b);

    if (auto e = dynamic_cast<NewStrExpr *>(expr))
        return this->check_new_str_expr(e, b);

    if (dynamic_cast<NullExpr *>(expr))
        return &Empty;

    throw runtime_error("Unrecognized expression");
}


void Checker::check_declaration(Declaration *dec, Block *b)
{
    if (this->look_up(dec->id, b))
        this->report_error(dec->line, dec->col, "variable has already been declared");
    this->verify_assignment(dec, dec->expr.get(), b);
    b->variables.insert({dec->id, dec});
}


void Checker::check_assignment(Assignment *asgn, Block *b)
{
    Declaration *result = this->look_up(asgn->id, b);
    if (!result)
        this->report_error(asgn->line, asgn->col, "variable hasn't been declared");
    this->verify_assignment(result, asgn->expr.get(), b);
}

void Checker::check_if_statement(IfStatement *st, Block *b, bool in_loop)
{
    if (this->check_expr(st->cond.get(), b) != &Bool)
        this->report_error(st->cond->line, st->cond->col, "if statement requires bool");
    this->check_statement(st->if_s.get(), b, in_loop);
    if (st->else_s)
        this->check_statement(st->else_s.get(), b, in_loop);
}

void Checker::check_while_statement(WhileStatement *st, Block *b)
{
    if (this->check_expr(st->cond.get(), b) != &Bool)
        this->report_error(st->cond->line, st->cond->col, "while statement requires bool");
    this->check_statement(st->statement.get(), b, true);
}

void Checker::check_for_statement(ForStatement *for_s, Block *b)
{
    // Checking init
    this->check_declaration(for_s->init.get(), b);
    
    // Checking cond
    if (this->check_expr(for_s->cond.get(), b) != &Bool)
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
        if (print->expr->type == &Empty)
            this->report_error(print->expr->line, 
                               print->expr->col, "Can't print null");
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


