#include "head.h"




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
                *s += 
                    " pop     rax\n"
                    " add     [rsp], rax\n";
                break;
            case Operation::Sub:
                *s += 
                    " pop     rax\n"
                    " sub     [rsp], rax\n";
                break;
            case Operation::Mul:
                *s += 
                    " pop     rax\n"
                    " pop     rbx\n"
                    " imul    rax, rbx\n"
                    " push    rax\n";
                break;
            case Operation::Div:
                *s += 
                    " pop     rbx\n"
                    " pop     rax\n"
                    " mov     rdx, 0\n"
                    " idiv    qword rbx\n"
                    " push    rax\n";
                break;
            case Operation::Mod:
                *s += 
                    " pop     rbx\n"
                    " pop     rax\n"
                    " mov     rdx, 0\n"
                    " idiv    qword rbx\n"
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
                    " set"+TypeConverter::operation_to_string(o) +"     cl\n"
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
    *s +=
        " mov       rax, 0\n"
        " mov       rdi, msg\n"
        " pop       rsi\n"
        " call      printf\n";
}

void Translator::translate_if_statement(string *s, IfStatement *st)
{
    string label_id = std::to_string(this->label_id++);
    *s += // asm comment
        "; if " + st->cond->to_string() + "\n";

    this->translate_expr(s, st->cond.get());
    *s +=
        " pop       rax\n"
        " cmp       rax, 0\n"
        " je        else_s"+label_id+"\n";
    this->translate_statement(s, st->if_s.get());
    if (!st->else_s) {
        *s +=
            "else_s"+label_id+":\n";
    } else {
        *s +=
            " jmp   end_s"+label_id+"\n"
            "else_s"+label_id+":\n";
        this->translate_statement(s, st->else_s.get());
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
    this->translate_statement(s, st->statement.get());
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
    this->translate_statement(s, for_s->body.get());
    this->translate_assignment(s, for_s->iter.get());
    *s +=
        " jmp       loop"+label_id+"\n"
        "loop_end"+label_id+":\n";
    
}

void Translator::translate_statement(string *s, Statement *statement)
{
    if (auto dec = dynamic_cast<Declaration *>(statement)) {
        this->translate_declaration(s, dec);
    } else if (auto asgn = dynamic_cast<Assignment *>(statement)) {
        this->translate_assignment(s, asgn);
    } else if (auto p = dynamic_cast<Print *>(statement)) {
        this->translate_print(s, p);
    } else if (auto b = dynamic_cast<Block *>(statement)) {
        this->translate_block(s, b);
    } else if (auto st = dynamic_cast<IfStatement *>(statement)) {
        this->translate_if_statement(s, st);
    } else if (auto st = dynamic_cast<WhileStatement *>(statement)) {
        this->translate_while_statement(s, st);
    } else if (auto for_s = dynamic_cast<ForStatement *>(statement)) {
        this->translate_for_statement(s, for_s);
    } else {
        throw runtime_error("Unknown statement");
    }
} 

void Translator::translate_block(string *s, Block *b)
{
    for (auto &statement : b->statements)
        this->translate_statement(s, statement.get());
}

string Translator::translate_program(Program* prog)
{
    string result = "";
    result += 
        "extern printf\n"
        "section .data\n"
        " msg     db      `%lld\\n`,0\n"
        "section .text\n"
        " global main\n"
        "main:\n";
    result += " push    rbp\n";
    result += " mov     rbp, rsp\n";

    this->translate_block(&result, prog->block.get());

    result += 
        " mov     rsp, rbp\n"
        " pop     rbp\n"
        " ret\n";
    result +=
        "section .bss\n";
    for (auto &id : this->variables)
        result +=
            " "+id+"      resq 1\n";
    return result;
}
