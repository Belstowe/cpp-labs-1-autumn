#include "gtest/gtest.h"

#include "librdb/rdbtoken.hpp"

TEST(TokenTest, DataStability) {
    rdb::parser::TokenType type = rdb::parser::Operation;
    std::string lexeme("<=");
    rdb::parser::Token token(type, lexeme);

    ASSERT_EQ(type, token.type_get());
    ASSERT_EQ(lexeme, token.lexeme_get());

    token = rdb::parser::Token();

    ASSERT_EQ(type, rdb::parser::Operation);
    ASSERT_EQ(lexeme, "<=");

    rdb::parser::Token token2(type, lexeme);
    type = rdb::parser::Unknown;
    lexeme = std::string();

    ASSERT_EQ(token2.lexeme_get(), "<=");
    ASSERT_EQ(token2.type_get(), rdb::parser::Operation);
}