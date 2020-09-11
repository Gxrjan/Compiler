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
    long long num;
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
        string s;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&s) && (s=="*" || s=="/" || s == "%")) {
            (this->scan)->next_token();
            unique_ptr<Expr> factor = this->parse_factor();
            expr = make_unique<OpExpr>(s, move(expr), move(factor), line, col);
        } else
            break;
    }
    return expr;
}

unique_ptr<Expr> Parser::parse_add_expr()
{
    unique_ptr<Expr> expr = parse_term();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    while (true) {
        string s;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&s) && (s=="+" || s=="-")) {
            (this->scan)->next_token();
            unique_ptr<Expr> term = this->parse_term();
            expr = make_unique<OpExpr>(s, move(expr), move(term), line, col);
        } else
            break;
    }
    return expr;
}

unique_ptr<Expr> Parser::parse_comp_expr()
{
    unique_ptr<Expr> expr = parse_add_expr();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    while (true) {
        string s;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&s) && (s=="<" || s==">" || s=="<=" || s==">=")) {
            (this->scan)->next_token();
            unique_ptr<Expr> term = this->parse_add_expr();
            expr = make_unique<OpExpr>(s, move(expr), move(term), line, col);
        } else
            break;
    }
    return expr;
}

unique_ptr<Expr> Parser::parse_expr()
{
    unique_ptr<Expr> expr = parse_comp_expr();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    while (true) {
        string s;
        Token* t = (this->scan)->peek_token();
        if (t && t->isOper(&s) && (s=="==" || s=="!=")) {
            (this->scan)->next_token();
            unique_ptr<Expr> term = this->parse_comp_expr();
            expr = make_unique<OpExpr>(s, move(expr), move(term), line, col);
        } else
            break;
    }
    return expr;
}

unique_ptr<Declaration> Parser::try_parse_declaration()
{
    Type type;
    Token *t = this->scan->peek_token();
    Id name;
    if (t && t->isType(&type)) {
        this->scan->next_token();
        unique_ptr<Token> next = this->scan->next_token();
        if (next && next->isId(&name)) {
            int line = this->scan->last_line;
            int col = this->scan->last_column;
            this->expect("=");
            unique_ptr<Expr> e = this->parse_expr();
            return make_unique<Declaration>(type, name, move(e), line, col);
        } else {
            this->report_error("Name of the variable should follow the type");
        }
    }
    return nullptr;
}

unique_ptr<Declaration> Parser::parse_declaration()
{
    unique_ptr<Declaration> dec = this->try_parse_declaration();
    if (!dec)
        this->report_error("Declaration expected");
    return dec;
}

unique_ptr<Assignment> Parser::try_parse_assignment()
{
    Token *t = this->scan->peek_token();
    Id name;
    if (t && t->isId(&name)) {
        this->scan->next_token();
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        this->expect("=");
        unique_ptr<Expr> e = this->parse_expr();
        return make_unique<Assignment>(name, move(e), line, col);
    }
    return nullptr;
}

unique_ptr<Assignment> Parser::parse_assignment()
{
    unique_ptr<Assignment> asgn = this->try_parse_assignment();
    if (!asgn)
        this->report_error("Assignment expected");
    return asgn;
}

unique_ptr<Print> Parser::try_parse_print()
{
    Token *t = this->scan->peek_token();
    if (t && t->isKeyword("print")) {
        this->scan->next_token();
        this->expect("(");
        unique_ptr<Expr> e = this->parse_expr();
        this->expect(")");
        return make_unique<Print>(move(e));
    }
    return nullptr;
}

unique_ptr<IfStatement> Parser::try_parse_if()
{
    Token *t = this->scan->peek_token();
    if (t && t->isKeyword("if")) {
        this->scan->next_token();
        this->expect("(");
        unique_ptr<Expr> cond = this->parse_expr();
        this->expect(")");
        unique_ptr<Statement> if_s = this->parse_statement();
        unique_ptr<Statement> else_s;
        Token *next = this->scan->peek_token();
        if (next && next->isKeyword("else")) {
            this->scan->next_token();
            else_s = this->parse_statement();
        }
        return make_unique<IfStatement>(move(cond), move(if_s), move(else_s)); 
    }
    return nullptr;
}

unique_ptr<WhileStatement> Parser::try_parse_while()
{
    Token *t = this->scan->peek_token();
    if (t && t->isKeyword("while")) {
        this->scan->next_token();
        this->expect("(");
        unique_ptr<Expr> cond = this->parse_expr();
        this->expect(")");
        unique_ptr<Statement> statement = this->parse_statement();
        return make_unique<WhileStatement>(move(cond), move(statement));
    }
    return nullptr;
}

unique_ptr<ForStatement> Parser::try_parse_for()
{
    Token *t = this->scan->peek_token();
    if (t && t->isKeyword("for")) {
        this->scan->next_token();
        this->expect("(");
        unique_ptr<Declaration> init = this->parse_declaration();
        this->expect(";");
        unique_ptr<Expr> cond = this->parse_expr();
        this->expect(";");
        unique_ptr<Assignment> iter = this->parse_assignment();
        this->expect(")");
        unique_ptr<Statement> body = this->parse_statement();
        return make_unique<ForStatement>(move(init), move(cond), 
                                        move(iter), move(body));
    }
    return nullptr;
}

unique_ptr<Statement> Parser::parse_statement()
{
    unique_ptr<Statement> s;

    // Declaration
    if ((s=this->try_parse_declaration())) {
        this->expect(";");
        return s;
    }

    // Assignment
    if ((s=this->try_parse_assignment())) {
        this->expect(";");
        return s;
    }

    // Print
    if ((s=this->try_parse_print())) {
        this->expect(";");
        return s;
    }
    
    // Block
    if ((s=this->try_parse_block()))
        return s;

    // If Else
    if ((s=this->try_parse_if()))
        return s;

    // While
    if ((s=this->try_parse_while()))
        return s;

    if ((s=this->try_parse_for()))
        return s;
    this->report_error("Statement expected");
    return nullptr; // Unreached
}


unique_ptr<Block> Parser::try_parse_block()
{
    Token *next = this->scan->peek_token();
    unique_ptr<Statement> statement;
    vector<unique_ptr<Statement>> statements;
    if (next && next->isSymbol("{")) {
        this->scan->next_token();
        while ((next=this->scan->peek_token())) {
            if (next->isSymbol("}"))
                break;
            statement = this->parse_statement();
            statements.push_back(move(statement));
        }
        this->expect("}");
        return make_unique<Block>(move(statements));
    }
    return nullptr;
}

unique_ptr<Block> Parser::parse_outer_block()
{
    unique_ptr<Statement> statement;
    vector<unique_ptr<Statement>> statements;
    while (this->scan->peek_token()) {
        statement = this->parse_statement();
        statements.push_back(move(statement));
    }
    return make_unique<Block>(move(statements));
}

unique_ptr<Program> Parser::parse_program()
{
    unique_ptr<Block> block = this->parse_outer_block();
    return make_unique<Program>(move(block));
}
