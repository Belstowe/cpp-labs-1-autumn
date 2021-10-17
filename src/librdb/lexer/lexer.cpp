#include "lexer.hpp"

#include <algorithm>
#include <array>
#include <cctype>

using rdb::parser::Lexer;
using rdb::parser::Token;

Lexer::Lexer(std::istream& source)
{
    instream = &source;
}

Token Lexer::token_extract_begin(std::string& lexeme)
{
    int c = 0;
    while (std::end(Lexer::spaces)
           != std::find(
                   std::begin(Lexer::spaces),
                   std::end(Lexer::spaces),
                   instream->peek())) {
        instream->get();
    }

    c = instream->peek();

    switch (c) {
    case '(':
    case ')':
    case ';':
    case ',':
    case '{':
    case '}':
        return token_extract_sym(lexeme);

    case '<':
    case '>':
    case '=':
    case '!':
        return token_extract_op(lexeme);

    case '+':
    case '-':
        return token_extract_sign(lexeme);

    case '0':
        return token_extract_int0(lexeme);

    case '.':
        lexeme += '0';
        return token_extract_real(lexeme);

    case '"':
        instream->get();
        return token_extract_str(lexeme);

    case EOF:
    case '\0':
        return Token(Token::EndOfFile, lexeme);

    default:
        break;
    }
    if (('1' <= c) && (c <= '9')) {
        return token_extract_int(lexeme);
    }
    if (('a' <= std::tolower(c)) && (std::tolower(c) <= 'z')) {
        return token_extract_id(lexeme);
    }

    char sym = '\0';
    instream->get(sym);
    lexeme += sym;
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_sym(std::string& lexeme)
{
    char c = '\0';
    instream->get(c);
    lexeme += c;

    switch (c) {
    case '(':
        return Token(Token::ParenthesisOpening, lexeme);

    case ')':
        return Token(Token::ParenthesisClosing, lexeme);

    case '{':
        return Token(Token::CurlyBracketOpening, lexeme);

    case '}':
        return Token(Token::CurlyBracketClosing, lexeme);

    case ';':
        return Token(Token::Semicolon, lexeme);

    case ',':
        return Token(Token::Comma, lexeme);

    default:
        break;
    }

    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_op(std::string& lexeme)
{
    static const std::array<std::string, 6> valid_operations
            = {"!=", "=", "<=", ">=", "<", ">"};

    char c = '\0';
    while (std::end(Lexer::operations_syms)
           != std::find(
                   std::begin(Lexer::operations_syms),
                   std::end(Lexer::operations_syms),
                   instream->peek())) {
        instream->get(c);
        lexeme += c;
    }

    if (std::end(valid_operations)
        != std::find(
                std::begin(valid_operations),
                std::end(valid_operations),
                lexeme)) {
        return Token(Token::Operation, lexeme);
    }

    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_sign(std::string& lexeme)
{
    char c = '\0';
    instream->get(c);
    lexeme += c;
    if ((c != '+') && (c != '-')) {
        return Token(Token::Unknown, lexeme);
    }

    if (instream->peek() == '0') {
        return token_extract_int0(lexeme);
    }
    if (('1' <= instream->peek()) && (instream->peek() <= '9')) {
        return token_extract_int(lexeme);
    }

    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_int0(std::string& lexeme)
{
    char c = '\0';
    instream->get(c);
    lexeme += c;
    if (c != '0') {
        return Token(Token::Unknown, lexeme);
    }

    if (instream->peek() == '.') {
        return token_extract_real(lexeme);
    }

    if (('0' <= instream->peek()) && (instream->peek() <= '9')) {
        return Token(Token::Unknown, lexeme);
    }

    return Token(Token::VarInt, lexeme);
}

Token Lexer::token_extract_int(std::string& lexeme)
{
    char c = '\0';
    instream->get(c);
    lexeme += c;
    if (('1' > c) || (c > '9')) {
        return Token(Token::Unknown, lexeme);
    }

    while (('0' <= instream->peek()) && (instream->peek() <= '9')) {
        instream->get(c);
        lexeme += c;
    }

    if (instream->peek() == '.') {
        return token_extract_real(lexeme);
    }

    return Token(Token::VarInt, lexeme);
}

Token Lexer::token_extract_real(std::string& lexeme)
{
    char c = '\0';
    instream->get(c);
    lexeme += c;
    if (c != '.') {
        return Token(Token::Unknown, lexeme);
    }

    if (('0' > instream->peek()) || (instream->peek() > '9')) {
        return Token(Token::Unknown, lexeme);
    }

    while (('0' <= instream->peek()) && (instream->peek() <= '9')) {
        instream->get(c);
        lexeme += c;
    }

    return Token(Token::VarReal, lexeme);
}

Token Lexer::token_extract_str(std::string& lexeme)
{
    char c = '\0';
    int next_char = instream->peek();
    while ((next_char != '"') && (next_char != '\0') && (next_char != EOF)) {
        if ((next_char == '\n') || (next_char == '\r')) {
            instream->get();
            next_char = instream->peek();
            continue;
        }
        if (next_char == '\\') {
            instream->get();
            next_char = instream->peek();
            if ((next_char == '\0') || (next_char == EOF)) {
                break;
            }
        }
        instream->get(c);
        lexeme += c;
        next_char = instream->peek();
    }

    if (next_char != '"') {
        return Token(Token::Unknown, lexeme);
    }

    instream->get();
    return Token(Token::VarText, lexeme);
}

Token Lexer::token_extract_id(std::string& lexeme)
{
    // TBD
    char c = '\0';
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::get()
{
    std::string lexeme;
    return token_extract_begin(lexeme);
}

Token Lexer::peek()
{
    // TBD
    std::string lexeme;
    return token_extract_begin(lexeme);
}