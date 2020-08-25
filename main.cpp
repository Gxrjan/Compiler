#include "head.h"


int main(int argc, char *argv[])
{
    Scanner scan;
    unique_ptr<Expr> expr = Parser(&scan).try_get_expr();
    cout << expr->to_string() << endl;
}
