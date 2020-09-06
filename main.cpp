#include "head.h"


int main(int argc, char *argv[])
{
    //ofstream file{ "a.asm", ios::out };
    //Scanner scan;
    //Translator tran;
    //unique_ptr<Expr> expr = Parser(&scan).try_get_expr();
    //string asm_code = tran.translate_expr(expr.get());
    //file << asm_code;
    //file.close();
    //system("make calc");
    
    Scanner scan;
    unique_ptr<Token> t;
    while (t = scan.next_token()) {
        cout << t->to_string() << endl;
    }
    
    //Scanner scan;
    //Parser p { &scan };
    //unique_ptr<Program> prog = p.parse_program();
    //Checker c;
    //c.check_program(prog.get());
    //cout << prog->to_string() << endl;
}
