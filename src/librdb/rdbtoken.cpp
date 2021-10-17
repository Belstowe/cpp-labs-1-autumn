#include "rdbtoken.hpp"

#include <unordered_map>

using rdb::parser::Token;

Token::Token(const Token::TokenType& type_arg, const std::string& lexeme_arg)
{
    type = type_arg;
    lexeme = lexeme_arg;
}

Token::TokenType Token::type_get()
{
    return type;
}

int Token::type_set(const Token::TokenType& new_type)
{
    type = new_type;
    return 0;
}

std::string_view Token::lexeme_get()
{
    return std::string_view(lexeme);
}

int Token::lexeme_set(const std::string& new_lexeme)
{
    lexeme.assign(new_lexeme);
    return 0;
}

Token::TokenType Token::str_to_type(const std::string& search_for)
{
    static const std::unordered_map<std::string, Token::TokenType>& keywords
            = {{"create", Token::KwCreate},
               {"select", Token::KwSelect},
               {"insert", Token::KwInsert},
               {"delete", Token::KwDelete},
               {"drop", Token::KwDrop},
               {"values", Token::KwValues},
               {"table", Token::KwTable},
               {"int", Token::KwInt},
               {"real", Token::KwReal},
               {"text", Token::KwText},
               {"where", Token::KwWhere},
               {"from", Token::KwFrom},
               {"into", Token::KwInto}};

    auto search = keywords.find(search_for);
    if (search != keywords.end()) {
        return search->second;
    }

    return Token::Unknown;
}