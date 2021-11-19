#include "rdbtoken.hpp"
#include <unordered_map>

using rdb::parser::Token;
using rdb::parser::TokenType;

Token::Token(const TokenType& type_arg, std::string_view lexeme_arg)
    : type{type_arg}, lexeme{lexeme_arg}
{
}

TokenType Token::type_get()
{
    return type;
}

int Token::type_set(const TokenType& new_type)
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
    lexeme = std::string_view(new_lexeme);
    return 0;
}