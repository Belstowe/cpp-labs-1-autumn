#include "gtest/gtest.h"

#include "librdb/rdbtoken.hpp"

TEST(TokenTest, DataStability) {
    rdb::parser::Token::TokenType type = rdb::parser::Token::Operation;
    std::string lexeme("<=");
    rdb::parser::Token token(type, lexeme);

    ASSERT_EQ(type, token.type_get());
    ASSERT_EQ(lexeme, token.lexeme_get());

    token = rdb::parser::Token();

    ASSERT_EQ(type, rdb::parser::Token::Operation);
    ASSERT_EQ(lexeme, "<=");

    rdb::parser::Token token2(type, lexeme);
    type = rdb::parser::Token::Unknown;
    lexeme = std::string();

    ASSERT_EQ(token2.lexeme_get(), "<=");
    ASSERT_EQ(token2.type_get(), rdb::parser::Token::Operation);
}