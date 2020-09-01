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




// TOKEN
class Token {
  public:
    virtual string to_string() = 0;
    virtual bool isNum(int *num) { return false; }
    virtual bool isOper(char *op) { return false; }
    virtual bool isParen(char *p) { return false; }
    virtual bool isId(string *name) { return false; }
    virtual bool isSymbol(char c) { return false; }
    virtual bool isPrint() { return false; }
};

class NumToken : public Token {
  public:
    int num;
    NumToken(int num);
    string to_string() override;
    bool isNum(int *num) override;
};

class OperToken : public Token {
  public:
    char op;
    OperToken(char op);
    string to_string() override;
    bool isOper(char *op) override;
};

class ParenToken : public Token {
  public:
    char paren;
    ParenToken(char paren);
    string to_string() override;
    bool isParen(char *p) override;
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
    char c;
    SymbolToken(char c);
    string to_string() override;
    bool isSymbol(char c) override;
};


class PrintToken : public Token {
  public:
    bool isPrint() override;
    string to_string() override;
};

// EXPRESSION
class Expr {  // abstract base class
  public:
    int line, col;
    Expr();
    Expr(int line, int col);
    virtual string to_string() = 0;
    virtual bool isLiteral(int *num) { return false; }
    virtual bool isOpExpr(char *op, Expr **left, Expr **right) { return false; }
    virtual bool isVariable(string *name) { return false; }
};

class Literal : public Expr {
  public:
    int num;
    Literal(int num);
    string to_string() override;
    bool isLiteral(int *num) override;
};

class OpExpr : public Expr {
  public:
    char op;
    unique_ptr<Expr> left, right;
    OpExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right);
    string to_string() override;
    bool isOpExpr(char *c, Expr **left, Expr **right) override;
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
    virtual string to_string() = 0;
    virtual bool isAssignment(IdToken** id, Expr** expr) { return false; }
    virtual bool isPrint(Expr** expr) { return false; }
};

class Assignment : public Statement {
  public:
    unique_ptr<Token> id;
    unique_ptr<Expr> expr;
    Assignment(unique_ptr<Token> id, unique_ptr<Expr> expr);
    string to_string() override;
    bool isAssignment(IdToken** id, Expr** expr) override;
    
};

class Print : public Statement {
  public:
    unique_ptr<Expr> expr;
    Print(unique_ptr<Expr> expr);
    string to_string() override;
    bool isPrint(Expr** expr) override;
};


// Program
class Program {
  public:
    vector<unique_ptr<Statement>> statements;
    Program(vector<unique_ptr<Statement>> statements);
    string to_string();
};


// Environment
class Env {
  public:
    set<string> variables;
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
    unique_ptr<Expr> parse_expr();
    unique_ptr<Expr> try_get_expr();
    unique_ptr<Statement> parse_statement();
    void check_expr(Expr *expr);
  public:
    Scanner* scan;
    Parser(Scanner* scan);
    unique_ptr<Program> parse_program();
    void check_program(Program *p);
};


class Checker {
    set<string> variables;
    void check_expr(Expr *expr);
    void report_error(int line, int col, string message);
  public:
    void check_program(Program *p);
};


// TRANSLATOR
class Translator {
  public:
    string translate_expr(Expr *expr);
    void translate_expr(string *s, Expr *expr);
};


#endif
