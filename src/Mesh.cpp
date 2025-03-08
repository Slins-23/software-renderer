#include "Mesh.h"

Mesh::Mesh(uint32_t& total_meshes) {
	// "Cube" dimensions
	double width = 0.30;
	double height = 0.30;
	double depth = 0.30;

	// Depth range is [0, 1] while other axes are [-1, 1], so twice as big to make it a cube
	double origin_x = 0;
	double origin_y = 0;
	double origin_z = 0;

	/// Front
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	faces_indices.push_back(std::vector<uint32_t>({ 1, 2, 3, 4 }));

	/// Back
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	faces_indices.push_back(std::vector<uint32_t>({6, 5, 8, 7}));

	/// Left
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	faces_indices.push_back(std::vector<uint32_t>({ 9, 10, 11, 12 }));

	/// Right
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));

	faces_indices.push_back(std::vector<uint32_t>({ 13, 14, 15, 16 }));

	/// Top
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y + (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	faces_indices.push_back(std::vector<uint32_t>({ 17, 18, 19, 20 }));

	/// Bottom
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z + (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x - (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	vertices.push_back(Mat({ {origin_x + (width / 2)}, {origin_y - (height / 2)}, {origin_z - (depth / 2)}, {1} }, 4, 1));
	faces_indices.push_back(std::vector<uint32_t>({ 24, 23, 22, 21 }));

	this->mesh_filename = "cube.obj";
	this->mesh_id = total_meshes;
	total_meshes++;
}

Mesh::Mesh(const char* model_path, const char* mesh_filename, uint32_t& total_meshes) {
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
					vertex.set(vy, 2, 1);

					//vertex.set(vz, 3, 1);
					// Z gets flipped here because the coordinate system is right-handed but z is treated as positive
					vertex.set(-vz, 3, 1);
					
					vertices.push_back(vertex);
				}

				// Texture coordinate
				else if (line[1] == 't') {
					Mat tex_coord = Mat({ {0}, {0} }, 2, 1);
					double x, y = 0;
					txt_stream >> sink >> sink >> x >> y;

					tex_coord.set(x, 1, 1);
					tex_coord.set(y, 2, 1);

					tex_coords.push_back(tex_coord);
				}

				// Normal direction
				else if (line[1] == 'n') {
					Mat normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);
					double x, y, z = 0;

					txt_stream >> sink >> sink >> x >> y >> z;

					normal.set(x, 1, 1);
					normal.set(y, 2, 1);

					
					//normal.set(z, 3, 1);
					// Z gets flipped here because the coordinate system is right-handed but z is treated as positive
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
							this->has_normals = true;
							uint32_t normal_index = split_slash[1];
							normals_indices.push_back(normal_index);
						}
					}

					// Indices, textures, and normals?
					else if (split_slash.size() == 3) {
						this->has_normals = true;
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

				if (this->has_normals) {
					normal_indices.push_back(normals_indices);
				}
			}
		}

		if (!has_normals) {
			has_normals = true;

			normals = std::vector<Mat>();
			normal_indices = std::vector<std::vector<uint32_t>>();
			normal_indices = faces_indices;

			for (uint32_t normal = 0; normal < vertices.size(); normal++) {
				normals.push_back(Mat({ {0}, {0}, {0}, {0} }, 4, 1));
			}

			for (uint32_t face = 0; face < faces_indices.size(); face++) {
				std::vector<uint32_t> indices = faces_indices[face];

				if (indices.size() == 3) {
					Mat& v0 = vertices[faces_indices[face][0] - 1];
					Mat& v1 = vertices[faces_indices[face][1] - 1];
					Mat& v2 = vertices[faces_indices[face][2] - 1];

					Mat face_normal = Mat::CrossProduct3D(v0 - v1, v2 - v1);

					Mat& v0_normal = normals[normal_indices[face][0] - 1];
					Mat& v1_normal = normals[normal_indices[face][1] - 1];
					Mat& v2_normal = normals[normal_indices[face][2] - 1];

					v0_normal += face_normal;
					v1_normal += face_normal;
					v2_normal += face_normal;
				}
				else if (indices.size() == 4) {
					Mat& v0 = vertices[faces_indices[face][0] - 1];
					Mat& v1 = vertices[faces_indices[face][1] - 1];
					Mat& v2 = vertices[faces_indices[face][2] - 1];
					Mat& v3 = vertices[faces_indices[face][3] - 1];

					Mat t0_face_normal = Mat::CrossProduct3D(v0 - v1, v2 - v1);
					Mat t1_face_normal = Mat::CrossProduct3D(v0 - v2, v3 - v2);

					Mat& v0_normal = normals[normal_indices[face][0] - 1];
					Mat& v1_normal = normals[normal_indices[face][1] - 1];
					Mat& v2_normal = normals[normal_indices[face][2] - 1];
					Mat& v3_normal = normals[normal_indices[face][3] - 1];

					v0_normal += t0_face_normal;
					v1_normal += t0_face_normal;
					v2_normal += t0_face_normal;

					v0_normal += t1_face_normal;
					v2_normal += t1_face_normal;
					v3_normal += t1_face_normal;
				}
			}

			for (uint32_t normal_idx = 0; normal_idx < normals.size(); normal_idx++) {
				Mat& normal = normals[normal_idx];
				normal.normalize();
			}
		}

		this->mesh_id = total_meshes;
		total_meshes++;
	}
	else {
		printf("Error: Could not open mesh file to load model '%s' at path '%s'.", mesh_filename, model_path);
		throw std::runtime_error("Error: Could not load model.");
	}

	/*
	if (strcmp(mesh_filename, "wolf.obj") == 0) {
		std::cout << this->mesh_filename << std::endl;
		std::cout << "Vertices: " << vertices.size() << std::endl;
		std::cout << "Normals: " << normals.size() << std::endl;
		std::cout << "Faces indices: " << faces_indices.size() << std::endl;
		std::cout << "Normal faces indices: " << normal_indices.size() << std::endl;
		exit(-1);
	}
	*/

}

uint32_t Mesh::total_vertices() const {
	return vertices.size();
}

uint32_t Mesh::total_normals() const {
	return normals.size();
}

uint32_t Mesh::total_faces() const {
	return faces_indices.size();
}