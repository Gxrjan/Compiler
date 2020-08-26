#include "head.h"


int main(int argc, char *argv[])
{
    ofstream file{ "a.asm", ios::out };
    Scanner scan;
    Translator tran;
    unique_ptr<Expr> expr = Parser(&scan).try_get_expr();
    string asm_code = tran.translate_expr(expr.get());
    cout << asm_code;
    file << asm_code;
    
     
}
