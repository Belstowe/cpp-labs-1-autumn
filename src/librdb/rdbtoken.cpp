#include "rdbtoken.hpp"

#include <iostream>

using rdb::parser::Token;
using rdb::parser::TokenType;

Token::Token(TokenType type, std::string_view lexeme)
    : type{type}, lexeme{lexeme}
{
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "(" << static_cast<int>(token.type) << ", " << token.lexeme << ")";
    return os;
}