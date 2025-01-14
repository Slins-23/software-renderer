#pragma once
#include <iostream>
#include <vector>
#include <cassert>
#include <array>
#include <fstream>
#include <strstream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <deque>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Utils.h"
#include "Matrix.h"
#include "json.hpp"

struct Quaternion {
	double x = 0;
	double y = 0;
	double z = 0;
	double w = 1;

	Quaternion() {}

	Quaternion(double x, double y, double z, double w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	static Quaternion AngleAxis(double x, double y, double z, double angle) {
		double q_x = sin(angle / 2) * x;
		double q_y = sin(angle / 2) * y;
		double q_z = sin(angle / 2) * z;
		double q_w = cos(angle / 2);

		return Quaternion(q_x, q_y, q_z, q_w);
	}

	// Defined in default coordinate system (i.e. positive x-axis to the right, positive y-axis up, positive z-axis outward/out of the screen)
	static Quaternion FromYawPitchRoll(double yaw, double pitch, double roll) {
		Mat y_axis = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
		Mat x_axis = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
		Mat z_axis = Mat({ {0}, {0}, {1}, {0} }, 4, 1);
		
		
		Quaternion rotationY = Quaternion::AngleAxis(y_axis.get(1, 1), y_axis.get(2, 1), y_axis.get(3, 1), yaw);

		// Rotated x and z axes
		x_axis = Quaternion::RotatePoint(x_axis, y_axis, yaw);
		z_axis = Quaternion::RotatePoint(z_axis, y_axis, yaw);

		Quaternion rotationX = Quaternion::AngleAxis(x_axis.get(1, 1), x_axis.get(2, 1), x_axis.get(3, 1), pitch);

		// Rotated y and z axes
		y_axis = Quaternion::RotatePoint(y_axis, x_axis, pitch);
		z_axis = Quaternion::RotatePoint(z_axis, x_axis, pitch);

		Quaternion rotationZ = Quaternion::AngleAxis(z_axis.get(1, 1), z_axis.get(2, 1), z_axis.get(3, 1), roll);

		Quaternion orientation = rotationZ * rotationX * rotationY;

		return orientation;
	}

	static Mat RotatePoint(const Mat& point, const Mat& axis, double angle) {
		Quaternion q = Quaternion::AngleAxis(axis.get(1, 1), axis.get(2, 1), axis.get(3, 1), angle);
		Quaternion q_point = Quaternion(point.get(1, 1), point.get(2, 1), point.get(3, 1), 0);
		Quaternion q_conjugate = q.get_complexconjugate();

		Quaternion rotated_point = q * q_point * q_conjugate;
		Mat final_point = rotated_point.get_4dvector();
		final_point.set(1, 4, 1);

		return final_point;
	}

	static Mat RotatePoint(const Mat& point, double axis_x, double axis_y, double axis_z) {

	}

	static Mat RotatePoint(double point_x, double point_y, double point_z, double axis_x, double axis_y, double axis_z, double angle) {
		const Mat point = Mat({ {point_x}, {point_y}, {point_z}, {1} }, 4, 1);
		const Mat axis = Mat({ {axis_x}, {axis_y}, {axis_z}, {1} }, 4, 1);

		return Quaternion::RotatePoint(point, axis, angle);
	}

	static Mat RotatePoint(double point_x, double point_y, double point_z, const Mat& axis) {

	}

	Mat get_3dvector() const {
		Mat vector_form = Mat(
			{
				{x},
				{y},
				{z},
				{0}
			}
		, 4, 1);

		return vector_form;
	}

	Mat get_4dvector() const {
		Mat vector_form = Mat(
			{
				{x},
				{y},
				{z},
				{w}
			}
		, 4, 1);

		return std::move(vector_form);
	}

	Mat get_rotationmatrix() const {
		Mat rotation_matrix = Mat(
			{
			   {1 - 2 * (this->y * this->y + this->z * this->z), 2 * (this->x * this->y - this->w * this->z), 2 * (this->x * this->z + this->w * this->y), 0},
				{2 * (this->x * this->y + this->w * this->z), 1 - 2 * (this->x * this->x + this->z * this->z), 2 * (this->y * this->z - this->w * this->x), 0},
				{2 * (this->x * this->z - this->w * this->y), 2 * (this->y * this->z + this->w * this->x), 1 - 2 * (this->x * this->x + this->y * this->y), 0},
				{0, 0, 0, 1}
			}
		, 4, 4);

		return rotation_matrix;
	}

	Quaternion get_complexconjugate() const {
		Quaternion result = Quaternion(-this->x, -this->y, -this->z, this->w);

		return result;
	}

	double get_angle() const {
		double angle = 2 * acos(this->w);
		return Utils::round_to(angle, 2);
	}

	double get_magnitude() const {
		return sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	double get_yaw() const {
		return 0;
	}

	double get_pitch() const {
		return 0;
	}

	double get_roll() const {
		return 0;
	}

	// Make quaternion a unit quaternion
	void normalize() {
		double length = this->get_magnitude();

		this->x /= length;
		this->y /= length;
		this->z /= length;
		this->w /= length;
	}

	Quaternion operator+(const Quaternion& other) {
		return Quaternion(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
	}

	Quaternion operator-(const Quaternion& other) {
		return Quaternion(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
	}

	Quaternion operator*(const Quaternion& other) {
		Quaternion result = Quaternion();
		result.x = (this->x * other.w) + (this->y * other.z) - (this->z * other.y) + (this->w * other.x);
		result.y = (this->y * other.w) + (this->z * other.x) + (this->w * other.y) - (this->x * other.z);
		result.z = (this->z * other.w) + (this->w * other.z) + (this->x * other.y) - (this->y * other.x);
		result.w = (this->w * other.w) - (this->x * other.x) - (this->y * other.y) - (this->z * other.z);

		return result;
	}
};

// Represented as 3 vertices
struct Triangle {
	Mat vertices[3] = {
		Mat({{1}, {0}, {0}, {1}}, 4, 1),
		Mat({{0}, {1}, {0}, {1}}, 4, 1),
		Mat({{0}, {0}, {1}, {1}}, 4, 1)
	};

	Triangle() {};

	Triangle(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c) {
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

	Triangle(const Mat vertices[3]) {
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
};

// Represented as 2 triangles (as is, wastes 2 vertices (duplicate) and draws 1 extra line (also duplicate))
struct Quad {
	Triangle triangles[2];

	Quad() {};

	Quad(const Mat& vertex_a, const Mat& vertex_b, const Mat& vertex_c, const Mat& vertex_d) {
		triangles[0] = Triangle(vertex_a, vertex_b, vertex_c);
		triangles[1] = Triangle(vertex_a, vertex_c, vertex_d);
	}

	Quad(const Mat vertices[4]) {
		triangles[0] = Triangle(vertices[0], vertices[1], vertices[2]);
		triangles[1] = Triangle(vertices[0], vertices[2], vertices[3]);
	}

	Quad(const Triangle& triangle_a, const Triangle& triangle_b) {
		this->triangles[0] = triangle_a;
		this->triangles[1] = triangle_b;
	}

	Quad(const Triangle triangles[2]) {
		this->triangles[0] = triangles[0];
		this->triangles[1] = triangles[1];
	}
};

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

	uint32_t total_vertices() const {
		return vertices.size();
	}

	uint32_t total_faces() const {
		return faces_indices.size();
	}

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
	Mesh(const char* model_path, const char* mesh_filename, uint32_t& total_meshes) {
		this->mesh_filename = std::string(mesh_filename);

		std::ifstream file(model_path);

		if (file.is_open()) {
			while (!file.eof()) {
				char line[255];
				file.getline(line, 255);

				std::strstream txt_stream;
				txt_stream << line;

				char sink;
				// Vertex coordinate
				if (line[0] == 'v') {

					// Just a vertex coordinate
					if (line[1] == ' ') {
						Mat vertex = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
						double vx, vy, vz = 0;
						txt_stream >> sink >> vx >> vy >> vz;

						vertex.set(vx, 1, 1);

						// The coordinate system I use is a right-handed coordinate system where the positive y axis points down,
						// and the positive z axis points into the screen (forward)
						// They get flipped here because most models I found were on left-handed coordinate systems
						// (positive y up, positive z outwards)
						
						// Y axis is flipped here because most models I found were on left-handed coordinate systems
						//vertex.set(-vy, 2, 1);
						vertex.set(vy, 2, 1);

						// Same as above, but for the Z axis
						//vertex.set(vz, 3, 1);
						vertex.set(-vz, 3, 1);

						vertices.push_back(vertex);
					}

					// Should also flip texture coordinates and normals to match y axis!!?!
					// Texture coordinate
					else if (line[1] == 't') {
						Mat tex_coord = Mat({ {0}, {0} }, 2, 1);
						double x, y = 0;
						txt_stream >> sink >> sink >> x >> y;

						tex_coord.set(x, 1, 1);
						tex_coord.set(y, 2, 1);

						tex_coords.push_back(tex_coord);
					}

					// Should also flip texture coordinates and normals to match y axis!!?!
					// Normal direction
					else if (line[1] == 'n') {
						Mat normal = Mat({ {0}, {0}, {0} }, 3, 1);
						double x, y, z = 0;

						txt_stream >> sink >> sink >> x >> y >> z;

						normal.set(x, 1, 1);
						normal.set(y, 2, 1);
						//normal.set(z, 3, 1);
						normal.set(-z, 3, 1);

						normals.push_back(normal);
					}
				}

				// Face indices
				else if (line[0] == 'f') {
					std::vector<uint32_t> vertices_indices;
					std::vector<uint32_t> textures_indices;
					std::vector<uint32_t> normals_indices;

					std::vector<char*> split_whitespace;
					char* walk;
					char* next_token = NULL;
					walk = strtok_s(line, " ", &next_token);
					while (walk != NULL) {
						char* cur_str = walk;
						split_whitespace.push_back(cur_str);
						walk = strtok_s(NULL, " ", &next_token);
					}

					std::string tmp_a = split_whitespace[1];
					std::string cmp = "/";
					bool has_textures = tmp_a.find(cmp) == std::string::npos;
					bool has_normals = false;

					for (int i = 1; i < split_whitespace.size(); i++) {
						std::vector<uint32_t> split_slash;
						next_token = NULL;
						walk = strtok_s(split_whitespace[i], "/", &next_token);
						while (walk != NULL) {
							char* cur_str = walk;
							char* end;
							uint32_t num = std::strtol(cur_str, &end, 10);

							if (end == cur_str) {
								throw std::runtime_error("Error: Could not convert from string to number.");
							}

							split_slash.push_back(num);
							walk = strtok_s(NULL, "/", &next_token);
						}

						uint32_t vertex_index = split_slash[0];
						vertices_indices.push_back(vertex_index);

						// Indices and textures?
						if (split_slash.size() == 2) {
							if (has_textures) {
								has_textures = true;
								uint32_t texture_index = split_slash[1];
								textures_indices.push_back(texture_index);
							}
							else {
								has_normals = true;
								uint32_t normal_index = split_slash[1];
								normals_indices.push_back(normal_index);
							}
						}

						// Indices, textures, and normals?
						else if (split_slash.size() == 3) {
							has_normals = true;
							uint32_t texture_index = split_slash[1];
							uint32_t normal_index = split_slash[2];
							textures_indices.push_back(texture_index);
							normals_indices.push_back(normal_index);
						}
					}

					faces_indices.push_back(vertices_indices);

					if (has_textures) {
						tex_indices.push_back(textures_indices);
					}

					if (has_normals) {
						normal_indices.push_back(normals_indices);
					}
				}
			}

			this->mesh_id = total_meshes;
			total_meshes++;
		}
		else {
			throw std::runtime_error("Error: Could not load model.");
		}
	}
};

struct Instance {
private:
	void create_instance_nameid(uint32_t& total_instances) {
		std::string final_string = std::string(mesh->mesh_filename);
		std::string new_id = "_";

		this->instance_id = total_instances;

		if (this->instance_id < 10) {
			new_id += "00";
		}
		else if (this->instance_id < 100) {
			new_id += "0";
		}

		new_id += std::to_string(instance_id);

		final_string.replace((final_string.length() - 4), (final_string.length() - 1), new_id);

		this->instance_name = final_string;

		total_instances++;
	}
public:
	void aQuaternion_GetAnglesFromQuaternionYP(const Quaternion& quaternion, double& yaw, double& pitch, double& roll) {
		Mat rotation_matrix = quaternion.get_rotationmatrix();
		pitch = asin(-Utils::clamp(rotation_matrix.get(2, 3), -1, 1));

		if (abs(rotation_matrix.get(2, 3) < 0.9999999)) {
			yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
			roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
		}
		else {
			yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
			roll = 0;
		}
	}

	Mat aquaternion_rotationX_matrix(double radians) {
		Quaternion quaternion = Quaternion::AngleAxis(1, 0, 0, radians);
		return quaternion.get_rotationmatrix();
	}

	Mat aquaternion_rotationY_matrix(double radians) {
		Quaternion quaternion = Quaternion::AngleAxis(0, 1, 0, radians);
		return quaternion.get_rotationmatrix();
	}

	Mat aquaternion_rotationZ_matrix(double radians) {
		Quaternion quaternion = Quaternion::AngleAxis(0, 0, 1, radians);
		return quaternion.get_rotationmatrix();
	}

	Mat ascale_matrix(double sx, double sy, double sz) {
		Mat scale_matrix = Mat(
			{
			{sx, 0, 0, 0},
			{0, sy, 0, 0},
			{0, 0, sz, 0},
			{0, 0, 0, 1}
			}
		, 4, 4);

		return scale_matrix;
	}

	Mat atranslation_matrix(double tx, double ty, double tz) {
		Mat translation_matrix = Mat(
			{
				{1, 0, 0, tx},
				{0, 1, 0, ty},
				{0, 0, 1, tz},
				{0, 0, 0, 1}
			}
		, 4, 4);

		return translation_matrix;
	}

	std::string instance_name = "";
	uint32_t instance_id = 0;
	Mesh* mesh = nullptr;

	double tx = 0;
	double ty = 0;
	double tz = 0;

	Quaternion orientation = Quaternion(0, 0, 0, 1);

	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	double sx = 1;
	double sy = 1;
	double sz = 1;

	Mat SCALING_MATRIX = Mat::identity_matrix(4);
	Mat ROTATION_MATRIX = Mat::identity_matrix(4);
	Mat TRANSLATION_MATRIX = Mat::identity_matrix(4);

	Mat MODEL_TO_WORLD = Mat::identity_matrix(4);

	bool show = true;

	Instance() {};

	Instance(Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;

		this->orientation = orientation;
		aQuaternion_GetAnglesFromQuaternionYP(orientation, this->yaw, this->pitch, this->roll);
		this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();

		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;

		this->orientation = orientation;
		aQuaternion_GetAnglesFromQuaternionYP(orientation, this->yaw, this->pitch, this->roll);
		this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();

		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;

		Quaternion orientation = Quaternion::FromYawPitchRoll(this->yaw, this->pitch, this->roll);
		this->orientation = orientation;

		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;



		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
		this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
		this->ROTATION_MATRIX = ROTATION_MATRIX;
		this->SCALING_MATRIX = SCALING_MATRIX;
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = TRANSLATION_MATRIX.get(1, 4);
		this->ty = TRANSLATION_MATRIX.get(2, 4);
		this->tz = TRANSLATION_MATRIX.get(3, 4);

		this->orientation = orientation;

		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll,

		this->sx = SCALING_MATRIX.get(1, 1);
		this->sy = SCALING_MATRIX.get(2, 2);
		this->sz = SCALING_MATRIX.get(3, 3);

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, bool show, uint32_t& total_instances) {
		this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
		this->ROTATION_MATRIX = ROTATION_MATRIX;
		this->SCALING_MATRIX = SCALING_MATRIX;
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = TRANSLATION_MATRIX.get(1, 4);
		this->ty = TRANSLATION_MATRIX.get(2, 4);
		this->tz = TRANSLATION_MATRIX.get(3, 4);

		this->orientation = orientation;

		this->sx = SCALING_MATRIX.get(1, 1);
		this->sy = SCALING_MATRIX.get(2, 2);
		this->sz = SCALING_MATRIX.get(3, 3);

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
		this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
		this->ROTATION_MATRIX = ROTATION_MATRIX;
		this->SCALING_MATRIX = SCALING_MATRIX;
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

		this->tx = TRANSLATION_MATRIX.get(1, 4);
		this->ty = TRANSLATION_MATRIX.get(2, 4);
		this->tz = TRANSLATION_MATRIX.get(3, 4);

		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;

		this->sx = SCALING_MATRIX.get(1, 1);
		this->sy = SCALING_MATRIX.get(2, 2);
		this->sz = SCALING_MATRIX.get(3, 3);

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances) {
		this->MODEL_TO_WORLD = MODEL_TO_WORLD;
		this->mesh = mesh;
		this->show = show;

		this->mesh = mesh;
		this->show = show;

		this->create_instance_nameid(total_instances);
	}

	Instance(std::string instance_name, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
		this->orientation = orientation;
		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;

		this->tx = TRANSLATION_MATRIX.get(1, 4);
		this->ty = TRANSLATION_MATRIX.get(2, 4);
		this->tz = TRANSLATION_MATRIX.get(3, 4);

		this->sx = SCALING_MATRIX.get(1, 1);
		this->sy = SCALING_MATRIX.get(2, 2);
		this->sz = SCALING_MATRIX.get(3, 3);

		this->instance_name = instance_name;
		this->mesh = mesh;
		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->ROTATION_MATRIX = orientation.get_rotationmatrix();
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
		this->show = show;
		this->instance_id = total_instances;



		//aQuaternion_GetAnglesFromQuaternionYP(orientation, this->yaw, this->pitch, this->roll);



		total_instances++;
	}

	Instance(std::string instance_name, Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
		this->orientation = orientation;
		this->instance_name = instance_name;
		this->mesh = mesh;
		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->ROTATION_MATRIX = orientation.get_rotationmatrix();
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
		this->show = show;
		this->instance_id = total_instances;

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;

		aQuaternion_GetAnglesFromQuaternionYP(orientation, this->yaw, this->pitch, this->roll);

		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		total_instances++;
	}

	Instance(std::string instance_name, Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
		Quaternion orientation = Quaternion::FromYawPitchRoll(yaw, pitch, roll);
		this->orientation = orientation;

		this->instance_name = instance_name;
		this->mesh = mesh;
		this->TRANSLATION_MATRIX = atranslation_matrix(tx, ty, tz);
		//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
		this->ROTATION_MATRIX = orientation.get_rotationmatrix();
		this->SCALING_MATRIX = ascale_matrix(sx, sy, sz);
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
		this->show = show;
		this->instance_id = total_instances;

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;

		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;

		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		total_instances++;
	}

	Instance(std::string instance_name, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, bool show, uint32_t& total_instances) {
		this->instance_name = instance_name;
		this->mesh = mesh;
		this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
		this->ROTATION_MATRIX = ROTATION_MATRIX;
		this->SCALING_MATRIX = SCALING_MATRIX;
		this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
		this->show = show;
		this->instance_id = total_instances;

		double tx = TRANSLATION_MATRIX.get(1, 4);
		double ty = TRANSLATION_MATRIX.get(2, 4);
		double tz = TRANSLATION_MATRIX.get(3, 4);

		this->tx = tx;
		this->ty = ty;
		this->tz = tz;

		double yaw = 0;
		double pitch = 0;
		double roll = 0;

		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;

		Quaternion orientation = Quaternion::FromYawPitchRoll(this->yaw, this->pitch, this->roll);
		this->orientation = orientation;

		double sx = SCALING_MATRIX.get(1, 1);
		double sy = SCALING_MATRIX.get(2, 2);
		double sz = SCALING_MATRIX.get(3, 3);

		this->sx = sx;
		this->sy = sy;
		this->sz = sz;

		total_instances++;
	}

	Instance(std::string instance_name, Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances) {
		this->instance_name = instance_name;
		this->mesh = mesh;
		this->MODEL_TO_WORLD = MODEL_TO_WORLD;
		this->show = show;
		this->instance_id = total_instances;

		total_instances++;
	}
};

struct Scene {
	uint32_t total_meshes = 0;
	uint32_t total_instances = 0;
	uint32_t total_triangles = 0;
	uint32_t total_vertices = 0;

	uint32_t rendered_meshes = 0;
	uint32_t rendered_instances = 0;
	uint32_t rendered_triangles = 0;
	uint32_t rendered_vertices = 0;

	std::deque<Mesh> scene_meshes;
	std::deque<Instance> scene_instances;

	std::string scene_filepath = "None";
	nlohmann::json scene_data;

	Scene() {

	}

	Scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_position, Mat& default_camera_direction, Mat& camera_direction, Mat& default_camera_up, Mat& camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, Mat& VIEW_MATRIX, bool use_scene_camera_settings) {
		this->load_scene(scenes_folder, scene_filename, models_folder, verbose, default_camera_position, camera_position, default_camera_direction, camera_direction, default_camera_up, camera_up, camera_yaw, camera_pitch, camera_roll, VIEW_MATRIX, use_scene_camera_settings);
	}
	
	/// <summary>
	///  Save scene to a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	void save_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, const Mat& default_camera_position, const Mat& camera_position, const Mat& default_camera_direction, const Mat& camera_direction, const Mat& default_camera_up, const Mat& camera_up, double yaw, double pitch, double roll);

	/// <summary>
	///  Load scene from a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	void load_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_position, Mat& default_camera_direction, Mat& camera_direction, Mat& default_camera_up, Mat& camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, Mat& VIEW_MATRIX, bool use_scene_camera_settings);

	Mesh get_mesh(uint32_t mesh_id);
	Mesh get_mesh(std::string mesh_filename);
	Mesh* get_mesh_ptr(uint32_t mesh_id);
	Mesh* get_mesh_ptr(std::string mesh_filename);

	Instance get_instance(uint32_t instance_id);
	Instance get_instance(std::string instance_name);
	Instance* get_instance_ptr(uint32_t instance_id);
	Instance* get_instance_ptr(std::string instance_name);
};

class Engine {
private:
	uint32_t* pixel_buffer = nullptr;
	double* depth_buffer = nullptr;

	SDL_Event event;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;

	void update_projection_matrix() {
		this->PROJECTION_MATRIX = Mat(
			{
				{(1 / (tan(this->FOVr / 2))), 0, 0, 0},
				{0, this->AR * (1 / (tan(this->FOVr / 2))), 0, 0},
				{0, 0, this->far / (this->far - this->near), (this->far * -this->near) / (this->far - this->near)},
				{0, 0, 1, 0}
			}
		, 4, 4);
	}


	void LookAt();
	void LookAt(const Mat& target_vector);
public:
	const char* TITLE = "Renderer";
	uint16_t WIDTH = 800;
	uint16_t HEIGHT = 600;

	const uint8_t FPS = 144;
	const uint8_t MSPERFRAME = 1000 / FPS;

	// SDL window clear/default color
	const uint32_t CLEAR_COLOR = 0xFFFFFFFF;

	// Background color
	const uint32_t BG_COLOR = 0x000000FF;

	// Line/wireframe color
	const uint32_t LINE_COLOR = 0x00FF00FF;
	
	// Rasterization color
	const uint32_t FILL_COLOR = 0xFF0000FF;

	bool playing = true;

	// How often, in milliseconds, should the average FPS over the given time interval be printed to the console
	double fps_update_interval = 500;

	bool zsort_instances = false; // Toggle 5
	bool cull_triangles = true; // Toggle 4
	bool shade_triangles = false; // Toggle 3
	bool rasterize_triangles = true; // Toggle 2
	bool wireframe_triangles = false; // Toggle 1

	bool z_sorted = false;

	double near = 0.1;
	double far = 1000;
	double FOV = 60;
	double FOVr = FOV * (M_PI / 180);
	double AR = (double) this->WIDTH / (double) this->HEIGHT;
	//double AR = 16/9.f;

	// Translation amounts
	double translation_amount = 0.01;
	double tx = translation_amount;
	double ty = translation_amount;
	double tz = translation_amount;

	// Scale factor
	// Scaling all factors equally does not change the apperance of the object
	double scale_factor = 0.05;
	double sx = 1;
	double sy = 1;
	double sz = 1;

	double use_scene_camera_settings = true;

	double camera_velocity = 0.1;
	double camera_xvelocity = camera_velocity;
	double camera_yvelocity = camera_velocity;
	double camera_zvelocity = camera_velocity;

	Mat default_camera_position = Mat({
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	Mat camera_position = Mat(
		{ 
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	Mat light_source_pos = Mat(
		{
			{0},
			{0},
			{0},
			{1},
		}, 4, 1
		);

	Mat light_source_dir = Mat(
		{
			{0},
			{0},
			{1},
			{0},
		}, 4, 1
		);

	double light_reach = 2;

	// Rotation angle
	double rotation_angle_degrees = 10;
	double rotation_angle = rotation_angle_degrees * (M_PI / 180);

	double default_camera_pitch = 0;
	double default_camera_yaw = 0;
	double default_camera_roll = 0;

	double camera_pitch = 0;
	double camera_yaw = 0;
	double camera_roll = 0; // Should not change camera direction

	// Initial camera direction (not standard camera direction, only what it starts at, the default position is (0, 0, -1))
	
	Quaternion q_camera = Quaternion(0, 0, 0, 1);

	Mat default_camera_direction = Mat({
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	// Camera direction vector relative to world space
	Mat camera_direction = Mat(
		{ 
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	Mat default_camera_up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat camera_up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat VIEW_MATRIX = Mat::identity_matrix(4);


	// For transforming from NDC space to Screen space (window coordinates)
	Mat SCALE_MATRIX = Mat(
		{
			{this->WIDTH / 2., 0, 0, this->WIDTH / 2.},
			{0, this->HEIGHT / 2., 0, this->HEIGHT / 2.},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}, 4, 4);

	// Flips Y and Z
	Mat PROJECTION_MATRIX = Mat(
		{
			{(1 / (tan(FOVr / 2))), 0, 0, 0},
			{0, AR * (1 / (tan(FOVr / 2))), 0, 0},
			{0, 0, far / (far - near), (far * -near) / (far - near)},
			{0, 0, 1, 0}
		}
	, 4, 4);

	const char* models_folder = "D:/Programming/Graphics/Prototyping/models/";
	const char* scenes_folder = "D:/Programming/Graphics/Prototyping/scenes/";
	const char* scene_filename = "hallway.json";
	const char* scene_save_name = "tst.json";
	Scene current_scene;

	double ac_yaw = 0;
	double ac_pitch = 0;
	double ac_roll = 0;
	
	bool setup();
	bool handle_events();

	void draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_mesh(const Mesh& mesh, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_triangle(Mat v0, Mat v1, Mat v2, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_line(double x1, double y1, double x2, double y2, uint32_t outline_color);
	Mat Instance_GetCenterVertex(const Instance& instance);
	void draw();
	void render();

	void fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const double& v0_originalz, const double& v1_originalz, const double& v2_originalz, uint32_t fill_color, bool shade);
 
	static void rotateX(Instance& mesh, double radians);
	static void rotateX(Mesh& mesh, double radians);
	static void rotateX(Quad& quad, double radians);
	static void rotateX(Triangle& triangle, double radians);
	static void rotateX(Mat& matrix, double radians);
	
	static void rotateY(Instance& mesh, double radians);
	static void rotateY(Mesh& mesh, double radians);
	static void rotateY(Quad& quad, double radians);
	static void rotateY(Triangle& triangle, double radians);
	static void rotateY(Mat& matrix, double radians);

	static void rotateZ(Instance& mesh, double radians);
	static void rotateZ(Mesh& mesh, double radians);
	static void rotateZ(Quad& quad, double radians);
	static void rotateZ(Triangle& triangle, double radians);
	static void rotateZ(Mat& matrix, double radians);

	static void translate(Instance& mesh, double tx, double ty, double tz);
	static void translate(Mesh& mesh, double tx, double ty, double tz);
	static void translate(Quad& quad, double tx, double ty, double tz);
	static void translate(Triangle& triangle, double tx, double ty, double tz);
	static void translate(Mat& matrix, double tx, double ty, double tz);

	static void scale(Instance& instance, double sx, double sy, double sz);
	static void scale(Mesh& mesh, double sx, double sy, double sz);
	static void scale(Quad& quad, double sx, double sy, double sz);
	static void scale(Triangle& triangle, double sx, double sy, double sz);
	static void scale(Mat& matrix, double sx, double sy, double sz);

	static Mat translation_matrix(double tx, double ty, double tz);
	static Mat scale_matrix(double sx, double sy, double sz);
	static Mat quaternion_rotationX_matrix(double radians);
	static Mat quaternion_rotationY_matrix(double radians);
	static Mat quaternion_rotationZ_matrix(double radians);
	static Mat euler_rotationX_matrix(double radians);
	static Mat euler_rotationY_matrix(double radians);
	static Mat euler_rotationZ_matrix(double radians);

	static bool LinePlaneIntersection(const Mat* plane_point, const Mat* plane_normal, const Mat* line_start, const Mat* line_end, Mat& intersection_point);

	static uint8_t ClipTriangleToPlane(const Mat* plane_point, const Mat* plane_normal, const Triangle* input_triangle, Triangle& clipped_triangle_a, Triangle& clipped_triangle_b);
	static double PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal);

	static double CrossProduct2D(const Mat& v1, const Mat& v2);
	static Mat CrossProduct3D(const Mat& v1, const Mat& v2);

	static void Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch);
	static void Quaternion_GetAnglesFromQuaternion(const Quaternion& quaternion, double& yaw, double& pitch, double& roll);
	static void Quaternion_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll);

	static void Euler_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll);

	static void GetRoll(const Mat& camera_direction, const Mat& camera_up, const double& yaw, const double& pitch, double& roll);


	static Mat LookAt(const Mat& camera_position, const Mat& camera_direction, const Mat& camera_up);
	static Mat LookAt(const Mat& camera_position, Mat& camera_direction, const Mat& target_vector, Mat& camera_up);

	void close();
};