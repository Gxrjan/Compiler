#ifndef HEAD_H
#define HEAD_H
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <curses.h>
#include <map>
#include <memory>
using namespace std;

struct TreeNode {
    // some 
};


class Expr {  // abstract base class
  public:
    virtual string to_string() = 0;
    virtual bool isLiteral(int *num) { return false; }
    virtual bool isOpExpr(char *op, Expr **left, Expr **right) { return false; }
};

class Literal : public Expr {
  public:
    int num;
    Literal(int num);
    string to_string() override;
    bool isLiteral(int *num);
};

class OpExpr : public Expr {
  public:
    char op;
    unique_ptr<Expr> left, right;
    OpExpr(char op, unique_ptr<Expr> left, unique_ptr<Expr> right);
    string to_string() override;
    bool isOpExpr(char *c, Expr **left, Expr **right);
};


class Token {
  public:
    virtual string to_string() = 0;
    virtual bool isNum(int *num) { return false; }
    virtual bool isOper(char *op) { return false; }
    virtual bool isParen(char *p) { return false; }
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

struct Scanner {
    unique_ptr<Token> next = nullptr;
    Token *peek_token();
    unique_ptr<Token> next_token();
};

struct Parser {
    Scanner* scan;
    Parser(Scanner* scan);
    unique_ptr<Expr> parse_factor();
    unique_ptr<Expr> parse_term();
    unique_ptr<Expr> parse_expr();
    unique_ptr<Expr> try_get_expr();
};

class Translator {
  public:
    string translate_expr(unique_ptr<Expr> expr);
    void translate_expr(string *s, Expr *expr);
};
#endif
