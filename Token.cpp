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
