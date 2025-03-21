#pragma once
#include "Matrix.h"
#include "Triangle.h"

// Represented as 2 triangles (as is, wastes 2 vertices (duplicate) and draws 1 extra line (also duplicate))
struct Quad {
	Triangle triangles[2];

	Quad();

	Quad(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c, const Mat& vertex_d);

	Quad(const Mat vertices[4]);

	Quad(const Triangle& triangle_a, const Triangle& triangle_b);

	Quad(const Triangle triangles[2]);
};
