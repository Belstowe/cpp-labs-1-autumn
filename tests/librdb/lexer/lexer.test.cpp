#include <algorithm>
#include <queue>
#include <string_view>
#include <sstream>
#include <vector>

#include "gtest/gtest.h"

#include "librdb/lexer/lexer.hpp"

TEST(LexerTest, HandlesRubbishInput)
{
    std::istringstream instream("!///\\    *&&^%$# @  <<<>\n'\"ghf");
    rdb::parser::Lexer lexer(instream);
    std::vector<rdb::parser::Token> token_seq;
    while (true) {
        token_seq.push_back(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::Token::EndOfFile)
            break;
    }
    std::for_each(
            token_seq.begin(),
            token_seq.end() - 1,
            [](rdb::parser::Token& token) {
                EXPECT_EQ(token.type_get(), rdb::parser::Token::Unknown);
            });
    EXPECT_EQ(token_seq.back().type_get(), rdb::parser::Token::EndOfFile);
}

TEST(LexerTest, HandlesExprInput)
{
    std::istringstream instream("((({<= <= >})))");
    rdb::parser::Lexer lexer(instream);
    std::queue<rdb::parser::Token> token_seq;
    std::queue<rdb::parser::Token::TokenType> token_expected_seq(
            {rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::CurlyBracketOpening,
             rdb::parser::Token::Operation,
             rdb::parser::Token::Operation,
             rdb::parser::Token::Operation,
             rdb::parser::Token::CurlyBracketClosing,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::EndOfFile});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::Token::EndOfFile)
            break;
    }

    EXPECT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(token_seq.front().type_get(), token_expected_seq.front());
        token_seq.pop();
        token_expected_seq.pop();
    }
}

TEST(LexerTest, HandlesIntInput)
{
    std::istringstream instream("(1.345 <= 4 )(4>.2)( 0< 66 )");
    rdb::parser::Lexer lexer(instream);
    std::queue<rdb::parser::Token> token_seq;
    std::queue<rdb::parser::Token::TokenType> token_expected_seq(
            {rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::VarReal,
             rdb::parser::Token::Operation,
             rdb::parser::Token::VarInt,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::VarInt,
             rdb::parser::Token::Operation,
             rdb::parser::Token::VarReal,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::VarInt,
             rdb::parser::Token::Operation,
             rdb::parser::Token::VarInt,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::EndOfFile});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::Token::EndOfFile)
            break;
    }

    EXPECT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(token_seq.front().type_get(), token_expected_seq.front());
        token_seq.pop();
        token_expected_seq.pop();
    }
}

TEST(LexerTest, HandlesStrInput)
{
    std::istringstream instream("(\"(lorem ipsum )\")\"66 domina\\\"   \n 0\"0");
    rdb::parser::Lexer lexer(instream);
    std::queue<rdb::parser::Token> token_seq;
    std::queue<rdb::parser::Token::TokenType> token_type_expected_seq(
            {rdb::parser::Token::ParenthesisOpening,
             rdb::parser::Token::VarText,
             rdb::parser::Token::ParenthesisClosing,
             rdb::parser::Token::VarText,
             rdb::parser::Token::VarInt,
             rdb::parser::Token::EndOfFile});
    std::queue<std::string_view> token_lexeme_expected_seq(
            {std::string_view("("),
             std::string_view("(lorem ipsum )"),
             std::string_view(")"),
             std::string_view("66 domina\"    0"),
             std::string_view("0"),
             std::string_view()});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::Token::EndOfFile)
            break;
    }

    EXPECT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(token_seq.front().type_get(), token_type_expected_seq.front());
        EXPECT_EQ(token_seq.front().lexeme_get(), token_lexeme_expected_seq.front());
        token_seq.pop();
        token_type_expected_seq.pop();
        token_lexeme_expected_seq.pop();
    }
}