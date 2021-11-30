#pragma once

#include <iostream>
#include <string_view>

namespace rdb::parser {
enum class TokenType {
    KwCreate,
    KwSelect,
    KwInsert,
    KwDelete,
    KwDrop,
    KwValues,
    KwTable,
    KwInt,
    KwReal,
    KwText,
    KwWhere,
    KwFrom,
    KwInto,
    VarInt,
    VarReal,
    VarText,
    VarId,
    Operation,
    CurlyBracketOpening,
    CurlyBracketClosing,
    ParenthesisOpening,
    ParenthesisClosing,
    Comma,
    Semicolon,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string_view lexeme;
    size_t parsed_col;
    size_t parsed_row;
    Token(TokenType type = TokenType::Unknown,
          std::string_view lexeme = "",
          size_t parsed_col = 0,
          size_t parsed_row = 0);
};
}

std::ostream& operator<<(std::ostream& os, const rdb::parser::Token& token);