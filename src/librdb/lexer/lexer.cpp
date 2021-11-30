#include "lexer.hpp"
#include <algorithm>
#include <cctype>
#include <regex>

using rdb::parser::Lexer;
using rdb::parser::Token;
using rdb::parser::TokenType;

namespace {
struct TokenRule {
    TokenType tokentype;
    std::regex regex;
};

// clang-format off
const std::vector<TokenRule> TokenRules = {
        {TokenType::VarText,             std::regex("\".*?\"", std::regex_constants::icase)},
        {TokenType::VarReal,             std::regex("[-+]?0\\.[0-9]+|[1-9][0-9]*\\.[0-9]+", std::regex_constants::icase)},
        {TokenType::VarInt,              std::regex("[-+]?0|[-+]?[1-9][0-9]*", std::regex_constants::icase)},
        {TokenType::KwCreate,            std::regex("CREATE", std::regex_constants::icase)},
        {TokenType::KwInsert,            std::regex("INSERT", std::regex_constants::icase)},
        {TokenType::KwDelete,            std::regex("DELETE", std::regex_constants::icase)},
        {TokenType::KwDrop,              std::regex("DROP", std::regex_constants::icase)},
        {TokenType::KwFrom,              std::regex("FROM", std::regex_constants::icase)},
        {TokenType::KwInt,               std::regex("INT", std::regex_constants::icase)},
        {TokenType::KwInto,              std::regex("INTO", std::regex_constants::icase)},
        {TokenType::KwReal,              std::regex("REAL", std::regex_constants::icase)},
        {TokenType::KwSelect,            std::regex("SELECT", std::regex_constants::icase)},
        {TokenType::KwTable,             std::regex("TABLE", std::regex_constants::icase)},
        {TokenType::KwText,              std::regex("TEXT", std::regex_constants::icase)},
        {TokenType::KwValues,            std::regex("VALUES", std::regex_constants::icase)},
        {TokenType::KwWhere,             std::regex("WHERE", std::regex_constants::icase)},
        {TokenType::VarId,               std::regex("[a-z][a-z0-9]*", std::regex_constants::icase)},
        {TokenType::Operation,           std::regex(">=|<=|!=|=|<|>", std::regex_constants::icase)},
        {TokenType::ParenthesisOpening,  std::regex("\\(", std::regex_constants::icase)},
        {TokenType::ParenthesisClosing,  std::regex("\\)", std::regex_constants::icase)},
        {TokenType::CurlyBracketOpening, std::regex("\\{", std::regex_constants::icase)},
        {TokenType::CurlyBracketClosing, std::regex("\\}", std::regex_constants::icase)},
        {TokenType::Semicolon,           std::regex(";", std::regex_constants::icase)},
        {TokenType::Comma,               std::regex(",", std::regex_constants::icase)}
};
// clang-format on
} // namespace

Lexer::Lexer(std::string_view parse_string_view)
    : parse_string{parse_string_view}, string_pos{0}, col{0}, row{1}
{
}

Token Lexer::peek()
{
    if (string_pos == parse_string.length()) {
        return Token(TokenType::EndOfFile, "", ++col, row);
    }

    while (std::end(Lexer::skipsym)
           != std::find(
                   std::begin(Lexer::skipsym),
                   std::end(Lexer::skipsym),
                   parse_string.at(string_pos))) {
        col++;
        if (parse_string.at(string_pos) == '\n') {
            col = 0;
            row++;
        }
        if (++string_pos == parse_string.length()) {
            return Token(TokenType::EndOfFile, "", ++col, row);
        }
    }

    for (auto&& rule : TokenRules) {
        std::cmatch match;
        if (std::regex_search(
                    parse_string.data() + string_pos,
                    match,
                    rule.regex,
                    std::regex_constants::match_continuous)) {
            Token token_to_return(
                    rule.tokentype,
                    std::string_view(
                            parse_string.data() + string_pos,
                            match.begin()->length()),
                    col + 1,
                    row);
            col += match.begin()->length();
            return token_to_return;
        }
    }

    return Token(
            TokenType::Unknown,
            std::string_view(parse_string.data() + string_pos++, 1),
            ++col,
            row);
}

Token Lexer::get()
{
    auto res_token = peek();
    string_pos += res_token.lexeme.length();
    return res_token;
}