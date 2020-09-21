#include "head.h"



BasicType Bool("bool"), Char("char"), Int("int"), String("string");

BasicType Empty("empty");   // type of 'null'

BasicType::BasicType(string name) { this->name = name; }


ArrayType::ArrayType(Type *base) { this->base = base; }

