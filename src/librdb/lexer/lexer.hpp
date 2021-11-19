#pragma once

#include "../rdbtoken.hpp"
#include <string_view>
#include <unordered_map>

using rdb::parser::Token;
namespace rdb::parser {
class Lexer {
public:
    explicit Lexer(std::string_view parse_string_view);
    Token get();
    Token peek();

private:
    std::string_view parse_string;
    size_t string_pos;
    int string_pos_increment();
    Token token_extract_begin();
    static constexpr char skipsym[] = {' ', '\n', '\r', '\t'};
};
}