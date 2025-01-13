#include "catch_amalgamated.hpp"

int main(int argc, char** argv) {
	Catch::Session session;

	int error_code = session.applyCommandLine(argc, argv);
	if (error_code != 0) return error_code;

	uint16_t failed_tests = session.run();

	return failed_tests;
}