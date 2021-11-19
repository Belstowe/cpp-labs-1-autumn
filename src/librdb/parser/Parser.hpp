#pragma once

#include "SqlStatement.hpp"
#include "librdb/lexer/lexer.hpp"
#include "librdb/rdbtoken.hpp"
#include <memory>

namespace rdb::parser {
    using SqlStatementPtr = std::unique_ptr<SqlStatement>;

    struct SqlScript {
        std::vector<SqlStatementPtr> _sql_statements;
    };

    typedef enum ErrorType {
        SyntaxError,
        Undefined
    } ErrorType;

    class Error {
        public:
            Error(Token token, ErrorType type);

        private:
            Token _token;
            ErrorType _type;
    };

    struct ParseResult {
        SqlScript _sql_script;
        std::vector<Error> _errors;
    };

    class Parser {
        public:
            explicit Parser(Lexer& lexer);
            struct ParseResult parse_sql();

        private:
            Lexer& _lexer;
    };
} // namespace rdb::parser