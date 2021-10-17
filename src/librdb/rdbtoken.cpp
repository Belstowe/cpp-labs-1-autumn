#include "rdbtoken.hpp"

using namespace rdb::parser;

Token::Token(const TokenType &type_arg, const std::string &lexeme_arg) {
    type = type_arg;
    lexeme = lexeme_arg;
}

TokenType Token::type_get() {
    return type;
}

int Token::type_set(const TokenType &new_type) {
    type = new_type;
    return 0;
}

std::string_view Token::lexeme_get() {
    return std::string_view(lexeme);
}

int Token::lexeme_set(std::string new_lexeme) {
    lexeme.assign(new_lexeme);
    return 0;
}