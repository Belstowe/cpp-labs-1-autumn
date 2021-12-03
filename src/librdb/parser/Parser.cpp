#include "Parser.hpp"

using namespace rdb::parser;

Parser::Parser(Lexer& lexer)
    : _lexer{lexer}
{
}