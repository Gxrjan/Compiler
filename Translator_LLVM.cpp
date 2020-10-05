#include "head.h"


void Translator_LLVM::report_error(int line, int col, string message)
{
    throw runtime_error("Translator_LLVM error line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

string Translator_LLVM::concat_cc(Type *left, Type *right)
{
    if (left == &String) {
        if (right == &String)
            return "";
        else if (right == &Int)
            return "_str_int";
        else
            return "_str_chr";
    } else if (left == &Int)
        return "_int_str";
     else
        return "_chr_str";
}

string Translator_LLVM::operation_to_cc(Operation op)
{
    switch (op) {
        case Operation::L:
            return "l";
            break;
        case Operation::G:
            return "g";
            break;
        case Operation::Le:
            return "le";
            break;
        case Operation::Ge:
            return "ge";
            break;
        case Operation::E:
            return "e";
            break;
        case Operation::Ne:
            return "ne";
            break;
        default:
            throw runtime_error("Uknown comparing operation");
            break;
    }
}

string Translator_LLVM::type_to_cc(Type *t)
{
    if (t == &Char)
        return "c";
    return "i";
}

string Translator_LLVM::translate_num_literal(string *s, NumLiteral *l)
{
    string register_id = std::to_string(this->register_id++);
    *s +=
        " %r"+register_id+" = add i32 "+std::to_string(l->num)+", 0\n";
    return register_id;
}


string Translator_LLVM::translate_bool_literal(string *s, BoolLiteral *l)
{
    string register_id = std::to_string(this->register_id++);
    if (l->b)
        *s +=
            " %r"+register_id+" = add i1 1, 0";
    else
        *s +=
            " %r"+register_id+" = add i1 0, 0";
    return register_id;
}

string Translator_LLVM::translate_variable(string *s, Variable *v) 
{
    return this->variables.at(v->name);
}

string Translator_LLVM::translate_char_literal(string *s, CharLiteral *l)
{
    string register_id = std::to_string(this->register_id++);
    *s +=
        " %r"+register_id+" = add i16 " + std::to_string((int)l->c) + ", 0\n";
    return register_id;
}

string Translator_LLVM::translate_string_literal(string *s, StringLiteral *l)
{
    int id = this->string_id++;
    string register_id = std::to_string(this->register_id++);
    string len_str = std::to_string(l->s.length());
    *s +=
        "%r"+register_id+" = getelementptr <{ i32, ["+len_str+" x i16]}>, "
        "<{ i32, ["+len_str+" x i16]}>* @str_"+std::to_string(id)+", i32 0, i32 1, i32 0\n";
    this->strings.insert({l, id});
    return register_id;
}

string Translator_LLVM::translate_elem_access_expr(string *s, ElemAccessExpr *e)
{
    string expr_id = this->translate_expr(s, e->expr.get());
    string index_id = this->translate_expr(s, e->index.get());
    if (e->expr->type == &String)
        *s +=
            " call  get\n";
    else {
        auto arr_t = dynamic_cast<ArrayType *>(e->expr->type);
        if (arr_t->base == &Char)
            *s +=
                " call get\n";
        else if (arr_t->base == &Bool)
            *s +=
                " call getb\n";
        else
            *s +=
                " call  getll\n";
    }

    *s +=
        " push  rax\n";
    return "";
}

string Translator_LLVM::translate_length_expr(string *s, LengthExpr *e)
{
    this->translate_expr(s, e->expr.get());
        *s +=
            " pop   rdi\n";

    if (e->type == &String)
        *s +=
            " call gstring_len\n";
    else
        *s +=
            " call arr_len\n";
    *s +=
        " push  rax\n";
    return "";
}

string Translator_LLVM::translate_type_cast_expr(string *s, TypeCastExpr *e)
{
    this->translate_expr(s, e->expr.get());
    if (e->type == &Char) {
        *s += 
            " pop       rcx\n"
            " mov       rax, 0\n"
            " mov       ax, cx\n"
            " push      rax\n";
    }
    return "";
}

string Translator_LLVM::translate_substr_expr(string *s, SubstrExpr *e)
{
    this->translate_expr(s, e->expr.get());
    if (e->arguments.size() == 1) {
        this->translate_expr(s, e->arguments[0].get());
        *s +=
            " pop       rsi\n"
            " pop       rdi\n"
            " call      substr_int\n"
            " push      rax\n";
    } else {
        this->translate_expr(s, e->arguments[0].get());
        this->translate_expr(s, e->arguments[1].get());
        *s +=
            " pop       rdx\n"
            " pop       rsi\n"
            " pop       rdi\n"
            " call      substr_int_int\n"
            " push      rax\n";
        
    }
    return "";
}

string Translator_LLVM::translate_int_parse_expr(string *s, IntParseExpr *e)
{
    this->translate_expr(s, e->arguments[0].get());
    *s +=
        " pop   rdi\n"
        " call  int_parse\n"
        " push  rax\n";
    return "";
}

string Translator_LLVM::translate_new_str_expr(string *s, NewStrExpr *e)
{
    
    this->translate_expr(s, e->arguments[0].get());
    this->translate_expr(s, e->arguments[1].get());
    *s +=
        " pop   rsi\n"
        " pop   rdi\n"
        " call  new_str_expr\n"
        " push   rax\n";
    return "";
}

string Translator_LLVM::translate_new_arr_expr(string *s, NewArrExpr *e)
{
    this->translate_expr(s, e->expr.get());
    *s +=
        " pop   rsi\n";
    if (e->type == &Bool)
        *s +=
            " mov      rdi, 1\n";
    else if (e->type == &Char) 
        *s +=
            " mov      rdi, 2\n";
    else
        *s +=
            " mov      rdi, 8\n";
    *s +=
        " call new_arr_expr\n"
        " push  rax\n";
    return "";
}

string Translator_LLVM::translate_op_expr(string *s, OpExpr *expr) 
{
    if (expr->op == "&&" || expr->op == "||") {
        Operation o;
        string label_id = std::to_string(this->label_id++);
        string register_id_left;
        string register_id_right;
        string register_id_result;
        string register_id_temp_left = std::to_string(this->register_id++);
        string register_id_temp_right = std::to_string(this->register_id++);
        *s +=
            " br label %start"+label_id+"\n"
            "start"+label_id+":\n";
        switch((o = TypeConverter::string_to_operation(expr->op))) {
            case Operation::And:
                *s += // asm comment 
                    "; " + expr->left->to_string() + "\n";
                register_id_left = this->translate_expr(s, expr->left.get());
                *s +=
                    " %r"+register_id_temp_left+" = icmp eq i1 %r"+register_id_left+", 1\n"
                    " br i1 %r"+register_id_temp_left+", label %continue"+label_id+", label %end"+label_id+"\n"
                    "continue"+label_id+":\n";
                *s += // asm comment 
                    "; " + expr->right->to_string() + "\n";
                register_id_right = this->translate_expr(s, expr->right.get());
                *s +=
                    " %r"+register_id_temp_right+" = icmp eq i1 %r"+register_id_right+", 1\n"
                    " br label %end\n";
                break;
            case Operation::Or:
                *s += // asm comment 
                    "; " + expr->left->to_string() + "\n";
                register_id_left = this->translate_expr(s, expr->left.get());
                *s +=
                    " %r"+register_id_temp_left+" = icmp eq i1 %r"+register_id_left+", 1\n"
                    " br i1 %r"+register_id_temp_left+", label %end"+label_id+", label %continue"+label_id+"\n"
                    "continue"+label_id+":\n";
                *s += // asm comment 
                    "; " + expr->right->to_string() + "\n";
                register_id_right = this->translate_expr(s, expr->right.get());
                *s +=
                    " %r"+register_id_temp_right+" = icmp eq i1 %r"+register_id_right+", 1\n"
                    " br label %end\n";
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
            
        }
        *s +=
            "end"+label_id+":\n"
            " %r"+register_id_result+" = phi i1 [ %r"+register_id_temp_left+", %start"+label_id+"]"
            ", [ %r"+register_id_temp_right+", %continue"+label_id+"]\n";

    } else {
        string register_id = std::to_string(this->register_id++);
        *s += // asm comment 
            "; " + expr->left->to_string() + "\n";
        string register_id_left = this->translate_expr(s, expr->left.get());
        *s += // asm comment 
            "; " + expr->right->to_string() + "\n";
        string register_id_right = this->translate_expr(s, expr->right.get());
        Operation o;
        switch((o = TypeConverter::string_to_operation(expr->op))) {
            case Operation::Add:
                if (expr->type == &Int) {
                    *s +=
                        " %r"+register_id+" = add i32 %r"+register_id_left+", %r"+register_id_right+"\n";
                } else {
                    //TODO
                }
                break;
            case Operation::Sub:
                *s +=
                        " %r"+register_id+" = sub i32 %r"+register_id_left+", %r"+register_id_right+"\n";
                break;
            case Operation::Mul:
                *s +=
                        " %r"+register_id+" = mul i32 %r"+register_id_left+", %r"+register_id_right+"\n";
                break;
            case Operation::Div:
                *s +=
                        " %r"+register_id+" = sdiv i32 %r"+register_id_left+", %r"+register_id_right+"\n";
                break;
            case Operation::Mod:
                *s +=
                        " %r"+register_id+" = srem i32 %r"+register_id_left+", %r"+register_id_right+"\n";
                break;
            case Operation::L:
            case Operation::G:
            case Operation::Le:
            case Operation::Ge:
                //TODO
                break;
            case Operation::E:
            case Operation::Ne:
                if (expr->left->type == &String && expr->right->type == &String) {
                    *s +=
                        " pop   rdi\n"
                        " pop   rsi\n"
                        " call  cmp_str\n";
                    if (o==Operation::Ne)
                        *s +=
                            " xor   rax, 1\n";
                    *s +=
                        " push  rax\n";
                } else {
                    *s +=
                        " pop       rax\n"
                        " mov       rcx, 0\n"
                        " cmp       qword [rsp], rax\n"
                        " set"+this->operation_to_cc(o) +"     cl\n"
                        " mov       [rsp], rcx\n";
                }
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
        }
        return register_id;

    }
    return "";
} 

string Translator_LLVM::translate_expr(string *s, Expr *e)
{
    if (auto expr = dynamic_cast<NumLiteral *>(e)) {
        return this->translate_num_literal(s, expr);
    } else if (auto expr = dynamic_cast<BoolLiteral *>(e)) {
        return this->translate_bool_literal(s, expr);
    } else if (auto var = dynamic_cast<Variable *>(e)) {
        return this->translate_variable(s, var);
    } else if (auto expr = dynamic_cast<OpExpr *>(e)) {
        return this->translate_op_expr(s, expr);
    } else if (auto expr = dynamic_cast<CharLiteral *>(e)) {
        return this->translate_char_literal(s, expr);
    } else if (auto expr = dynamic_cast<StringLiteral *>(e)) {
        return this->translate_string_literal(s, expr);
    } else if (auto expr = dynamic_cast<ElemAccessExpr *>(e)) {
        return this->translate_elem_access_expr(s, expr);
    } else if (auto expr = dynamic_cast<LengthExpr *>(e)) {
        return this->translate_length_expr(s, expr);
    } else if (auto expr = dynamic_cast<TypeCastExpr *>(e)) {
        return this->translate_type_cast_expr(s, expr);
    } else if (auto expr = dynamic_cast<SubstrExpr *>(e)) {
        return this->translate_substr_expr(s, expr);
    } else if (auto expr = dynamic_cast<IntParseExpr *>(e)) {
        return this->translate_int_parse_expr(s, expr);
    } else if (auto expr = dynamic_cast<NewStrExpr *>(e)) {
        return this->translate_new_str_expr(s, expr);
    } else if (dynamic_cast<NullExpr *>(e)) {
        string register_id = std::to_string(this->register_id++);
        *s += 
            " %r"+register_id+" = add i64 0, 0\n";
        return register_id;
    } else if (auto expr = dynamic_cast<NewArrExpr *>(e)) {
        return this->translate_new_arr_expr(s, expr);
    } else if (auto expr = dynamic_cast<IncExpr *>(e)) {
        return this->translate_inc_expr(s, expr);
    } else
        throw runtime_error("Unknown type of expression");
    return "";
}


void Translator_LLVM::translate_declaration(string *s, Declaration *dec)
{
    *s += // asm comment
        "; " + dec->to_string() + "\n";
    string register_id = this->translate_expr(s, dec->expr.get());
    this->variables.insert({dec->id, register_id});
}

void Translator_LLVM::translate_assignment(string *s, Assignment *asgn)
{
    *s += // asm comment
        "; " + asgn->to_string() + "\n";
    if (auto var = dynamic_cast<Variable *>(asgn->id.get())) {
        string register_id = this->translate_expr(s, asgn->expr.get());
        this->variables[var->name] = register_id;
    } else {
        auto el = dynamic_cast<ElemAccessExpr *>(asgn->id.get());
        this->translate_expr(s, el->expr.get());
        this->translate_expr(s, el->index.get());
        this->translate_expr(s, asgn->expr.get());
        auto arr_t = dynamic_cast<ArrayType *>(el->expr->type);
        *s +=
            " pop       rdx\n"
           " pop       rsi\n"
           " pop       rdi\n";
        if (arr_t->base == &Bool)
            *s +=
                " call  setb\n";
        else if (arr_t->base == &Char)
            *s +=
                " call set\n";
        else
            *s +=
                " call setll\n";
    }
}

void Translator_LLVM::translate_print(string *s, Print *p)
{
    *s += // asm comment 
        "; " + p->to_string() + "\n";
    string register_id = this->translate_expr(s, p->expr.get());
    if (p->expr->type == &String) {
        *s +=
            "call void (i16*) @printg(i16* %r"+register_id+")\n";
        return;
    } else if (p->expr->type == &Int ){
        string format_id = std::to_string(this->register_id++);
        *s +=
            "%r"+format_id+" = getelementptr [4 x i8], [4 x i8]* @fmt_i"
            ", i32 0, i32 0\n"
            "call i32 (i8*, ...) @printf(i8* %r"+format_id+", i32 %r"+register_id+")\n";
    } else if (p->expr->type == &Char){
        string format_id = std::to_string(this->register_id++);
        *s +=
            "%r"+format_id+" = getelementptr [4 x i8], [4 x i8]* @fmt_c"
            ", i32 0, i32 0\n"
            "call i32 (i8*, ...) @printf(i8* %r"+format_id+", i16 %r"+register_id+")\n";
    } else {
        string format_id = std::to_string(this->register_id++);
        *s +=
            "%r"+format_id+" = getelementptr [4 x i8], [4 x i8]* @fmt_i"
            ", i32 0, i32 0\n"
            "call i32 (i8*, ...) @printf(i8* %r"+format_id+", i1 %r"+register_id+")\n";
    }
}

void Translator_LLVM::translate_if_statement(string *s, IfStatement *st, string loop_end_label)
{
    string label_id = std::to_string(this->label_id++);
    *s += // asm comment
        "; if " + st->cond->to_string() + "\n";

    this->translate_expr(s, st->cond.get());
    *s +=
        " pop       rax\n"
        " cmp       rax, 0\n"
        " je        else_s"+label_id+"\n";
    this->translate_statement(s, st->if_s.get(), loop_end_label);
    if (!st->else_s) {
        *s +=
            "else_s"+label_id+":\n";
    } else {
        *s +=
            " jmp   end_s"+label_id+"\n"
            "else_s"+label_id+":\n";
        this->translate_statement(s, st->else_s.get(), loop_end_label);
        *s +=
            "end_s"+label_id+":\n";
    }
}

void Translator_LLVM::translate_while_statement(string *s, WhileStatement *st)
{
    string label_id = std::to_string(this->label_id++);
    *s += 
        "loop" + label_id + ":\n";
    *s += // asm comment
        "; while " + st->cond->to_string() + "\n";
    this->translate_expr(s, st->cond.get());
    *s +=
        " pop       rax\n"
        " cmp       rax, 0\n"
        " je        loop_end"+label_id+"\n";
    this->translate_statement(s, st->statement.get(), label_id);
    *s +=
        " jmp       loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
}

void Translator_LLVM::translate_for_statement(string *s, ForStatement *for_s)
{
    string label_id = std::to_string(this->label_id++);

    this->translate_declaration(s, for_s->init.get());

    *s +=
        "loop" + label_id + ":\n";
    *s +=
        "; for " + for_s->cond->to_string() + "\n";

    this->translate_expr(s, for_s->cond.get());
    *s +=
        " pop       rax\n"
        " cmp       rax, 0\n"
        " je        loop_end"+label_id+"\n";
    this->translate_statement(s, for_s->body.get(), label_id);
    if (auto es = dynamic_cast<ExpressionStatement *>(for_s->iter.get()))
        this->translate_expression_statement(s, es);
    else if (auto asgn = dynamic_cast<Assignment *>(for_s->iter.get()))
        this->translate_assignment(s, asgn);
    *s +=
        " jmp       loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
    
}

string Translator_LLVM::bool_to_op(bool inc)
{
    return (inc ? "inc" : "dec");
}

string Translator_LLVM::translate_inc_expr(string *s, IncExpr *expr)
{
    // TODO
    if (auto var = dynamic_cast<Variable *>(expr->expr.get())) {
        this->translate_variable(s, var);
        *s +=
            " "+this->bool_to_op(expr->inc)+"   qword ["+var->name+"]\n";
    } else {
        auto el = dynamic_cast<ElemAccessExpr *>(expr->expr.get());
        this->translate_expr(s, el->expr.get());
        this->translate_expr(s, el->index.get());
        *s +=
            " mov   rsi, qword [rsp]\n"
            " mov   rdi, qword [rsp+8]\n"
            " call  getll\n"
            " pop   rsi\n"
            " pop   rdi\n"
            " mov   rdx, rax\n"
            " push  rax\n"
            " "+this->bool_to_op(expr->inc)+"   rdx\n"
            " call  setll\n";
    }
    return "";
}


void Translator_LLVM::translate_expression_statement(string *s, ExpressionStatement *expr)
{
    this->translate_inc_expr(s, dynamic_cast<IncExpr *>(expr->expr.get()));
}

void Translator_LLVM::translate_statement(string *s, Statement *statement, string loop_end_label)
{
    if (auto dec = dynamic_cast<Declaration *>(statement)) {
        this->translate_declaration(s, dec);
    } else if (auto asgn = dynamic_cast<Assignment *>(statement)) {
        this->translate_assignment(s, asgn);
    } else if (auto p = dynamic_cast<Print *>(statement)) {
        this->translate_print(s, p);
    } else if (auto b = dynamic_cast<Block *>(statement)) {
        this->translate_block(s, b, loop_end_label);
    } else if (auto st = dynamic_cast<IfStatement *>(statement)) {
        this->translate_if_statement(s, st, loop_end_label);
    } else if (auto st = dynamic_cast<WhileStatement *>(statement)) {
        this->translate_while_statement(s, st);
    } else if (auto for_s = dynamic_cast<ForStatement *>(statement)) {
        this->translate_for_statement(s, for_s);
    } else if (dynamic_cast<BreakStatement *>(statement)) {
        // Translation
        *s += // asm comment
            ";break;\n";
        *s +=
            " jmp       loop_end"+loop_end_label+"\n";
    } else if (auto es = dynamic_cast<ExpressionStatement *>(statement)) {
        this->translate_expression_statement(s, es);
    } else{
        throw runtime_error("Unknown statement");
    }
} 

void Translator_LLVM::translate_block(string *s, Block *b, string loop_end_label)
{
    for (auto &statement : b->statements)
        this->translate_statement(s, statement.get(), loop_end_label);
}

string Translator_LLVM::translate_program(Program* prog)
{
    string result = "";
    result += 
            "@fmt_i = constant [4 x i8] c\"%d\\0A\\00\"\n"
            "@fmt_c = constant [4 x i8] c\"%c\\0A\\00\"\n"
            "declare i32 @printf(i8*, ...)\n"
            "declare void @printg(i16*)\n"
            "declare i32 @get(i16*, i32)\n"
            "declare i8 @getb(i8*, i32)\n"
            "declare i32 @getll(i32*, i32)\n"
            "extern void @set(i16*, i32, i16)\n"
            "extern void @setb(i8*, i32, i8)\n"
            "extern void @setll(i32*, i32, i32)\n"
            "extern i16* @concat(i16*, i16*)\n"
            "extern i32 @cmp_str(i16*, i16*)\n"
            "extern i16* @concat_str_int(i16*, i32)\n"
            "extern i16* @concat_int_str(i32, i16*)\n"
            "extern i16* @concat_str_chr(i16*, i16)\n"
            "extern i16* @concat_chr_str(i16, i16*)\n"
            "extern i16* @substr_int(i16*, i32)\n"
            "extern i16* @substr_int_int(i16*, i32, i32)\n"
            "extern i32 @int_parse(i16*)\n"
            "extern i16* @new_str_expr(i16, i32)\n"
            "extern i32* @new_int_arr_expr(i32)\n"
            "extern i32* @new_char_arr_expr(i32)\n"
            "extern i32* @new_bool_arr_expr(i32)\n"
            "extern i16** @new_string_arr_expr(i32)\n"
            "extern i32 @gstring_len(i16*)\n"
            "extern i32 @arr_len(i8*)\n"
            "define i32 @main() {\n";

    this->translate_block(&result, prog->block.get(), "");

    result += 
        "ret i32 0\n"
        "}\n";
    for (auto &p : this->strings) {
        string str = p.first->s;
        int length = str.length();
        string len_str = std::to_string(length);
        int id = p.second;
        result +=
            "@str_"+std::to_string(id)+" = constant"
            "<{ i32, [ "+len_str+" x i16 ]}>"
            "<{ i32 "+len_str+", [ "+len_str+" x i16] [";
        for (int i=0;i<length;i++)
            result += (i==length-1) ? "i16 "+std::to_string((int)str[i])+"] }>\n" :
                                        "i16 "+std::to_string((int)str[i])+",";
    }
    return result;
}
