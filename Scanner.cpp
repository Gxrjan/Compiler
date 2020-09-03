#include "head.h"


void Scanner::report_error(string message)
{
    throw runtime_error("Scanner error "
                        "line " + std::to_string(this->last_line)
                      + ", col " + std::to_string(this->last_column)
                      + ": " + message);
}

Token* Scanner::peek_token()
{
    if (this->next) {
        return this->next.get();
    }
    this->next = this->next_token();
    return this->next.get();
}


unique_ptr<Token> Scanner::next_token() {
    if (this->next) {
        unique_ptr<Token> result = move(this->next);
        return result;
    }
    char c;
    //cin >> ws; // Consume leading whitespace
    while (isspace(cin.peek())) {
        this->getc();
    }
    this->last_line = this->line;
    this->last_column = this->column;
    if ((c=this->getc())==EOF)
        return nullptr;
    if (c == '=' || c == ';') {
        return make_unique<SymbolToken>(c);
    }

    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return make_unique<OperToken>(c);
    }

    if (c == '(' || c == ')' || c == '{' || c == '}') {
        return make_unique<SymbolToken>(c);
    }

    if (isdigit(c)) {
        string num = { c };
        while (isdigit(cin.peek())) {
            c = this->getc();
            num = num + c;
        }
        return make_unique<NumToken>(stoi(num));
    }
    if (isalpha(c)) {
        string name = { c };
        while (isalnum(cin.peek())) {
            c = this->getc();
            name = name + c;
        }
        if (name == "print")
            return make_unique<PrintToken>();
        if (name == "bool")
            return make_unique<TypeToken>(Type::Bool);
        if (name == "int")
            return make_unique<TypeToken>(Type::Int);
        return make_unique<IdToken>(name);
    }
    this->report_error("unrecognized token");
    return nullptr;
}

char Scanner::getc()
{
    char c = cin.get();
    if (c == '\n') {
        this->line++;
        this->column = 1;
    } else {
        this->column++;
    }
    return c;
}
