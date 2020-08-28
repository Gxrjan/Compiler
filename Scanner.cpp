#include "head.h"

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
        cin >> ws; // Consume leading whitespace
        if (!(cin>>c))
            return nullptr;
        
        if (c == '=' || c == ';')
            return make_unique<SymbolToken>(c);

        if (c == '+' || c == '-' || c == '*' || c == '/')
            return make_unique<OperToken>(c);

        if (c == '(' || c == ')')
            return make_unique<ParenToken>(c);

        if (isdigit(c)) {
            string num = { c };
            while (isdigit(cin.peek())) {
                cin >> c;
                num = num + c;
            }
            return make_unique<NumToken>(stoi(num));
        }
        if (isalpha(c)) {
            string name = { c };
            while (isalnum(cin.peek())) {
                cin >> c;
                name = name + c;
            }
            if (name == "print")
                return make_unique<PrintToken>();
            return make_unique<IdToken>(name);
        }

        throw runtime_error("syntax error");
}


