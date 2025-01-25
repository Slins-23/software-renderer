#include "Matrix.h"
#include <vector>
#include <fstream>
#include <strstream>
#pragma once

struct Mesh {
	//Mat origin = Mat({ {(x_min + x_max) / 2}, {(y_min + y_max) / 2}, {(z_min + z_max) / 2} }, 3, 1);
	std::vector<Mat> vertices; // 3-dimensional (3x1) vertex coordinates
	std::vector<std::vector<uint32_t>> faces_indices; // List of faces (each element/face contains 4 indices)
	std::vector<Mat> tex_coords; // 2-dimensional (2x1) texture coordinates
	std::vector<std::vector<uint32_t>> tex_indices;
	std::vector<Mat> normals; // 3-dimensional (3x1) normal directions
	std::vector<std::vector<uint32_t>> normal_indices;
	std::string mesh_filename = "";
	uint32_t mesh_id = 0;

	/*
	double origin_x = origin.get(1, 1);
	double origin_y = origin.get(2, 1);
	double origin_z = origin.get(3, 1);
	*/

	uint32_t total_vertices() const;
	uint32_t total_faces() const;

	// Defaults to a cube
	/*
	Mesh(uint32_t& total_meshes) {
		// "Cube" dimensions
		double width = 0.25;
		double height = 0.25;
		double depth = 0.25;
		// Depth range is [0, 1] while other axes are [-1, 1], so twice as big to make it a cube

		double origin_x = 0;
		double origin_y = 0;
		double origin_z = 0;

		/// Front
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 1, 2, 3, 4 }));

		/// Back
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 5, 6, 7, 8 }));

		/// Left
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 9, 10, 11, 12 }));

		/// Right
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 13, 14, 15, 16 }));

		/// Top
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 17, 18, 19, 20 }));

		/// Bottom
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
		faces_indices.push_back(std::vector<uint32_t>({ 21, 22, 23, 24 }));

		this->mesh_filename = "cube.obj";
		this->mesh_id = total_meshes;
		total_meshes++;
	}
	*/

	// Loads from a .OBJ file
	Mesh(const char* model_path, const char* mesh_filename, uint32_t& total_meshes);
};
