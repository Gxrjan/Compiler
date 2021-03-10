#include "head.h"


void Checker::expect_type(Expr *e, Block *b, Type *t)
{
    if (this->check_expr(e, b) != t)
        this->report_error(e->line, e->col,
        t->to_string()+" expected");
}

bool Checker::convertible_to_int(Type *type)
{
    return (type == &Int || type == &Char);
}

bool Checker::nullable(Type *type)
{
    return ((type == &String) || dynamic_cast<ArrayType *>(type));
}

Type *Checker::check_compatability(OpExpr *expr, Block *b)
{
    if (expr->op == "!") {
        Type *left_t = this->check_expr(expr->left.get(), b);
        if (left_t!=&Bool)
            this->report_error(expr->left->line, expr->left->col, "bool expected");
        return &Bool;
    }
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
            if (!(
(left_t == right_t) || 
(this->convertible_to_int(left_t) && this->convertible_to_int(right_t)) || 
(left_t == &String && right_t == &Empty) ||
(right_t == &String && left_t == &Empty) ||
(dynamic_cast<ArrayType *>(left_t) && right_t == &Empty) ||
(dynamic_cast<ArrayType *>(right_t) && left_t == &Empty)
))
                this->report_error(expr->line, expr->col, "invalid operand types");
            return &Bool;
            break;
        case Operation::And:
        case Operation::Or:
            if (left_t != &Bool || right_t != &Bool)
                this->report_error(expr->line, expr->col, "invalid operand typeshehe");
            return &Bool;
            break;
        default:
            this->report_error(expr->line, expr->col, "Unrecognized operation");
            break;
    }
    throw runtime_error("For GCC");
}

void Checker::verify_assignment(Expr *left, Expr *right, Block *b)
{
    Type *left_t = this->check_expr(left, b);
    Type *right_t = this->check_expr(right, b);
    if (left_t == &Int) {
        if (!this->convertible_to_int(right_t))
            this->report_error(right->line, right->col, "int or char expected");
    } else if (left_t == &Bool) {
        if (right_t != &Bool)
            this->report_error(right->line, right->col, "bool expected");
    } else if (left_t == &Char) {
        if (right_t != &Char)
            this->report_error(right->line, right->col, "char expected");
    } else
        if (!((left_t == right_t) || 
                (right_t == &Empty)))
            this->report_error(right->line, right->col, left_t->to_string() + " or null expected");
}

void Checker::compare_types(Type *left_t, Type *right_t) {
    // if (left_t == &Int) {
    //     if (!this->convertible_to_int(right_t))
    //         this->report_error(right->line, right->col, "int or char expected");
    // } else if (left_t == &Bool) {
    //     if (right_t != &Bool)
    //         this->report_error(right->line, right->col, "bool expected");
    // } else if (left_t == &Char) {
    //     if (right_t != &Char)
    //         this->report_error(right->line, right->col, "char expected");
    // } else
    //     if (!((left_t == right_t) || 
    //             (right_t == &Empty)))
    //         this->report_error(right->line, right->col, left_t->to_string() + " or null expected");
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
        auto it = find(this->p->globals.begin(), this->p->globals.end(), var->name);
        if (this->current && (it != this->p->globals.end())) {
            this->current->globals_called.insert(var->name);
        }
        return result->type;
}

Type *Checker::check_elem_access_expr(ElemAccessExpr *expr, Block *b)
{
    Type *left_type = this->check_expr(expr->expr.get(), b);
    Type *right_type = this->check_expr(expr->index.get(), b);
    if (left_type == &String)
        return &Char;
    else if (auto arr_type = dynamic_cast<ArrayType *>(left_type)) {
        return arr_type->base;
    } else 
        this->report_error(expr->line, expr->col, "[] only operates on Strings and arrays");
    if (!this->convertible_to_int(right_type))
        this->report_error(expr->index->line, expr->index->col, "[] accepts only int or char");
    return nullptr; // unreachable
}

