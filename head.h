#ifndef HEAD_H
#define HEAD_H
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <curses.h>
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <set>
#include <cstring>

using namespace std;
using Id = string;
enum class Type { Bool, Int, Char, String };
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
    Ne,
    And,
    Or
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
            case Type::Char:
                return "char";
                break;
            case Type::String:
                return "string";
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
        if (op == "&&")return Operation::And;
        if (op == "||")return Operation::Or;
        throw runtime_error("Unknown Operation");
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
    virtual bool isId(string name) { return false; }
    virtual bool isSymbol(string s) { return false; }
    virtual bool isKeyword(string name) { return false; }
    virtual bool isType(Type *t) { return false; }
    virtual bool isOper(string op) { return false; }
    virtual bool isChar(char *c) { return false; }
    virtual bool isString(string *s) { return false; }
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

class StringToken : public Token {
  public:
    string s;
    StringToken(string s);
    string to_string() override;
    bool isString(string *s) override;
};

class OperToken : public Token {
  public:
    string op;
    OperToken(string op);
    string to_string() override;
    bool isOper(string *op) override;
    bool isOper(string op) override;
};


class IdToken : public Token {
  public:
    string name;
    IdToken(string name);
    string to_string() override;
    bool isId(string *name) override;
    bool isId(string name) override;
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

class CharToken : public Token {
  public:
    char c;
    CharToken(char c);
    string to_string();
    bool isChar(char *c) override;
};


// EXPRESSION
class Expr {  // abstract base class
  public:
    Type type;
    int line, col;
    Expr();
    Expr(int line, int col);
    virtual string to_string() = 0;
    virtual bool isNumLiteral(long long *num) { return false; }
    virtual bool isBoolLiteral(bool *b) { return false; }
    virtual bool isOpExpr(string *op, Expr **left, Expr **right) { return false; }
    virtual bool isVariable(string *name) { return false; }
    virtual bool isElemAccessExpr(Expr **expr, Expr **index) { return false; }
    virtual bool isLengthExpr(Expr **expr) { return false; }
    virtual bool isTypeCastExpr(Type *t, Expr **expr) { return false; }
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

class CharLiteral : public Expr {
  public:
    char c;
    CharLiteral(char, int line, int col);
    string to_string() override;
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

class StringLiteral : public Expr {
  public:
    string s;
    StringLiteral(string s, int line, int col);
    string to_string() override;
};


class ElemAccessExpr : public Expr {
  public:
    unique_ptr<Expr> expr;
    unique_ptr<Expr> index;
    ElemAccessExpr(unique_ptr<Expr> expr, unique_ptr<Expr> index, int line, int col);
    string to_string() override;
    bool isElemAccessExpr(Expr **expr, Expr **index) override;
};

class LengthExpr : public Expr {
  public:
    unique_ptr<Expr> expr;
    LengthExpr(unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
    bool isLengthExpr(Expr **expr) override;
};

class TypeCastExpr : public Expr {
  public:
    Type type;
    unique_ptr<Expr> expr;
    TypeCastExpr(Type t, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
    bool isTypeCastExpr(Type *t, Expr **expr) override;
};


class SubstrExpr : public Expr {
  public:
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> arguments;
    SubstrExpr(unique_ptr<Expr> expr, vector<unique_ptr<Expr>> arguments, int line, int col);
    string to_string() override;
};

class IntParseExpr : public Expr {
  public:
    vector<unique_ptr<Expr>> arguments;
    IntParseExpr(vector<unique_ptr<Expr>> arguments, int line, int col);
    string to_string() override;
    
};

class NewStrExpr : public Expr {
  public:
    vector<unique_ptr<Expr>> arguments;
    NewStrExpr(vector<unique_ptr<Expr>> arguments);
    string to_string() override;
};


// Statement
class Statement {
  public:
    int line, col;
    Statement();
    Statement(int line, int col);
    virtual string to_string() = 0;
};

class Assignment : public Statement {
  public:
    Id id;
    unique_ptr<Expr> expr;
    Assignment(Id id, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
};

class Print : public Statement {
  public:
    unique_ptr<Expr> expr;
    Print(unique_ptr<Expr> expr);
    string to_string() override;
};

class Declaration : public Statement {
  public:
    Type type;
    Id id;
    unique_ptr<Expr> expr;
    Declaration(Type t, Id id, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
};



class Block : public Statement {
  public:
    map<Id, Declaration *> variables;
    Block *parent;
    vector<unique_ptr<Statement>> statements;
    Block(vector<unique_ptr<Statement>> statements);
    string to_string() override;
};

class IfStatement : public Statement {
  public:
    unique_ptr<Expr> cond;
    unique_ptr<Statement> if_s;
    unique_ptr<Statement> else_s;
    IfStatement(unique_ptr<Expr> cond, unique_ptr<Statement> if_s, unique_ptr<Statement> else_s);
    string to_string() override;
};

class WhileStatement : public Statement {
  public:
    unique_ptr<Expr> cond;
    unique_ptr<Statement> statement;
    WhileStatement(unique_ptr<Expr> cond, unique_ptr<Statement> s);
    string to_string() override;
};

class ForStatement : public Statement {
  public:
    unique_ptr<Declaration> init;
    unique_ptr<Expr> cond;
    unique_ptr<Assignment> iter;
    unique_ptr<Statement> body;
    ForStatement(unique_ptr<Declaration> init, unique_ptr<Expr> cond, unique_ptr<Assignment> iter, unique_ptr<Statement> body);
    string to_string() override;
};

class BreakStatement : public Statement {
  public:
    BreakStatement(int line, int col);
    string to_string() override;
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
    char next_char = -1;
    bool has_next = false;
    ifstream file;
    char getc();
    char peekc();
    void consume_ws();
    void report_error(string message);
    int line = 1;
    int column = 1;
  public:
    Scanner();
    Scanner(char *file_name);
    int last_line = 1;     // Line number of the last token read or peeked 
    int last_column = 1;   // Column number of the last token read or peeked
    unique_ptr<Token> next = nullptr;
    Token *peek_token();
    unique_ptr<Token> next_token();
};


// PARSER
class Parser {
    map<string, int> precedence;
    void report_error(string message);
    bool isBinary(string op);
    unique_ptr<Expr> parse_unary();
    unique_ptr<Expr> parse_primary();
    unique_ptr<Expr> parse_expression(unique_ptr<Expr> lhs, int prec);
    unique_ptr<Expr> parse_expr();
    unique_ptr<Declaration> try_parse_declaration();
    unique_ptr<Declaration> parse_declaration();
    unique_ptr<Assignment> try_parse_assignment();
    unique_ptr<Assignment> parse_assignment();
    vector<unique_ptr<Expr>> parse_arguments();
    unique_ptr<Print> try_parse_print();
    unique_ptr<IfStatement> try_parse_if();
    unique_ptr<WhileStatement> try_parse_while();
    unique_ptr<ForStatement> try_parse_for();
    unique_ptr<Statement> parse_statement();
    unique_ptr<Block> try_parse_block();
    unique_ptr<BreakStatement> try_parse_break();
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
    Type check_expr_type(Expr *expr, Block *b);
    void expect_type(Expr *e, Block *b, Type t);
    bool convertible_to_int(Type t);
    void check_declaration(Declaration *dec, Block *b);
    void check_assignment(Assignment *asgn, Block *b);
    void verify_assignment(Declaration *dec, Expr *expr, Block *b);
    void check_if_statement(IfStatement *st, Block *b, bool in_loop);
    void check_while_statement(WhileStatement *st, Block *b);
    void check_for_statement(ForStatement *for_s, Block *b);
    void check_statement(Statement *s, Block *b, bool in_loop);
    void check_block(Block *b, bool in_loop);
    Type check_compatability(OpExpr *expr, Block *b);
    void report_error(int line, int col, string message);
  public:
    void check_program(Program *p);
};


// TRANSLATOR
class Translator {
    int label_id = 0;
    int string_id = 0;
    set<Id> variables;
    map<StringLiteral *, int> strings;
    string concat_cc(Type left, Type right);
    void translate_num_literal(string *s, NumLiteral *l);
    void translate_bool_literal(string *s, BoolLiteral *l);
    void translate_char_literal(string *s, CharLiteral *l);
    void translate_string_literal(string *s, StringLiteral *l);
    void translate_elem_access_expr(string *s, ElemAccessExpr *expr);
    void translate_length_expr(string *s, LengthExpr *expr);
    void translate_type_cast_expr(string *s, TypeCastExpr *expr);
    void translate_substr_expr(string *s, SubstrExpr *expr);
    void translate_int_parse_expr(string *s, IntParseExpr *expr);
    void translate_new_str_expr(string *s, NewStrExpr *expr);
    void translate_variable(string *s, Variable *var);
    void translate_op_expr(string *s, OpExpr *expr);
    void translate_expr(string *s, Expr *expr);
    void translate_declaration(string *s, Declaration *dec);
    void translate_assignment(string *s, Assignment *asgn);
    void translate_print(string *s, Print *p);
    void translate_if_statement(string *s, IfStatement *st, string loop_end_label);
    void translate_while_statement(string *s, WhileStatement *st);
    void translate_for_statement(string *s, ForStatement *for_s);
    void translate_statement(string *s, Statement *statement, string loop_end_label);
    void translate_block(string *s, Block *b, string loop_end_label);
    string type_to_cc(Type t);
    string operation_to_cc(Operation op);
  public:
    string translate_program(Program *p);
};


#endif
