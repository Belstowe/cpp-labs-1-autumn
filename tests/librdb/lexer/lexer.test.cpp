#include <algorithm>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

#include "gtest/gtest.h"

#include "librdb/lexer/lexer.hpp"

using rdb::parser::Lexer;
using rdb::parser::Token;

TEST(LexerTest, HandlesRubbishInput)
{
    std::string instring("!///\\    *&&^%$# @  .\n'\"");
    Lexer lexer(instring);
    std::vector<Token> token_seq;

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type_get() != Token::EndOfFile);

    std::for_each(token_seq.begin(), token_seq.end() - 1, [](Token& token) {
        ASSERT_EQ(token.type_get(), Token::Unknown);
    });
    ASSERT_EQ(token_seq.back().type_get(), Token::EndOfFile);
}

TEST(LexerTest, HandlesExprInput)
{
    std::string instring("((({<= <= >})))");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<Token::TokenType> token_expected_seq(
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

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type_get() != Token::EndOfFile);

    ASSERT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type_get(), token_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesIntInput)
{
    std::string instring("(1.345 <= 4 )(4>0.2)( 0< 66 )");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<Token::TokenType> token_expected_seq(
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

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type_get() != Token::EndOfFile);

    ASSERT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type_get(), token_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesStrInput)
{
    std::string instring("(\"(lorem ipsum )\")\"66 domina\\\"   \n 00");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<Token::TokenType> token_type_expected_seq(
            {Token::ParenthesisOpening,
             Token::VarText,
             Token::ParenthesisClosing,
             Token::VarText,
             Token::VarInt,
             Token::VarInt,
             Token::EndOfFile});
    std::vector<std::string_view> token_lexeme_expected_seq(
            {std::string_view("("),
             std::string_view("\"(lorem ipsum )\""),
             std::string_view(")"),
             std::string_view("\"66 domina\\\""),
             std::string_view("0"),
             std::string_view("0"),
             std::string_view()});

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type_get() != Token::EndOfFile);

    ASSERT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type_get(), token_type_expected_seq[i]);
        ASSERT_EQ(token_seq[i].lexeme_get(), token_lexeme_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesIdKwInput)
{
    std::string instring(
            "TEXT tXt = (\"{Never gonna give you up! Never gonna let you "
            "down!}\");\nint i=0;\nReAl PI =3.1415;");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<Token::TokenType> token_type_expected_seq(
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
    std::vector<std::string_view> token_lexeme_expected_seq(
            {std::string_view("TEXT"),
             std::string_view("tXt"),
             std::string_view("="),
             std::string_view("("),
             std::string_view("\"{Never gonna give you up! Never gonna let you "
                              "down!}\""),
             std::string_view(")"),
             std::string_view(";"),
             std::string_view("int"),
             std::string_view("i"),
             std::string_view("="),
             std::string_view("0"),
             std::string_view(";"),
             std::string_view("ReAl"),
             std::string_view("PI"),
             std::string_view("="),
             std::string_view("3.1415"),
             std::string_view(";"),
             std::string_view()});

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type_get() != Token::EndOfFile);

    ASSERT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type_get(), token_type_expected_seq[i]);
        ASSERT_EQ(token_seq[i].lexeme_get(), token_lexeme_expected_seq[i]);
    }
}

TEST(LexerTest, LexerPeekTest)
{
    std::string instring("INT a = 0;");
    Lexer lexer(instring);
    Token token;

    token = lexer.peek();
    ASSERT_EQ(token.type_get(), Token::KwInt);
    ASSERT_EQ(token.lexeme_get(), "INT");
    token = lexer.get();
    ASSERT_EQ(token.type_get(), Token::KwInt);
    ASSERT_EQ(token.lexeme_get(), "INT");
    token = lexer.peek();
    ASSERT_EQ(token.type_get(), Token::VarId);
    ASSERT_EQ(token.lexeme_get(), "a");
}