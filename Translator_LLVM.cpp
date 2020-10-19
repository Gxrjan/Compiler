#include "head.h"

string Translator_LLVM::assign_register()
{
    return "%r"+std::to_string(this->register_id++);
}

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
            return "slt";
            break;
        case Operation::G:
            return "sgt";
            break;
        case Operation::Le:
            return "sle";
            break;
        case Operation::Ge:
            return "sge";
            break;
        case Operation::E:
            return "eq";
            break;
        case Operation::Ne:
            return "ne";
            break;
        default:
            throw runtime_error("Unknown comparing operation");
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
    string result_register = this->assign_register();
    *s +=
        " "+result_register+" = add i32 "+std::to_string(l->num)+", 0\n";
    return result_register;
}


string Translator_LLVM::translate_bool_literal(string *s, BoolLiteral *l)
{
    string result_register = this->assign_register();
    if (l->b)
        *s +=
            " "+result_register+" = add i1 1, 0\n";
    else
        *s +=
            " "+result_register+" = add i1 0, 0\n";
    return result_register;
}

string Translator_LLVM::translate_variable(string *s, Variable *v) 
{
    string result_register = this->assign_register();
    string var_storage = this->variables.at(v->name).first;
    string var_type = this->variables.at(v->name).second;
    *s += 
        " "+result_register+" = load "+var_type+", "+var_type+"* "+var_storage+"\n";
    return result_register;
}

string Translator_LLVM::translate_char_literal(string *s, CharLiteral *l)
{
    string result_register = this->assign_register();
    *s +=
        " "+result_register+" = add i16 " + std::to_string((int)l->c) + ", 0\n";
    return result_register;
}

string Translator_LLVM::translate_string_literal(string *s, StringLiteral *l)
{
    int id = this->string_id++;
    string result_register = this->assign_register();
    string len_str = std::to_string(l->s.length());
    *s +=
        ""+result_register+" = getelementptr <{ i32, ["+len_str+" x i16]}>, "
        "<{ i32, ["+len_str+" x i16]}>* @str_"+std::to_string(id)+", i32 0, i32 1, i32 0\n";
    this->strings.insert({l, id});
    return result_register;
}


string Translator_LLVM::translate_elem_access_expr(string *s, ElemAccessExpr *e)
{
    string expr_register = this->translate_expr(s, e->expr.get());
    string index_register = this->translate_expr(s, e->index.get());
    string result_register = this->assign_register();
    string temp_register = this->assign_register();
    if (e->expr->type == &String)
        *s +=
            " "+result_register+" = call i16 @get16(i16* "+expr_register+", i32 "+index_register+")\n";
    else {
        auto arr_t = dynamic_cast<ArrayType *>(e->expr->type);
        string reg_type = this->type_to_llvm_type(arr_t);
        *s +=
            " \n";
        if (arr_t->base == &Char)
            *s +=
                " "+temp_register+" = bitcast "+reg_type+" "+expr_register+" to i16*\n"
                " "+result_register+" = call i16 @get16(i16* "+temp_register+", i32 "+index_register+")\n";
        else if (arr_t->base == &Bool) {
            string bool_register = this->assign_register();
            *s +=
                " "+temp_register+" = bitcast "+reg_type+" "+expr_register+" to i8*\n"
                " "+result_register+" = call i1 @get8(i8* "+temp_register+", i32 "+index_register+")\n";
        } else if (arr_t->base == &Int)
            *s +=
                " "+temp_register+" = bitcast "+reg_type+" "+expr_register+" to i32*\n"
                " "+result_register+" = call i32 @get32(i32* "+temp_register+", i32 "+index_register+")\n";
        else if (arr_t->base == &String) {
            string str_register = this->assign_register();
            *s +=
                " "+temp_register+" = bitcast "+reg_type+" "+expr_register+" to i8**\n"
                " "+str_register+" = call i8* @get64(i8** "+temp_register+", i32 "+index_register+")\n"
                " "+result_register+" = bitcast i8* "+str_register+" to i16*\n";
        } else {
            string result_reg_type = this->type_to_llvm_type(arr_t->base);
            string result_temp_register = this->assign_register();
            *s +=
                " "+temp_register+" = bitcast "+reg_type+" "+expr_register+" to i8**\n"
                " "+result_temp_register+" = call i8* @get64(i8** "+temp_register+", i32 "+index_register+")\n"
                " "+result_register+" = bitcast i8* "+result_temp_register+" to "+result_reg_type+"\n";
        }
    }
    return result_register;
}


