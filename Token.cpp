#include "head.h"

NumToken::NumToken(int num) { this->num = num; }
string NumToken::to_string() { return "Number: " + std::to_string(this->num); }
bool NumToken::isNum(int *num)
{
    (*num) = this->num;
    return true;
}

OperToken::OperToken(char op) { this->op = op; }
string OperToken::to_string() { return "Operator: " + string(1, this->op); }
bool OperToken::isOper(char *op) 
{
    (*op) = this->op;
    return true;
}

ParenToken::ParenToken(char paren) { this->paren = paren; }
string ParenToken::to_string() { return "Paren: " + string(1, this->paren); }
bool ParenToken::isParen(char *p)
{
    (*p) = this->paren;
    return true;
}

IdToken::IdToken(string name) { this->name = name; }
bool IdToken::isId(string *name)
{
    *name = this->name;
    return true;
}
string IdToken::to_string() { return "" + this->name; }


SymbolToken::SymbolToken(char c) { this->c = c; }
bool SymbolToken::isSymbol(char c) { return this->c==c; }
string SymbolToken::to_string() { return "Symbol: " + string(1, this->c); }


bool PrintToken::isPrint() { return true; }
string PrintToken::to_string() { return "Print token: print"; }

TypeToken::TypeToken(Type t) { this->type = t; }
bool TypeToken::isType(Type *t) 
{
    *t = this->type;
    return true;
}
string TypeToken::to_string()
{
    string result = "";
    switch(this->type) {
        case Type::Bool:
            result = "bool";
            break;
        case Type::Int:
            result = "int";
            break;
        default:
            throw runtime_error("Unknown type");
            break;
    }
    return result;
}
