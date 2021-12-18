#pragma once

#include "Error.hpp"
#include "SqlStatement.hpp"
#include "librdb/Token.hpp"
#include "librdb/lexer/Lexer.hpp"
#include <memory>

namespace rdb::parser {
struct SqlScript {
    explicit SqlScript() = default;
    std::vector<std::unique_ptr<SqlStatement>> sql_statements;
};

std::ostream& operator<<(std::ostream& os, const rdb::parser::SqlScript& sql);

struct ParseResult {
    SqlScript sql_script;
    std::vector<Error> errors;
};

ParseResult parse_sql(std::string_view);
} // namespace rdb::parser