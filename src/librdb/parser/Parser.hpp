#pragma once

#include "Error.hpp"
#include "SqlStatement.hpp"
#include "librdb/Token.hpp"
#include "librdb/lexer/Lexer.hpp"
#include <memory>

namespace rdb::parser {
struct SqlScript {
    explicit SqlScript() = default;
    // SqlScript(const SqlScript&) = delete;
    // SqlScript& operator=(const SqlScript&) = delete;
    std::vector<std::unique_ptr<SqlStatement>> _sql_statements;
};

std::ostream& operator<<(std::ostream& os, const rdb::parser::SqlScript& sql);

struct ParseResult {
    SqlScript _sql_script;
    std::vector<Error> _errors;
};

ParseResult parse_sql(std::string_view);
} // namespace rdb::parser