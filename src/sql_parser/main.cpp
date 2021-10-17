#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "librdb/lexer/lexer.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    CLI::App app("SQLParser");

    std::string input_file;
    std::string output_file;

    std::ifstream input_file_stream;
    std::ofstream output_file_stream;

    CLI::Option *opt_i = app.add_option<std::string>("-i,--input", input_file, "Input File");
    CLI::Option *opt_o = app.add_option<std::string>("-o,--output", output_file, "Output File");

    opt_i->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    input_file_stream.open(input_file, std::ifstream::in);
    if (*opt_o)
        output_file_stream.open(output_file, std::ofstream::out | std::ofstream::app);

    rdb::parser::Lexer lexer(input_file_stream);
    rdb::parser::Token current_token;
    while (true) {
        current_token = lexer.get();
        std::cout << '(' << current_token.type_get() << ", " << current_token.lexeme_get() << ')' << '\n';
        if (current_token.type_get() == rdb::parser::Token::EndOfFile)
            break;
        if (current_token.type_get() == rdb::parser::Token::Unknown)
            std::clog << current_token.lexeme_get() << ": Unknown type\n";
    }

    input_file_stream.close();
    if (output_file_stream.is_open())
        output_file_stream.close();
    
    return 0;
}
