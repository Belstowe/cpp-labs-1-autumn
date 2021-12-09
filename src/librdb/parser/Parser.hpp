#pragma once

#include "SqlStatement.hpp"
#include "Error.hpp"
#include "librdb/lexer/Lexer.hpp"
#include "librdb/Token.hpp"
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

    class Parser {
        public:
            explicit Parser(Lexer& lexer);
            void parse_sql(ParseResult& sql);

        private:
            Lexer& _lexer;
            ParseResult _sql;

            void parse_token(const TokenType expected_token, std::string *value);
            template<typename T> void convert_lexeme_to_var(Token& token, Value& value, const TokenType& token_type);
            void parse_operand(Operand& operand);
            void parse_column_def(std::vector<ColumnDef>& column_def_seq);
            void parse_column_list(std::vector<std::string>& column_name_seq);

            void parse_argument_table(std::string& table_name);
            void parse_argument_into(std::string& table_name, std::vector<std::string>& column_name_seq);
            void parse_argument_values(std::vector<Value>& value_seq);
            void parse_argument_where(Expression& expression);
            void parse_argument_from(std::string& table_name, Expression& expression);
            
            void parse_statement_create();
            void parse_statement_insert();
            void parse_statement_select();
            void parse_statement_delete();
            void parse_statement_drop();
    };
} // namespace rdb::parser