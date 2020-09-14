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

void Scanner::consume_ws()
{
    while (isspace(cin.peek())) {
        this->getc();
    }
}

unique_ptr<Token> Scanner::next_token() {
    if (this->next) {
        unique_ptr<Token> result = move(this->next);
        return result;
    }
    char c;
    this->consume_ws();
    if (cin.peek() == '/') {
        this->getc();
        if (cin.peek() == '/') {
            this->getc();
            while ((c=this->getc())!='\n' && c != EOF) {}
            return this->next_token();
        } else 
            return make_unique<OperToken>("/");
    }

    this->last_line = this->line;
    this->last_column = this->column;
    if ((c=this->getc())==EOF)
        return nullptr;
    
    if (c == '\'') {
        char16_t wc = (char16_t)this->getc();
        if (wc && wc != '\n' && wc != '\'') {
            c = this->getc();
            if (c != '\'')
                this->report_error("' expected after character");
            return make_unique<CharToken>(wc);
        } else {
            this->report_error("char syntax error");
        }
    }
     
    if (c == '&' || c == '|') {
        char nc = cin.peek();
        if (nc == '&' || nc == '|') {
            this->getc();
            return make_unique<OperToken>(string{c, nc});
        }      
    }
     
    if (c == '!' || c == '=' || c == '<' || c == '>') {
        if (cin.peek() == '=') {
            this->getc();
            return make_unique<OperToken>(string{c, '='});      // !=, ==, <=, >=
        } else {
            if (c != '!') {
                if (c == '=')
                    return make_unique<SymbolToken>(string{c}); // =
                return make_unique<OperToken>(string{c});       // <, >
            } else
                return make_unique<SymbolToken>(string{c});     // !
        }
    }
    

    if (c == '+' || c == '-' || c == '*' || c == '%') {
        return make_unique<OperToken>(string{c});
    }

    if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';') {
        return make_unique<SymbolToken>(string{c});
    }
    

    if (isdigit(c)) {
        string num = { c };
        while (isdigit(cin.peek())) {
            c = this->getc();
            num = num + c;
        }
        return make_unique<NumToken>(stoll(num));
    }
    if (isalpha(c) || c == '_') {
        string name = { c };
        while (isalnum(cin.peek()) || cin.peek() == '_') {
            c = this->getc();
            name = name + c;
        }
        if (name == "bool")
            return make_unique<TypeToken>(Type::Bool);
        if (name == "int")
            return make_unique<TypeToken>(Type::Int);
        if (name == "char")
            return make_unique<TypeToken>(Type::Char);
        if (name == "true")
            return make_unique<BoolToken>(true);
        if (name == "false")
            return make_unique<BoolToken>(false);
        if (name == "print" || name == "if" || name == "else" ||
            name == "while" || name == "for" || name == "break")
            return make_unique<KeywordToken>(name);
        return make_unique<IdToken>(name);
    }
    this->report_error("unrecognized token");
    return nullptr;
}

char Scanner::getc()
{
    char c = cin.get();
    if (c== EOF)
        return c;
    if (c == '\n') {
        this->line++;
        this->column = 1;
    } else {
        this->column++;
    }
    return c;
}
