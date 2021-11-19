#include "lexer.hpp"
#include <algorithm>
#include <cctype>
#include <regex>

using rdb::parser::Lexer;
using rdb::parser::Token;
using rdb::parser::TokenType;

namespace {
struct TokenRule {
    std::regex regex;
    TokenType tokentype;
};

const std::vector<TokenRule> TokenRules = {
        {std::regex("\".*?\"", std::regex_constants::icase),
         TokenType::VarText},
        {std::regex(
                 "[-+]?0\\.[0-9]+|[1-9][0-9]*\\.[0-9]+",
                 std::regex_constants::icase),
         TokenType::VarReal},
        {std::regex("[-+]?0|[-+]?[1-9][0-9]*", std::regex_constants::icase),
         TokenType::VarInt},
        {std::regex("CREATE", std::regex_constants::icase),
         TokenType::KwCreate},
        {std::regex("INSERT", std::regex_constants::icase),
         TokenType::KwInsert},
        {std::regex("DELETE", std::regex_constants::icase),
         TokenType::KwDelete},
        {std::regex("DROP", std::regex_constants::icase), TokenType::KwDrop},
        {std::regex("FROM", std::regex_constants::icase), TokenType::KwFrom},
        {std::regex("INT", std::regex_constants::icase), TokenType::KwInt},
        {std::regex("INTO", std::regex_constants::icase), TokenType::KwInto},
        {std::regex("REAL", std::regex_constants::icase), TokenType::KwReal},
        {std::regex("SELECT", std::regex_constants::icase),
         TokenType::KwSelect},
        {std::regex("TABLE", std::regex_constants::icase), TokenType::KwTable},
        {std::regex("TEXT", std::regex_constants::icase), TokenType::KwText},
        {std::regex("VALUES", std::regex_constants::icase),
         TokenType::KwValues},
        {std::regex("WHERE", std::regex_constants::icase), TokenType::KwWhere},
        {std::regex("[a-z][a-z0-9]*", std::regex_constants::icase),
         TokenType::VarId},
        {std::regex(">=|<=|!=|=|<|>", std::regex_constants::icase),
         TokenType::Operation},
        {std::regex("\\(", std::regex_constants::icase),
         TokenType::ParenthesisOpening},
        {std::regex("\\)", std::regex_constants::icase),
         TokenType::ParenthesisClosing},
        {std::regex("\\{", std::regex_constants::icase),
         TokenType::CurlyBracketOpening},
        {std::regex("\\}", std::regex_constants::icase),
         TokenType::CurlyBracketClosing},
        {std::regex(";", std::regex_constants::icase), TokenType::Semicolon},
        {std::regex(",", std::regex_constants::icase), TokenType::Comma}};
}

Lexer::Lexer(std::string_view parse_string_view)
    : parse_string{parse_string_view}, string_pos{0}
{
}

Token Lexer::peek()
{
    if (string_pos == parse_string.length())
        return Token(TokenType::EndOfFile, "");

    while (std::end(Lexer::skipsym)
           != std::find(
                   std::begin(Lexer::skipsym),
                   std::end(Lexer::skipsym),
                   parse_string.at(string_pos))) {
        if (++string_pos == parse_string.length())
            return Token(TokenType::EndOfFile, "");
    }

    for (auto&& rule : TokenRules) {
        std::cmatch match;
        if (std::regex_search(
                    parse_string.data() + string_pos,
                    match,
                    rule.regex,
                    std::regex_constants::match_continuous)) {
            return Token(
                    rule.tokentype,
                    std::string_view(
                            parse_string.data() + string_pos,
                            match.begin()->length()));
        }
    }

    return Token(
            TokenType::Unknown,
            std::string_view(parse_string.data() + string_pos++, 1));
}

Token Lexer::get()
{
    auto res_token = peek();
    string_pos += res_token.lexeme_get().length();
    return res_token;
}