#include "gtest/gtest.h"
#include "librdb/lexer/Lexer.hpp"
#include <algorithm>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

using rdb::parser::Lexer;
using rdb::parser::Token;
using rdb::parser::TokenType;

TEST(LexerTest, HandlesRubbishInput)
{
    std::string instring("!///\\    *&&^%$# @  .\n'\"");
    Lexer lexer(instring);
    std::vector<Token> token_seq;

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type != TokenType::EndOfFile);

    std::for_each(token_seq.begin(), token_seq.end() - 1, [](Token& token) {
        ASSERT_EQ(token.type, TokenType::Unknown);
    });
    ASSERT_EQ(token_seq.back().type, TokenType::EndOfFile);
}

TEST(LexerTest, HandlesExprInput)
{
    std::string instring("((({<= <= >})))");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<TokenType> token_expected_seq(
            {TokenType::ParenthesisOpening,
             TokenType::ParenthesisOpening,
             TokenType::ParenthesisOpening,
             TokenType::CurlyBracketOpening,
             TokenType::Operation,
             TokenType::Operation,
             TokenType::Operation,
             TokenType::CurlyBracketClosing,
             TokenType::ParenthesisClosing,
             TokenType::ParenthesisClosing,
             TokenType::ParenthesisClosing,
             TokenType::EndOfFile});

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type != TokenType::EndOfFile);

    ASSERT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type, token_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesIntInput)
{
    std::string instring("(1.345 <= 4 )(4>0.2)( 0< 66 )");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<TokenType> token_expected_seq(
            {TokenType::ParenthesisOpening,
             TokenType::VarReal,
             TokenType::Operation,
             TokenType::VarInt,
             TokenType::ParenthesisClosing,
             TokenType::ParenthesisOpening,
             TokenType::VarInt,
             TokenType::Operation,
             TokenType::VarReal,
             TokenType::ParenthesisClosing,
             TokenType::ParenthesisOpening,
             TokenType::VarInt,
             TokenType::Operation,
             TokenType::VarInt,
             TokenType::ParenthesisClosing,
             TokenType::EndOfFile});

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type != TokenType::EndOfFile);

    ASSERT_EQ(token_expected_seq.size(), token_seq.size());
    int count = token_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type, token_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesStrInput)
{
    std::string instring("(\"(lorem ipsum )\")\"66 domina\\\"   \n 00");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<TokenType> token_type_expected_seq(
            {TokenType::ParenthesisOpening,
             TokenType::VarText,
             TokenType::ParenthesisClosing,
             TokenType::VarText,
             TokenType::VarInt,
             TokenType::VarInt,
             TokenType::EndOfFile});
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
    } while (token_seq.back().type != TokenType::EndOfFile);

    ASSERT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type, token_type_expected_seq[i]);
        ASSERT_EQ(token_seq[i].lexeme, token_lexeme_expected_seq[i]);
    }
}

TEST(LexerTest, HandlesIdKwInput)
{
    std::string instring(
            "TEXT tXt = (\"{Never gonna give you up! Never gonna let you "
            "down!}\");\nint i=0;\nReAl PI =3.1415;");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<TokenType> token_type_expected_seq(
            {TokenType::KwText,
             TokenType::VarId,
             TokenType::Operation,
             TokenType::ParenthesisOpening,
             TokenType::VarText,
             TokenType::ParenthesisClosing,
             TokenType::Semicolon,
             TokenType::KwInt,
             TokenType::VarId,
             TokenType::Operation,
             TokenType::VarInt,
             TokenType::Semicolon,
             TokenType::KwReal,
             TokenType::VarId,
             TokenType::Operation,
             TokenType::VarReal,
             TokenType::Semicolon,
             TokenType::EndOfFile});
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
    } while (token_seq.back().type != TokenType::EndOfFile);

    ASSERT_EQ(token_type_expected_seq.size(), token_seq.size());
    int count = token_type_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].type, token_type_expected_seq[i]);
        ASSERT_EQ(token_seq[i].lexeme, token_lexeme_expected_seq[i]);
    }
}

TEST(LexerTest, LexerPeekTest)
{
    std::string instring("INT a = 0;");
    Lexer lexer(instring);
    Token token;

    token = lexer.peek();
    ASSERT_EQ(token.type, TokenType::KwInt);
    ASSERT_EQ(token.lexeme, "INT");
    token = lexer.get();
    ASSERT_EQ(token.type, TokenType::KwInt);
    ASSERT_EQ(token.lexeme, "INT");
    token = lexer.peek();
    ASSERT_EQ(token.type, TokenType::VarId);
    ASSERT_EQ(token.lexeme, "a");
}

TEST(LexerTest, CorrectColRowInfo)
{
    std::string instring("INT a;\nText s = \"aaa bbb\";\nreal b = 1.0");
    Lexer lexer(instring);
    std::vector<Token> token_seq;
    std::vector<size_t> token_col_expected_seq(
            {1, 5, 6, 1, 6, 8, 10, 19, 1, 6, 8, 10, 13});
    std::vector<size_t> token_row_expected_seq(
            {1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3});

    do {
        token_seq.push_back(lexer.get());
    } while (token_seq.back().type != TokenType::EndOfFile);

    ASSERT_EQ(token_col_expected_seq.size(), token_seq.size());
    int count = token_col_expected_seq.size();
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(token_seq[i].parsed_col, token_col_expected_seq[i]);
        ASSERT_EQ(token_seq[i].parsed_row, token_row_expected_seq[i]);
    }
}