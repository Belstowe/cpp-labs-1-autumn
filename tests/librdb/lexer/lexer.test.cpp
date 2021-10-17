#include <algorithm>
#include <sstream>
#include <vector>
#include <queue>

#include "gtest/gtest.h"

#include "librdb/lexer/lexer.hpp"

TEST(LexerTest, HandlesRubbishInput) {
    std::istringstream instream("!///\\    *&&^%$# @  <<<>\n'");
    rdb::parser::Lexer lexer(instream);
    std::vector<rdb::parser::Token> token_seq;
    while (true) {
        token_seq.push_back(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::Token::EndOfFile)
            break;
    }
    std::for_each(token_seq.begin(), token_seq.end() - 1, [](rdb::parser::Token &token) { EXPECT_EQ(token.type_get(), rdb::parser::Token::Unknown); } );
    EXPECT_EQ(token_seq.back().type_get(), rdb::parser::Token::EndOfFile);
}

TEST(LexerTest, HandlesExprInput) {
    std::istringstream instream("((({<= <= >})))");
    rdb::parser::Lexer lexer(instream);
    std::queue<rdb::parser::Token> token_seq;
    std::queue<rdb::parser::Token::TokenType> token_expected_seq(
        { rdb::parser::Token::ParenthesisOpening, rdb::parser::Token::ParenthesisOpening, rdb::parser::Token::ParenthesisOpening,
          rdb::parser::Token::CurlyBracketOpening, rdb::parser::Token::Operation, rdb::parser::Token::Operation,
          rdb::parser::Token::Operation, rdb::parser::Token::CurlyBracketClosing, rdb::parser::Token::ParenthesisClosing,
          rdb::parser::Token::ParenthesisClosing, rdb::parser::Token::ParenthesisClosing, rdb::parser::Token::EndOfFile });
    
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