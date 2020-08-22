#include "head.h"

Parser::Parser(Scanner* scan) { this->scan = scan; }
unique_ptr<Expr> Parser::parse_factor() 
{
    unique_ptr<Token> t = (this->scan)->next_token();
    unique_ptr<Expr> e;
    string class_name = t->get_class();
    if (class_name == "NumToken") {
        NumToken* cp = (NumToken*)t.get();
        return make_unique<Literal>(cp->num);
    }
    if (class_name == "ParenToken" && t->paren == '(') {
        e = this->parse_expr();
        if (((this->scan)->next_token())->paren != ')')
            throw runtime_error("syntax error");
        return e;
    }
    throw runtime_error("syntax error");
}


unique_ptr<Expr> Parser::parse_term()
{
    return nullptr;
}
unique_ptr<Expr> Parser::parse_expr()
{
    return nullptr;
}
