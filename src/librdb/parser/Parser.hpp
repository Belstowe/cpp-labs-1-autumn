#pragma once

#include "SqlStatement.hpp"
#include "Error.hpp"
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

    std::ostream& operator<<(std::ostream& os, const rdb::parser::SqlScript& sql);

    struct ParseResult {
        SqlScript _sql_script;
        std::vector<Error> _errors;
    };

    std::ostream& operator<<(std::ostream& os, const rdb::parser::ParseResult& parse_result);

    class Parser {
        public:
            explicit Parser(Lexer& lexer);
            void parse_sql(ParseResult& sql);

        private:
            Lexer& _lexer;
            ParseResult _sql;
            void parse_token(const TokenType expected_token, std::string *value, bool *catched_error);
            template<typename T> void convert_lexeme_to_var(Token &token, Value &value, const TokenType &token_type, bool *catched_error);
            void parse_operand(Operand &operand, TokenType &token_type, bool *catched_error);
            void parse_var_type(TokenType &token_type, bool *catched_error);
            int parse_column_def(std::vector<ColumnDef> &column_def_seq, bool& catched_error);
            void parse_statement_create();
            void parse_statement_insert();
            void parse_statement_select();
            void parse_statement_delete();
            void parse_statement_drop();
    };
} // namespace rdb::parser