string Translator_LLVM::translate_length_expr(string *s, LengthExpr *e)
{
    string expr_register = this->translate_expr(s, e->expr.get());
    string result_register = this->assign_register();
    if (e->expr->type == &String) {
        *s +=
            " "+result_register+" = call i32 @gstring_len(i16* "+expr_register+")\n";
        return result_register;
    } else {
        string temp_register = this->assign_register();
        string var_type = this->type_to_llvm_type(e->expr->type);
        *s +=
            " "+temp_register+" = bitcast "+var_type+" "+expr_register+" to i8*\n"
            " "+result_register+" = call i32 @arr_len(i8* "+temp_register+")\n";
        return result_register;
    }
}


string Translator_LLVM::translate_type_cast_expr(string *s, TypeCastExpr *e)
{
    string expr_register = this->translate_expr(s, e->expr.get());
    string result_register = this->assign_register();
    if (e->expr->type == &Int) {
        if (e->type == &Char) {
            *s += 
                " "+result_register+" = trunc i32 "+expr_register+" to i16\n";
        } else
            return expr_register;
    } else {
        if (e->type == &Int) {
            *s +=
                " "+result_register+" = zext i16 "+expr_register+" to i32\n";
        } else
            return expr_register;
    }
    return result_register;
}



string Translator_LLVM::translate_substr_expr(string *s, SubstrExpr *e)
{
    string str_register = this->translate_expr(s, e->expr.get());
    string result_register = this->assign_register();
    if (e->arguments.size() == 1) {
        string from_register = this->assign_register();
        if (e->arguments[0]->type == &Char) {
            string temp_from_register = this->translate_expr(s, e->arguments[0].get());
            *s +=
                " "+from_register+" = zext i16 "+temp_from_register+" to i32\n";
        } else {
            from_register = this->translate_expr(s, e->arguments[0].get());
        }
        *s +=
            " "+result_register+" = call i16* @substr_int(i16* "+str_register+",i32 "+from_register+")\n";
    } else {

        string from_register = this->assign_register();
        string len_register = this->assign_register();
        if (e->arguments[0]->type == &Char) {
            string temp_from_register = this->translate_expr(s, e->arguments[0].get());
            *s +=
                " "+from_register+" = zext i16 "+temp_from_register+" to i32\n";
        } else {
            from_register = this->translate_expr(s, e->arguments[0].get());
        }

        if (e->arguments[1]->type == &Char) {
            string temp_len_register = this->translate_expr(s, e->arguments[1].get());
            *s +=
                " "+len_register+" = zext i16 "+temp_len_register+" to i32\n";
        } else {
            len_register = this->translate_expr(s, e->arguments[1].get());
        }
        *s +=
            " "+result_register+" = call i16* @substr_int_int(i16* "+str_register+",i32 "+from_register+", i32 "+len_register+")\n";    
    }
    return result_register;
}


string Translator_LLVM::translate_int_parse_expr(string *s, IntParseExpr *e)
{
    string str_register = this->translate_expr(s, e->arguments[0].get());
    string result_register = this->assign_register();
    *s +=
        " "+result_register+" = call i32 @int_parse(i16* "+str_register+")\n";
    return result_register;
}


string Translator_LLVM::translate_new_str_expr(string *s, NewStrExpr *e)
{
    
    string chr_register = this->translate_expr(s, e->arguments[0].get());
    string len_register = this->translate_expr(s, e->arguments[1].get());
    string result_register = this->assign_register();
    *s +=
        " "+result_register+" = call i16* @new_str_expr(i16 "+chr_register+", i32 "+len_register+")\n";
    return result_register;
}

