#pragma once
#include "Matrix.h"
#include <vector>
#include <fstream>
#include <strstream>

struct Mesh {
	//Mat origin = Mat({ {(x_min + x_max) / 2}, {(y_min + y_max) / 2}, {(z_min + z_max) / 2} }, 3, 1);
	std::vector<Mat> vertices; // 4-dimensional (4x1) vertex coordinates
	std::vector<std::vector<uint32_t>> faces_indices; // List of faces (each element/face contains 4 indices)
	std::vector<Mat> tex_coords; // 2-dimensional (2x1) texture coordinates
	std::vector<std::vector<uint32_t>> tex_indices;
	std::vector<Mat> normals; // 4-dimensional (4x1) normal directions
	std::vector<std::vector<uint32_t>> normal_indices;
	std::string mesh_filename = "";
	uint32_t mesh_id = 0;

	bool has_normals = false;

	/*
	double origin_x = origin.get(1, 1);
	double origin_y = origin.get(2, 1);
	double origin_z = origin.get(3, 1);
	*/

	uint32_t total_vertices() const;
	uint32_t total_faces() const;
	uint32_t total_normals() const;

	// Defines a cube mesh
	Mesh();

	// Initializes a default mesh
	Mesh(uint32_t& total_ever_meshes) { total_ever_meshes++; this->mesh_id = total_ever_meshes; };

	// Loads from a .OBJ file
	Mesh(const char* model_path, const char* mesh_filename, uint32_t& total_ever_meshes);
};
