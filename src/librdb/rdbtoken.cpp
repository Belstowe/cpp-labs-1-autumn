#include "rdbtoken.hpp"

using namespace rdb::parser;

Token::Token(const TokenType &type_arg, const std::string &lexeme_arg) {
    type = type_arg;
    lexeme = std::string_view(lexeme_arg);
}

TokenType Token::type_get() {
    return type;
}

int Token::type_set(TokenType new_type) {
    type = new_type;
    return 0;
}

std::string_view Token::lexeme_get() {
    return lexeme;
}

int Token::lexeme_set(std::string_view new_lexeme) {
    lexeme = new_lexeme;
    return 0;
}