string Translator_LLVM::type_to_size(ArrayType *t)
{
    if (t->base == &Bool) {
            return "1";
    } else if (t->base == &Char) 
        return "2";
    else if (t->base == &Int)
        return "4";
    else
        return "8";
}

string Translator_LLVM::type_to_llvm_type(Type *t)
{
    if (t == &Bool) {
        return "i1";
    } else if (t == &Char) 
        return "i16";
    else if (t == &Int)
        return "i32";
    else if (t == &String)
        return "i16*";
    else {
        auto st = dynamic_cast<ArrayType *>(t);
        return this->type_to_llvm_type(st->base)+"*";
    }
}

string Translator_LLVM::translate_new_arr_expr(string *s, NewArrExpr *e)
{
    string len_register = this->translate_expr(s, e->expr.get());
    string size_register = this->assign_register();
    string result_register = this->assign_register();
    string temp_register = this->assign_register();
    auto t = dynamic_cast<ArrayType *>(e->type);
    *s +=
        " "+size_register+" = add i32 "+this->type_to_size(t)+", 0\n";

    *s +=
        " "+temp_register+" = call i8* @new_arr_expr(i32 "+size_register+", i32 "+len_register+")\n";
    
    *s +=
        " "+result_register+" = bitcast i8* "+temp_register+" to "+type_to_llvm_type(e->type)+"\n";
    return result_register;
}

string Translator_LLVM::translate_bool_expr(string *s, OpExpr *expr)
{
    Operation o = TypeConverter::string_to_operation(expr->op);
    string label_id = std::to_string(this->label_id++);
    string register_left;
    string register_right;
    string register_result = this->assign_register();
    string register_temp_left = this->assign_register();
    string register_temp_right = this->assign_register();
    *s +=
        " br label %start"+label_id+"\n"
        "start"+label_id+":\n";
    switch(o) {
        case Operation::And:
            *s += // asm comment 
                "; " + expr->left->to_string() + "\n";
            register_left = this->translate_expr(s, expr->left.get());
            *s +=
                " "+register_temp_left+" = icmp eq i1 "+register_left+", 1\n"
                " br i1 "+register_temp_left+", label %continue"+label_id+", label %end"+label_id+"\n"
                "continue"+label_id+":\n";
            *s += // asm comment 
                "; " + expr->right->to_string() + "\n";
            register_right = this->translate_expr(s, expr->right.get());
            *s +=
                " "+register_temp_right+" = icmp eq i1 "+register_right+", 1\n"
                " br label %end"+label_id+"\n";
            break;
        case Operation::Or:
            *s += // asm comment 
                "; " + expr->left->to_string() + "\n";
            register_left = this->translate_expr(s, expr->left.get());
            *s +=
                " "+register_temp_left+" = icmp eq i1 "+register_left+", 1\n"
                " br i1 "+register_temp_left+", label %end"+label_id+", label %continue"+label_id+"\n"
                "continue"+label_id+":\n";
            *s += // asm comment 
                "; " + expr->right->to_string() + "\n";
            register_right = this->translate_expr(s, expr->right.get());
            *s +=
                " "+register_temp_right+" = icmp eq i1 "+register_right+", 1\n"
                " br label %end"+label_id+"\n";
            break;
        default:
            throw runtime_error("Unknown operator");
            break;
        
    }
    *s +=
        "end"+label_id+":\n"
        " "+register_result+" = phi i1 [ "+register_temp_left+", %start"+label_id+"]"
        ", [ "+register_temp_right+", %continue"+label_id+"]\n";
    return register_result;
}

