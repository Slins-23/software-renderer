#include "Quad.h"

Quad::Quad() {

}

Quad::Quad(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c, const Mat& vertex_d) {
	triangles[0] = Triangle(vertex_a, vertex_b, vertex_c);
	triangles[1] = Triangle(vertex_a, vertex_c, vertex_d);
}

Quad::Quad(const Mat vertices[4]) {
	triangles[0] = Triangle(vertices[0], vertices[1], vertices[2]);
	triangles[1] = Triangle(vertices[0], vertices[2], vertices[3]);
}

Quad::Quad(const Triangle& triangle_a, const Triangle& triangle_b) {
	this->triangles[0] = triangle_a;
	this->triangles[1] = triangle_b;
}

Quad::Quad(const Triangle triangles[2]) {
	this->triangles[0] = triangles[0];
	this->triangles[1] = triangles[1];
}