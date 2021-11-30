#include "rdbtoken.hpp"

#include <iostream>

using rdb::parser::Token;
using rdb::parser::TokenType;

Token::Token(
        TokenType type,
        std::string_view lexeme,
        size_t parsed_col,
        size_t parsed_row)
    : type{type}, lexeme{lexeme}, parsed_col{parsed_col}, parsed_row{parsed_row}
{
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "(" << static_cast<int>(token.type) << ", " << token.lexeme
       << ") at row " << token.parsed_row << ", col " << token.parsed_col;
    return os;
}