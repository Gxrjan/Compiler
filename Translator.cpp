#include "head.h"


string Translator::concat_cc(Type *left, Type *right)
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

string Translator::operation_to_cc(Operation op)
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

string Translator::type_to_cc(Type *t)
{
    if (t == &Char)
        return "c";
    return "i";
}

void Translator::translate_num_literal(string *s, NumLiteral *l)
{
    *s +=
        " mov       rax, "+std::to_string(l->num)+"\n" 
        " push      rax\n";
}


void Translator::translate_bool_literal(string *s, BoolLiteral *l)
{
    if (l->b)
        *s += 
            " push        1\n";
    else
        *s += 
            " push        0\n";
}

void Translator::translate_variable(string *s, Variable *v) 
{
    *s += 
        " push     qword ["+v->name+"]\n";
}

void Translator::translate_char_literal(string *s, CharLiteral *l)
{
    *s +=
        " push          " + std::to_string((int)l->c) + "\n";
}

void Translator::translate_string_literal(string *s, StringLiteral *l)
{
    int id = string_id++;
    *s += 
        " push  str_"+std::to_string(id)+"\n";
    this->strings.insert({l, id});
}

void Translator::translate_elem_access_expr(string *s, ElemAccessExpr *e)
{
    this->translate_expr(s, e->expr.get());
    this->translate_expr(s, e->index.get());
    *s +=
        " pop   rsi\n"
        " pop   rdi\n"
        " call  get\n"
        " push  rax\n";
}

void Translator::translate_length_expr(string *s, LengthExpr *e)
{
    this->translate_expr(s, e->expr.get());
    *s +=
        " pop   rcx\n"
        " sub   rcx, 8\n"
        " push  qword [rcx]\n";
}

void Translator::translate_type_cast_expr(string *s, TypeCastExpr *e)
{
    this->translate_expr(s, e->expr.get());
    if (e->type == &Char) {
        *s += 
            " pop       rcx\n"
            " mov       rax, 0\n"
            " mov       ax, cx\n"
            " push      rax\n";
    }
}

void Translator::translate_substr_expr(string *s, SubstrExpr *e)
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
}

void Translator::translate_int_parse_expr(string *s, IntParseExpr *e)
{
    this->translate_expr(s, e->arguments[0].get());
    *s +=
        " pop   rdi\n"
        " call  int_parse\n"
        " push  rax\n";
}

void Translator::translate_new_str_expr(string *s, NewStrExpr *e)
{
    
    this->translate_expr(s, e->arguments[0].get());
    this->translate_expr(s, e->arguments[1].get());
    *s +=
        " pop   rsi\n"
        " pop   rdi\n"
        " call  new_str_expr\n"
        " push   rax\n";
}

void Translator::translate_op_expr(string *s, OpExpr *expr) 
{
    if (expr->op == "&&" || expr->op == "||") {
        Operation o;
        string label_id = std::to_string(this->label_id++);
        switch((o = TypeConverter::string_to_operation(expr->op))) {
            case Operation::And:
                *s += // asm comment 
                    "; " + expr->left->to_string() + "\n";
                this->translate_expr(s, expr->left.get());
                *s +=
                    " cmp       qword [rsp], 0\n"
                    " je        end"+label_id+"\n"
                    " pop       rax\n";
                *s += // asm comment 
                    "; " + expr->right->to_string() + "\n";
                this->translate_expr(s, expr->right.get());
                break;
            case Operation::Or:
                // translating goes here
                *s += // asm comment 
                    "; " + expr->left->to_string() + "\n";
                this->translate_expr(s, expr->left.get());
                *s +=
                    " cmp       qword [rsp], 1\n"
                    " je        end"+label_id+"\n"
                    " pop       rax\n";
                *s += // asm comment 
                    "; " + expr->right->to_string() + "\n";
                this->translate_expr(s, expr->right.get());
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
            
        }
        *s +=
            "end"+label_id+":\n";

    } else {
        *s += // asm comment 
            "; " + expr->left->to_string() + "\n";
        this->translate_expr(s, expr->left.get());
        *s += // asm comment 
            "; " + expr->right->to_string() + "\n";
        this->translate_expr(s, expr->right.get());
        Operation o;
        switch((o = TypeConverter::string_to_operation(expr->op))) {
            case Operation::Add:
                if (expr->type == &Int) {
                    *s += 
                        " pop     rax\n"
                        " add     [rsp], rax\n";
                } else {
                    *s +=
                        " pop   rsi\n"
                        " pop   rdi\n"
                        " call  concat"+this->concat_cc(expr->left->type, expr->right->type)+"\n"
                        " push  rax\n";
                }
                break;
            case Operation::Sub:
                *s += 
                    " pop     rax\n"
                    " sub     [rsp], rax\n";
                break;
            case Operation::Mul:
                *s += 
                    " pop     rax\n"
                    " pop     rcx\n"
                    " imul    rax, rcx\n"
                    " push    rax\n";
                break;
            case Operation::Div:
                *s += 
                    " pop     rcx\n"
                    " pop     rax\n"
                    " mov     rdx, 0\n"
                    " idiv    qword rcx\n"
                    " push    rax\n";
                break;
            case Operation::Mod:
                *s += 
                    " pop     rcx\n"
                    " pop     rax\n"
                    " mov     rdx, 0\n"
                    " idiv    qword rcx\n"
                    " push    rdx\n";
                break;
            case Operation::L:
            case Operation::G:
            case Operation::Le:
            case Operation::Ge:
            case Operation::E:
            case Operation::Ne:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " set"+this->operation_to_cc(o) +"     cl\n"
                    " mov       [rsp], rcx\n";
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
        }

    }
} 

