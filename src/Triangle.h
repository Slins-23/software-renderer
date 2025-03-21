#pragma once
#include "Matrix.h"
#include <string>

// Represented as 3 vertices
struct Triangle {
	Mat vertices[3] = {
		Mat({{1}, {0}, {0}, {1}}, 4, 1),
		Mat({{0}, {1}, {0}, {1}}, 4, 1),
		Mat({{0}, {0}, {1}, {1}}, 4, 1)
	};

	Triangle() {};

	Triangle(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c);

	Triangle(const Mat vertices[3]);
};