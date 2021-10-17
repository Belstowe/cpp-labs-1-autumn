#include "lexer.hpp"

#include <cctype>

using namespace rdb::parser;

Lexer::Lexer(std::istream& source)
{
    instream = &source;
}

Token Lexer::token_extract_begin(std::string& lexeme)
{
    int c;
    while (std::end(Lexer::spaces)
           != std::find(
                   std::begin(Lexer::spaces),
                   std::end(Lexer::spaces),
                   instream->peek()))
        c = instream->get();
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
        c = instream->get();
        return token_extract_str(lexeme);

    case EOF:
    case '\0':
        return Token(Token::EndOfFile, lexeme);
    }
    if (('1' <= c) && (c <= '9'))
        return token_extract_int(lexeme);
    if (('a' <= std::tolower(c)) && (std::tolower(c) <= 'z'))
        return token_extract_id(lexeme);

    c = instream->get();
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_sym(std::string& lexeme)
{
    char c;
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
    }

    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_op(std::string& lexeme)
{
    static const std::string valid_operations[]
            = {"!=", "=", "<=", ">=", "<", ">"};

    char c;
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
                lexeme))
        return Token(Token::Operation, lexeme);

    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_sign(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_int0(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_int(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_real(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_str(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::token_extract_id(std::string& lexeme)
{
    // TBD
    char c;
    instream->get(c);
    return Token(Token::Unknown, lexeme);
}

Token Lexer::get()
{
    std::string lexeme("");
    return token_extract_begin(lexeme);
}

Token Lexer::peek()
{
    // TBD
    return Token(Token::EndOfFile, "");
}