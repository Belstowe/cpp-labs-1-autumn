#include "Token.hpp"

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

std::ostream& rdb::parser::operator<<(std::ostream& os, const Token& token)
{
    os << "(" << token.type << ", " << token.lexeme
       << ") at row " << token.parsed_row << ", col " << token.parsed_col;
    return os;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const TokenType& token_type)
{
    switch (token_type) {
        case TokenType::KwCreate: os << "KwCreate"; break;
        case TokenType::KwSelect: os << "KwSelect"; break;
        case TokenType::KwInsert: os << "KwInsert"; break;
        case TokenType::KwDelete: os << "KwDelete"; break;
        case TokenType::KwDrop: os << "KwDrop"; break;
        case TokenType::KwValues: os << "KwValues"; break;
        case TokenType::KwTable: os << "KwTable"; break;
        case TokenType::KwInt: os << "KwInt"; break;
        case TokenType::KwReal: os << "KwReal"; break;
        case TokenType::KwText: os << "KwText"; break;
        case TokenType::KwWhere: os << "KwWhere"; break;
        case TokenType::KwFrom: os << "KwFrom"; break;
        case TokenType::KwInto: os << "KwInto"; break;
        case TokenType::VarId: os << "VarId"; break;
        case TokenType::VarInt: os << "VarInt"; break;
        case TokenType::VarReal: os << "VarReal"; break;
        case TokenType::VarText: os << "VarText"; break;
        case TokenType::Operation: os << "Operation"; break;
        case TokenType::CurlyBracketOpening: os << "CurlyBracketOpening"; break;
        case TokenType::CurlyBracketClosing: os << "CurlyBracketClosing"; break;
        case TokenType::ParenthesisOpening: os << "ParenthesisOpening"; break;
        case TokenType::ParenthesisClosing: os << "ParenthesisClosing"; break;
        case TokenType::Comma: os << "Comma"; break;
        case TokenType::Semicolon: os << "Semicolon"; break;
        case TokenType::EndOfFile: os << "EndOfFile"; break;
        case TokenType::Unknown: os << "Unknown"; break;
        default: os << static_cast<int>(token_type);
    }
    return os;
}