string Translator_LLVM::translate_arithm_expr(string *s, OpExpr *expr)
{
    Operation o = TypeConverter::string_to_operation(expr->op);
    *s += // asm comment 
        "; " + expr->left->to_string() + "\n";
    string register_left = this->translate_expr(s, expr->left.get());
    *s += // asm comment 
        "; " + expr->right->to_string() + "\n";
    string register_right = this->translate_expr(s, expr->right.get());
    string result_register = this->assign_register();
    switch(o) {
        case Operation::Add:
            if (expr->type == &Int) {
                *s +=
                    " "+result_register+" = add i32 "+register_left+", "+register_right+"\n";
            } else {
                if (expr->left->type == &String && expr->right->type == &String)
                    *s += 
                        " "+result_register+" = call i16* @concat(i16* "+register_left+"," 
                        "i16* "+register_right+")";
                else if (expr->left->type == &String) {
                    if (expr->right->type == &Int)
                        *s += 
                            " "+result_register+" = call i16* @concat_str_int(i16* "+register_left+"," 
                            "i32 "+register_right+")";
                    else
                        *s += 
                            " "+result_register+" = call i16* @concat_str_chr(i16* "+register_left+"," 
                            "i16 "+register_right+")";
                } else {
                    if (expr->left->type == &Int)
                        *s += 
                            " "+result_register+" = call i16* @concat_int_str(i32 "+register_left+"," 
                            "i16* "+register_right+")";
                    else
                        *s += 
                            " "+result_register+" = call i16* @concat_chr_str(i16 "+register_left+"," 
                            "i16* "+register_right+")";
                }
                return result_register;
            }
            break;
        case Operation::Sub:
            *s +=
                    " "+result_register+" = sub i32 "+register_left+", "+register_right+"\n";
            break;
        case Operation::Mul:
            *s +=
                    " "+result_register+" = mul i32 "+register_left+", "+register_right+"\n";
            break;
        case Operation::Div:
            *s +=
                    " "+result_register+" = sdiv i32 "+register_left+", "+register_right+"\n";
            break;
        case Operation::Mod:
            *s +=
                    " "+result_register+" = srem i32 "+register_left+", "+register_right+"\n";
            break;
        case Operation::L:
        case Operation::G:
        case Operation::Le:
        case Operation::Ge:
            *s +=
                " "+result_register+" = icmp "+this->operation_to_cc(o)+" i32 "+
                register_left+", "+register_right+"\n";
            return result_register;
            break;
        case Operation::E:
        case Operation::Ne:
            if (expr->left->type == &String && expr->right->type == &String) {
                string temp_register = this->assign_register();
                *s +=
                    " "+temp_register+" = call i1 @cmp_str(i16* "+register_left+", i16* "+
                    register_right+")\n";
                if (o==Operation::Ne)
                    *s +=
                        " "+result_register+" = sub i1 1, "+temp_register+"\n";
                else
                    *s +=
                        " "+result_register+" = add i1 0, "+temp_register+"\n";
                return result_register;
            } else {
                *s +=
                    " "+result_register+" = icmp "+this->operation_to_cc(o)+" i32 "+
                    register_left+", "+register_right+"\n";
                return result_register;
            }
            break;
        default:
            throw runtime_error("Unknown operator");
            break;
    }
    return result_register;

}

string Translator_LLVM::translate_op_expr(string *s, OpExpr *expr) 
{
    if (expr->op == "&&" || expr->op == "||") {
        return this->translate_bool_expr(s, expr);
    } else {
        return this->translate_arithm_expr(s, expr);
    }
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
        return "null";
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
    string result_register = this->assign_register();
    *s += // asm comment
        "; " + dec->to_string() + "\n";
    string expr_register = this->translate_expr(s, dec->expr.get());
    string var_type = this->type_to_llvm_type(dec->type);
    if (dec->type == &Int && dec->expr->type == &Char) {
        string temp_register = this->assign_register();
        *s +=
            " "+temp_register+" = zext i16 "+expr_register+" to i32\n"
            " "+result_register+" = alloca "+var_type+"\n"
            " store "+var_type+" "+temp_register+", "+var_type+"* "+result_register+"\n";
    } else {
        *s += 
            " "+result_register+" = alloca "+var_type+"\n"
            " store "+var_type+" "+expr_register+", "+var_type+"* "+result_register+"\n";
    }
    this->variables.insert_or_assign(dec->id, std::make_pair(result_register, var_type));
}


string Translator_LLVM::arr_type_to_func_size(ArrayType *t)
{
    if (t->base == &Bool)
        return "8";
    else if (t->base == &Char)
        return "16";
    else if (t->base == &Int)
        return "32";
    else
        return "64";
}

