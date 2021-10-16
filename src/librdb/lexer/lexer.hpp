#pragma once

#include "rdbtoken.hpp"
#include <iostream>

namespace rdb::parser {
    class Lexer {
        public:
            explicit Lexer(const std::istream &source = std::cin);
            Token get();
            Token peek();

        private:
            const std::istream *instream;
    };
}