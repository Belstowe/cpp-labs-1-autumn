#pragma once

#include "rdbtoken.hpp"
#include <iostream>

namespace rdb::parser {
    class Lexer {
        public:
            explicit Lexer(std::istream &source = std::cin);
            Token get();
            Token peek();

        private:
            std::istream *instream;
            Token token_extract_begin(std::string &lexeme);
            Token token_extract_sym(std::string &lexeme);
            Token token_extract_op(std::string &lexeme);
            Token token_extract_sign(std::string &lexeme);
            Token token_extract_int0(std::string &lexeme);
            Token token_extract_int(std::string &lexeme);
            Token token_extract_real(std::string &lexeme);
            Token token_extract_str(std::string &lexeme);
            Token token_extract_id(std::string &lexeme);
            static constexpr char spaces[] = {' ', '\n', '\r', '\t'};
            static constexpr char operations_syms[] = {'<', '>', '!', '='};
    };
}