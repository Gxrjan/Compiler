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
};

class Literal : public Expr {
  public:
    int num;
    Literal(int num);
};

class OpExpr : public Expr {
  public:
    char op;
    unique_ptr<Expr> left, right;
    OpExpr(char op);
};


class Token {
  public:
    virtual string to_string() = 0;
    virtual string get_class() = 0;
};

class NumToken : public Token {
  public:
    int num;
    NumToken(int num);
    string to_string() override;
    string get_class() override;
};

class OperToken : public Token {
  public:
    char op;
    OperToken(char op);
    string to_string() override;
    string get_class() override;
};

class ParenToken : public Token {
  public:
    char paren;
    ParenToken(char paren);
    string to_string() override;
    string get_class() override;
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
};


#endif