void Translator::translate_expr(string *s, Expr *e)
{
    if (auto expr = dynamic_cast<NumLiteral *>(e)) {
        this->translate_num_literal(s, expr);
    } else if (auto expr = dynamic_cast<BoolLiteral *>(e)) {
        this->translate_bool_literal(s, expr);
    } else if (auto var = dynamic_cast<Variable *>(e)) {
        this->translate_variable(s, var);
    } else if (auto expr = dynamic_cast<OpExpr *>(e)) {
        this->translate_op_expr(s, expr);
    } else if (auto expr = dynamic_cast<CharLiteral *>(e)) {
        this->translate_char_literal(s, expr);
    } else if (auto expr = dynamic_cast<StringLiteral *>(e)) {
        this->translate_string_literal(s, expr);
    } else if (auto expr = dynamic_cast<ElemAccessExpr *>(e)) {
        this->translate_elem_access_expr(s, expr);
    } else if (auto expr = dynamic_cast<LengthExpr *>(e)) {
        this->translate_length_expr(s, expr);
    } else if (auto expr = dynamic_cast<TypeCastExpr *>(e)) {
        this->translate_type_cast_expr(s, expr);
    } else if (auto expr = dynamic_cast<SubstrExpr *>(e)) {
        this->translate_substr_expr(s, expr);
    } else if (auto expr = dynamic_cast<IntParseExpr *>(e)) {
        this->translate_int_parse_expr(s, expr);
    } else if (auto expr = dynamic_cast<NewStrExpr *>(e)) {
        this->translate_new_str_expr(s, expr);
    } else if (dynamic_cast<NullExpr *>(e)) {
        *s +=
            " push      0\n";
    } else
        throw runtime_error("Unknown type of expression");
}


void Translator::translate_declaration(string *s, Declaration *dec)
{
    *s += // asm comment
        "; " + dec->to_string() + "\n";
    
    this->variables.insert(dec->id);
    this->translate_expr(s, dec->expr.get());
    *s +=
       " pop        qword ["+dec->id+"]\n";
}

void Translator::translate_assignment(string *s, Assignment *asgn)
{
    *s += // asm comment
        "; " + asgn->to_string() + "\n";

    this->translate_expr(s, asgn->expr.get());
    *s +=
       " pop        qword ["+asgn->id+"]\n";
}

void Translator::translate_print(string *s, Print *p)
{
    *s += // asm comment 
        "; " + p->to_string() + "\n";
    this->translate_expr(s, p->expr.get());
    if (p->expr->type == &String) {
        *s +=
            " pop       rdi\n"
            " call      printg\n";
        return;
    }

    *s +=
        " mov       rax, 0\n"
        " mov       rdi, msg"+this->type_to_cc(p->expr->type)+"\n"
        " pop       rsi\n"
        " call      printf\n";
}

void Translator::translate_if_statement(string *s, IfStatement *st, string loop_end_label)
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

void Translator::translate_while_statement(string *s, WhileStatement *st)
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

void Translator::translate_for_statement(string *s, ForStatement *for_s)
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
    this->translate_assignment(s, for_s->iter.get());
    *s +=
        " jmp       loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
    
}

void Translator::translate_statement(string *s, Statement *statement, string loop_end_label)
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
    } else {
        throw runtime_error("Unknown statement");
    }
} 

void Translator::translate_block(string *s, Block *b, string loop_end_label)
{
    for (auto &statement : b->statements)
        this->translate_statement(s, statement.get(), loop_end_label);
}

string Translator::translate_program(Program* prog)
{
    string result = "";
    result += 
        "%define u(x) __?utf16?__(x) \n"
        "extern printf\n"
        "extern get\n"
        "extern concat\n"
        "extern concat_str_int\n"
        "extern concat_int_str\n"
        "extern concat_str_chr\n"
        "extern concat_chr_str\n"
        "extern substr_int\n"
        "extern substr_int_int\n"
        "extern int_parse\n"
        "extern new_str_expr\n"
        "extern printg\n"
        "section .text\n"
        " global main\n"
        "main:\n";
    result += " push    rbp\n";
    result += " mov     rbp, rsp\n";

    this->translate_block(&result, prog->block.get(), "");

    result += 
        " mov     rsp, rbp\n"
        " pop     rbp\n"
        " ret\n";
    result +=
        "section .bss\n";
    for (auto &id : this->variables)
        result +=
            " "+id+"      resq 1\n";
    result +=
        "section .data\n"
        " msgi     db      `%lld\\n`,0\n"
        " msgc     db      `%c\\n`,0\n";
    for (auto &p : this->strings)
        result +=
            "   dq        "+std::to_string(p.first->s.length())+"\n"
            "   str_"+std::to_string(p.second)+"  dw      u('"+p.first->s+"')\n";
    return result;
}
