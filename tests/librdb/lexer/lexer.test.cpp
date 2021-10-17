#include <algorithm>
#include <queue>
#include <sstream>
#include <string_view>
#include <vector>

#include "gtest/gtest.h"

#include "librdb/lexer/lexer.hpp"

using rdb::parser::Lexer;
using rdb::parser::Token;

TEST(LexerTest, HandlesRubbishInput)
{
    std::istringstream instream("!///\\    *&&^%$# @  <<<>\n'\"ghf");
    Lexer lexer(instream);
    std::vector<Token> token_seq;
    while (true) {
        token_seq.push_back(lexer.get());
        if (token_seq.back().type_get() == Token::EndOfFile)
            break;
    }
    std::for_each(token_seq.begin(), token_seq.end() - 1, [](Token& token) {
        EXPECT_EQ(token.type_get(), Token::Unknown);
    });
    EXPECT_EQ(token_seq.back().type_get(), Token::EndOfFile);
}

TEST(LexerTest, HandlesExprInput)
{
    std::istringstream instream("((({<= <= >})))");
    Lexer lexer(instream);
    std::queue<Token> token_seq;
    std::queue<Token::TokenType> token_expected_seq(
            {Token::ParenthesisOpening,
             Token::ParenthesisOpening,
             Token::ParenthesisOpening,
             Token::CurlyBracketOpening,
             Token::Operation,
             Token::Operation,
             Token::Operation,
             Token::CurlyBracketClosing,
             Token::ParenthesisClosing,
             Token::ParenthesisClosing,
             Token::ParenthesisClosing,
             Token::EndOfFile});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == Token::EndOfFile)
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
    Lexer lexer(instream);
    std::queue<Token> token_seq;
    std::queue<Token::TokenType> token_expected_seq(
            {Token::ParenthesisOpening,
             Token::VarReal,
             Token::Operation,
             Token::VarInt,
             Token::ParenthesisClosing,
             Token::ParenthesisOpening,
             Token::VarInt,
             Token::Operation,
             Token::VarReal,
             Token::ParenthesisClosing,
             Token::ParenthesisOpening,
             Token::VarInt,
             Token::Operation,
             Token::VarInt,
             Token::ParenthesisClosing,
             Token::EndOfFile});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == Token::EndOfFile)
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
    std::istringstream instream(
            "(\"(lorem ipsum )\")\"66 domina\\\"   \n 0\"0");
    Lexer lexer(instream);
    std::queue<Token> token_seq;
    std::queue<Token::TokenType> token_type_expected_seq(
            {Token::ParenthesisOpening,
             Token::VarText,
             Token::ParenthesisClosing,
             Token::VarText,
             Token::VarInt,
             Token::EndOfFile});
    std::queue<std::string_view> token_lexeme_expected_seq(
            {std::string_view("("),
             std::string_view("(lorem ipsum )"),
             std::string_view(")"),
             std::string_view("66 domina\"    0"),
             std::string_view("0"),
             std::string_view()});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == Token::EndOfFile)
            break;
    }

    EXPECT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(
                token_seq.front().type_get(), token_type_expected_seq.front());
        EXPECT_EQ(
                token_seq.front().lexeme_get(),
                token_lexeme_expected_seq.front());
        token_seq.pop();
        token_type_expected_seq.pop();
        token_lexeme_expected_seq.pop();
    }
}

TEST(LexerTest, HandlesIdKwInput)
{
    std::istringstream instream(
            "TEXT tXt = (\"{Never gonna give you up! Never gonna let you "
            "down!}\");\nint i=0;\nReAl PI =3.1415;");
    Lexer lexer(instream);
    std::queue<Token> token_seq;
    std::queue<Token::TokenType> token_type_expected_seq(
            {Token::KwText,
             Token::VarId,
             Token::Operation,
             Token::ParenthesisOpening,
             Token::VarText,
             Token::ParenthesisClosing,
             Token::Semicolon,
             Token::KwInt,
             Token::VarId,
             Token::Operation,
             Token::VarInt,
             Token::Semicolon,
             Token::KwReal,
             Token::VarId,
             Token::Operation,
             Token::VarReal,
             Token::Semicolon,
             Token::EndOfFile});
    std::queue<std::string_view> token_lexeme_expected_seq(
            {std::string_view("text"),
             std::string_view("txt"),
             std::string_view("="),
             std::string_view("("),
             std::string_view(
                     "{Never gonna give you up! Never gonna let you down!}"),
             std::string_view(")"),
             std::string_view(";"),
             std::string_view("int"),
             std::string_view("i"),
             std::string_view("="),
             std::string_view("0"),
             std::string_view(";"),
             std::string_view("real"),
             std::string_view("pi"),
             std::string_view("="),
             std::string_view("3.1415"),
             std::string_view(";"),
             std::string_view()});

    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == Token::EndOfFile)
            break;
    }

    EXPECT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(
                token_seq.front().type_get(), token_type_expected_seq.front());
        ASSERT_EQ(
                token_seq.front().lexeme_get(),
                token_lexeme_expected_seq.front());
        token_seq.pop();
        token_type_expected_seq.pop();
        token_lexeme_expected_seq.pop();
    }
}