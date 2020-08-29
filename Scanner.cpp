#include "head.h"

Token* Scanner::peek_token()
{
    if (this->next) {
        return this->next.get();
    }
    this->next = this->next_token(true);
    return this->next.get();
}


unique_ptr<Token> Scanner::next_token(bool peek) {
        if (!peek) {
            this->last_line = this->line;
            this->last_column = this->column;
        }
        if (this->next && !peek) {
            unique_ptr<Token> result = move(this->next);
            this->last_line = this->line;
            this->last_column = this->column;
            return result;
        }
        char c;
        //cin >> ws; // Consume leading whitespace
        while (isspace(cin.peek())) {
            c = cin.get();
            if (c == '\n') {
                this->column = 0;
                this->line++;
            }
            this->column++;
        }
        if (!(cin>>c))
            return nullptr;
        this->column++;
        if (c == '=' || c == ';') {
            return make_unique<SymbolToken>(c);
        }

        if (c == '+' || c == '-' || c == '*' || c == '/') {
            return make_unique<OperToken>(c);
        }

        if (c == '(' || c == ')') {
            return make_unique<SymbolToken>(c);
        }

        if (isdigit(c)) {
            string num = { c };
            while (isdigit(cin.peek())) {
                cin >> c;
                num = num + c;
                this->column++;
            }
            return make_unique<NumToken>(stoi(num));
        }
        if (isalpha(c)) {
            string name = { c };
            while (isalnum(cin.peek())) {
                cin >> c;
                name = name + c;
                this->column++;
            }
            if (name == "print")
                return make_unique<PrintToken>();
            return make_unique<IdToken>(name);
        }

        throw runtime_error("syntax error");
}