void Translator_LLVM::translate_assignment(string *s, Assignment *asgn)
{
    *s += // asm comment
        "; " + asgn->to_string() + "\n";
    if (auto var = dynamic_cast<Variable *>(asgn->id.get())) {
        string expr_register = this->translate_expr(s, asgn->expr.get());
        string var_storage = this->variables.at(var->name).first;
        string var_type = this->variables.at(var->name).second;
        *s +=
            " store "+var_type+" "+expr_register+", "+var_type+"* "+var_storage+"\n";
    } else {
        auto el = dynamic_cast<ElemAccessExpr *>(asgn->id.get());
        string expr_register = this->translate_expr(s, el->expr.get());
        string index_register = this->translate_expr(s, el->index.get());
        string asgn_register = this->translate_expr(s, asgn->expr.get());
        auto arr_t = dynamic_cast<ArrayType *>(el->expr->type);
        string temp_register = this->assign_register();
        string temp_asgn_register = this->assign_register();
        string var_type = this->type_to_llvm_type(arr_t);
        string size = this->arr_type_to_func_size(arr_t);
        string asgn_type = this->type_to_llvm_type(asgn->expr->type);
        if (arr_t->base == &Bool || 
            arr_t->base == &Char ||
            arr_t->base == &Int) {
            *s +=
                " call void @set"+size+"("+var_type+" "+expr_register+", i32 "+index_register+", "+asgn_type+" "+asgn_register+")\n";
        } else {
            *s +=
                " "+temp_register+" = bitcast "+var_type+" "+expr_register+" to i8**\n"
                " "+temp_asgn_register+" = bitcast "+asgn_type+" "+asgn_register+" to i8*\n"
                " call void @set"+size+"(i8** "+temp_register+", i32 "+index_register+", i8* "+temp_asgn_register+")\n";
        }
    }
}



void Translator_LLVM::translate_print(string *s, Print *p)
{
    *s += // asm comment 
        "; " + p->to_string() + "\n";
    string result_register = this->translate_expr(s, p->expr.get());
    if (p->expr->type == &String) {
        *s +=
            "call void (i16*) @printg(i16* "+result_register+")\n";
        return;
    } else if (p->expr->type == &Int ){
        string format_register = this->assign_register();
        *s +=
            " "+format_register+" = getelementptr [4 x i8], [4 x i8]* @fmt_i"
            ", i32 0, i32 0\n"
            " call i32 (i8*, ...) @printf(i8* "+format_register+", i32 "+result_register+")\n";
    } else if (p->expr->type == &Char){
        string format_register = this->assign_register();
        *s +=
            ""+format_register+" = getelementptr [4 x i8], [4 x i8]* @fmt_c"
            ", i32 0, i32 0\n"
            "call i32 (i8*, ...) @printf(i8* "+format_register+", i16 "+result_register+")\n";
    } else {
        string format_register = this->assign_register();
        *s +=
            ""+format_register+" = getelementptr [4 x i8], [4 x i8]* @fmt_i"
            ", i32 0, i32 0\n"
            "call i32 (i8*, ...) @printf(i8* "+format_register+", i1 "+result_register+")\n";
    }
}


void Translator_LLVM::translate_if_statement(string *s, IfStatement *st, string loop_end_label)
{
    string label_id = std::to_string(this->label_id++);
    *s += // asm comment
        "; if " + st->cond->to_string() + "\n";

    string cond_register = this->translate_expr(s, st->cond.get());
    *s +=
        " br i1 "+cond_register+", label %if_s"+label_id+", label %else_s"+label_id+"\n"
        "if_s"+label_id+":\n";
    this->translate_statement(s, st->if_s.get(), loop_end_label);
    *s +=
        " br label %end_s"+label_id+"\n"
        "else_s"+label_id+":\n";
    if (st->else_s)
        this->translate_statement(s, st->else_s.get(), loop_end_label);
    *s +=
        " br label %end_s"+label_id+"\n"
        "end_s"+label_id+":\n";
}


