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
        if (token_seq.back().type_get() == rdb::parser::EndOfFile)
            break;
    }
    std::for_each(token_seq.begin(), token_seq.end() - 1, [](rdb::parser::Token &token) { EXPECT_EQ(token.type_get(), rdb::parser::Unknown); } );
    EXPECT_EQ(token_seq.back().type_get(), rdb::parser::EndOfFile);
}

TEST(LexerTest, HandlesExprInput) {
    std::istringstream instream("((({<= <= >})))");
    rdb::parser::Lexer lexer(instream);
    std::queue<rdb::parser::Token> token_seq;
    std::queue<rdb::parser::TokenType> token_expected_seq(
        { rdb::parser::ParenthesisOpening, rdb::parser::ParenthesisOpening, rdb::parser::ParenthesisOpening,
          rdb::parser::CurlyBracketOpening, rdb::parser::Operation, rdb::parser::Operation,
          rdb::parser::Operation, rdb::parser::CurlyBracketClosing, rdb::parser::ParenthesisClosing,
          rdb::parser::ParenthesisClosing, rdb::parser::ParenthesisClosing, rdb::parser::EndOfFile });
    
    while (true) {
        token_seq.push(lexer.get());
        if (token_seq.back().type_get() == rdb::parser::EndOfFile)
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