#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

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

    /* 
     * In this section we would then pass our Input Stream to Lexer;
     * Lexer would then return sequence of Tokens;
     * This sequence would then, together with Output Stream, get passed into Parser.
     */

    input_file_stream.close();
    if (output_file_stream.is_open())
        output_file_stream.close();
    
    return 0;
}
