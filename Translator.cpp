#include "head.h"


void Translator::translate_expr(string *s, Expr *e)
{
    int num;
    char op;
    Expr *left;
    Expr * right;
    if (e->isLiteral(&num)) {
        *s += " push    " + std::to_string(num) + "\n";
        return;
    }
    if (e->isOpExpr(&op, &left, &right)) {
        translate_expr(s, left);
        translate_expr(s, right);
        //*s += " pop r1\n";
        //*s += " pop r2\n";
        switch(op) {
            case '+':
                *s += " pop     rax\n";
                *s += " add     [rsp], rax\n";
                break;
            case '-':
                *s += " pop     rax\n";
                *s += " sub     [rsp], rax\n";
                break;
            case '*':
                *s += " pop     rax\n";
                *s += " pop     rbx\n";
                *s += " imul    rax, rbx\n";
                *s += " push    rax\n";
                break;
            case '/':
                *s += " pop     rbx\n";
                *s += " pop     rax\n";
                *s += " mov     rdx, 0\n";
                *s += " idiv    qword rbx\n";
                *s += " push    rax\n";
                break;
            default:
                throw runtime_error("Unknown operator");
                break;
        }
        //*s += " push r1\n";
    }
}


string Translator::translate_expr(Expr* expr)
{
    string result = "";
    result += "extern printf\n";
    result += "section .data\n";
    result += " msg     db      `%d\\n`,0\n";
    result += "section .text\n";
    result += " global main\n";
    result += "main:\n";
    //result += " push    rbp\n";
    //result ++ " mov     rbp, rsp\n";
    this->translate_expr(&result, expr);
    result += " mov     rax, 0\n";
    result += " mov     rdi, msg\n";
    result += " pop     rsi\n";
    result += " call printf\n";
    result += " mov     rax, 60\n";
    result += " mov     rdi, 0\n";
    result += " syscall\n";
    return result;
}
