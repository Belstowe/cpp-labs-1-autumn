#include "Lexer.hpp"
#include <algorithm>
#include <array>
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
const std::array<TokenRule, 24> TokenRules{ {
        {TokenType::KwCreate,            std::regex("CREATE(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwInsert,            std::regex("INSERT(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwDelete,            std::regex("DELETE(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwDrop,              std::regex("DROP(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwFrom,              std::regex("FROM(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwInto,              std::regex("INTO(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwInt,               std::regex("INT(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwReal,              std::regex("REAL(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwSelect,            std::regex("SELECT(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwTable,             std::regex("TABLE(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwText,              std::regex("TEXT(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwValues,            std::regex("VALUES(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::KwWhere,             std::regex("WHERE(?=\\s|$|\\(|\\)|;|,)", std::regex_constants::icase)},
        {TokenType::VarText,             std::regex("\".*?\"", std::regex_constants::icase)},
        {TokenType::VarReal,             std::regex("[-+]?0\\.[0-9]+|[1-9][0-9]*\\.[0-9]+", std::regex_constants::icase)},
        {TokenType::VarInt,              std::regex("[-+]?0|[-+]?[1-9][0-9]*", std::regex_constants::icase)},
        {TokenType::VarId,               std::regex("[a-z][a-z0-9]*", std::regex_constants::icase)},
        {TokenType::Operation,           std::regex(">=|<=|!=|=|<|>", std::regex_constants::icase)},
        {TokenType::ParenthesisOpening,  std::regex("\\(", std::regex_constants::icase)},
        {TokenType::ParenthesisClosing,  std::regex("\\)", std::regex_constants::icase)},
        {TokenType::CurlyBracketOpening, std::regex("\\{", std::regex_constants::icase)},
        {TokenType::CurlyBracketClosing, std::regex("\\}", std::regex_constants::icase)},
        {TokenType::Semicolon,           std::regex(";", std::regex_constants::icase)},
        {TokenType::Comma,               std::regex(",", std::regex_constants::icase)}
} };
// clang-format on
} // namespace

Lexer::Lexer(std::string_view parse_string_view)
    : parse_string{parse_string_view}, string_pos{0}, col{1}, row{1}
{
}

Token Lexer::peek()
{
    if (string_pos == parse_string.length()) {
        return Token(TokenType::EndOfFile, "", col, row);
    }

    while (std::end(Lexer::skipsym)
           != std::find(
                   std::begin(Lexer::skipsym),
                   std::end(Lexer::skipsym),
                   parse_string.at(string_pos))) {
        col++;
        if (parse_string.at(string_pos) == '\n') {
            col = 1;
            row++;
        }
        if (++string_pos == parse_string.length()) {
            return Token(TokenType::EndOfFile, "", col, row);
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
                    col,
                    row);
            return token_to_return;
        }
    }

    return Token(
            TokenType::Unknown,
            std::string_view(parse_string.data() + string_pos++, 1),
            col,
            row);
}

Token Lexer::get()
{
    auto res_token = peek();
    for (char sym : res_token.lexeme) {
        col++;
        if (sym == '\n') {
            col = 1;
            row++;
        }
    }
    string_pos += res_token.lexeme.length();
    return res_token;
}