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
                *s += 
                    " pop     rax\n"
                    " add     [rsp], rax\n";
                break;
            case '-':
                *s += 
                    " pop     rax\n"
                    " sub     [rsp], rax\n";
                break;
            case '*':
                *s += 
                    " pop     rax\n"
                    " pop     rbx\n"
                    " imul    rax, rbx\n"
                    " push    rax\n";
                break;
            case '/':
                *s += 
                    " pop     rbx\n"
                    " pop     rax\n"
                    " mov     rdx, 0\n"
                    " idiv    qword rbx\n"
                    " push    rax\n";
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
    result += 
        "extern printf\n"
        "section .data\n"
        " msg     db      `%d\\n`,0\n"
        "section .text\n"
        " global main\n"
        "main:\n";
    //result += " push    rbp\n";
    //result ++ " mov     rbp, rsp\n";
    this->translate_expr(&result, expr);
    result += 
        " mov     rax, 0\n"
        " mov     rdi, msg\n"
        " pop     rsi\n"
        " call printf\n"
        " mov     rax, 60\n"
        " mov     rdi, 0\n"
        " syscall\n";
    return result;
}
