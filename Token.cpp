#include "head.h"

NumToken::NumToken(int num) { this->num = num; }
string NumToken::to_string() { return "Number: " + std::to_string(this->num); }
string NumToken::get_class() { return "NumToken"; }

OperToken::OperToken(char op) { this->op = op; }
string OperToken::to_string() { return "Operator: " + string(1, this->op); }
string OperToken::get_class() { return "OperToken"; }


ParenToken::ParenToken(char paren) { this->paren = paren; }
string ParenToken::to_string() { return "Paren: " + string(1, this->paren); }
string ParenToken::get_class() { return "ParenToken"; }

