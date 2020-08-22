#include "head.h"


int main(int argc, char *argv[])
{
    Scanner scan;
    unique_ptr<Token> t;
    while ((t=scan.next_token())) {
        cout << t->to_string() <<  endl;
    }
    return 0;
}
