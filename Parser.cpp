#include "head.h"

Parser::Parser(Scanner* scan)
{ 
    this->scan = scan;
    ifstream file{ "precedence", ios::in };
    string op;
    int prec;
    while (file >> op >> prec) {
        this->precedence.insert({op, prec });
    } 

}

Type *Parser::parse_type()
{
    unique_ptr<Token> type = this->scan->next_token();
    Type *t;
    type->isType(&t);
    Token *peek;
    while ((peek = this->scan->peek_token()) && peek->isSymbol("[")) {
        this->scan->next_token();
        t = ArrayType::make(t);
        this->expect("]");
    }
    return t;
}

vector<unique_ptr<Expr>> Parser::parse_arguments()
{
    vector<unique_ptr<Expr>> result;
    Token *t;
    while ((t=this->scan->peek_token()) && !t->isSymbol(")")) {
        unique_ptr<Expr> e = this->parse_expr();
        if (e)
            result.push_back(move(e));
        t = this->scan->peek_token();
        if (t && !t->isSymbol(","))
            break;
        this->scan->next_token();
    }
    return result;
}

void Parser::report_error(string message)
{
    throw runtime_error("Parser error line " + std::to_string(this->scan->last_line)
                    +  ", col " + std::to_string(this->scan->last_column)
                    +  ": " + message);
}

bool Parser::isBinary(string op) { return op != "!"; }

void Parser::expect(string c)
{
    unique_ptr<Token> next = this->scan->next_token();
    if (!next || !next->isSymbol(c))
        this->report_error("'" + c + "' expected");
}

unique_ptr<Expr> Parser::parse_unary() 
{
    Token *peek = this->scan->peek_token();
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    if (peek && peek->isSymbol("!")) {
        this->scan->next_token();
        unique_ptr<Expr> expr = this->parse_unary();
        return make_unique<OpExpr>("!", move(expr), nullptr, line, col);
    }
    if (peek && peek->isOper("-")) {
        this->scan->next_token();
        unique_ptr<Expr> expr = this->parse_unary();
        unique_ptr<Expr> zero = make_unique<NumLiteral>(0, -1, -1);
        return make_unique<OpExpr>("-", move(zero), move(expr), line, col);
    }
    if (peek && peek->isSymbol("(")) {
        this->scan->next_token();
        peek = this->scan->peek_token();
        Type *type;
        if (peek && peek->isType(&type)) {
            this->scan->next_token();
            this->expect(")"); 
            unique_ptr<Expr> expr = this->parse_unary();
            return make_unique<TypeCastExpr>(type, move(expr), line, col);
        } else {
            unique_ptr<Expr> expr = this->parse_expr();
            this->expect(")"); 
            return expr;
        }
    }
    return this->parse_primary();
}

unique_ptr<Expr> Parser::parse_primary() {
    Type *type;
    long long int num;
    bool b;
    Id id;
    int line = this->scan->last_line;
    int col = this->scan->last_column;
    char wc;
    string s;
    unique_ptr<Token> t = this->scan->next_token();
    unique_ptr<Expr> prim;
    if (t->isSymbol("(")) {
        prim = this->parse_expr();
        this->expect(")"); 
    } else if (t->isNum(&num)) {
        prim = make_unique<NumLiteral>(num, line, col);
    } else if (t->isBool(&b)) {
        prim =  make_unique<BoolLiteral>(b, line, col);
    } else if (t->isId(&id)) {
        prim = make_unique<Variable>(id, line, col);
    }else if (t->isChar(&wc)) {
        prim = make_unique<CharLiteral>(wc, line, col);
    } else if (t->isString(&s)) {
        prim = make_unique<StringLiteral>(s, line, col);
    } else if (t->isType(&type) && type == &Int ) {
        this->expect(".");
        unique_ptr<Token> next = this->scan->next_token();
        if (next && next->isId("Parse")) {
            this->expect("(");
            vector<unique_ptr<Expr>> arguments = this->parse_arguments();
            this->expect(")");  
            prim = make_unique<IntParseExpr>(move(arguments), line, col);
        } else
            this->report_error("Parse call expected");
    } else if (t->isKeyword("new")) {
        t = this->scan->next_token();
        if (t && t->isType(&type)) {
            if (type == &String) {
                Token *peek = this->scan->peek_token();
                if (peek->isSymbol("(")) {
                    this->scan->next_token();
                    vector<unique_ptr<Expr>> arguments = 
                        this->parse_arguments();
                    this->expect(")");
                    return make_unique<NewStrExpr>(move(arguments));
                } else if (peek->isSymbol("[")) {
                    goto arr_label;
                } else
                    this->report_error("'(' or '[' expected");
            } else if (type == &Empty) {
                this->report_error("can't declare null array");
            } else {
arr_label:
                this->expect("[");
                unique_ptr<Expr> expr = this->parse_expr();
                this->expect("]");
                Type *arr_t = ArrayType::make(type);
                Token *peek;
                while ((peek=this->scan->peek_token())&&peek->isSymbol("[")) {
                    this->scan->next_token();
                    arr_t = ArrayType::make(arr_t);
                    this->expect("]");
                }
                return make_unique<NewArrExpr>(arr_t, move(expr), line, col);
            }
        } else
            this->report_error("type expected");
    } else if (t->isKeyword("null")) {
        prim = make_unique<NullExpr>(line, col);
    } else
        this->report_error("Syntax error");
    Token *peek;
    while ((peek=this->scan->peek_token()) && (peek->isSymbol(".") || peek->isSymbol("[") || 
            peek->isOper("++") || peek->isOper("--"))) {
        if (peek && peek->isSymbol("[")) {
            this->scan->next_token();
            unique_ptr<Expr> expr = this->parse_expr();
            this->expect("]");
            prim =  make_unique<ElemAccessExpr>(move(prim), move(expr), line, col);
        } else if (peek && peek->isSymbol(".")) {
            this->scan->next_token();
            unique_ptr<Token> tok = this->scan->next_token();
            if (tok && tok->isId("Length"))
                return make_unique<LengthExpr>(move(prim), line, col);
            else if (tok && tok->isId("Substring")) {
                this->expect("(");
                vector<unique_ptr<Expr>> args = this->parse_arguments();
                this->expect(")");  
                prim = make_unique<SubstrExpr>(move(prim), move(args), line, col);
            } else
                this->report_error("Unrecognized function call");
        } else if (peek->isOper("++")) {
            this->scan->next_token();
            return make_unique<IncExpr>(true, move(prim), line, col);
        } else if (peek->isOper("--")) {
            this->scan->next_token();
            return make_unique<IncExpr>(false, move(prim), line, col);
        } else
            this->report_error("'[' or '.' expected");
        
    }
    return prim;
}

