#include "gtest/gtest.h"

#include "librdb/rdbtoken.hpp"

using rdb::parser::Token;

TEST(TokenTest, DataStability)
{
    Token::TokenType type = Token::Operation;
    std::string lexeme("<=");
    Token token(type, lexeme);

    ASSERT_EQ(type, token.type_get());
    ASSERT_EQ(lexeme, token.lexeme_get());

    token = Token();

    ASSERT_EQ(type, Token::Operation);
    ASSERT_EQ(lexeme, "<=");

    Token token2(type, lexeme);
    type = Token::Unknown;
    lexeme = std::string();

    ASSERT_EQ(token2.lexeme_get(), "<=");
    ASSERT_EQ(token2.type_get(), Token::Operation);
}