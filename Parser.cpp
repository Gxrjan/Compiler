#include "head.h"

Parser::Parser(Scanner* scan)
{ 
    this->scan = scan; 
    this->line = 1;
}
unique_ptr<Expr> Parser::parse_factor() 
{
    unique_ptr<Token> t = (this->scan)->next_token(false);
    int num;
    string name;
    if (t->isNum(&num)) {
        return make_unique<Literal>(num);
    }
    if (t->isId(&name))
        return make_unique<Variable>(name);
    if (t->isSymbol('(')) {
        unique_ptr<Expr> e = parse_expr();
        unique_ptr<Token> next = (this->scan)->next_token(false);
        if (!next || !next->isSymbol(')'))
            throw runtime_error("line " + std::to_string(this->scan->last_line)
                             +  ", col " + std::to_string(this->scan->last_column)
                             +  ": ')' expected");
        return e;
    }
    throw runtime_error("line " + std::to_string(this->scan->last_line)
                     +  ", col " + std::to_string(this->scan->last_column)
                     +  ": syntax error");
}


unique_ptr<Expr> Parser::parse_term()
{
    unique_ptr<Expr> expr = parse_factor();
    char c;
    while (true) {
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&c) && (c=='*' || c=='/')) {
            (this->scan)->next_token(false);
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
            (this->scan)->next_token(false);
            unique_ptr<Expr> term = this->parse_term();
            expr = make_unique<OpExpr>(c, move(expr), move(term));
        } else
            break;
    }
    return expr;
}

// Unused
unique_ptr<Expr> Parser::try_get_expr()
{
    unique_ptr<Expr> expr = this->parse_expr();
    unique_ptr<Token> rest = this->scan->next_token(false);
    if (rest)
        throw runtime_error("Extra input error");
    return expr;
}


unique_ptr<Program> Parser::parse_program()
{
    unique_ptr<Statement> statement;
    vector<unique_ptr<Statement>> statements;
    while (statement=this->parse_statement()) {
        unique_ptr<Token> t = this->scan->next_token(false);
        if (!t || !t->isSymbol(';'))
            throw runtime_error("line " + std::to_string(this->scan->last_line)
                             +  ", col " + std::to_string(this->scan->last_column)
                             +  ": ';' expected");
        statements.push_back(move(statement));
        this->line++;
    }
    return make_unique<Program>(move(statements));
}

unique_ptr<Statement> Parser::parse_statement()
{
    unique_ptr<Token> t = this->scan->next_token(false);
    unique_ptr<Expr> e;
    unique_ptr<Token> next;
    string name;
    if (t && t->isPrint()) {
        t = this->scan->next_token(false);

        if (t && t->isSymbol('('))
            e = this->parse_expr();
        else
            throw runtime_error("line " + std::to_string(this->scan->last_line)
                             +  ", col " + std::to_string(this->scan->last_column)
                             +  ": '(' expected");

        t = this->scan->next_token(false);
        if (!t || !t->isSymbol(')'))
            throw runtime_error("line " + std::to_string(this->scan->last_line)
                             +  ", col " + std::to_string(this->scan->last_column)
                             +  ": ')' expected");

        return make_unique<Print>(move(e));
    }

    if (t && t->isId(&name)) {
        next = this->scan->next_token(false);
        if (next && next->isSymbol('=')) {
            e = parse_expr();
            return make_unique<Assignment>(move(t), move(e));
        } else {
            throw runtime_error("line " + std::to_string(this->scan->last_line)
                             +  ", col " + std::to_string(this->scan->last_column)
                             +  ": '=' expected after a variable");
        }
    }

    if (t)
        throw runtime_error("line " + std::to_string(this->scan->last_line)
                         +  ", col " + std::to_string(this->scan->last_column)
                         +  ": Id or print token expected");
    return nullptr;
}
