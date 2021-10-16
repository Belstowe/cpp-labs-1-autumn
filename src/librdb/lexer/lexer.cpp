#include "lexer.hpp"

using namespace rdb::parser;

Lexer::Lexer(const std::istream &source) {
    instream = &source;
}

Token Lexer::get() {
    return Token(EndOfFile, "");
}

Token Lexer::peek() {
    return Token(EndOfFile, "");
}