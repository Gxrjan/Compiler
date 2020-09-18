#include "head.h"

NumToken::NumToken(long long num) { this->num = num; }
string NumToken::to_string() { return "Number: " + std::to_string(this->num); }
bool NumToken::isNum(long long *num)
{
    (*num) = this->num;
    return true;
}


BoolToken::BoolToken(bool b) { this->b = b; }
string BoolToken::to_string() 
{ 
    string s = (this->b) ?  "true" : "false";
    return "Bool: " + s; 
}
bool BoolToken::isBool(bool *b)
{
    (*b) = this->b;
    return true;
}

OperToken::OperToken(string op) { this->op = op; }
string OperToken::to_string() { return "Operator: " + this->op; }
bool OperToken::isOper(string *op) 
{
    (*op) = this->op;
    return true;
}
bool OperToken::isOper(string op) { return this->op == op; }


IdToken::IdToken(string name) { this->name = name; }
bool IdToken::isId(string *name)
{
    *name = this->name;
    return true;
}
bool IdToken::isId(string name) { return this->name == name; }
string IdToken::to_string() { return "" + this->name; }


SymbolToken::SymbolToken(string s) { this->s = s; }
bool SymbolToken::isSymbol(string s) { return this->s==s; }
string SymbolToken::to_string() { return "Symbol: " + this->s; }



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
        case Type::Char:
            result = "char";
            break;
        default:
            throw runtime_error("Unknown type");
            break;
    }
    return result;
}

KeywordToken::KeywordToken(string name) { this->name = name; }
bool KeywordToken::isKeyword(string name) { return this->name == name; }
string KeywordToken::to_string() { return "Keyword Token: "+ this->name; }


CharToken::CharToken(char c) { this->c = c; }
string CharToken::to_string() { return "Char token: '" + string{this->c} +"'"; }
bool CharToken::isChar(char *c) {
    *c = this->c;
    return true;
}


StringToken::StringToken(string s) { this->s = s; }
string StringToken::to_string() { return "String token: \"" + this->s +"\""; }
bool StringToken::isString(string *s) {
    *s = this->s;
    return true;
}
