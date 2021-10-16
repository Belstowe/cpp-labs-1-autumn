#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    CLI::App app("SQLParser");

    std::string input_file;
    std::string output_file;

    app.add_option<std::string>("-i,--input", input_file, "Input File");
    app.add_option<std::string>("-o,--output", output_file, "Output File");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    return 0;
}
