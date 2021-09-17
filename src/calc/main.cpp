#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "sum.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    CLI::App app("BelMoProTech Lab #1");

    double a = .0;
    double b = .0;
    app.add_option<double>("--first", a, "First number");
    app.add_option<double>("--second", b, "Second number");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if ((app.count("--first") != 1) || (app.count("--second") != 1)) {
        std::cout << "Valid use is:" << std::endl;
        std::cout << " $ ./calc_app --first {num} --second {num}" << std::endl;
        return 0;
    }

    std::cout << Summator::sum(a, b) << std::endl;
    return 0;
}
