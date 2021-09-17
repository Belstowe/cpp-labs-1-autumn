#include <sum.hpp>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

#include <iostream>

int main(int argc, char **argv) {
	CLI::App app("BelMoProTech Lab #1");

	double a, b;
	app.add_option<double>("--first", a, "First number");
	app.add_option<double>("--second", b, "Second number");

	try {
		app.parse(argc, argv);
	} catch (const CLI::ParseError &e) {
		return app.exit(e);
	}

	std::cout << Summator::sum(a, b) << std::endl;
	return 0;
}
