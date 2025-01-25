#include "Triangle.h"

Triangle::Triangle(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c) {
	// Mat class validates whether all given vertices are exactly 3x1 inside its constructors
	if (vertex_a.rows != 3 && vertex_a.rows != 4) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 3 or 4 rows, it has " + std::to_string(vertex_a.rows) + " row(s). (Should have exactly 3 or 4)");
	}
	else if (vertex_a.cols != 1) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 1 column, it has " + std::to_string(vertex_a.cols) + " column(s). (Should have exactly 1)");
	}

	this->vertices[0] = Mat(
		{
			{vertex_a.get(1, 1)},
			{vertex_a.get(2, 1)},
			{vertex_a.get(3, 1)},
			{1}
		}
	, 4, 1);

	if (vertex_b.rows != 3 && vertex_b.rows != 4) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 3 or 4 rows, it has " + std::to_string(vertex_b.rows) + " row(s). (Should have exactly 3 or 4)");
	}
	else if (vertex_b.cols != 1) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 1 column, it has " + std::to_string(vertex_b.cols) + " column(s). (Should have exactly 1)");
	}

	this->vertices[1] = Mat(
		{
			{vertex_b.get(1, 1)},
			{vertex_b.get(2, 1)},
			{vertex_b.get(3, 1)},
			{1}
		}
	, 4, 1);

	if (vertex_c.rows != 3 && vertex_c.rows != 4) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 3 or 4 rows, it has " + std::to_string(vertex_c.rows) + " row(s). (Should have exactly 3 or 4)");
	}
	else if (vertex_c.cols != 1) {
		throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 1 column, it has " + std::to_string(vertex_c.cols) + " column(s). (Should have exactly 1)");
	}

	this->vertices[2] = Mat(
		{
			{vertex_c.get(1, 1)},
			{vertex_c.get(2, 1)},
			{vertex_c.get(3, 1)},
			{1}
		}
	, 4, 1);
};

Triangle::Triangle(const Mat vertices[3]) {
	for (int i = 0; i < 3; i++) {
		if (vertices[i].rows != 3 && vertices[i].rows != 4) {
			throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 3 or 4 rows, it has " + std::to_string(vertices[i].rows) + " row(s). (Should have exactly 3 or 4)");
		}
		else if (vertices[i].cols != 1) {
			throw std::runtime_error("Error: Could not create a triangle. At least one of the vertices does not have 1 column, it has " + std::to_string(vertices[i].cols) + " column(s). (Should have exactly 1)");
		}

		this->vertices[i] = Mat(
			{
				{vertices[i].get(1, 1)},
				{vertices[i].get(2, 1)},
				{vertices[i].get(3, 1)},
				{1}
			}
		, 4, 1);
	}
}