unique_ptr<Expr> Parser::parse_expression(unique_ptr<Expr> lhs, 
                                            int min_precedence)
{
    Token *t = this->scan->peek_token();
    string op_1;
    while (t && t->isOper(&op_1) && 
            isBinary(op_1) && precedence[op_1] >= min_precedence) {
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        this->scan->next_token();
        unique_ptr<Expr> rhs = this->parse_unary();
        t = this->scan->peek_token();
        string op_2;
        while (t && t->isOper(&op_2) &&
                isBinary(op_2) && precedence[op_2] > precedence[op_1]) {
            rhs = this->parse_expression(move(rhs), precedence[op_2]);
            t = this->scan->peek_token();
        }
        lhs = make_unique<OpExpr>(op_1, move(lhs), move(rhs), line, col);
    }
    return lhs;
}

unique_ptr<Expr> Parser::parse_expr()
{
    return move(this->parse_expression(move(parse_unary()), 0));
}

unique_ptr<Declaration> Parser::try_parse_declaration()
{
    Type *type;
    Token *t = this->scan->peek_token();
    Id name;
    if (t && t->isType(&type)) {
        type = this->parse_type();
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
        unique_ptr<Expr> expr = this->parse_primary();
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        this->expect("=");
        unique_ptr<Expr> e = this->parse_expr();
        return make_unique<Assignment>(move(expr), move(e), line, col);
    }
    return nullptr;
}

unique_ptr<Statement> Parser::parse_expression_statement_or_assignment()
{
    unique_ptr<Statement> dec = this->try_parse_expression_statement_or_assignment();
    if (!dec)
        this->report_error("Assignment or inc expr expected");
    return dec;
}

unique_ptr<Statement> Parser::try_parse_expression_statement_or_assignment()
{
    Token *t = this->scan->peek_token();
    Id name;
    if (t && t->isId(&name)) {
        unique_ptr<Expr> expr = this->parse_primary();
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        if (dynamic_cast<IncExpr *>(expr.get()))
            return make_unique<ExpressionStatement>(move(expr), line, col);
        this->expect("=");
        unique_ptr<Expr> e = this->parse_expr();
        return make_unique<Assignment>(move(expr), move(e), line, col);
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
        unique_ptr<Statement> iter = this->parse_expression_statement_or_assignment();
        this->expect(")");
        unique_ptr<Statement> body = this->parse_statement();
        return make_unique<ForStatement>(move(init), move(cond), 
                                        move(iter), move(body));
    }
    return nullptr;
}

unique_ptr<BreakStatement> Parser::try_parse_break()
{
    Token *t = this->scan->peek_token();
    if (t && t->isKeyword("break")) {
        this->scan->next_token();
        int line = this->scan->last_line;
        int col = this->scan->last_column;
        this->expect(";");
        return make_unique<BreakStatement>(line, col);
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

    // Assignment or incexpr
    if ((s=this->try_parse_expression_statement_or_assignment())) {
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
    
    // For
    if ((s=this->try_parse_for()))
        return s;

    // Break
    if ((s=this->try_parse_break()))
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
