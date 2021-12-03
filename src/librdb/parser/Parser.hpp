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

    enum class ErrorType {
        SyntaxError,
        VarSyntaxError,
        TypeSyntaxError,
        IncorrectVarType,
        NotStatement,
        VarOutOfRange,
        UnknownType,
        UnexpectedEOF,
        Undefined
    };

    class Error {
        public:
            Error(Token token, ErrorType type, TokenType expected = TokenType::Unknown);

        private:
            Token _token;
            ErrorType _type;
            TokenType _expected;
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
            struct ParseResult _sql;
            void parse_token(const TokenType expected_token, std::string *value, bool *catched_error, const bool is_required = true, TokenType *got_token = NULL);
            void parse_value(Value &value, TokenType &token_type, bool *catched_error);
            void parse_var_type(TokenType &token_type, bool *catched_error);
            int parse_column_def(std::vector<ColumnDef> &column_def_seq, bool& catched_error);
            void parse_statement_create();
            void parse_statement_delete();
            void parse_statement_insert();
            void parse_statement_select();
            void parse_statement_drop();
    };
} // namespace rdb::parser