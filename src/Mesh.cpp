#include "Mesh.h"

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

uint32_t Mesh::total_vertices() const {
	return vertices.size();
}

uint32_t Mesh::total_faces() const {
	return faces_indices.size();
}