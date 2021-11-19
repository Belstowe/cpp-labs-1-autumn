#pragma once

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
    Token(TokenType type = TokenType::Unknown, std::string_view lexeme = "");
};
}