Type *Checker::check_length_expr(LengthExpr *expr, Block *b)
{
    if (!(this->nullable(this->check_expr(expr->expr.get(), b))))
        this->report_error(expr->expr->line, expr->expr->col, "Length function call is only applicable to strings or arrays");
        
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

Type *Checker::check_new_arr_expr(NewArrExpr *e, Block *b)
{
    if (TypeConverter::get_base_type(e->type)==&Void)
        this->report_error(e->line, e->col, "Can't create an array of void");
    if (!this->convertible_to_int(this->check_expr(e->expr.get(), b)))
        this->report_error(e->expr->line, e->expr->col, "int or char expected");
    return e->type;
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

    if (auto e = dynamic_cast<NewArrExpr *>(expr))
        return this->check_new_arr_expr(e, b);

    if (auto inc = dynamic_cast<IncExpr *>(expr))
        return this->check_inc_expr(inc, b);
    
    if (auto fc = dynamic_cast<FunctionCall *>(expr))
        return this->check_function_call(fc, b);

    throw runtime_error("Unrecognized expression");
}


void Checker::check_declaration(Declaration *dec, Block *b)
{
    if (TypeConverter::get_base_type(dec->type)==&Void)
        this->report_error(dec->line, dec->col, "Can't declare void type variable");
    if (this->look_up(dec->id, b))
        this->report_error(dec->line, dec->col, "variable has already been declared");
    Variable v(dec->id, dec->line, dec->col);
    b->variables.insert({dec->id, dec});
    this->verify_assignment(&v, dec->expr.get(), b);
}


bool Checker::try_get_id(Expr *e, Id *id)
{
    if (auto var = dynamic_cast<Variable *>(e)) {
        *id = var->name;
        return true;
    } else if (auto expr = dynamic_cast<ElemAccessExpr *>(e)) {
        return try_get_id(expr->expr.get(), id);
    } else
        return false;
}

void Checker::check_assignment(Assignment *asgn, Block *b)
{
    Id id;
    if (!this->try_get_id(asgn->id.get(), &id))
        this->report_error(asgn->line, asgn->col, "variable expected");
    this->verify_assignment(asgn->id.get(), asgn->expr.get(), b);
    if (auto ea = dynamic_cast<ElemAccessExpr*>(asgn->id.get()))
        if (ea->expr->type == &String)
            this->report_error(ea->line, ea->col, "Strings are immutable");
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
    this->check_statement(for_s->iter.get(), b, false);
    

    // Checking body
    this->check_statement(for_s->body.get(), b, true);

    // Removing id after for statement is over
    b->variables.erase(for_s->init->id);
}

void Checker::check_expression_statement(ExpressionStatement *s, Block *b) 
{
    this->check_expr(s->expr.get(), b);
}


Type *Checker::check_inc_expr(IncExpr *expr, Block *b)
{
    if (!(dynamic_cast<Variable *>(expr->expr.get()) ||
            dynamic_cast<ElemAccessExpr *>(expr->expr.get())))
        this->report_error(expr->line, expr->col, "you can increment a variable or an array element");
    if (this->check_expr(expr->expr.get(), b) != &Int)
        this->report_error(expr->line, expr->col, "must increment int");
    return &Int;
}

void Checker::check_statement(Statement *s, Block *b, bool in_loop)
{
    if (auto dec = dynamic_cast<Declaration *>(s)) {
        this->check_declaration(dec, b);
    }else if (auto asgn = dynamic_cast<Assignment *>(s)) {
        this->check_assignment(asgn, b);
    } else if (auto st = dynamic_cast<IfStatement *>(s)) {
        this->check_if_statement(st, b, in_loop);
    } else if (auto st = dynamic_cast<WhileStatement *>(s)) {
        this->check_while_statement(st, b); 
    } else if (auto for_s = dynamic_cast<ForStatement *>(s)) {
        this->check_for_statement(for_s, b);
    } else if (auto br = dynamic_cast<BreakStatement *>(s)) { 
        if (!in_loop)
            this->report_error(br->line, br->col, "break outside of loop");
    } else if (auto es = dynamic_cast<ExpressionStatement *>(s)) {
        this->check_expression_statement(es, b);
    } else if (auto rs = dynamic_cast<ReturnStatement *>(s)) {
        this->check_return_statement(rs, b);
    } else {
        Block *b1 = dynamic_cast<Block *>(s);
        if (b1) {
            b1->parent = b;
            b->children.push_back(b1);
            this->check_block(b1, in_loop);
        } else {
            cout << s->to_string() << endl;
            throw runtime_error("Unknown statement type");
        }
    }

}


Type *Checker::check_function_call(FunctionCall *fc, Block *b) {
    vector<pair<Type*, vector<Type*>>> overloads = this->functions[fc->name];
    vector<Type*> types;
    for (auto &a : fc->args)
        types.push_back(this->check_expr(a.get(), b));
    for (auto o : overloads) {
        if (o.second == types)
            return o.first;
    }

    this->report_error(fc->line, fc->col, "No function mathces these arguments");
    return nullptr;
}

void Checker::check_block(Block *b, bool in_loop)
{

    for (auto &s : b->statements) {
        this->check_statement(s.get(), b, in_loop);
    }
}

void Checker::check_program(Program *p)
{
    this->p = p;
    this->check_outer_block(p->block.get());
}


void Checker::check_outer_block(Block *b) {
    this->populate_functions(b);
    for (auto &s : b->statements) {
        if (auto ed = dynamic_cast<ExternalDefinition*>(s.get()))
            this->check_external_definition(ed, b);
        else
            this->report_error(s->line, s->col, "External definition expected");
    }
}


void Checker::check_external_definition(ExternalDefinition *s, Block *b) {
    if (auto dec = dynamic_cast<Declaration*>(s->s.get()))
        this->check_external_declaration(dec, b);
    else if (auto fd = dynamic_cast<FunctionDefinition*>(s->s.get()))
        this->check_function_definition(fd, b);
}

void Checker::populate_functions(Block *b) {
    this->functions["print"].push_back({&Void, vector<Type*>(1, &Char)});
    this->functions["print"].push_back({&Void, vector<Type*>(1, &Int)});
    this->functions["print"].push_back({&Void, vector<Type*>(1, &String)});
    for (auto &s : b->statements) {
        if (auto ed = dynamic_cast<ExternalDefinition*>(s.get())) {
            if (auto fd = dynamic_cast<FunctionDefinition*>(ed->s.get())) {
                vector<Type*> types;
                for (auto &p : fd->params)
                    types.push_back(p.first);
                pair<Type*, vector<Type*>> overload = make_pair(fd->ret_type, types);
                auto tup = make_tuple(fd->ret_type, fd->name, types);
                this->p->overloads[tup] = this->func_id++;
                if (fd->name=="main" && this->functions[fd->name].size()!=0)
                    this->report_error(fd->line, fd->col, "Main has already been defined");
                for (auto over : this->functions[fd->name]) {
                    if (over.second == overload.second)
                        this->report_error(fd->line, fd->col, "Function with the same parameters has already been defined");
                }
                this->functions[fd->name].push_back(overload);
            }
        } else
            this->report_error(s->line, s->col, "External definition expected");
    }
}


void Checker::check_function_definition(FunctionDefinition* fd, Block *b) {
    this->current = fd;
    for (auto &p : fd->params) {
        if (p.first==&Void)
            this->report_error(fd->line, fd->col, "Parameter can't be void");
        Declaration *dec = new Declaration(p.first, p.second, nullptr, 0, 0);
        fd->body->variables.insert({dec->id, dec});
    }
    fd->body->parent = b;
    b->children.push_back(fd->body->parent);
    this->check_block(fd->body.get(), false);
    this->current = nullptr;
}

void Checker::check_return_statement(ReturnStatement *rs, Block *b) {
    if (rs->expr) {
        if (current->ret_type == &Void)
            this->report_error(rs->line, rs->col, "A return keyword must not be followed by any expression when method returns void");
        Type *left_t = current->ret_type;
        Type *right_t = this->check_expr(rs->expr.get(), b);
        if (left_t == &Int) {
            if (!this->convertible_to_int(right_t))
                this->report_error(rs->line, rs->col, "int or char expected");
        } else if (left_t == &Bool) {
            if (right_t != &Bool)
                this->report_error(rs->line, rs->col, "bool expected");
        } else if (left_t == &Char) {
            if (right_t != &Char)
                this->report_error(rs->line, rs->col, "char expected");
        } else
            if (!((left_t == right_t) || 
                    (right_t == &Empty)))
                this->report_error(rs->line, rs->col, left_t->to_string() + " or null expected");
    }

    
}

bool Checker::return_inside(Statement *s) {
    if (!s)
        return true;
    if (auto st = dynamic_cast<IfStatement *>(s)) {
        return this->return_inside(st->if_s.get()) && this->return_inside(st->else_s.get());
    } else if (auto st = dynamic_cast<WhileStatement *>(s)) { 
        return this->return_inside(st->statement.get());
    } else if (auto for_s = dynamic_cast<ForStatement *>(s)) {
        return this->return_inside(for_s->body.get());
    } else if (dynamic_cast<ReturnStatement *>(s)) {
        return true;
    } else if (auto b = dynamic_cast<Block *>(s)){
        for (auto &st : b->statements)
            if (this->return_inside(st.get()))
                return true;
    }
    return false;
}

bool Checker::function_inside(Expr *expr) {
    if (auto e = dynamic_cast<OpExpr *>(expr))
        return function_inside(e->right.get()) || function_inside(e->left.get());
    
    if (auto e = dynamic_cast<ElemAccessExpr *>(expr))
        return function_inside(e->expr.get()) || function_inside(e->index.get());

    if (auto e = dynamic_cast<LengthExpr *>(expr))
        return function_inside(e->expr.get());
    
    if (auto e = dynamic_cast<TypeCastExpr *>(expr))
        return function_inside(e->expr.get());

    if (auto e = dynamic_cast<SubstrExpr *>(expr)) {
        if (function_inside(e->expr.get()))
            return true;
        for (auto &ex : e->arguments)
            if (function_inside(ex.get()))
                return true;
        return false;
    }

    if (auto e = dynamic_cast<IntParseExpr *>(expr)) {
        for (auto &ex : e->arguments)
            if (function_inside(ex.get()))
                return true;
    }

    if (auto e = dynamic_cast<NewStrExpr *>(expr)) {
        for (auto &ex : e->arguments)
            if (function_inside(ex.get()))
                return true;
    }

    if (auto e = dynamic_cast<NewArrExpr *>(expr))
        return function_inside(e->expr.get());

    if (auto inc = dynamic_cast<IncExpr *>(expr))
        return function_inside(inc->expr.get());
    
    if (dynamic_cast<FunctionCall *>(expr))
        return true;
    
    return false;

}

void Checker::check_external_declaration(Declaration *dec, Block *b) {
    if (TypeConverter::get_base_type(dec->type)==&Void)
        this->report_error(dec->line, dec->col, "Can't declare void type variable");
    if (this->look_up(dec->id, b))
        this->report_error(dec->line, dec->col, "variable has already been declared");
    if (this->function_inside(dec->expr.get()))
        this->report_error(dec->line, dec->col, "Can't use functions to declare global variables");
    Variable v(dec->id, dec->line, dec->col);
    this->p->globals.push_back(dec->id);
    b->variables.insert({dec->id, dec});
    this->verify_assignment(&v, dec->expr.get(), b);
}
