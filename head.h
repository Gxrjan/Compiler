#ifndef HEAD_H
#define HEAD_H
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <curses.h>
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <set>

using namespace std;
using Id = string;
enum class Type { Bool, Int };
enum class Operation {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    L,
    G,
    Le,
    Ge,
    E,
    Ne
};

class TypeConverter {
  public:
    static string enum_to_string(Type t)
    {
        switch(t) {
            case Type::Int:
                return "int";
                break;
            case Type::Bool:
                return "bool";
                break;
            default:
                throw runtime_error("Unknown Type");
                break;
        }
    }

    static Operation string_to_operation(string op)
    {
        if (op == "+") return Operation::Add;
        if (op == "-") return Operation::Sub;
        if (op == "*") return Operation::Mul;
        if (op == "/") return Operation::Div;
        if (op == "%") return Operation::Mod;
        if (op == "<") return Operation::L;
        if (op == ">") return Operation::G;
        if (op == "<=")return Operation::Le;
        if (op == ">=")return Operation::Ge;
        if (op == "==")return Operation::E;
        if (op == "!=")return Operation::Ne;
        throw runtime_error("Unknown Operation");
    }

    static string operation_to_string(Operation op)
    {
        switch (op) {
            case Operation::L:
                return "l";
                break;
            case Operation::G:
                return "g";
                break;
            case Operation::Le:
                return "le";
                break;
            case Operation::Ge:
                return "ge";
                break;
            case Operation::E:
                return "e";
                break;
            case Operation::Ne:
                return "ne";
                break;
            default:
                throw runtime_error("Uknown comparing operation");
                break;
        }
    }
};


// TOKEN
class Token {
  public:
    virtual string to_string() = 0;
    virtual bool isNum(long long *num) { return false; }
    virtual bool isBool(bool *b) { return false; }
    virtual bool isOper(string *op) { return false; }
    virtual bool isId(string *name) { return false; }
    virtual bool isSymbol(string s) { return false; }
    virtual bool isKeyword(string name) { return false; }
    virtual bool isType(Type *t) { return false; }
};

class NumToken : public Token {
  public:
    long long num;
    NumToken(long long num);
    string to_string() override;
    bool isNum(long long *num) override;
};

class BoolToken : public Token {
  public:
    bool b;
    BoolToken(bool b);
    string to_string() override;
    bool isBool(bool *b) override;
};

class OperToken : public Token {
  public:
    string op;
    OperToken(string op);
    string to_string() override;
    bool isOper(string *op) override;
};


class IdToken : public Token {
  public:
    string name;
    IdToken(string name);
    string to_string() override;
    bool isId(string *name) override;
};


class SymbolToken : public Token {
  public:
    string s;
    SymbolToken(string s);
    string to_string() override;
    bool isSymbol(string s) override;
};


class KeywordToken : public Token {
  public:
    string name;
    KeywordToken(string name);
    string to_string() override;
    bool isKeyword(string name) override;
};



class TypeToken : public Token {
  public:
    Type type;
    TypeToken(Type t);
    bool isType(Type *t) override;
    string to_string() override;
};


// EXPRESSION
class Expr {  // abstract base class
  public:
    int line, col;
    Expr();
    Expr(int line, int col);
    virtual string to_string() = 0;
    virtual bool isNumLiteral(long long *num) { return false; }
    virtual bool isBoolLiteral(bool *b) { return false; }
    virtual bool isOpExpr(string *op, Expr **left, Expr **right) { return false; }
    virtual bool isVariable(string *name) { return false; }
};

class NumLiteral : public Expr {
  public:
    long long num;
    NumLiteral(long long num, int line, int col);
    string to_string() override;
    bool isNumLiteral(long long *num) override;
};

class BoolLiteral : public Expr {
  public:
    bool b;
    BoolLiteral(bool b, int line, int col);
    string to_string() override;
    bool isBoolLiteral(bool *b) override;
};


class OpExpr : public Expr {
  public:
    string op;
    unique_ptr<Expr> left, right;
    OpExpr(string op, unique_ptr<Expr> left, unique_ptr<Expr> right, int line, int col);
    string to_string() override;
    bool isOpExpr(string *c, Expr **left, Expr **right) override;
};

class Variable : public Expr {
  public:
    string name;
    Variable(string name, int line, int col);
    string to_string() override;
    bool isVariable(string *name) override;
};




// Statement
class Statement {
  public:
    int line, col;
    Statement();
    Statement(int line, int col);
    virtual string to_string() = 0;
    virtual bool isAssignment(Id* id, Expr** expr) { return false; }
    virtual bool isPrint(Expr** expr) { return false; }
    virtual bool isDeclaration(Type *t, Id *id, Expr **expr) { return false; }
    virtual bool isBlock(vector<Statement*> statements) { return false; }
    virtual bool isIfStatement(Expr **cond, Statement **if_s, Statement **else_s) { return false; }
    virtual bool isWhileStatement(Expr **cond, Statement **s) { return false; }
    //virtual bool isForStatement(Statement **init, Expr **cond, Statement **iter) { return false; };
};

