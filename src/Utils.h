#pragma once
#include <string>

namespace Utils {
	static double normalize(double prev_value, double prev_min, double prev_max, double new_min, double new_max) {
		if (prev_value < prev_min) {
			throw std::runtime_error("Error: Could not normalize value " + std::to_string(prev_value) + " to range" + "[" + std::to_string(new_min) + ", " + std::to_string(new_max) + "]" + ". Value is not within supplied range : [" + std::to_string(prev_min) + ", " + std::to_string(prev_max) + "]. (Value is less than " + std::to_string(prev_min) + ")");
		}
		else if (prev_value > prev_max) {
			throw std::runtime_error("Error: Could not normalize value " + std::to_string(prev_value) + " to range" + "[" + std::to_string(new_min) + ", " + std::to_string(new_max) + "]" + ". Value is not within supplied range: [" + std::to_string(prev_min) + ", " + std::to_string(prev_max) + "]. (Value is greater than " + std::to_string(prev_max) + ")");
		}

		double prev_value_range = prev_max - prev_min;
		double percentage_of_previous_range = (prev_value - prev_min) / prev_value_range;
		double new_value_range = new_max - new_min;
		return new_min + (new_value_range * percentage_of_previous_range);
	}

	static double round_to(double number, double decimal_places) {
		double scale = pow(10, decimal_places);
		return round(number * scale) / scale;
	}
}