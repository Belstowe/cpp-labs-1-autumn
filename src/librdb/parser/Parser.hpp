#pragma once

#include "SqlStatement.hpp"
#include "librdb/lexer/lexer.hpp"
#include "librdb/rdbtoken.hpp"
#include <memory>

namespace rdb::parser {
    struct SqlScript {
        explicit SqlScript() = default;
        SqlScript(const SqlScript&) = delete;
        SqlScript& operator=(const SqlScript&) = delete;
        std::vector<std::unique_ptr<SqlStatement>> _sql_statements;
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

        friend std::ostream& operator<<(std::ostream& os, const rdb::parser::Error& error);
    };

    struct ParseResult {
        SqlScript _sql_script;
        std::vector<Error> _errors;
    };

    class Parser {
        public:
            explicit Parser(Lexer& lexer);
            void parse_sql(ParseResult& sql);

        private:
            Lexer& _lexer;
            ParseResult _sql;
            void parse_token(const TokenType expected_token, std::string *value, bool *catched_error, const bool is_required = true, TokenType *got_token = NULL);
            template<typename T> void convert_lexeme_to_var(Token &token, Value &value, const TokenType &token_type, bool *catched_error);
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