class Assignment : public Statement {
  public:
    Id id;
    unique_ptr<Expr> expr;
    Assignment(Id id, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
    bool isAssignment(Id* id, Expr** expr) override;
};

class Print : public Statement {
  public:
    unique_ptr<Expr> expr;
    Print(unique_ptr<Expr> expr);
    string to_string() override;
    bool isPrint(Expr** expr) override;
};

class Declaration : public Statement {
  public:
    Type type;
    Id id;
    unique_ptr<Expr> expr;
    Declaration(Type t, Id id, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
    bool isDeclaration(Type *t, Id *id, Expr **expr) override;
};



class Block : public Statement {
  public:
    map<Id, Declaration *> variables;
    Block *parent;
    vector<unique_ptr<Statement>> statements;
    Block(vector<unique_ptr<Statement>> statements);
    bool isBlock(vector<Statement*> statements);
    string to_string() override;
};

class IfStatement : public Statement {
  public:
    unique_ptr<Expr> cond;
    unique_ptr<Statement> if_s;
    unique_ptr<Statement> else_s;
    IfStatement(unique_ptr<Expr> cond, unique_ptr<Statement> if_s, unique_ptr<Statement> else_s);
    string to_string() override;
    bool isIfStatement(Expr **cond, Statement **if_s, Statement **else_s) override;
};

class WhileStatement : public Statement {
  public:
    unique_ptr<Expr> cond;
    unique_ptr<Statement> statement;
    WhileStatement(unique_ptr<Expr> cond, unique_ptr<Statement> s);
    string to_string() override;
    bool isWhileStatement(Expr **cond, Statement **statement) override;
};

class ForStatement : public Statement {
  public:
    unique_ptr<Declaration> init;
    unique_ptr<Expr> cond;
    unique_ptr<Assignment> iter;
    unique_ptr<Statement> body;
    ForStatement(unique_ptr<Declaration> init, unique_ptr<Expr> cond, unique_ptr<Assignment> iter, unique_ptr<Statement> body);
    string to_string() override;
    //bool isForStatement(Declaration **init, Expr **cond, Assignment **iter, Statement **body) override;
};


// Program
class Program {
  public:
    unique_ptr<Block> block;
    Program(unique_ptr<Block> block);
    string to_string();
};


// SCANNER
class Scanner {
    char getc();
    void report_error(string message);
    int line = 1;
    int column = 1;
  public:
    int last_line = 1;     // Line number of the last token read or peeked 
    int last_column = 1;   // Column number of the last token read or peeked
    unique_ptr<Token> next = nullptr;
    Token *peek_token();
    unique_ptr<Token> next_token();
};


// PARSER
class Parser {
    void report_error(string message);
    unique_ptr<Expr> parse_factor();
    unique_ptr<Expr> parse_term();
    unique_ptr<Expr> parse_add_expr();
    unique_ptr<Expr> parse_comp_expr();
    unique_ptr<Expr> parse_expr();
    unique_ptr<Declaration> try_parse_declaration();
    unique_ptr<Declaration> parse_declaration();
    unique_ptr<Assignment> try_parse_assignment();
    unique_ptr<Assignment> parse_assignment();
    unique_ptr<Print> try_parse_print();
    unique_ptr<IfStatement> try_parse_if();
    unique_ptr<WhileStatement> try_parse_while();
    unique_ptr<ForStatement> try_parse_for();
    unique_ptr<Statement> parse_statement();
    unique_ptr<Block> try_parse_block();
    unique_ptr<Block> parse_outer_block();
    void check_expr(Expr *expr);
    void expect(string c);
  public:
    Scanner* scan;
    Parser(Scanner* scan);
    unique_ptr<Program> parse_program();
    void check_program(Program *p);
};

// CHECKER
class Checker {
    Declaration *look_up(Id id, Block *b);
    Type check_expr(Expr *expr, Block *b);
    void check_declaration(Declaration *dec, Block *b);
    void check_assignment(Assignment *asgn, Block *b);
    void check_if_statement(IfStatement *st, Block *b);
    void check_while_statement(WhileStatement *st, Block *b);
    void check_for_statement(ForStatement *for_s, Block *b);
    void check_statement(Statement *s, Block *b);
    void check_block(Block *b);
    void report_error(int line, int col, string message);
  public:
    void check_program(Program *p);
};


// TRANSLATOR
class Translator {
    int label_id = 0;
    set<Id> variables;
    void translate_expr(string *s, Expr *expr);
    void translate_statement(string *s, Statement *statement);
    void translate_block(string *s, Block *b);
  public:
    string translate_program(Program *p);
};


#endif
