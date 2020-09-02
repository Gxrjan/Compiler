#include "head.h"


Program::Program(unique_ptr<Block> block)
{
    this->block = move(block);
}
string Program::to_string()
{
    return this->block->to_string();
}
