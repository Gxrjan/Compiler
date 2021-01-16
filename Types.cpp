#include "head.h"

map<Type *, ArrayType*> ArrayType::array_types;

BasicType Bool("bool"), Char("char"), Int("int"), String("string"), Byte("byte"), Void("void");

BasicType Empty("empty");   // type of 'null'

BasicType::BasicType(string name) { this->name = name; }

string BasicType::to_string()
{
    return this->name;
}


ArrayType::ArrayType(Type *base) { this->base = base; }


string ArrayType::to_string()
{
    return this->base->to_string() + "[]";
}
