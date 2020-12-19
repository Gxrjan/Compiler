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
    if (v->name == "argc") {
        string result_register = this->assign_register();
        *s +=
            " "+result_register+" = add i32 0, %0\n";
        return result_register;
    } else if (v->name == "argv") {
        return "%argv";
    }
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


void Translator_LLVM::create_bounds_check(string *s, string expr_register, string index_register, Type *type) {
    string reg_type = this->g_type_to_llvm_type(type);
    string label_id = std::to_string(this->label_id++);
    string conv_register = this->assign_register();
    string len_register_ptr = this->assign_register();
    string len_register = this->assign_register();
    string upper_bound_check = this->assign_register();
    string lower_bound_check = this->assign_register();
    string msg_loc = this->assign_register();
    *s += 
        "; bounds check start\n"
        " "+conv_register+" = bitcast "+reg_type+" "+expr_register+" to i32*\n"
        " "+len_register_ptr+" = getelementptr i32 , i32* "+conv_register+", i32 -1\n"
        " "+len_register+" = load i32, i32* "+len_register_ptr+"\n"
        " "+upper_bound_check+" = icmp slt i32 "+index_register+", "+len_register+"\n"
        " br i1 "+upper_bound_check+", label %continue"+label_id+", label %trap"+label_id+"\n"
        "continue"+label_id+":\n"
        " "+lower_bound_check+" = icmp sge i32 "+index_register+", 0\n"
        " br i1 "+lower_bound_check+", label %end"+label_id+", label %trap"+label_id+"\n"
        "trap"+label_id+":\n"
        " "+msg_loc+" = getelementptr [21 x i8], [21 x i8]* @index_out_of_bounds_msg, i32 0, i32 0\n"
        " call void (i8*) @report_error(i8* "+msg_loc+")\n"
        " br label %end"+label_id+"\n"
        "end"+label_id+":"
        "; bounds check end\n";
}

string Translator_LLVM::create_getelementptr_load(string *s, Type *result_type, Type *expr_type, string expr_register, string index_register) {
    string result_register = this->assign_register();
    string temp_register = this->assign_register();
    string result_llvm_type = this->g_type_to_llvm_type(result_type);
    string expr_llvm_type = this->g_type_to_llvm_type(expr_type);
    *s +=
        " "+temp_register+" = getelementptr "+result_llvm_type+", "+expr_llvm_type+" "+expr_register+", i32 "+index_register+"\n"
        " "+result_register+" = load "+result_llvm_type+", "+expr_llvm_type+" "+temp_register+"\n";
    return result_register;
}

string Translator_LLVM::translate_elem_access_expr(string *s, ElemAccessExpr *e)
{
    *s += // asm comment
        "; "+e->to_string()+"\n";
    string expr_register = this->translate_expr(s, e->expr.get());
    string index_register = this->translate_expr(s, e->index.get());
    if (e->expr->type == &String) {
        this->create_bounds_check(s, expr_register, index_register, &String);
        return create_getelementptr_load(s, &Char, &String, expr_register, index_register);
    } else {
        auto arr_t = dynamic_cast<ArrayType *>(e->expr->type);
        string expr_llvm_type = this->g_type_to_llvm_type(arr_t);
        string result_llvm_type = this->g_type_to_llvm_type(arr_t->base);
        if (arr_t->base == &Bool ||
            arr_t->base == &Char ||
            arr_t->base == &Int) {
                this->create_bounds_check(s, expr_register, index_register, arr_t);
                string temp_register = this->assign_register();
                return create_getelementptr_load(s, arr_t->base, arr_t, expr_register, index_register);
        } else {
            string result_type = this->g_type_to_llvm_type(arr_t->base);
            this->create_bounds_check(s, expr_register, index_register, e->expr->type);
            return create_getelementptr_load(s, e->type, e->expr->type, expr_register, index_register);
        }
    }
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
        string var_type = this->g_type_to_llvm_type(e->expr->type);
        *s +=
            " "+temp_register+" = bitcast "+var_type+" "+expr_register+" to i8*\n"
            " "+result_register+" = call i32 @arr_len(i8* "+temp_register+")\n";
        return result_register;
    }
}

