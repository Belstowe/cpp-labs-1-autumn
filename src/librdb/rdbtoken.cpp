#include "rdbtoken.hpp"

using rdb::parser::Token;
using rdb::parser::TokenType;

Token::Token(TokenType type, std::string_view lexeme)
    : type{type}, lexeme{lexeme}
{
}