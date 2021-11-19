#pragma once

#include <string>
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
class Token {
public:
    explicit Token(
            const TokenType& = TokenType::Unknown, std::string_view = "");
    TokenType type_get();
    int type_set(const TokenType&);
    std::string_view lexeme_get();
    int lexeme_set(const std::string&);

private:
    TokenType type;
    std::string_view lexeme;
};
}