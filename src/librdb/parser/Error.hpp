#pragma once

#include "librdb/Token.hpp"

namespace rdb::parser {
enum class ErrorType {
    SyntaxError,
    VarSyntaxError,
    TypeSyntaxError,
    IncorrectVarType,
    WrongListDefinition,
    NotStatement,
    VarOutOfRange,
    UnknownType,
    UnexpectedEOF,
    Undefined
};

class Error : public std::exception {
public:
    Error(Token token, ErrorType type, TokenType expected = TokenType::Unknown);
    ErrorType type() const;
    TokenType token_type() const;

private:
    Token token_;
    ErrorType type_;
    TokenType expected_;

    friend std::ostream&
    operator<<(std::ostream& os, const rdb::parser::Error& error);
};

std::ostream& operator<<(std::ostream& os, const rdb::parser::Error& error);
std::ostream&
operator<<(std::ostream& os, const rdb::parser::ErrorType& error_type);
} // namespace rdb::parser