#include "head.h"

Parser::Parser(Scanner* scan)
{ 
    this->scan = scan; 
}

void Parser::report_error(string message)
{
    throw runtime_error("line " + std::to_string(this->scan->last_line)
                    +  ", col " + std::to_string(this->scan->last_column)
                    +  ": " + message);
}


void Parser::report_error(int line, int col, string message)
{
    throw runtime_error("line " + std::to_string(line)
                    +  ", col " + std::to_string(col)
                    +  ": " + message);
}

unique_ptr<Expr> Parser::parse_factor() 
{
    unique_ptr<Token> t = (this->scan)->next_token();
    int num;
    string name;
    if (t->isNum(&num)) {
        return make_unique<Literal>(num);
    }
    if (t->isId(&name)) {
        return make_unique<Variable>(name, this->scan->last_line, 
                                    this->scan->last_column);
    }

    if (t->isSymbol('(')) {
        unique_ptr<Expr> e = parse_expr();
        unique_ptr<Token> next = (this->scan)->next_token();
        if (!next || !next->isSymbol(')'))
            this->report_error("')' expected");
        return e;
    }
    this->report_error("syntax error");
    return nullptr;
}


unique_ptr<Expr> Parser::parse_term()
{
    unique_ptr<Expr> expr = parse_factor();
    while (true) {
        char c;
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
    while (true) {
        char c;
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


unique_ptr<Program> Parser::parse_program()
{
    unique_ptr<Statement> statement;
    vector<unique_ptr<Statement>> statements;
    while (statement=this->parse_statement()) {
        unique_ptr<Token> t = this->scan->next_token();
        if (!t || !t->isSymbol(';'))
            this->report_error("';' expected");
        statements.push_back(move(statement));
    }
    return make_unique<Program>(move(statements));
}

unique_ptr<Statement> Parser::parse_statement()
{
    unique_ptr<Token> t = this->scan->next_token();
    unique_ptr<Expr> e;
    string name;
    if (t && t->isPrint()) {
        t = this->scan->next_token();

        if (t && t->isSymbol('('))
            e = this->parse_expr();
        else
            this->report_error("'(' expected");

        t = this->scan->next_token();
        if (!t || !t->isSymbol(')'))
            this->report_error("')' expected");
        return make_unique<Print>(move(e));
    }

    if (t && t->isId(&name)) {
        unique_ptr<Token> next = this->scan->next_token();

        if (next && next->isSymbol('=')) {
            e = parse_expr();
            return make_unique<Assignment>(move(t), move(e));
        } else {
            this->report_error("'=' expected after a variable");
        }
    }

    if (t)
        this->report_error("Id or Print token expected");
    return nullptr;
}


void Parser::check_expr(Expr *expr)
{
    int num;
    if (expr->isLiteral(&num))
        return;

    string name;
    if (expr->isVariable(&name)) {
        if (this->variables.find(name) == this->variables.end())
            this->report_error(expr->line, expr->col, "Variable not declared");
    }
    
    Expr* left;
    Expr* right;
    char c;
    if (expr->isOpExpr(&c, &left, &right)) {
        this->check_expr(left);
        this->check_expr(right);
    }
}

void Parser::check_program(Program *p)
{
    for (auto &s : p->statements) {
        string name;
        Expr *expr;
        IdToken *id;
        if (s->isPrint(&expr)) {
            this->check_expr(expr);
        }

        if (s->isAssignment(&id, &expr)) {
            this->check_expr(expr);
            this->variables.insert(id->name);
        }

    }
}


