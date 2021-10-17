#include "rdbtoken.hpp"

using rdb::parser::Token;

Token::Token(const Token::TokenType& type_arg, const std::string& lexeme_arg)
{
    type = type_arg;
    lexeme = lexeme_arg;
}

Token::TokenType Token::type_get()
{
    return type;
}

int Token::type_set(const Token::TokenType& new_type)
{
    type = new_type;
    return 0;
}

std::string_view Token::lexeme_get()
{
    return std::string_view(lexeme);
}

int Token::lexeme_set(const std::string& new_lexeme)
{
    lexeme.assign(new_lexeme);
    return 0;
}