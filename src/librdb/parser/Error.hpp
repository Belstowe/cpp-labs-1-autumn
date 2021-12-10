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

class Error {
public:
    Error(Token token, ErrorType type, TokenType expected = TokenType::Unknown);
    ErrorType type() const;
    TokenType token_type() const;

private:
    Token _token;
    ErrorType _type;
    TokenType _expected;

    friend std::ostream&
    operator<<(std::ostream& os, const rdb::parser::Error& error);
};

std::ostream& operator<<(std::ostream& os, const rdb::parser::Error& error);
std::ostream&
operator<<(std::ostream& os, const rdb::parser::ErrorType& error_type);
} // namespace rdb::parser