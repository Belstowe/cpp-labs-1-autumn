#include "lexer.hpp"

#include <algorithm>
#include <cctype>
#include <regex>

using rdb::parser::Lexer;
using rdb::parser::Token;

namespace {
struct TokenRule {
    std::regex regex;
    Token::TokenType tokentype;
};

const std::vector<TokenRule> TokenRules
        = {{std::regex("\".*?\"", std::regex_constants::icase), Token::VarText},
           {std::regex(
                    "[-+]?0\\.[0-9]+|[1-9][0-9]*\\.[0-9]+",
                    std::regex_constants::icase),
            Token::VarReal},
           {std::regex("[-+]?0|[-+]?[1-9][0-9]*", std::regex_constants::icase),
            Token::VarInt},
           {std::regex("CREATE", std::regex_constants::icase), Token::KwCreate},
           {std::regex("INSERT", std::regex_constants::icase), Token::KwInsert},
           {std::regex("DELETE", std::regex_constants::icase), Token::KwDelete},
           {std::regex("DROP", std::regex_constants::icase), Token::KwDrop},
           {std::regex("FROM", std::regex_constants::icase), Token::KwFrom},
           {std::regex("INT", std::regex_constants::icase), Token::KwInt},
           {std::regex("INTO", std::regex_constants::icase), Token::KwInto},
           {std::regex("REAL", std::regex_constants::icase), Token::KwReal},
           {std::regex("SELECT", std::regex_constants::icase), Token::KwSelect},
           {std::regex("TABLE", std::regex_constants::icase), Token::KwTable},
           {std::regex("TEXT", std::regex_constants::icase), Token::KwText},
           {std::regex("VALUES", std::regex_constants::icase), Token::KwValues},
           {std::regex("WHERE", std::regex_constants::icase), Token::KwWhere},
           {std::regex("[a-z][a-z0-9]*", std::regex_constants::icase),
            Token::VarId},
           {std::regex(">=|<=|!=|=|<|>", std::regex_constants::icase),
            Token::Operation},
           {std::regex("\\(", std::regex_constants::icase),
            Token::ParenthesisOpening},
           {std::regex("\\)", std::regex_constants::icase),
            Token::ParenthesisClosing},
           {std::regex("\\{", std::regex_constants::icase),
            Token::CurlyBracketOpening},
           {std::regex("\\}", std::regex_constants::icase),
            Token::CurlyBracketClosing},
           {std::regex(";", std::regex_constants::icase), Token::Semicolon},
           {std::regex(",", std::regex_constants::icase), Token::Comma}};
}

Lexer::Lexer(std::string_view parse_string_view)
    : parse_string{parse_string_view}, string_pos{0}
{
}

Token Lexer::peek()
{
    if (string_pos == parse_string.length())
        return Token(Token::EndOfFile, "");

    while (std::end(Lexer::skipsym)
           != std::find(
                   std::begin(Lexer::skipsym),
                   std::end(Lexer::skipsym),
                   parse_string.at(string_pos))) {
        if (++string_pos == parse_string.length())
            return Token(Token::EndOfFile, "");
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
            Token::Unknown,
            std::string_view(parse_string.data() + string_pos++, 1));
}

Token Lexer::get()
{
    auto res_token = peek();
    string_pos += res_token.lexeme_get().length();
    return res_token;
}