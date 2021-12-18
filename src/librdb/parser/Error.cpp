#include "Error.hpp"

using rdb::parser::Error;
using rdb::parser::ErrorType;
using rdb::parser::TokenType;

Error::Error(Token token, ErrorType type, TokenType expected)
    : token_{token}, type_{type}, expected_{expected}
{
}

ErrorType Error::type() const
{
    return type_;
}

TokenType Error::token_type() const
{
    return token_.type;
}

std::ostream& rdb::parser::operator<<(std::ostream& os, const Error& error)
{
    os << "! " << error.token_ << ":\n";
    os << "  Error::" << error.type_;
    if (error.expected_ != TokenType::Unknown) {
        os << " (expected " << error.expected_ << ")";
    }
    return os;
}

std::ostream&
rdb::parser::operator<<(std::ostream& os, const ErrorType& error_type)
{
    switch (error_type) {
    case ErrorType::SyntaxError:
        os << "SyntaxError";
        break;
    case ErrorType::VarSyntaxError:
        os << "VarSyntaxError";
        break;
    case ErrorType::TypeSyntaxError:
        os << "TypeSyntaxError";
        break;
    case ErrorType::IncorrectVarType:
        os << "IncorrectVarType";
        break;
    case ErrorType::WrongListDefinition:
        os << "WrongListDefinition";
        break;
    case ErrorType::NotStatement:
        os << "NotStatement";
        break;
    case ErrorType::VarOutOfRange:
        os << "VarOutOfRange";
        break;
    case ErrorType::UnknownType:
        os << "UnknownType";
        break;
    case ErrorType::UnexpectedEOF:
        os << "UnexpectedEOF";
        break;
    case ErrorType::Undefined:
        os << "Undefined";
        break;
    default:
        os << static_cast<int>(error_type);
    }
    return os;
}