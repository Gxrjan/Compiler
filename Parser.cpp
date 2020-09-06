#include "head.h"

Parser::Parser(Scanner* scan)
{ 
    this->scan = scan; 
}

void Parser::report_error(string message)
{
    throw runtime_error("Parser error line " + std::to_string(this->scan->last_line)
                    +  ", col " + std::to_string(this->scan->last_column)
                    +  ": " + message);
}


void Parser::expect(string c)
{
    unique_ptr<Token> next = this->scan->next_token();
    if (!next || !next->isSymbol(c))
        this->report_error("'" + c + "' expected");
}


unique_ptr<Expr> Parser::parse_factor() 
{
    unique_ptr<Token> t = (this->scan)->next_token();
    int num;
    bool b;
    string name;
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    if (t->isNum(&num)) {
        return make_unique<NumLiteral>(num, line, col);
    }
    if (t->isBool(&b))
        return make_unique<BoolLiteral>(b, line, col);
    if (t->isId(&name)) {
        return make_unique<Variable>(name, this->scan->last_line, 
                                    this->scan->last_column);
    }

    if (t->isSymbol("(")) {
        unique_ptr<Expr> e = parse_expr();
        this->expect(")");
        return e;
    }
    this->report_error("syntax error");
    return nullptr;
}


unique_ptr<Expr> Parser::parse_term()
{
    unique_ptr<Expr> expr = parse_factor();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    while (true) {
        string c;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&c) && (c=="*" || c=="/")) {
            (this->scan)->next_token();
            unique_ptr<Expr> factor = this->parse_factor();
            expr = make_unique<OpExpr>(c, move(expr), move(factor), line, col);
        } else
            break;
    }
    return expr;
}
unique_ptr<Expr> Parser::parse_expr()
{
    unique_ptr<Expr> expr = parse_term();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    while (true) {
        string c;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&c) && (c=="+" || c=="-")) {
            (this->scan)->next_token();
            unique_ptr<Expr> term = this->parse_term();
            expr = make_unique<OpExpr>(c, move(expr), move(term), line, col);
        } else
            break;
    }
    return expr;
}



unique_ptr<Statement> Parser::parse_statement()
{
    unique_ptr<Token> t = this->scan->next_token();
    unique_ptr<Token> next;
    unique_ptr<Expr> e;
    Type type;
    string name;
    

    // Declaration
    if (t && t->isType(&type)) {
        t = this->scan->next_token();
        if (t && t->isId(&name)) {
            int line = this->scan->last_line;
            int col = this->scan->last_column;
            this->expect("=");
            e = this->parse_expr();
            this->expect(";");
            return make_unique<Declaration>(type, name, move(e), line, col);
        } else {
            this->report_error("Name of the variable should follow the type");
        }
    }

    // Assignment
    if (t && t->isId(&name)) {
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        this->expect("=");
        e = this->parse_expr();
        this->expect(";");
        return make_unique<Assignment>(name, move(e), line, col);
    }
    
    // Print
    if (t && t->isPrint()) {
        this->expect("(");
        e = this->parse_expr();
        this->expect(")");
        this->expect(";");
        return make_unique<Print>(move(e));
    }

    // Block
    unique_ptr<Block> block;
    vector<unique_ptr<Statement>> statements;
    if (t && t->isSymbol("{")) {
        block = parse_block();
        this->expect("}");
        return block;
    }

    if (t)
        this->report_error("Statement expected");
    return nullptr;
}


unique_ptr<Block> Parser::parse_block()
{
    Token *next;
    unique_ptr<Statement> statement;
    vector<unique_ptr<Statement>> statements;
    while ((next=this->scan->peek_token())) {
        if (next->isSymbol("}"))
            break;
        statement = this->parse_statement();
        statements.push_back(move(statement));
    }
    return make_unique<Block>(move(statements));
}

unique_ptr<Program> Parser::parse_program()
{
    unique_ptr<Block> block = this->parse_block();
    return make_unique<Program>(move(block));
}
