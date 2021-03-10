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
#include <stack>
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <stack>
// #include <algorithm>
using namespace std;
using Id = string;


class Type { 
  public:
    virtual string to_string()=0;
};

using g_type = Type*;

class BasicType : public Type {
    string name;
  public:
    BasicType(string name);
    string to_string() override;
};

extern BasicType Bool, Char, Int, String, Empty, Byte, Void;

static inline bool is_ref_type(g_type t) {
    return !(t==&Bool || t==&Int || t==&Char || t==&Byte);
}

class ArrayType : public Type {
    ArrayType(Type *base);

    
  public:
    static map<Type *, ArrayType*> array_types;  // int -> int[],  int[] -> int[][]
    Type *base;
    string to_string() override;
    static ArrayType *make(Type *base) {
        auto it = array_types.find(base);
        if (it != array_types.end())
            return it->second;

        return array_types[base] = new ArrayType(base);
    }
};

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

    static Type *get_base_type(Type *a) {
        if (a==&Bool || a==&Char || a==&Int || a==&String ||  a==&Void)
          return a;
        else { 
          auto arr = dynamic_cast<ArrayType*>(a);
          return get_base_type(arr->base);
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
    virtual bool isType(Type **t) { return false; }
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
    Type *type;
    TypeToken(Type *t);
    bool isType(Type **t) override;
    string to_string() override;
};

class CharToken : public Token {
  public:
    char c;
    CharToken(char c);
    string to_string() override;
    bool isChar(char *c) override;
};


// EXPRESSION
class Expr {  // abstract base class
  public:
    Type *type;
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
    virtual bool isTypeCastExpr(Type **t, Expr **expr) { return false; }
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
    Type *type;
    unique_ptr<Expr> expr;
    TypeCastExpr(Type *t, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
    bool isTypeCastExpr(Type **t, Expr **expr) override;
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


class NullExpr : public Expr {
  public:
    NullExpr(int line, int col);
    string to_string() override;
};


class NewArrExpr : public Expr {
  public:
    Type *type;
    unique_ptr<Expr> expr;
    NewArrExpr(Type *type, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
};

class IncExpr : public Expr {
  public:
    bool inc;
    unique_ptr<Expr> expr;
    IncExpr(bool inc, unique_ptr<Expr> expr, int line, int col);
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
    unique_ptr<Expr> id;
    unique_ptr<Expr> expr;
    Assignment(unique_ptr<Expr> id, unique_ptr<Expr> expr, int line, int col);
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
    Type *type;
    Id id;
    unique_ptr<Expr> expr;
    Declaration(Type *t, Id id, unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
};



class Block : public Statement {
  public:
    map<Id, Declaration *> variables;
    map<Id, string> optimized_arrays; // first: variable name, second: register containing the length
    Block *parent;
    vector<Block*> children;
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

class ExpressionStatement : public Statement {
  public:
    unique_ptr<Expr> expr;
    ExpressionStatement(unique_ptr<Expr> expr, int line, int col);
    string to_string() override;

};

class ForStatement : public Statement {
  public:
    unique_ptr<Declaration> init;
    unique_ptr<Expr> cond;
    unique_ptr<Statement> iter;
    unique_ptr<Statement> body;
    ForStatement(unique_ptr<Declaration> init, unique_ptr<Expr> cond, unique_ptr<Statement> iter, unique_ptr<Statement> body);
    string to_string() override;
};

class BreakStatement : public Statement {
  public:
    BreakStatement(int line, int col);
    string to_string() override;
};

class FunctionCall : public Expr {
  public:
    Id name;
    vector<unique_ptr<Expr>> args;
    FunctionCall(Id name, vector<unique_ptr<Expr>> args, int line, int col);
    string to_string() override;
};

class FunctionDefinition : public Statement {
  public:
    Id name;
    Type *ret_type;
    vector<pair<Type *, string>> params;
    unique_ptr<Block> body;
    // vector<string> globals_called;
    FunctionDefinition(Id name, Type *ret_type, vector<pair<Type *, string>> params, unique_ptr<Block> body, int line, int col);
    string to_string() override;
};

class ExternalDefinition : public Statement {
  public:
    unique_ptr<Statement> s;
    ExternalDefinition(unique_ptr<Statement> s, int line, int col);
    string to_string() override;
};

class ReturnStatement : public Statement {
  public:
    unique_ptr<Expr> expr;
    ReturnStatement(unique_ptr<Expr> expr, int line, int col);
    string to_string() override;
};



// Program
class Program {
  public:
    unique_ptr<Block> block;
    map<tuple<g_type, string, vector<g_type>>, size_t> overloads;
    // vector<string> globals;
    Program(unique_ptr<Block> block);
    string to_string();
};


// SCANNER
class Scanner {
    char next_char = -1;
    bool has_next = false;
    int line = 1;
    int column = 1;
    unique_ptr<Token> next;
    ifstream file;
    void report_error(string message);
    char getc();
    char peekc();
    void consume_ws();
  public:
    Scanner(char *file_name);
    int last_line = 1;     // Line number of the last token read or peeked 
    int last_column = 1;   // Column number of the last token read or peeked
    Token *peek_token();
    unique_ptr<Token> next_token();
};


// PARSER
class Parser {
    map<string, int> precedence;
    void report_error(string message);
    bool isBinary(string op);
    Type *parse_type();
    unique_ptr<Expr> parse_unary();
    unique_ptr<Expr> parse_primary();
    unique_ptr<Expr> parse_expression(unique_ptr<Expr> lhs, int prec);
    unique_ptr<Expr> parse_expr();
    unique_ptr<Declaration> try_parse_declaration();
    unique_ptr<Declaration> parse_declaration();
    unique_ptr<Assignment> try_parse_assignment();
    unique_ptr<Assignment> parse_assignment();
    vector<unique_ptr<Expr>> parse_arguments();
    unique_ptr<IfStatement> try_parse_if();
    unique_ptr<WhileStatement> try_parse_while();
    unique_ptr<ForStatement> try_parse_for();
    unique_ptr<Statement> parse_statement();
    unique_ptr<Block> try_parse_block();
    unique_ptr<BreakStatement> try_parse_break();
    unique_ptr<Block> parse_outer_block();
    unique_ptr<Statement> try_parse_expression_statement_or_assignment();
    unique_ptr<Statement> parse_expression_statement_or_assignment();
    unique_ptr<Expr> consume_rest(unique_ptr<Expr> prim, int line, int col);
    unique_ptr<ExternalDefinition> parse_external_definition();
    unique_ptr<ReturnStatement> try_parse_return_statement();
    vector<pair<Type*, string>> parse_params();
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
    size_t func_id = 0;
    Program *p;
    Declaration *look_up(Id id, Block *b);
    Type *check_expr(Expr *expr, Block *b);
    Type *check_expr_type(Expr *expr, Block *b);
    FunctionDefinition *current;    // points to the current function that is being checked
    void expect_type(Expr *e, Block *b, Type *t);
    bool convertible_to_int(Type *t);
    bool nullable(Type *t);
    bool try_get_id(Expr *e, Id *id);
    Type *check_variable(Variable *var, Block *b);
    Type *check_elem_access_expr(ElemAccessExpr *expr, Block *b);
    Type *check_length_expr(LengthExpr *expr, Block *b);
    Type *check_type_cast_expr(TypeCastExpr *expr, Block *b);
    Type *check_substr_expr(SubstrExpr *expr, Block *b);
    Type *check_int_parse_expr(IntParseExpr *expr, Block *b);
    Type *check_new_str_expr(NewStrExpr *expr, Block *b);
    Type *check_new_arr_expr(NewArrExpr *expr, Block *b);
    void check_declaration(Declaration *dec, Block *b);
    void check_assignment(Assignment *asgn, Block *b);
    void verify_assignment(Expr *left, Expr *right, Block *b);
    void check_if_statement(IfStatement *st, Block *b, bool in_loop);
    void check_while_statement(WhileStatement *st, Block *b);
    void check_for_statement(ForStatement *for_s, Block *b);
    void check_statement(Statement *s, Block *b, bool in_loop);
    void check_block(Block *b, bool in_loop);
    void check_expression_statement(ExpressionStatement *s, Block *b);
    Type *check_function_call(FunctionCall *fc, Block *b);
    Type *check_inc_expr(IncExpr *expr, Block *b);
    bool verify_int(Type *t);
    Type *check_compatability(OpExpr *expr, Block *b);
    void report_error(int line, int col, string message);
    void check_outer_block(Block *b);
    void check_external_definition(ExternalDefinition *s, Block *b);
    void check_function_definition(FunctionDefinition* fd, Block *b);
    void check_external_declaration(Declaration *dec, Block *b);
    void populate_functions(Block *b);
    void check_return_statement(ReturnStatement *rs, Block *b);
    bool compare_arguments(vector<Type*> params, vector<Expr*> args, Block *b);
    void compare_types(Type *left_t, Type *right_t);
    bool return_inside(Statement *s);
  public:
    static bool function_inside(Expr *expr);
    void check_program(Program *p);
    map<Id, vector<pair<Type*, vector<Type*>>>> functions; // function map from names to all possible overloads
};


// LLVM TRANSLATOR
class Translator_LLVM {
    map<tuple<Type*, string, vector<Type*>>, size_t> overloads;
    size_t loop_depth = 0;
    int bounds, ref, free, null;
    int func_id = 0;
    int register_id = 0;
    int label_id = 0;
    int string_id = 0;
    bool ret = false;
    bool first = true;
    stack<Block*> block_stack;
    FunctionDefinition *current_fd;
    Program *current_prog;
    stack<pair<string,g_type>> references; // first: address, second: llvm_type
    map<Id, pair<string, g_type>> variables;  // first: address, second: llvm_type
    string arr_len;
    map<Id, string> arrays; // first: address of variable, second: length of the array
    map<Id, Expr*> globals;
    map<StringLiteral *, int> strings;
    string assign_register();
    string assign_global_register();
    string concat_cc(Type *left, Type *right);
    string translate_num_literal(string *s, NumLiteral *l);
    string translate_bool_literal(string *s, BoolLiteral *l);
    string translate_char_literal(string *s, CharLiteral *l);
    string translate_string_literal(string *s, StringLiteral *l);
    string translate_elem_access_expr(string *s, ElemAccessExpr *expr);
    string translate_length_expr(string *s, LengthExpr *expr);
    string translate_type_cast_expr(string *s, TypeCastExpr *expr);
    string translate_substr_expr(string *s, SubstrExpr *expr);
    string translate_int_parse_expr(string *s, IntParseExpr *expr);
    string translate_new_str_expr(string *s, NewStrExpr *expr);
    string array_type_to_bytes(ArrayType *t);
    string arr_type_to_func_size(ArrayType *t);
    string g_type_to_llvm_type(Type *t);
    string translate_new_arr_expr(string *s, NewArrExpr *expr);
    string translate_variable(string *s, Variable *var);
    string translate_not_expr(string *s, OpExpr *expr);
    void create_nullptr_check(string *s, string reg, string llvm_type);
    string translate_bool_expr(string *s, OpExpr *expr);
    string translate_arithm_expr(string *s, OpExpr *expr);
    string translate_op_expr(string *s, OpExpr *expr);
    string translate_expr(string *s, Expr *expr);
    void translate_declaration(string *s, Declaration *dec);
    void translate_external_declaration(string *s, Declaration *dec);
    void translate_assignment(string *s, Assignment *asgn);
    void translate_if_statement(string *s, IfStatement *st, string loop_end_label);
    void translate_while_statement(string *s, WhileStatement *st);
    void translate_for_statement(string *s, ForStatement *for_s);
    void translate_statement(string *s, Statement *statement, string loop_end_label);
    void translate_block(string *s, Block *b, string loop_end_label);
    void translate_expression_statement(string *s, ExpressionStatement *expr);
    string translate_inc_expr(string *s, IncExpr *expr);
    string bool_to_op(bool inc);
    string type_to_cc(Type *t);
    string operation_to_cc(Operation op);
    void report_error(int line, int col, string message);
    void create_bounds_check(string *s, string expr_register, string index_register, Type *type);
    string create_getelementptr_load(string *s, Type *result_type, Type *expr_type, string expr_register, string index_register);
    string create_conversion(string *s, string expr_register, Type *from, Type *to);
    string create_convert_ptr(string *s, string expr_register, Type *from, Type *to);
    string create_add(string *s, Type *type, string register_left, string register_right);
    string create_sub(string *s, Type *type, string register_left, string register_right);
    string create_mul(string *s, Type *type, string register_left, string register_right);
    string create_div(string *s, Type *type, string register_left, string register_right);
    string create_mod(string *s, Type *type, string register_left, string register_right);
    string create_cmp(string *s, Operation o, Type *type, string register_left, string register_right);
    string create_allocate_and_store(string *s, Type *t, string expr_register);
    string create_store(string *s, string llvm_type, string expr_register, string ptr);
    string create_getelementptr(string *s, string llvm_type, string expr_llvm_type, string expr_register, string index_register);
    string create_inc_dec(string *s, bool inc_dec, string expr_register);
    string translate_function_call(string *s, FunctionCall *fc);
    string create_alloca(string *s, g_type t);
    void init_globals(string *s);
    void free_globals(string *s);
    bool is_reference(g_type type);
    bool is_basic_type(g_type type);
    bool is_one_dimensional_array(g_type t);
    void translate_return_statement(string *s, ReturnStatement *rs);
    void translate_external_definition(string *s, ExternalDefinition *ed);
    void translate_function_definition(string *s, FunctionDefinition *fd);
    void free_variables(string *s, map<Id, Declaration*> variables);
    void translate_outer_block(string *s, Block *b);
    void change_reference_count(string *s, Type *g_type, string ptr_register, int i);
    void free_unused_memory(string *s);
    void free_argv(string *s);
    string get_array_len(string *s, string reg, g_type t);
    int g_type_to_depth(g_type type);
    void create_return_default(string *s, g_type type);
    void free_types(string *s);
    void create_storage_before_loop(string *s, Statement *st);
    bool is_global_variable(Id name);
    bool not_reassigned(Id name);
    bool is_assigned(Id name, Block *b); 
    bool is_assigned(Id name, Statement *s);
    Block *is_optimized(Id name);
    Block *is_optimized(Id name, Block *b);
    bool not_reassigned_global(Id name);
    void create_bounds_check_opt(string *s, Id name, Block *b, string index_register);
  public:
    Translator_LLVM(int bounds, int ref, int free, int null): bounds{bounds},ref{ref},free{free},null{null}{};
    string translate_program(Program *p);
};
#endif
