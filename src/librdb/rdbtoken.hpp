#pragma once

#include <string_view>

namespace rdb::parser {
    typedef enum TokenType {
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
    } TokenType;

    class Token {
        private:
            TokenType type;
            std::string_view lexeme;

        public:
            explicit Token(const TokenType &, const std::string &);
            TokenType type_get();
            int type_set(TokenType);
            std::string_view lexeme_get();
            int lexeme_set(std::string_view);
    };
}