void Translator_LLVM::translate_while_statement(string *s, WhileStatement *st)
{
    string label_id = std::to_string(this->label_id++);
    *s += 
        " br label %loop"+label_id+"\n"
        "loop" + label_id + ":\n";
    *s += // asm comment
        "; while " + st->cond->to_string() + "\n";
    string cond_register = this->translate_expr(s, st->cond.get());
    *s +=
        " br i1 "+cond_register+",label %loop_body"+label_id+", label %loop_end"+label_id+"\n"
        "loop_body"+label_id+":\n";
    this->translate_statement(s, st->statement.get(), label_id);
    *s +=
        " br label %loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
}


void Translator_LLVM::translate_for_statement(string *s, ForStatement *for_s)
{
    string label_id = std::to_string(this->label_id++);

    this->translate_declaration(s, for_s->init.get());

    *s +=
        " br label %loop"+label_id+"\n"
        "loop" + label_id + ":\n";
    *s +=
        "; for " + for_s->cond->to_string() + "\n";

    string cond_register = this->translate_expr(s, for_s->cond.get());
    *s +=
        " br i1 "+cond_register+", label %loop_body"+label_id+" ,label %loop_end"+label_id+"\n"
        "loop_body"+label_id+":\n";
    this->translate_statement(s, for_s->body.get(), label_id);
    if (auto es = dynamic_cast<ExpressionStatement *>(for_s->iter.get()))
        this->translate_expression_statement(s, es);
    else if (auto asgn = dynamic_cast<Assignment *>(for_s->iter.get()))
        this->translate_assignment(s, asgn);
    *s +=
        " br label %loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
    
}

string Translator_LLVM::bool_to_op(bool inc)
{
    return (inc ? "add" : "sub");
}


string Translator_LLVM::translate_inc_expr(string *s, IncExpr *expr)
{
    if (auto var = dynamic_cast<Variable *>(expr->expr.get())) {
        string var_register = this->translate_variable(s, var);
        string result_register = this->assign_register();
        string var_storage = this->variables.at(var->name).first;
        *s +=
            " "+result_register+ " = "+this->bool_to_op(expr->inc)+" i32 "+var_register+", 1\n"
            " store i32 "+result_register+", i32* "+var_storage+"\n";
        return var_register;
        
    } else {
        auto el = dynamic_cast<ElemAccessExpr *>(expr->expr.get());
        string arr_register = this->translate_expr(s, el->expr.get());
        string index_register = this->translate_expr(s, el->index.get());
        string value_register = this->assign_register();
        string temp_register = this->assign_register();
        string arr_temp_register = this->assign_register();
        *s +=
            " "+value_register+" = call i32 @get32(i32* "+arr_register+", i32 "+index_register+")\n"
            " "+temp_register+" = "+this->bool_to_op(expr->inc)+" i32 "+value_register+", 1\n"
            " call void @set32(i32* "+arr_register+", i32 "+index_register+", i32 "+temp_register+")\n";
        return value_register;
    }
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
            " br label %loop_end"+loop_end_label+"\n";
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
            "declare i16 @get16(i16*, i32)\n"
            "declare i1 @get8(i8*, i32)\n"
            "declare i32 @get32(i32*, i32)\n"
            "declare i8* @get64(i8**, i32)\n"
            "declare void @set16(i16*, i32, i16)\n"
            "declare void @set8(i1*, i32, i1)\n"
            "declare void @set32(i32*, i32, i32)\n"
            "declare void @set64(i8**, i32, i8*)\n"
            "declare i16* @concat(i16*, i16*)\n"
            "declare i1 @cmp_str(i16*, i16*)\n"
            "declare i16* @concat_str_int(i16*, i32)\n"
            "declare i16* @concat_int_str(i32, i16*)\n"
            "declare i16* @concat_str_chr(i16*, i16)\n"
            "declare i16* @concat_chr_str(i16, i16*)\n"
            "declare i16* @substr_int(i16*, i32)\n"
            "declare i16* @substr_int_int(i16*, i32, i32)\n"
            "declare i32 @int_parse(i16*)\n"
            "declare i16* @new_str_expr(i16, i32)\n"
            "declare i8* @new_arr_expr(i32, i32)\n"
            "declare i32 @gstring_len(i16*)\n"
            "declare i32 @arr_len(i8*)\n"
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
            "; string: "+str+"\n";
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
