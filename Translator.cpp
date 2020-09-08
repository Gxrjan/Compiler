#include "head.h"




void Translator::translate_expr(string *s, Expr *e)
{
    int num;
    bool b;
    string op;
    Id id;
    Expr *left;
    Expr * right;
    if (e->isNumLiteral(&num)) {
        *s += 
            " push    " + std::to_string(num) + "\n";
        return;
    }
    if (e->isBoolLiteral(&b)) {
        if (b)
            *s += 
                " push        1\n";
        else
            *s += 
                " push        0\n";
        return;
    }
    if (e->isVariable(&id)) {
        *s += 
            " push     qword ["+id+"]\n";
        return;
    }


    if (e->isOpExpr(&op, &left, &right)) {
        translate_expr(s, left);
        translate_expr(s, right);
        switch(TypeConverter::string_to_operation(op)) {
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
            case Operation::L:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " setl      cl\n"
                    " mov       [rsp], rcx\n";
                break;
            case Operation::G:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " setg      cl\n"
                    " mov       [rsp], rcx\n";
                break;
            case Operation::Le:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " setle     cl\n"
                    " mov       [rsp], rcx\n";
                break;
            case Operation::Ge:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " setge     cl\n"
                    " mov       [rsp], rcx\n";
                break;
            case Operation::Eq:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " sete      cl\n"
                    " mov       [rsp], rcx\n";
                break;
            case Operation::Neq:
                *s +=
                    " pop       rax\n"
                    " mov       rcx, 0\n"
                    " cmp       qword [rsp], rax\n"
                    " setne     cl\n"
                    " mov       [rsp], rcx\n";
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
        }
    }
}



void Translator::translate_statement(string *s, Statement *statement)
{
    if (auto dec = dynamic_cast<Declaration *>(statement)) {
        this->bss += 
            " "+dec->id+"      resq 1\n";
        this->translate_expr(s, dec->expr.get());
        *s +=
           " pop        qword ["+dec->id+"]\n";
    } else if (auto asg = dynamic_cast<Assignment *>(statement)) {
        this->translate_expr(s, asg->expr.get());
        *s +=
           " pop        qword ["+asg->id+"]\n";
    } else if (auto p = dynamic_cast<Print *>(statement)) {
        this->translate_expr(s, p->expr.get());
        *s +=
            " mov       rax, 0\n"
            " mov       rdi, msg\n"
            " pop       rsi\n"
            " call      printf\n";
    } else if (auto b = dynamic_cast<Block *>(statement)) {
        this->translate_block(s, b);
    } else if (auto st = dynamic_cast<IfStatement *>(statement)) {
        int curr_id = this->label_id++;
        this->translate_expr(s, st->cond.get());
        *s +=
            " pop       rax\n"
            " cmp       rax, 0\n"
            " je        else_s"+std::to_string(curr_id)+"\n";
        this->translate_statement(s, st->if_s.get());
        if (!st->else_s) {
            *s +=
                "else_s"+std::to_string(curr_id)+":\n";
        } else {
            *s +=
                " jmp   end_s"+std::to_string(curr_id)+"\n"
                "else_s"+std::to_string(curr_id)+":\n";
            this->translate_statement(s, st->else_s.get());
            *s +=
                "end_s"+std::to_string(curr_id)+":\n";
        }
    } else if (auto st = dynamic_cast<WhileStatement *>(statement)) {
        int curr_id = this->label_id++;
        *s += 
            "loop"+std::to_string(curr_id)+":\n";
        this->translate_expr(s, st->cond.get());
        *s +=
            " je        loop_end"+std::to_string(curr_id)+"\n";
        this->translate_statement(s, st->statement.get());
        *s +=
            " jmp       loop"+std::to_string(curr_id)+"\n"
            "loop_end"+std::to_string(curr_id)+":\n";
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
        " msg     db      `%d\\n`,0\n"
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
        "section .bss\n" +
        this->bss;
    return result;
}
