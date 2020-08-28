#include "head.h"

Parser::Parser(Scanner* scan) { this->scan = scan; }
unique_ptr<Expr> Parser::parse_factor() 
{
    unique_ptr<Token> t = (this->scan)->next_token();
    int num;
    char c;
    string name;
    if (t->isNum(&num)) {
        return make_unique<Literal>(num);
    }
    if (t->isId(&name))
        return make_unique<Variable>(name);
    if (t->isParen(&c) && c == '(') {
        unique_ptr<Expr> e = parse_expr();
        unique_ptr<Token> next = (this->scan)->next_token();
        if (!next || !next->isParen(&c) ||  c != ')')
            throw runtime_error("syntax error1");
        return e;
    }
    throw runtime_error("syntax error2");
}


unique_ptr<Expr> Parser::parse_term()
{
    unique_ptr<Expr> expr = parse_factor();
    char c;
    while (true) {
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&c) && (c=='*' || c=='/')) {
            (this->scan)->next_token();
            unique_ptr<Expr> factor = this->parse_factor();
            expr = make_unique<OpExpr>(c, move(expr), move(factor));
        } else
            break;
    }
    return expr;
}
unique_ptr<Expr> Parser::parse_expr()
{
    unique_ptr<Expr> expr = parse_term();
    char c;
    while (true) {
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&c) && (c=='+' || c=='-')) {
            (this->scan)->next_token();
            unique_ptr<Expr> term = this->parse_term();
            expr = make_unique<OpExpr>(c, move(expr), move(term));
        } else
            break;
    }
    return expr;
}


unique_ptr<Expr> Parser::try_get_expr()
{
    unique_ptr<Expr> expr = this->parse_expr();
    unique_ptr<Token> rest = this->scan->next_token();
    if (rest)
        throw runtime_error("Extra input error");
    return expr;
}


unique_ptr<Program> Parser::parse_program()
{
    unique_ptr<Statement> statement = this->parse_statement();
    if (!statement)
        return nullptr;
    unique_ptr<Token> t = this->scan->next_token();
    if (t && t->isSymbol(';'))
        return make_unique<Program>(move(statement), this->parse_program());
    throw runtime_error("Syntax error 4(No semicolon after statement)");
}

unique_ptr<Statement> Parser::parse_statement()
{
    unique_ptr<Token> t = this->scan->next_token();
    unique_ptr<Expr> e;
    unique_ptr<Token> next;
    string name;
    char c;

    if (t && t->isPrint()) {
        t = this->scan->next_token();

        if (t && t->isParen(&c) && c == '(')
            e = this->parse_expr();
        else
            throw runtime_error("Syntax error 6");

        t = this->scan->next_token();
        if (!t || !t->isParen(&c) || c != ')')
            throw runtime_error("Syntax error 3");

        return make_unique<Print>(move(e));
    }

    if (t && t->isId(&name)) {
        next = this->scan->next_token();
        if (next && next->isSymbol('=')) {
            e = parse_expr();
            if (e)
                return make_unique<Assignment>(move(t), move(e));
            else
                throw runtime_error("Syntax error 8");
        } else {
            throw runtime_error("Syntax error 7");
        }
    }
    return nullptr;
    throw runtime_error("Syntax error 5");
}
