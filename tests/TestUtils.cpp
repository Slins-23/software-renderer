#include "catch_amalgamated.hpp"
#include "Utils.h"

TEST_CASE("Normalize", "[utils]") {


	SECTION("Base cases") {
		double prev_min = 2;
		double prev_value = 1;
		double prev_max = 4;

		double new_min = -4;
		double new_max = -2;

		// Value was outside given range (less than the minimum)
		REQUIRE_THROWS_AS(Utils::normalize(prev_value, prev_min, prev_max, new_min, new_max), std::runtime_error);

		prev_value = 5;

		// Value was outside given range (more than the maximum)
		REQUIRE_THROWS_AS(Utils::normalize(prev_value, prev_min, prev_max, new_min, new_max), std::runtime_error);

		prev_value = 3;
		double value = Utils::normalize(prev_value, prev_min, prev_max, new_min, new_max);
		REQUIRE(value == -3);

		prev_min = -1;
		prev_value = 0;
		prev_max = 1;

		new_min = -6;
		new_max = 3;

		value = Utils::normalize(prev_value, prev_min, prev_max, new_min, new_max);
		REQUIRE(value == -1.5);
	}
}

TEST_CASE("Decimal place rounding", "[utils]") {

	SECTION("Base cases") {
		double result = Utils::round_to(2.4444, 1);
		REQUIRE(result == 2.4);

		result = Utils::round_to(2.4444, 2);
		REQUIRE(result == 2.44);

		result = Utils::round_to(2.4464, 2);
		REQUIRE(result == 2.45);

		result = Utils::round_to(3.1415, 2);
		REQUIRE(result == 3.14);

	}
}