string Translator_LLVM::create_conversion(string *s, string expr_register, Type *from, Type *to) {
    if (from == &Empty || to == &Empty)
        return expr_register;
    string result_register = this->assign_register();
    string llvm_from = this->g_type_to_llvm_type(from);
    string llvm_to = this->g_type_to_llvm_type(to);
    if (llvm_from < llvm_to) {
        *s +=
            " "+result_register+" = zext "+llvm_from+" "+expr_register+" to "+llvm_to+"\n";
        return result_register;
    } else if (llvm_from > llvm_to) {
        *s +=
            " "+result_register+" = trunc "+llvm_from+" "+expr_register+" to "+llvm_to+"\n";
        return result_register;
    }
    return expr_register;
}

string Translator_LLVM::translate_type_cast_expr(string *s, TypeCastExpr *e)
{
    string expr_register = this->translate_expr(s, e->expr.get());
    return this->create_conversion(s, expr_register, e->expr->type, e->type);
}



string Translator_LLVM::translate_substr_expr(string *s, SubstrExpr *e)
{
    string str_register = this->translate_expr(s, e->expr.get());
    string result_register = this->assign_register();
    if (e->arguments.size() == 1) {
        string from_register;
        if (e->arguments[0]->type == &Char) {
            string temp_from_register = this->translate_expr(s, e->arguments[0].get());
            this->create_conversion(s, temp_from_register, e->arguments[0]->type, &Int);
        } else {
            from_register = this->translate_expr(s, e->arguments[0].get());
        }
        *s +=
            " "+result_register+" = call i16* @substr_int(i16* "+str_register+",i32 "+from_register+")\n";
    } else {

        string from_register;
        string len_register;
        if (e->arguments[0]->type == &Char) {
            string temp_from_register = this->translate_expr(s, e->arguments[0].get());
            from_register = this->create_conversion(s, temp_from_register, e->arguments[0]->type, &Int);
        } else {
            from_register = this->translate_expr(s, e->arguments[0].get());
        }

        if (e->arguments[1]->type == &Char) {
            string temp_len_register = this->translate_expr(s, e->arguments[1].get());
            len_register = this->create_conversion(s, temp_len_register, e->arguments[1]->type, &Int);
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

string Translator_LLVM::array_type_to_bytes(ArrayType *t)
{
    if (t->base == &Bool || t->base == &Byte) {
            return "1";
    } else if (t->base == &Char) 
        return "2";
    else if (t->base == &Int)
        return "4";
    else
        return "8";
}

string Translator_LLVM::g_type_to_llvm_type(Type *t)
{
    if (t == &Bool) {
        return "i1";
    } else if (t == &Char) 
        return "i16";
    else if (t == &Int)
        return "i32";
    else if (t == &String)
        return "i16*";
    else if (t == &Byte)
        return "i8";
    else {
        auto st = dynamic_cast<ArrayType *>(t);
        return this->g_type_to_llvm_type(st->base)+"*";
    }
}

string Translator_LLVM::create_convert_ptr(string *s, string expr_register, Type *from, Type *to) {
    string result_register = this->assign_register();
    string llvm_from = this->g_type_to_llvm_type(from);
    string llvm_to = this->g_type_to_llvm_type(to);
    *s +=
        " "+result_register+" = bitcast "+llvm_from+" "+expr_register+" to "+llvm_to+"\n";
    return result_register;
}

string Translator_LLVM::translate_new_arr_expr(string *s, NewArrExpr *e)
{
    string len_register = this->translate_expr(s, e->expr.get());
    string size_register = this->assign_register();
    string result_register = this->assign_register();
    string temp_register = this->assign_register();
    auto t = dynamic_cast<ArrayType *>(e->type);
    *s +=
        " "+size_register+" = add i32 "+this->array_type_to_bytes(t)+", 0\n";

    *s +=
        " "+temp_register+" = call i8* @new_arr_expr(i32 "+size_register+", i32 "+len_register+")\n";
    //return this->create_conversion(s, temp_register, ArrayType::make(&Byte), e->type);
    return this->create_convert_ptr(s, temp_register, ArrayType::make(&Byte), e->type);
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

string Translator_LLVM::create_add(string *s, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = add "+llvm_type+" "+register_left+", "+register_right+"\n";
    return result_register;
}

string Translator_LLVM::create_sub(string *s, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = sub "+llvm_type+" "+register_left+", "+register_right+"\n";
    return result_register;
}

string Translator_LLVM::create_mul(string *s, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = mul "+llvm_type+" "+register_left+", "+register_right+"\n";
    return result_register;
}

string Translator_LLVM::create_div(string *s, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = sdiv "+llvm_type+" "+register_left+", "+register_right+"\n";
    return result_register;
}

string Translator_LLVM::create_mod(string *s, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = srem "+llvm_type+" "+register_left+", "+register_right+"\n";
    return result_register;
}

string Translator_LLVM::create_cmp(string *s, Operation o, Type *type, string register_left, string register_right) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(type);
    *s +=
        " "+result_register+" = icmp "+this->operation_to_cc(o)+" "+llvm_type+" "+
                register_left+", "+register_right+"\n";
    return result_register;
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
    string temp_register_left;
    string temp_register_right;
    if (expr->type == &Int) {
        temp_register_left = this->create_conversion(s, register_left, expr->left->type, expr->type);
        temp_register_right = this->create_conversion(s, register_right, expr->right->type, expr->type);
    }
    switch(o) {
        case Operation::Add:
            if (expr->type == &Int) {
                return this->create_add(s, expr->type, temp_register_left, temp_register_right);
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
            return this->create_sub(s, expr->type, temp_register_left, temp_register_right);
        case Operation::Mul:
            return this->create_mul(s, expr->type, temp_register_left, temp_register_right);
        case Operation::Div:
            return this->create_div(s, expr->type, temp_register_left, temp_register_right);
        case Operation::Mod:
            return this->create_mod(s, expr->type, temp_register_left, temp_register_right);
        case Operation::L:
        case Operation::G:
        case Operation::Le:
        case Operation::Ge:
            return this->create_cmp(s, o, expr->left->type, register_left, register_right);
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
                return this->create_cmp(s, o, expr->left->type, register_left, register_right);
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


string Translator_LLVM::create_allocate_and_store(string *s, Type *t, string expr_register) {
    string result_register = this->assign_register();
    string llvm_type = this->g_type_to_llvm_type(t);
    *s +=
        " "+result_register+" = alloca "+llvm_type+"\n"
        " store "+llvm_type+" "+expr_register+", "+llvm_type+"* "+result_register+"\n";
    return result_register;
}

void Translator_LLVM::translate_declaration(string *s, Declaration *dec)
{
    *s += // asm comment
        "; " + dec->to_string() + "\n";
    string var_type = this->g_type_to_llvm_type(dec->type);
    string expr_register = this->translate_expr(s, dec->expr.get());
    string temp_register = this->create_conversion(s, expr_register, dec->expr->type, dec->type);
    string result_register = this->create_allocate_and_store(s, dec->type, temp_register);
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
        string var_type = this->g_type_to_llvm_type(arr_t);
        string size = this->arr_type_to_func_size(arr_t);
        string asgn_type = this->g_type_to_llvm_type(asgn->expr->type);
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
    } else {
        string reg_type = this->g_type_to_llvm_type(p->expr->type);
        string format_register = this->assign_register();
        *s +=
            " "+format_register+" = getelementptr [4 x i8], [4 x i8]* @fmt_"+this->type_to_cc(p->expr->type)+
            ", i32 0, i32 0\n"
            " call i32 (i8*, ...) @printf(i8* "+format_register+", "+reg_type+" "+result_register+")\n";
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
    *s += // asm comment
        "; "+expr->to_string()+"\n";
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
        string value_register = this->create_getelementptr_load(s, el->type, el->expr->type, arr_register, index_register);
        string temp_register = this->assign_register();
        *s +=
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
            "@fmt_s = constant [4 x i8] c\"%s\\0A\\00\"\n"
            "@index_out_of_bounds_msg = constant [21 x i8] c\"index out of bounds\\0A\\00\"\n"
            "declare void @report_error(i8*)"
            "declare i32 @printf(i8*, ...)\n"
            "declare void @printg(i16*)\n"
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
            "declare i16* @to_gstring(i8*)\n"
            "declare i16** @to_argv(i32, i8**)\n"
            "define i32 @main(i32, i8**) {\n"
            "%argc = add i32 %0, 0\n"
            "%argv = call i16** @to_argv(i32 %0, i8** %1)\n";


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
