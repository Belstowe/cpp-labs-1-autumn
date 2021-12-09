#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "librdb/lexer/lexer.hpp"
#include "librdb/parser/Parser.hpp"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    CLI::App app("SQLParser");

    std::string input_file;
    std::string output_file;

    std::ifstream input_file_stream;
    std::ofstream output_file_stream;

    CLI::Option* opt_i = app.add_option<std::string>(
            "-i,--input", input_file, "Input File");
    CLI::Option* opt_o = app.add_option<std::string>(
            "-o,--output", output_file, "Output File");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    std::string sql_inquiry;
    if (*opt_i) {
        input_file_stream.open(input_file, std::ifstream::in);
        sql_inquiry = std::string(
                std::istreambuf_iterator<char>(input_file_stream), {});
    } else {
        std::cout << "Type an SQL-expression:" << '\n';
        std::getline(std::cin, sql_inquiry);
    }

    std::ostream* output_stream = &std::cout;
    if (*opt_o) {
        output_file_stream.open(
                output_file, std::ofstream::out);
        output_stream = &output_file_stream;
    }

    rdb::parser::Lexer lexer(sql_inquiry);
    rdb::parser::Parser parser(lexer);
    rdb::parser::ParseResult sql;
    parser.parse_sql(sql);

    *output_stream << sql._sql_script << "\n";
    for (auto&& error: sql._errors) {
        std::clog << error << "\n";
    }

    if (input_file_stream.is_open()) {
        input_file_stream.close();
    }
    if (output_file_stream.is_open()) {
        output_file_stream.close();
    }

    return 0;
}
