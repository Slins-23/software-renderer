#include "Engine.h"
#include "Windows/Tabs/CameraTab.h"
#include "Windows/Tabs/InstancesTab.h"
#include "Windows/Tabs/LightTab.h"

#include <cassert>
bool Engine::setup() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL video. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 0;
	}

	this->window = SDL_CreateWindow(this->TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->WIDTH, this->HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (this->window == nullptr) {
		std::cout << "Could not create window. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 0;
	}

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
	if (this->renderer == nullptr) {
		std::cout << "Could not create renderer. Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(this->window);
		SDL_Quit();
		return 0;
	}

	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->WIDTH, this->HEIGHT);
	if (this->texture == nullptr) {
		std::cout << "Could not create texture. Error: " << SDL_GetError() << std::endl;
		SDL_DestroyRenderer(this->renderer);
		SDL_DestroyWindow(this->window);
		SDL_Quit();
		return 0;
	}

	window_manager.initialize(this->window, this->renderer);

	this->pixel_buffer = (uint32_t*) malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);
	this->depth_buffer = (double*)malloc(sizeof(double) * this->WIDTH * this->HEIGHT);

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		this->pixel_buffer[i] = window_manager.general_window.settings_tab.BG_COLOR;
		this->depth_buffer[i] = std::numeric_limits<double>::max();
	}



	// Enables relative mouse tracking (used for rotating the camera along with the mouse)
	SDL_SetRelativeMouseMode(SDL_TRUE);

	return 1;
}



void Engine::draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	draw_mesh(*instance.mesh, instance.MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade, instance.is_light_source, instance.is_axes);
}

void Engine::draw_mesh(const Mesh& mesh, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes) {
	uint32_t total_vertices_count = mesh.total_vertices();
	uint32_t total_normals_count = 0;

	if (mesh.has_normals) {
		total_normals_count = mesh.total_normals() ;
	}

	for (uint32_t face = 0; face < mesh.total_faces(); face++) {
		uint32_t idx_a = mesh.faces_indices[face][0] - 1;
		uint32_t idx_b = mesh.faces_indices[face][1] - 1;
		uint32_t idx_c = mesh.faces_indices[face][2] - 1;

		uint32_t normal_idx_a = 0;
		uint32_t normal_idx_b = 0;
		uint32_t normal_idx_c = 0;
		uint32_t normal_idx_d = 0;

		if (mesh.has_normals) {
			normal_idx_a = mesh.normal_indices[face][0] - 1;
			normal_idx_b = mesh.normal_indices[face][1] - 1;
			normal_idx_c = mesh.normal_indices[face][2] - 1;
		}

		// Skip if not a valid vertex
		if ((idx_a >= total_vertices_count || idx_b >= total_vertices_count || idx_c >= total_vertices_count)
			||
			(idx_a < 0 || idx_b < 0 || idx_c < 0)
			) {
			continue;
		}

		if (mesh.has_normals) {
			// Skip if not a valid normal
			if ((normal_idx_a >= total_normals_count || normal_idx_b >= total_normals_count || normal_idx_c >= total_normals_count)
				||
				(normal_idx_a < 0 || normal_idx_b < 0 || normal_idx_c < 0)
				) {
				continue;
			}
		}


		Mat vertices[4] = {mesh.vertices[idx_a], mesh.vertices[idx_b], mesh.vertices[idx_c], mesh.vertices[idx_c]};

		// Include 4th vertex if face is given as a quad
		if (mesh.faces_indices[face].size() == 4) {
			uint32_t idx_d = mesh.faces_indices[face][3] - 1;

			if (idx_d >= total_vertices_count || idx_d < 0) {
				continue;
			}

			vertices[3] = mesh.vertices[idx_d];


		}

		Mat v0_normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);
		Mat v1_normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);
		Mat v2_normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);
		Mat v3_normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);

		if (mesh.has_normals) {
			Mat normals[4] = { mesh.normals[normal_idx_a], mesh.normals[normal_idx_b], mesh.normals[normal_idx_c], mesh.normals[normal_idx_c] };
			
			v0_normal = normals[0];
			v1_normal = normals[1];
			v2_normal = normals[2];

			if (mesh.faces_indices[face].size() == 4) {
				uint32_t normal_idx_d = mesh.normal_indices[face][3] - 1;

				if (normal_idx_d >= total_normals_count || normal_idx_d < 0) {
					continue;
				}

				normals[3] = mesh.normals[normal_idx_d];
				v3_normal = normals[3];
			}
			
		}

		// Draw quad if face is rectangular
		if (mesh.faces_indices[face].size() == 4) {
			draw_quad(vertices[0], vertices[1], vertices[2], vertices[3], v0_normal, v1_normal, v2_normal, v3_normal, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade, is_light_source, is_axes);
		}

		// Draw triangle if face is triangular
		else if (mesh.faces_indices[face].size() == 3) {
			draw_triangle(vertices[0], vertices[1], vertices[2], v0_normal, v1_normal, v2_normal, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade, is_light_source, is_axes);
		}

	}
}


void Engine::draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, Mat v0_normal, Mat v1_normal, Mat v2_normal, Mat v3_normal, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes) {
	draw_triangle(v0, v1, v2, v0_normal, v1_normal, v2_normal, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade, is_light_source, is_axes);
	draw_triangle(v0, v2, v3, v0_normal, v2_normal, v3_normal, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade, is_light_source, is_axes);
}

void Engine::draw_triangle(Mat v0, Mat v1, Mat v2, Mat v0_normal, Mat v1_normal, Mat v2_normal, const Mat& MODEL_TO_WORLD,  bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes) {
	// Vectors here are assumed to have 1 in the 4th dimension (position vectors)
	Mat world_v0 = MODEL_TO_WORLD * v0;
	Mat world_v1 = MODEL_TO_WORLD * v1;
	Mat world_v2 = MODEL_TO_WORLD * v2;

	// Vectors here are assumed to have 0 in the 4th dimension (direction vectors)
	v0_normal = MODEL_TO_WORLD * v0_normal;
	v1_normal = MODEL_TO_WORLD * v1_normal;
	v2_normal = MODEL_TO_WORLD * v2_normal;

	double world_v0z = world_v0.get(3, 1);
	double world_v1z = world_v1.get(3, 1);
	double world_v2z = world_v2.get(3, 1);

	v0 =  window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v0;
	v1 =  window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v1;
	v2 =  window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v2;

	Mat view_v0 = v0;
	Mat view_v1 = v1;
	Mat view_v2 = v2;

	Mat view_v0_normal = v0_normal;
	Mat view_v1_normal = v1_normal;
	Mat view_v2_normal = v2_normal;

	Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, view_v0_normal, false);
	Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, view_v1_normal, false);
	Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, view_v2_normal, false);

	// MODEL SPACE -> WORLD SPACE ->  NOW IN CAMERA SPACE

	// Cull triangle if all vertices are further than the far plane
	if (abs(v0.get(3, 1)) > window_manager.general_window.scene_tab.current_scene.camera.far || abs(v1.get(3, 1)) > window_manager.general_window.scene_tab.current_scene.camera.far || abs(v2.get(3, 1)) > window_manager.general_window.scene_tab.current_scene.camera.far) {
		return;
	}

	// Should include the z flip in the view or projection matrix instead of when loading meshes?

	//v0.set(-v0.get(3, 1), 3, 1);
	//v1.set(-v1.get(3, 1), 3, 1);
	//v2.set(-v2.get(3, 1), 3, 1);

	Triangle current_triangle = Triangle(v0, v1, v2);
	Triangle clipped_triangles[2];

	const Mat near_plane_point = Mat(
		{ 
			{0},
			{0},
			{window_manager.general_window.scene_tab.current_scene.camera.near},
			{0}
		}
	, 4, 1);

	const Mat near_plane_normal = Mat(
		{
			{0},
			{0},
			{1},
			{0}
		}
		, 4, 1
	);

	uint8_t n_clipped_triangles = Engine::ClipTriangleToPlane(&near_plane_point, &near_plane_normal, &current_triangle, clipped_triangles[0], clipped_triangles[1]);

	if (n_clipped_triangles == 0) return;
	
	double new_near = window_manager.general_window.scene_tab.current_scene.camera.near;

	double xright = new_near * tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2);
	double xleft = -xright;
	double ytop = xright / window_manager.general_window.scene_tab.current_scene.camera.AR;
	double ybottom = -ytop;

	Mat left_plane_normal = Mat(
		{
			{window_manager.general_window.scene_tab.current_scene.camera.near / xright},
			{0},
			{1},
			{0}
		}
	, 4, 1);

	left_plane_normal = left_plane_normal / left_plane_normal.norm();


	Mat right_plane_normal = Mat(
		{
			{-window_manager.general_window.scene_tab.current_scene.camera.near / xright},
			{0},
			{1},
			{0}
		}
	, 4, 1);

	right_plane_normal = right_plane_normal / right_plane_normal.norm();

	Mat top_plane_normal = Mat(
		{
			{0},
			{-window_manager.general_window.scene_tab.current_scene.camera.near/ ytop},
			{1},
			{0}
		}
	, 4, 1);

	top_plane_normal = top_plane_normal / top_plane_normal.norm();

	Mat bottom_plane_normal = Mat(
		{
			{0},
			{window_manager.general_window.scene_tab.current_scene.camera.near / ytop},
			{1},
			{0}
		}
	, 4, 1);

	bottom_plane_normal = bottom_plane_normal / bottom_plane_normal.norm();

	const Mat left_plane_point = Mat(
		{
			{xleft},
			{0},
			{window_manager.general_window.scene_tab.current_scene.camera.near},
			{0}
		}
	, 4, 1);

	const Mat right_plane_point = Mat(
		{
			{xright},
			{0},
			{window_manager.general_window.scene_tab.current_scene.camera.near},
			{0}
		}
	, 4, 1);

	const Mat top_plane_point = Mat(
		{
			{0},
			{ytop},
			{window_manager.general_window.scene_tab.current_scene.camera.near},
			{0}
		}
	, 4, 1);

	const Mat bottom_plane_point = Mat(
		{
			{0},
			{ybottom},
			{window_manager.general_window.scene_tab.current_scene.camera.near},
			{0}
		}
	, 4, 1);

	// Triangles get clipped, processed, then rendered
	if (draw_outline || fill) {
		for (uint8_t n_clipped_triangle = 0; n_clipped_triangle < n_clipped_triangles; n_clipped_triangle++) {
			Triangle starting_clipped_triangle = clipped_triangles[n_clipped_triangle];
			Mat v0 = starting_clipped_triangle.vertices[0];
			Mat v1 = starting_clipped_triangle.vertices[1];
			Mat v2 = starting_clipped_triangle.vertices[2];

			starting_clipped_triangle.vertices[0] = v0;
			starting_clipped_triangle.vertices[1] = v1;
			starting_clipped_triangle.vertices[2] = v2;

			std::deque<Triangle> sub_clipped_triangles;
			sub_clipped_triangles.push_back(starting_clipped_triangle);
			Triangle current_sub_clipped_triangles[2];

			for (uint8_t plane = 0; plane < 4; plane++) {
				size_t n_current_plane_triangles = sub_clipped_triangles.size();
				for (int i = 0; i < n_current_plane_triangles; i++) {
					Triangle current_sub_clipped_triangle = sub_clipped_triangles.front();
					sub_clipped_triangles.pop_front();

					uint8_t n_clipped_sub_triangles = 0;

					switch (plane) {
					case 0:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&bottom_plane_point, &bottom_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 1:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&left_plane_point, &left_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 2:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&top_plane_point, &top_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 3:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&right_plane_point, &right_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					}

					for (n_clipped_sub_triangles; n_clipped_sub_triangles > 0; n_clipped_sub_triangles--) {
						sub_clipped_triangles.push_back(current_sub_clipped_triangles[n_clipped_sub_triangles - 1]);
					}
				}
			}

			for (Triangle current_triangle : sub_clipped_triangles) {
				Mat v0 = current_triangle.vertices[0];
				Mat v1 = current_triangle.vertices[1];
				Mat v2 = current_triangle.vertices[2];

				Mat clipped_world_v0 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_INVERSE * v0;
				Mat clipped_world_v1 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_INVERSE * v1;
				Mat clipped_world_v2 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_INVERSE * v2;

				world_v0z = clipped_world_v0.get(3, 1);
				world_v1z = clipped_world_v1.get(3, 1);
				world_v2z = clipped_world_v2.get(3, 1);

				bool visible = true;

				// This projection matrix's `z` coordinate gets multiplied by the `far` plane value
				// So, the range after the projection of the `z` coordinate is (0, far), not (0, 1)
				// This is so that after the perspective divide, which happens in the sequence, it becomes (0, 1), already including it

				v0 = window_manager.general_window.scene_tab.current_scene.camera.PROJECTION_MATRIX * v0;
				v1 = window_manager.general_window.scene_tab.current_scene.camera.PROJECTION_MATRIX * v1;
				v2 = window_manager.general_window.scene_tab.current_scene.camera.PROJECTION_MATRIX * v2;

				// NOW IN CLIP SPACE FOR SOME GRAPHICS LIBRARIES

				double v0_originalz = v0.get(4, 1);
				double v1_originalz = v1.get(4, 1);
				double v2_originalz = v2.get(4, 1);

				double view_v0z = v0_originalz;
				double view_v1z = v1_originalz;
				double view_v2z = v2_originalz;

				/*
				std::cout << "View: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX.print();
				std::cout << "Inversion: " << std::endl;
				(VIEW_INVERSE_ROT * (VIEW_INVERSE_TRANS * window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX)).print();
				*/

				Mat clipped_view_v0 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * clipped_world_v0;
				Mat clipped_view_v1 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * clipped_world_v1;
				Mat clipped_view_v2 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * clipped_world_v2;

				// Cull triangle if all vertices are further than the far plane
				//if (v0_originalz > window_manager.general_window.scene_tab.current_scene.camera.far && v1_originalz > window_manager.general_window.scene_tab.current_scene.camera.far && v2_originalz > window_manager.general_window.scene_tab.current_scene.camera.far) continue;

				/*
				world_v0.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr/2))) * world_v0.get(1, 1)) / abs(world_v0z), 1, 1);
				world_v0.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2))) * window_manager.general_window.scene_tab.current_scene.camera.AR * world_v0.get(2, 1)) / abs(world_v0z), 2, 1);

				world_v1.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2))) * world_v1.get(1, 1)) / world_v1z, 1, 1);
				world_v1.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2))) * window_manager.general_window.scene_tab.current_scene.camera.AR * world_v1.get(2, 1)) / abs(world_v1z), 2, 1);

				world_v2.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2)))* world_v2.get(1, 1)) / world_v2z, 1, 1);
				world_v2.set(((1 / (tan(window_manager.general_window.scene_tab.current_scene.camera.FOVr / 2)))* window_manager.general_window.scene_tab.current_scene.camera.AR* world_v2.get(2, 1)) / abs(world_v2z), 2, 1);
				*/



				// Perspective divide
				v0 /= v0_originalz;
				v1 /= v1_originalz;
				v2 /= v2_originalz;


				bool cull_triangle = false;

				if (window_manager.general_window.settings_tab.backface_cull) {
					Mat vec_a = v0 - v1;
					vec_a.normalize();
					Mat vec_b = v2 - v1;
					vec_b.normalize();
					Mat triangle_normal = Mat::CrossProduct3D(vec_a, vec_b);
					triangle_normal.normalize();

					//Mat middle_point = v1 + (0.5 * (v0 - v1)) + (0.5 * (v2 - v1));

					//Mat eye_to_triangle_dist = middle_point - camera_position;
					Mat eye_to_triangle_dist = v1;
					eye_to_triangle_dist.normalize();

					double correlation = Mat::dot(triangle_normal, eye_to_triangle_dist);
					if (correlation >= 0) {
						//eye_to_triangle_dist.print();
						cull_triangle = true;
						continue;
					}
				}

				Mat clipped_v0_normal = v0_normal;
				Mat clipped_v1_normal = v1_normal;
				Mat clipped_v2_normal = v2_normal;

				Mat clipped_view_v0_normal = clipped_v0_normal;
				Mat clipped_view_v1_normal = clipped_v1_normal;
				Mat clipped_view_v2_normal = clipped_v2_normal;

				Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, clipped_view_v0_normal, false);
				Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, clipped_view_v1_normal, false);
				Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, clipped_view_v2_normal, false);

				Mat clipped_v0_color = Mat({ {0}, {0}, {0}, {255} }, 4, 1);
				Mat clipped_v1_color = Mat({ {0}, {0}, {0}, {255} }, 4, 1);
				Mat clipped_v2_color = Mat({ {0}, {0}, {0}, {255} }, 4, 1);

				// Triangle lighting/shading happens here
				// Using world vertices for entire triangle instead of clipped triangles since flat shading is the same for the entire triangle despite subdivisions, since the surface normal is the same for the entire triangle
				if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Flat) {
					Mat a_vec_a = world_v0 - world_v1;
					a_vec_a.normalize();
					Mat a_vec_b = world_v2 - world_v1;
					a_vec_b.normalize();
					Mat world_normal = Mat::CrossProduct3D(a_vec_a, a_vec_b);
					world_normal.normalize();

					Mat triangle_world_center = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

					triangle_world_center.set((world_v0.get(1, 1) + world_v1.get(1, 1) + world_v2.get(1, 1)) / 3, 1, 1);
					triangle_world_center.set((world_v0.get(2, 1) + world_v1.get(2, 1) + world_v2.get(2, 1)) / 3, 2, 1);
					triangle_world_center.set((world_v0.get(3, 1) + world_v1.get(3, 1) + world_v2.get(3, 1)) / 3, 3, 1);

					Mat normalized_light_source = window_manager.general_window.scene_tab.current_scene.light_source.position - triangle_world_center;
					double distance = normalized_light_source.norm();
					normalized_light_source /= distance;

					double world_similarity = Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, world_normal);
					//double view_similarity = Mat::dot(normalized_light_source, triangle_normal);

					double similarity = -world_similarity;

					// If the triangle faces the light source (and the triangle is not part of the light source model), interpolate the fill color (the default color for all models) with the light source color, proportional to the light intensity for the given triangle
					if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::point) {
						similarity = Mat::dot(normalized_light_source, world_normal);
						//similarity = 1;
					}
					else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::directional) {
						similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, world_normal);
					}
					else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::spotlight) {
						// How similar the light ray and the light direction are (represents whether the triangle is within the field of view of the light)
						double direction_similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, world_normal);

						// How similar the light ray and the triangle normal vectors are (represents the light intensity)
						double position_similarity = Mat::dot(normalized_light_source, world_normal);

						// If either the light direction does not positively correlate with the triangle's negative normal = direction_similarity (a backface relative to the light)
						// Or the triangle is a backface relative to the light source's position = position_similarity
						// Set the similarity to 0 / unlit
						if (Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, -1 * normalized_light_source) < 0) {
							similarity = 0;
						}
						else if (direction_similarity && position_similarity > 0) {
							similarity = direction_similarity * position_similarity;
						}
						// Combine both the direction and position similarities
						else {
							similarity = 0;
						}

						//similarity = Mat::dot(normalized_light_source, camera_position - world_v0);
					}

					if (similarity > 0) {
						double attenuation = 0;

						if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type != LightType::directional && distance != 0) attenuation = 1.0 / (distance * distance);
						else attenuation = 1.0;

						//double distance_multiplier = light_intensity * attenuation * base_intensity;
						//distance_multiplier = Utils::clamp(distance_multiplier, 0, 1);

						//light_intensity = light_intensity * distance_multiplier;
						double light_intensity = similarity * window_manager.general_window.scene_tab.current_scene.light_source.intensity * attenuation;
						light_intensity = Utils::clamp(light_intensity, 0, 1);

						uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
						uint8_t light_color_red = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 24) & 0x000000FF;
						
						uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
						uint8_t light_color_green = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 16) & 0x000000FF;
						
						uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;
						uint8_t light_color_blue = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 8) & 0x000000FF;

						uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
						uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
						uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

						uint8_t red = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_red, light_color_red);
						uint8_t green = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_green, light_color_green);
						uint8_t blue = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_blue, light_color_blue);

						//uint8_t red = (uint8_t)(light_intensity * ((light_source->color >> 24) & 0x000000FF));
						//uint8_t green = (uint8_t)(light_intensity * ((light_source->color >> 16) & 0x000000FF));
						//uint8_t blue = (uint8_t)(light_intensity * ((light_source->color >> 8) & 0x000000FF));

						fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
					}

					// If the triangle does not face the light source (and is not part of the light source model itself) set it to the minimum light exposure
					else {
						uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
						uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
						uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;

						uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
						uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
						uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

						fill_color = 0x000000FF | (minimum_red << 24) | (minimum_green << 16) | (minimum_blue << 8);
					}
				}

				// Calculate and store vertex colors and/or normals in order to interpolate them depending on Gouraud or Phong shading
				else if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx != ShadingType::Flat) {
					for (uint8_t vertex = 0; vertex < 3; vertex++) {
						const Mat* current_clipped_vertex = &clipped_world_v0;
						if (vertex == 0) {
							current_clipped_vertex = &clipped_world_v0;
						}
						else if (vertex == 1) {
							current_clipped_vertex = &clipped_world_v1;
						}
						else if (vertex == 2) {
							current_clipped_vertex = &clipped_world_v2;
						}

						/*
						double division = (((view_v0.get(1, 1) - view_v1.get(1, 1)) * (view_v2.get(2, 1) - view_v1.get(2, 1))) - ((view_v0.get(2, 1) - view_v1.get(2, 1)) * (view_v2.get(1, 1) - view_v1.get(1, 1))));

						division = 0;


						if (division == 0) {
							exit(-1);
							continue;
							throw std::runtime_error("Error: Division by 0 during barycentric coordinates calculation (clipping).");
						}

						double alpha = (((current_clipped_vertex->get(1, 1) - view_v1.get(1, 1)) * (view_v2.get(2, 1) - view_v1.get(2, 1))) - ((current_clipped_vertex->get(2, 1) - view_v1.get(2, 1)) * (view_v2.get(1, 1) - view_v1.get(1, 1)))) / division;
						double beta = (((view_v0.get(1, 1) - view_v1.get(1, 1)) * (current_clipped_vertex->get(2, 1) - view_v1.get(2, 1))) - ((view_v0.get(2, 1) - view_v1.get(2, 1)) * (current_clipped_vertex->get(1, 1) - view_v1.get(1, 1)))) / division;
						double omega = 0;

						double d = 1 - alpha - beta - omega;
						double c = omega; // always on v1
						double b = beta; // always on v2, unless v2 is at the same height as v1, then v0 and v2 swap and this is now at v0
						double a = alpha; // always on v0, unless same case as beta above, in this case it stays at v2

						bool left_edge = true;
						bool top_edge = true;

						Mat interpolated_normal = ((view_v0_normal * a) + (view_v1_normal * c) + (view_v2_normal * b));
						interpolated_normal.normalize();
						*/

						// Given: point_xyz (the point we're testing)
						//        v0_xyz, v1_xyz, v2_xyz (the triangle vertices in 3D)
						// Each of these has x, y, and z components

						// First, calculate the vectors from vertex v0 to the other points
						double v0_to_v1_x = world_v1.get(1, 1) - world_v0.get(1, 1);
						double v0_to_v1_y = world_v1.get(2, 1) - world_v0.get(2, 1);
						double v0_to_v1_z = world_v1.get(3, 1) - world_v0.get(3, 1);

						double v0_to_v2_x = world_v2.get(1, 1) - world_v0.get(1, 1);
						double v0_to_v2_y = world_v2.get(2, 1) - world_v0.get(2, 1);
						double v0_to_v2_z = world_v2.get(3, 1) - world_v0.get(3, 1);

						double v0_to_point_x = current_clipped_vertex->get(1, 1) - world_v0.get(1, 1);
						double v0_to_point_y = current_clipped_vertex->get(2, 1) - world_v0.get(2, 1);
						double v0_to_point_z = current_clipped_vertex->get(3, 1) - world_v0.get(3, 1);

						// Calculate dot products needed for the barycentric formula
						// dot(v0_to_v1, v0_to_v1)
						double d00 = (v0_to_v1_x * v0_to_v1_x) +
							(v0_to_v1_y * v0_to_v1_y) +
							(v0_to_v1_z * v0_to_v1_z);

						// dot(v0_to_v1, v0_to_v2)
						double d01 = (v0_to_v1_x * v0_to_v2_x) +
							(v0_to_v1_y * v0_to_v2_y) +
							(v0_to_v1_z * v0_to_v2_z);

						// dot(v0_to_v2, v0_to_v2)
						double d11 = (v0_to_v2_x * v0_to_v2_x) +
							(v0_to_v2_y * v0_to_v2_y) +
							(v0_to_v2_z * v0_to_v2_z);

						// dot(v0_to_v1, v0_to_point)
						double d20 = (v0_to_v1_x * v0_to_point_x) +
							(v0_to_v1_y * v0_to_point_y) +
							(v0_to_v1_z * v0_to_point_z);

						// dot(v0_to_v2, v0_to_point)
						double d21 = (v0_to_v2_x * v0_to_point_x) +
							(v0_to_v2_y * v0_to_point_y) +
							(v0_to_v2_z * v0_to_point_z);

						// Calculate denominator for barycentric coordinates
						double denom = (d00 * d11) - (d01 * d01);

						// Check for degenerate triangle
						if (denom == 0) {
							continue;
							throw std::runtime_error("Error: Degenerate triangle detected in 3D barycentric calculation");
						}

						// Calculate beta (weight for v1)
						double beta = ((d11 * d20) - (d01 * d21)) / denom;

						// Calculate gamma (weight for v2)
						double gamma = ((d00 * d21) - (d01 * d20)) / denom;

						// Calculate alpha (weight for v0)
						double alpha = 1.0 - beta - gamma;

						Mat interpolated_clipped_normal = Mat({ {0}, {0}, {0}, {0} }, 4, 1);

						interpolated_clipped_normal = (v0_normal * alpha) +
							(v1_normal * beta) +
							(v2_normal * gamma);

						interpolated_clipped_normal.normalize();

						if (window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Phong) {
							if (vertex == 0) {
								clipped_v0_normal = interpolated_clipped_normal;
							}
							else if (vertex == 1) {
								clipped_v1_normal = interpolated_clipped_normal;
							}
							else if (vertex == 2) {
								clipped_v2_normal = interpolated_clipped_normal;
							}
						}
						else if (window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Gouraud) {
							// Calculates and updates the color for the given vertex if using Gouraud shading
							Mat vertex_color = Mat({ {0}, {0}, {0}, {255} }, 4, 1);

							// Vertex lighting/shading happens here
							Mat normalized_light_source = window_manager.general_window.scene_tab.current_scene.light_source.position - *current_clipped_vertex;

							double distance = normalized_light_source.norm();
							normalized_light_source /= distance;

							double similarity = 0;

							// If the triangle faces the light source (and the triangle is not part of the light source model), interpolate the fill color (the default color for all models) with the light source color, proportional to the light intensity for the given triangle
							if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::point) {
								similarity = Mat::dot(normalized_light_source, interpolated_clipped_normal);
								//similarity = 1;
							}
							else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::directional) {


								similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, interpolated_clipped_normal);
							}
							else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::spotlight) {
								// How similar the light ray and the light direction are (represents whether the triangle is within the field of view of the light)
								double direction_similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, normalized_light_source);

								// How similar the light ray and the triangle normal vectors are (represents the light intensity)
								double position_similarity = Mat::dot(interpolated_clipped_normal, normalized_light_source);

								// If either the light direction does not positively correlate with the triangle's negative normal = direction_similarity (a backface relative to the light)
								// Or the triangle is a backface relative to the light source's position = position_similarity
								// Set the similarity to 0 / unlit
								if (direction_similarity && position_similarity < 0) {
									similarity = 0;
								}
								// Combine both the direction and position similarities
								else {
									similarity = direction_similarity * position_similarity;
								}
							}

							if (similarity > 0) {
								double attenuation = 0;

								if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type != LightType::directional && distance != 0) attenuation = 1.0 / (distance * distance);
								else attenuation = 1.0;

								double light_intensity = similarity * window_manager.general_window.scene_tab.current_scene.light_source.intensity * attenuation;
								light_intensity = Utils::clamp(light_intensity, 0, 1);

								uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
								uint8_t light_color_red = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 24) & 0x000000FF;

								uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
								uint8_t light_color_green = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 16) & 0x000000FF;

								uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;
								uint8_t light_color_blue = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 8) & 0x000000FF;

								uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
								uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
								uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

								uint8_t red = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_red, light_color_red);
								uint8_t green = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_green, light_color_green);
								uint8_t blue = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_blue, light_color_blue);

								vertex_color.set(red, 1, 1);
								vertex_color.set(green, 2, 1);
								vertex_color.set(blue, 3, 1);
							}

							// If the triangle does not face the light source (and is not part of the light source model itself) set it to the minimum light exposure
							else {
								uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
								uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
								uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;

								uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
								uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
								uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

								vertex_color.set(minimum_red, 1, 1);
								vertex_color.set(minimum_green, 2, 1);
								vertex_color.set(minimum_blue, 3, 1);
							}

							if (vertex == 0) {
								clipped_v0_color = vertex_color;
							}
							else if (vertex == 1) {
								clipped_v1_color = vertex_color;
							}
							else if (vertex == 2) {
								clipped_v2_color = vertex_color;
							}
						}
					}
				}
				// If shading but light source is disabled, use default fill color
				else if (!is_light_source && shade && !window_manager.general_window.scene_tab.current_scene.light_source.enabled) {
					uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
					uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
					uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;

					fill_color = 0x000000FF | (fill_color_red << 24) | (fill_color_green << 16) | (fill_color_blue << 8);
				}
				// Light source model is always lit up to the maximum
				else if (is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled) {
					uint8_t red = window_manager.general_window.scene_tab.current_scene.light_source.color >> 24;
					uint8_t green = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 16) & 0x000000FF;
					uint8_t blue = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 8) & 0x000000FF;

					fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
				}

				// FLIP X AND Y
				v0.set(-v0.get(1, 1), 1, 1);
				v0.set(-v0.get(2, 1), 2, 1);

				v1.set(-v1.get(1, 1), 1, 1);
				v1.set(-v1.get(2, 1), 2, 1);

				v2.set(-v2.get(1, 1), 1, 1);
				v2.set(-v2.get(2, 1), 2, 1);

				// NOW IN NDC SPACE
				
				
				
				
				// Transformation into screen space coordinates (i.e. 2D window x-y)
				// Positive x is to the right
				// Positive y is down (i.e. top is 0 and bottom is window height - 1)
				v0 = window_manager.general_window.scene_tab.current_scene.camera.SCALE_MATRIX * v0;
				v1 = window_manager.general_window.scene_tab.current_scene.camera.SCALE_MATRIX * v1;
				v2 = window_manager.general_window.scene_tab.current_scene.camera.SCALE_MATRIX * v2;

				// Gives triangles random colors each frame
				/*
				uint8_t alpha = 0xFF;
				uint8_t red = rand() % 0xFF;
				uint8_t blue = rand() % 0xFF;
				uint8_t green = rand() % 0xFF;
				uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;


				outline_color = color;
				*/


				//fill_color = color;

				bool is_v0_equal_v1 = v0 == v1;
				bool is_v0_equal_v2 = v0 == v2;
				bool is_v1_equal_v2 = v1 == v2;

				//draw_outline = true;
				//fill = true;

				// Clamps pixels to window boundaries if exceeding
				if (v0.get(1, 1) < 0) v0.set(0, 1, 1);
				if (v0.get(2, 1) < 0) v0.set(0, 2, 1);

				if (v1.get(1, 1) < 0) v1.set(0, 1, 1);
				if (v1.get(2, 1) < 0) v1.set(0, 2, 1);

				if (v2.get(1, 1) < 0) v2.set(0, 1, 1);
				if (v2.get(2, 1) < 0) v2.set(0, 2, 1);

				if (v0.get(1, 1) >= WIDTH - 1) v0.set(WIDTH - 1, 1, 1);
				if (v0.get(2, 1) >= HEIGHT - 1) v0.set(HEIGHT - 1, 2, 1);

				if (v1.get(1, 1) >= WIDTH - 1) v1.set(WIDTH - 1, 1, 1);
				if (v1.get(2, 1) >= HEIGHT - 1) v1.set(HEIGHT - 1, 2, 1);

				if (v2.get(1, 1) >= WIDTH - 1) v2.set(WIDTH - 1, 1, 1);
				if (v2.get(2, 1) >= HEIGHT - 1) v2.set(HEIGHT - 1, 2, 1);

				if ((fill)) {
					// Ignore triangle rasterization if 2 of the vertices are the same (meaning the triangle is basically a line, which is already handled when the lines are drawn)
					if (is_v0_equal_v1 || is_v0_equal_v2 || is_v1_equal_v2) {
						return;
					}

					fill_triangle(v0, v1, v2, clipped_v0_normal, clipped_v1_normal, clipped_v2_normal, clipped_world_v0, clipped_world_v1, clipped_world_v2, clipped_v0_color, clipped_v1_color, clipped_v2_color, fill_color, shade, is_light_source, is_axes);
				}

				if (draw_outline) {
					// Checks whether 2 of the triangle's vertices are the same, and only draws line if that is not the case (avoids drawing the same line more than once)
					if (!is_v0_equal_v1) {
						draw_line(
							v0.get(1, 1), v0.get(2, 1),
							v1.get(1, 1), v1.get(2, 1),
							v0, v1,
							view_v0z, view_v1z,
							outline_color,
							is_axes
						);
					}
					
					if (!is_v1_equal_v2 && v2 != v0) {
						draw_line(
							v1.get(1, 1), v1.get(2, 1),
							v2.get(1, 1), v2.get(2, 1),
							v1, v2,
							view_v1z, view_v2z,
							outline_color,
							is_axes
						);
					}

					if (!is_v0_equal_v2 && v0 != v1 && v2 != v1) {
						draw_line(
							v2.get(1, 1), v2.get(2, 1),
							v0.get(1, 1), v0.get(2, 1),
							v2, v0,
							view_v2z, view_v0z,
							outline_color,
							is_axes
						);
					}
				}


			}
		}
	}
}

void Engine::draw_line(double x1, double y1, double x2, double y2, const Mat& vec_a, const Mat& vec_b, const double& vec_a_originalz, const double& vec_b_originalz, uint32_t outline_color, bool is_axes) {
	// If not a vertical line
	// (if x's are too close it's considered a vertical line, this accounts for small floating point errors)

	// Ignore lines if they are outside of the window

	/*
	if ((x1 < 0 && x2 < 0) || (x1 >= WIDTH && x2 >= WIDTH)) {
		return;
	}
	if ((y1 < 0 && y2 < 0) || (y1 >= HEIGHT && y2 >= HEIGHT)) {
		return;
	}
	*/


	Mat start_vector = vec_a;
	Mat end_vector = vec_b;
	double start_vector_original_z = vec_a_originalz;
	double end_vector_original_z = vec_b_originalz;

	if (abs(x1 - x2) > 0.5) {
		double original_x = fmin(x1, x2);
		double y = (original_x == x1) ? y1 : y2;

		bool starts_at_vec_a = true;
		if (original_x == x2) {
			starts_at_vec_a = false;
			std::swap(start_vector, end_vector);
			std::swap(start_vector_original_z, end_vector_original_z);
		}

		//double dy = (y2 - y1) / (x2 - x1);
		double dy = (end_vector.get(2, 1) - start_vector.get(2, 1)) / (end_vector.get(1, 1) - start_vector.get(1, 1));
		double abs_dx = abs(1 / dy);

		// Clip edges of the line if they're out of the screen to optimize (check intersections with screen) x and y must be within screen boundaries
		if (abs(dy) <= abs_dx) {
			uint16_t x = round(fmax(0, original_x));
			if (x != original_x) {
				y += (x - original_x) * dy;
			}

			uint16_t rounded_y = round(y);
			//start_vector.set(x, 1, 1);
			//start_vector.set(rounded_y, 2, 1);

			double start_vector_x = start_vector.get(1, 1);
			double start_vector_y = start_vector.get(2, 1);
			double end_vector_x = end_vector.get(1, 1);
			double end_vector_y = end_vector.get(2, 1);

			Mat direction_vector = end_vector - start_vector;
			double direction_x = direction_vector.get(1, 1);
			double direction_y = direction_vector.get(2, 1);

			double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

			for (x;
				x < (uint16_t) round(fmax(x1, x2)); x++) {

				// Check and update depth buffer, or only do so when rasterizing?

				if (!window_manager.general_window.settings_tab.depth_test || is_axes) {
					this->pixel_buffer[(WIDTH * rounded_y) + x] = outline_color;
				}
				else {
					double walked_length = sqrt(pow(x - start_vector_x, 2) + pow(rounded_y - start_vector_y, 2));
					double alpha = walked_length / total_length;

					if (total_length == 0) {
						//return;
						throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
					}

					double a = alpha;
					double b = 1 - alpha;

					//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
					double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);
					if (window_manager.general_window.settings_tab.rasterize) interpolated_z *= window_manager.general_window.settings_tab.z_fighting_tolerance;
					//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));

					if (interpolated_z <= this->depth_buffer[(this->WIDTH * rounded_y) + x]) {
						this->pixel_buffer[(this->WIDTH * rounded_y) + x] = outline_color;
					}

				}

				y += dy;
				rounded_y = round(y);
			}
		}
		else {
			double x = fmax(0, original_x);
			double original_y = y;
			double target_y = original_y == y1 ? y2 : y1;

			if (x != original_x) {
				original_y += (x - original_x) * dy;
			}

			uint16_t rounded_x = round(x);
			int16_t y = round(original_y);
			//start_vector.set(rounded_x, 1, 1);
			//start_vector.set(y, 2, 1);

			double start_vector_x = start_vector.get(1, 1);
			double start_vector_y = start_vector.get(2, 1);
			double end_vector_x = end_vector.get(1, 1);
			double end_vector_y = end_vector.get(2, 1);

			Mat direction_vector = end_vector - start_vector;
			double direction_x = direction_vector.get(1, 1);
			double direction_y = direction_vector.get(2, 1);

			double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

			for (y;
				y != round(target_y); y += dy > 0 ? 1 : -1) {

				// Check and update depth buffer, or only do so when rasterizing?

				//this->pixel_buffer[(WIDTH * (int16_t)y) + rounded_x] = outline_color;

				if (!window_manager.general_window.settings_tab.depth_test || is_axes) {
					this->pixel_buffer[(WIDTH * y) + rounded_x] = outline_color;
				}
				else {
					double walked_length = sqrt(pow(rounded_x - start_vector_x, 2) + pow(y - start_vector_y, 2));
					double alpha = walked_length / total_length;

					if (total_length == 0) {
						//return;
						throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
					}

					double a = alpha;
					double b = 1 - alpha; // always on v1

					//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
					double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);
					if (window_manager.general_window.settings_tab.rasterize) interpolated_z *= window_manager.general_window.settings_tab.z_fighting_tolerance;
					//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));
					//double interpolated_z = ((start_vector_original_z) + (alpha * ((end_vector_original_z) - (start_vector_original_z))));

					if (interpolated_z <= this->depth_buffer[(this->WIDTH * y) + rounded_x]) {
						this->pixel_buffer[(this->WIDTH * y) + rounded_x] = outline_color;
					}

				}

				x += abs_dx;
				rounded_x = round(x);

			}
		}


	}

	// If a vertical line
	else {
		uint16_t x = round(x1);
		uint16_t y = round(fmax(0, fmin(y1, y2)));

		if (y2 < y1) {
			std::swap(start_vector, end_vector);
			std::swap(start_vector_original_z, end_vector_original_z);
		}

		double start_vector_x = x;
		double start_vector_y = start_vector.get(2, 1);
		double end_vector_x = x;
		double end_vector_y = end_vector.get(2, 1);

		Mat direction_vector = end_vector - start_vector;
		double direction_x = x;
		double direction_y = direction_vector.get(2, 1);

		double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

		for (y; y < (uint16_t)round(fmax(y1, y2)); y++) {
			// Check and update depth buffer, or only do so when rasterizing?

			if (!window_manager.general_window.settings_tab.depth_test || is_axes) {
				this->pixel_buffer[(WIDTH * y) + x] = outline_color;
			}
			else {
				double walked_length = sqrt(pow(x - start_vector_x, 2) + pow(y - start_vector_y, 2));
				double alpha = walked_length / total_length;

				if (total_length == 0) {
					//return;
					throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
				}

				double a = alpha;
				double b = 1 - alpha; // always on v1

				//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
				double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);

				if (window_manager.general_window.settings_tab.rasterize) interpolated_z *= window_manager.general_window.settings_tab.z_fighting_tolerance;
				//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));

				if (interpolated_z <= this->depth_buffer[(this->WIDTH * y) + x]) {
					this->pixel_buffer[(this->WIDTH * y) + x] = outline_color;
				}
			}
		}

	}

}

void Engine::fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& world_v0_normal, const Mat& world_v1_normal, const Mat& world_v2_normal, const Mat& world_v0, const Mat& world_v1, const Mat& world_v2, const Mat& v0_color, const Mat& v1_color, const Mat& v2_color, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes) {

	Mat view_v0 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v0;
	Mat view_v1 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v1;
	Mat view_v2 = window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * world_v2;

	double view_v0z = view_v0.get(3, 1);
	double view_v1z = view_v1.get(3, 1);
	double view_v2z = view_v2.get(3, 1);

	double v0_x = v0.get(1, 1);
	double v0_y = v0.get(2, 1);

	double v1_x = v1.get(1, 1);
	double v1_y = v1.get(2, 1);

	double v2_x = v2.get(1, 1);
	double v2_y = v2.get(2, 1);

	uint16_t bounding_box_left_x = round(fmin(fmin(v0_x, v1_x), v2_x));
	uint16_t bounding_box_right_x = round(fmax(fmax(v0_x, v1_x), v2_x));
	uint16_t bounding_box_top_y = round(fmin(fmin(v0_y, v1_y), v2_y));
	uint16_t bounding_box_bottom_y = round(fmax(fmax(v0_y, v1_y), v2_y));

	const Mat vec_a = v0 - v1;
	const Mat vec_b = v2 - v1;

	double vec_a_x = vec_a.get(1, 1);
	double vec_a_y = vec_a.get(2, 1);

	double vec_b_x = vec_b.get(1, 1);
	double vec_b_y = vec_b.get(2, 1);

	Mat view_light_direction = window_manager.general_window.scene_tab.current_scene.light_source.direction;
	Quaternion::RotatePoint(window_manager.general_window.scene_tab.current_scene.camera.orientation, view_light_direction, false);

	Mat view_light_position = (window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX * window_manager.general_window.scene_tab.current_scene.light_source.position);

	for (uint16_t pixel_y = bounding_box_top_y; pixel_y <= bounding_box_bottom_y; pixel_y++) {
		if (pixel_y >= this->HEIGHT || pixel_y < 0) return;
		for (uint16_t pixel_x = bounding_box_left_x; pixel_x <= bounding_box_right_x; pixel_x++) {
			if (pixel_x >= this->WIDTH || pixel_x < 0) return;
			// C = V1 (Red) 
			// A = V0 (Green)
			// B = V2 (Blue)
			// Picturing a flat bottom triangle, the bottom left vertex would V1 (red), top middle vertex would be V0 (green), and bottom right vertex would be V2 (blue)
			//                 (V0, green)
			//                    /\
			//                   /  \
 			//                  /    \
			// 			       /      \
			//                /        \
			//               /          \
			//   (V1, red)  /____________\  (V2, blue)
			// 
			// 
			//           * V0 and V2 are swapped (and consequently their colors) if V1 and V2 have the same y (as in this triangle), becoming:
			// 
			// 			       (V2, blue)
			//                    /\
			//                   /  \
 			//                  /    \
			// 			       /      \
			//                /        \
			//               /          \
			//   (V1, red)  /____________\  (V0, green)          
			// 

			double division = (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));

			if (division == 0) {
				return;
				throw std::runtime_error("Error: Division by 0 during barycentric coordinates calculation (rasterization).");
			}

			double alpha = (((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / division;
			double beta = (((v0_x - v1_x) * (pixel_y - v1_y)) - ((v0_y - v1_y) * (pixel_x - v1_x))) / division;

			// Checks whether is within triangle/edge
			//if ((pixel_t0 >= 0 && pixel_t1 >= 0) || (pixel_t0 <= 1 && pixel_t1 <= 1)) {

			if (alpha >= 0 && beta >= 0 && alpha + beta <= 1) {
				double c = 1 - alpha - beta; // always on v1
				double b = beta; // always on v2, unless v2 is at the same height as v1, then v0 and v2 swap and this is now at v0
				double a = alpha; // always on v0, unless same case as beta above, in this case it stays at v2

				bool left_edge = true;
				bool top_edge = true;

				// Skips pixel if sits at bottom/right edge
				// Ignore t0 == 0 && t1 == 0 as well as

				/*
				if ((alpha == 0 || beta == 0) && !left_edge && !top_edge) {
					continue;
				}
				*/

				// Interpolates each vertex for red, green, and blue (if not shading)
				if (!shade) {
					uint8_t alpha = 0xFF;
					uint8_t red = c * 0xFF; // v1
					uint8_t green = a * 0xFF; // v0 (when v2 at same height as v1 swaps with v2)
					uint8_t blue = b * 0xFF; // v2 (when at same height as v1 swaps with v0)
					fill_color = (red << 24) | (green << 16) | (blue << 8) | alpha;
				}


				if (pixel_y < this->HEIGHT && pixel_y >= 0 && pixel_x >= 0 && pixel_x < this->WIDTH) {
					if (is_axes) {
						this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
						continue;
					}

					if (window_manager.general_window.settings_tab.depth_test) {
						//double interpolated_z = 1 / (((1 / v0_originalz) * a) + ((1 / v1_originalz) * c) + ((1 / v2_originalz) * b));
						double interpolated_z = (view_v0z * a) + (view_v1z * c) + (view_v2z * b);

						/*
						Mat interpolated_normal = ((v0_normal * a) + (v1_normal * c) + (v2_normal * b)) / interpolated_z;
						//interpolated_normal.normalize();
						
						double simi = -Mat::dot(light_source_dir, interpolated_normal);
						simi = Utils::clamp(simi, 0, 1);

						//if (simi > 0 && simi < 0.8) { printf("%f | %f | %f\n", interpolated_z, a, b); v0_normal.print(); Mat vz = ((v0_normal * a)); vz.normalize(); vz.print(); }

						uint8_t colo = simi * 0xFF;
						fill_color = (((0x000000FF | (colo << 24)) | (colo << 16)) | (colo << 8));
						*/

						if (interpolated_z <= this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x]) {
							if (interpolated_z < this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x]) this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x] = interpolated_z;

							if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Gouraud) {
								Mat color = ((v0_color * a) + (v1_color * c) + (v2_color * b));

								fill_color = (uint8_t)color.get(1, 1) << 24 | (uint8_t)color.get(2, 1) << 16 | (uint8_t)color.get(3, 1) << 8 | (uint8_t)color.get(4, 1);
							}
							else if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Phong) {
								// Vertex lighting/shading happens here
								Mat interpolated_normal = ((world_v0_normal * a) + (world_v1_normal * c) + (world_v2_normal * b));
								interpolated_normal.normalize();

								Mat current_triangle_vertex = (world_v0 * a) + (world_v1 * c) + (world_v2 * b);
								current_triangle_vertex.set(1, 4, 1);

								Mat normalized_light_source = window_manager.general_window.scene_tab.current_scene.light_source.position - current_triangle_vertex;

								double distance = normalized_light_source.norm();
								normalized_light_source /= distance;

								double similarity = 0;

								// If the triangle faces the light source (and the triangle is not part of the light source model), interpolate the fill color (the default color for all models) with the light source color, proportional to the light intensity for the given triangle
								if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::point) {
									similarity = Mat::dot(normalized_light_source, interpolated_normal);
									//similarity = 1;
								}
								else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::directional) {
									similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, interpolated_normal);
								}
								else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::spotlight) {
									// How similar the light ray and the light direction are (represents whether the triangle is within the field of view of the light)
									double direction_similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, interpolated_normal);

									// How similar the light ray and the triangle normal vectors are (represents the light intensity)
									double position_similarity = Mat::dot(normalized_light_source, interpolated_normal);

									// If either the light direction does not positively correlate with the triangle's negative normal = direction_similarity (a backface relative to the light)
									// Or the triangle is a backface relative to the light source's position = position_similarity
									// Set the similarity to 0 / unlit
									if (direction_similarity && position_similarity < 0) {
										similarity = 0;
									}
									// Combine both the direction and position similarities
									else {
										similarity = direction_similarity * position_similarity;
									}
								}

								if (similarity > 0) {
									double attenuation = 0;

									if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type != LightType::directional && distance != 0) attenuation = 1.0 / (distance * distance);
									else attenuation = 1.0;

									double light_intensity = similarity * window_manager.general_window.scene_tab.current_scene.light_source.intensity * attenuation;
									light_intensity = Utils::clamp(light_intensity, 0, 1);

									uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
									uint8_t light_color_red = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 24) & 0x000000FF;

									uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
									uint8_t light_color_green = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 16) & 0x000000FF;

									uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;
									uint8_t light_color_blue = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 8) & 0x000000FF;

									uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
									uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
									uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

									uint8_t red = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_red, light_color_red);
									uint8_t green = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_green, light_color_green);
									uint8_t blue = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_blue, light_color_blue);

									fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
								}

								// If the triangle does not face the light source (and is not part of the light source model itself) set it to the minimum light exposure
								else {
									uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
									uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
									uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;

									uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
									uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
									uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

									fill_color = 0x000000FF | (minimum_red << 24) | (minimum_green << 16) | (minimum_blue << 8);
								}
							}

							this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
						}
						
					}
					else {
						if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Gouraud) {
							Mat color = ((v0_color * a) + (v1_color * c) + (v2_color * b));

							fill_color = (uint8_t)color.get(1, 1) << 24 | (uint8_t)color.get(2, 1) << 16 | (uint8_t)color.get(3, 1) << 8 | (uint8_t)color.get(4, 1);
						}
						else if (!is_light_source && shade && window_manager.general_window.scene_tab.current_scene.light_source.enabled && window_manager.general_window.scene_tab.light_tab->shading_selected_idx == ShadingType::Phong) {
							// Vertex lighting/shading happens here
							Mat interpolated_normal = ((world_v0_normal * a) + (world_v1_normal * c) + (world_v2_normal * b));
							interpolated_normal.normalize();

							Mat current_triangle_vertex = (world_v0 * a) + (world_v1 * c) + (world_v2 * b);
							current_triangle_vertex.set(1, 4, 1);

							Mat normalized_light_source = window_manager.general_window.scene_tab.current_scene.light_source.position - current_triangle_vertex;

							double distance = normalized_light_source.norm();
							normalized_light_source /= distance;

							double similarity = 0;

							// If the triangle faces the light source (and the triangle is not part of the light source model), interpolate the fill color (the default color for all models) with the light source color, proportional to the light intensity for the given triangle
							if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::point) {
								similarity = Mat::dot(normalized_light_source, interpolated_normal);
								//similarity = 1;
							}
							else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::directional) {
								similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, interpolated_normal);
							}
							else if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type == LightType::spotlight) {
								// How similar the light ray and the light direction are (represents whether the triangle is within the field of view of the light)
								double direction_similarity = -Mat::dot(window_manager.general_window.scene_tab.current_scene.light_source.direction, interpolated_normal);

								// How similar the light ray and the triangle normal vectors are (represents the light intensity)
								double position_similarity = Mat::dot(normalized_light_source, interpolated_normal);

								// If either the light direction does not positively correlate with the triangle's negative normal = direction_similarity (a backface relative to the light)
								// Or the triangle is a backface relative to the light source's position = position_similarity
								// Set the similarity to 0 / unlit
								if (direction_similarity && position_similarity < 0) {
									similarity = 0;
								}
								// Combine both the direction and position similarities
								else {
									similarity = direction_similarity * position_similarity;
								}
							}

							if (similarity > 0) {
								double attenuation = 0;

								if (window_manager.general_window.scene_tab.current_scene.light_source.lighting_type != LightType::directional && distance != 0) attenuation = 1.0 / (distance * distance);
								else attenuation = 1.0;

								double light_intensity = similarity * window_manager.general_window.scene_tab.current_scene.light_source.intensity * attenuation;
								light_intensity = Utils::clamp(light_intensity, 0, 1);

								uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
								uint8_t light_color_red = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 24) & 0x000000FF;

								uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
								uint8_t light_color_green = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 16) & 0x000000FF;

								uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;
								uint8_t light_color_blue = (window_manager.general_window.scene_tab.current_scene.light_source.color >> 8) & 0x000000FF;

								uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
								uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
								uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

								uint8_t red = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_red, light_color_red);
								uint8_t green = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_green, light_color_green);
								uint8_t blue = (uint8_t)Utils::normalize(light_intensity, 0, 1, minimum_blue, light_color_blue);

								fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
							}

							// If the triangle does not face the light source (and is not part of the light source model itself) set it to the minimum light exposure
							else {
								uint8_t fill_color_red = (window_manager.general_window.settings_tab.FILL_COLOR >> 24) & 0x000000FF;
								uint8_t fill_color_green = (window_manager.general_window.settings_tab.FILL_COLOR >> 16) & 0x000000FF;
								uint8_t fill_color_blue = (window_manager.general_window.settings_tab.FILL_COLOR >> 8) & 0x000000FF;

								uint8_t minimum_red = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_red);
								uint8_t minimum_green = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_green);
								uint8_t minimum_blue = (uint8_t)(window_manager.general_window.scene_tab.current_scene.light_source.minimum_exposure * fill_color_blue);

								fill_color = 0x000000FF | (minimum_red << 24) | (minimum_green << 16) | (minimum_blue << 8);
							}
						}


						
						this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
					}
				}
			}
			else {
				//std::cout << "Alpha: " << alpha << std::endl;
				//std::cout << "Beta: " << beta << std::endl;
			}
		}
	}

	return;
}

void Engine::draw() {
	// Clears pixel buffer to the background/clear color
	for (int i = 0; i < this->WIDTH * this->HEIGHT; i++) {
		this->pixel_buffer[i] = window_manager.general_window.settings_tab.BG_COLOR;
		this->depth_buffer[i] = std::numeric_limits<double>::max();
	}

	for (const Instance& instance : window_manager.general_window.scene_tab.current_scene.scene_instances) {
		if (instance.show) {
			draw_instance(instance, window_manager.general_window.settings_tab.wireframe_render, window_manager.general_window.settings_tab.LINE_COLOR, window_manager.general_window.settings_tab.rasterize, window_manager.general_window.settings_tab.FILL_COLOR, window_manager.general_window.settings_tab.shade);

			if (instance.is_axes && window_manager.general_window.scene_tab.show_transform_axes && (window_manager.general_window.scene_tab.is_instances_open || window_manager.general_window.scene_tab.is_light_open)) {
				draw_instance(window_manager.general_window.scene_tab.current_scene.axes_instance, window_manager.general_window.settings_tab.wireframe_render, window_manager.general_window.settings_tab.LINE_COLOR, window_manager.general_window.settings_tab.rasterize, window_manager.general_window.settings_tab.FILL_COLOR, false);
			}
		}
	}
}

void Engine::render() {
	SDL_RenderSetScale(this->renderer, window_manager.io.DisplayFramebufferScale.x, window_manager.io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(this->renderer, window_manager.general_window.settings_tab.CLEAR_COLOR >> 24, window_manager.general_window.settings_tab.CLEAR_COLOR >> 16, window_manager.general_window.settings_tab.CLEAR_COLOR >> 8, window_manager.general_window.settings_tab.CLEAR_COLOR);
	SDL_RenderClear(this->renderer);
	SDL_UpdateTexture(this->texture, nullptr, this->pixel_buffer, this->WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), this->renderer);
	SDL_RenderPresent(this->renderer);
}

void Engine::rotateX(Instance& instance, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	instance.pitch += radians;

	instance.orientation = Quaternion::FromYawPitchRoll(orientation, instance.yaw, instance.pitch, instance.roll, default_world_right, default_world_up, default_world_direction);

	instance.ROTATION_MATRIX = instance.orientation.get_rotationmatrix();
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	//instance.orientation.GetAngles(instance.yaw, instance.pitch, instance.roll);
}

void Engine::rotateX(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	light_source.pitch += radians;

	light_source.orientation = Quaternion::FromYawPitchRoll(orientation, light_source.yaw, light_source.pitch, light_source.roll, default_world_right, default_world_up, default_world_direction);

	light_source.direction = light_source.default_direction;
	light_source.up = light_source.default_up;

	Quaternion::RotatePoint(light_source.orientation, light_source.direction, false);
	Quaternion::RotatePoint(light_source.orientation, light_source.up, false);

	if (light_source.has_model) {
		light_source.instance->pitch += radians;

		light_source.instance->orientation = light_source.orientation;

		light_source.instance->ROTATION_MATRIX = light_source.instance->orientation.get_rotationmatrix();
		light_source.instance->MODEL_TO_WORLD = light_source.instance->TRANSLATION_MATRIX * light_source.instance->ROTATION_MATRIX * light_source.instance->SCALING_MATRIX;
	}
}

void Engine::rotateX(Mesh& mesh, double radians, const Mat& default_world_right) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_right.get(1, 1), default_world_right.get(2, 1), default_world_right.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateX(Quad& quad, double radians, const Mat& default_world_right) {
	rotateX(quad.triangles[0], radians, default_world_right);
	rotateX(quad.triangles[1], radians, default_world_right);
}

void Engine::rotateX(Triangle& triangle, double radians, const Mat& default_world_right) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_right.get(1, 1), default_world_right.get(2, 1), default_world_right.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateX(Mat& matrix, double radians, const Mat& default_world_right) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_right.get(1, 1), default_world_right.get(2, 1), default_world_right.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	matrix = rotation * matrix;
}

void Engine::rotateY(Instance& instance, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	instance.yaw += radians;

	instance.orientation = Quaternion::FromYawPitchRoll(orientation, instance.yaw, instance.pitch, instance.roll, default_world_right, default_world_up, default_world_direction);
	
	instance.ROTATION_MATRIX = instance.orientation.get_rotationmatrix();
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	//instance.orientation.GetAngles(instance.yaw, instance.pitch, instance.roll);
}

void Engine::rotateY(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	light_source.yaw += radians;

	light_source.orientation = Quaternion::FromYawPitchRoll(orientation, light_source.yaw, light_source.pitch, light_source.roll, default_world_right, default_world_up, default_world_direction);

	light_source.direction = light_source.default_direction;
	light_source.up = light_source.default_up;

	Quaternion::RotatePoint(light_source.orientation, light_source.direction, false);
	Quaternion::RotatePoint(light_source.orientation, light_source.up, false);

	if (light_source.has_model) {
		light_source.instance->yaw += radians;

		light_source.instance->orientation = light_source.orientation;

		light_source.instance->ROTATION_MATRIX = light_source.instance->orientation.get_rotationmatrix();
		light_source.instance->MODEL_TO_WORLD = light_source.instance->TRANSLATION_MATRIX * light_source.instance->ROTATION_MATRIX * light_source.instance->SCALING_MATRIX;
	}
}

void Engine::rotateY(Mesh& mesh, double radians, const Mat& default_world_up) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_up.get(1, 1), default_world_up.get(2, 1), default_world_up.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateY(Quad& quad, double radians, const Mat& default_world_up) {
	rotateY(quad.triangles[0], radians, default_world_up);
	rotateY(quad.triangles[1], radians, default_world_up);
}

void Engine::rotateY(Triangle& triangle, double radians, const Mat& default_world_up) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_up.get(1, 1), default_world_up.get(2, 1), default_world_up.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateY(Mat& matrix, double radians, const Mat& default_world_up) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_up.get(1, 1), default_world_up.get(2, 1), default_world_up.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	matrix = rotation * matrix;
}

void Engine::rotateZ(Instance& instance, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	instance.roll += radians;

	instance.orientation = Quaternion::FromYawPitchRoll(orientation, instance.yaw, instance.pitch, instance.roll, default_world_right, default_world_up, default_world_direction);

	instance.ROTATION_MATRIX = instance.orientation.get_rotationmatrix();
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	//instance.orientation.GetAngles(instance.yaw, instance.pitch, instance.roll);
}

void Engine::rotateZ(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right) {
	light_source.roll += radians;

	light_source.orientation = Quaternion::FromYawPitchRoll(orientation, light_source.yaw, light_source.pitch, light_source.roll, default_world_right, default_world_up, default_world_direction);

	light_source.direction = light_source.default_direction;
	light_source.up = light_source.default_up;

	Quaternion::RotatePoint(light_source.orientation, light_source.direction, false);
	Quaternion::RotatePoint(light_source.orientation, light_source.up, false);

	if (light_source.has_model) {
		light_source.instance->roll += radians;

		light_source.instance->orientation = light_source.orientation;

		light_source.instance->ROTATION_MATRIX = light_source.instance->orientation.get_rotationmatrix();
		light_source.instance->MODEL_TO_WORLD = light_source.instance->TRANSLATION_MATRIX * light_source.instance->ROTATION_MATRIX * light_source.instance->SCALING_MATRIX;
	}
}

void Engine::rotateZ(Mesh& mesh, double radians, const Mat& default_world_direction) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_direction.get(1, 1), default_world_direction.get(2, 1), default_world_direction.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateZ(Quad& quad, double radians, const Mat& default_world_direction) {
	rotateZ(quad.triangles[0], radians, default_world_direction);
	rotateZ(quad.triangles[1], radians, default_world_direction);
}

// Always done in world space
void Engine::rotateZ(Triangle& triangle, double radians, const Mat& default_world_direction) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_direction.get(1, 1), default_world_direction.get(2, 1), default_world_direction.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

// Always done in world space
void Engine::rotateZ(Mat& matrix, double radians, const Mat& default_world_direction) {
	Quaternion quaternion_rotation = Quaternion::AngleAxis(default_world_direction.get(1, 1), default_world_direction.get(2, 1), default_world_direction.get(3, 1), radians);

	Mat rotation = quaternion_rotation.get_rotationmatrix();

	matrix = rotation * matrix;
}

void Engine::translate(Instance& instance, double tx, double ty, double tz) {
	instance.tx += tx;
	instance.ty += ty;
	instance.tz += tz;

	Mat translation_matrix = Mat::translation_matrix(instance.tx, instance.ty, instance.tz);

	instance.TRANSLATION_MATRIX = translation_matrix;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::translate(Mesh& mesh, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	for (Mat& vertex : mesh.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Quad& quad, double tx, double ty, double tz) {
	Engine::translate(quad.triangles[0], tx, ty, tz);
	Engine::translate(quad.triangles[1], tx, ty, tz);
}

void Engine::translate(Triangle& triangle, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	for (Mat& vertex : triangle.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Mat& matrix, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	matrix = translation_matrix * matrix;
}

void Engine::scale(Instance& instance, double sx, double sy, double sz, bool set) {
	// Also include incremental scaling option eventually

	if (set) {
		instance.sx = sx;
		instance.sy = sy;
		instance.sz = sz;
	}
	else {
		instance.sx *= sx;
		instance.sy *= sy;
		instance.sz *= sz;
	}

	Mat scaling_matrix = Mat::scale_matrix(instance.sx, instance.sy, instance.sz);

	instance.SCALING_MATRIX = scaling_matrix * instance.SCALING_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::scale(Mesh& mesh, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	for (Mat& vertex : mesh.vertices) {
		vertex = scale_matrix * vertex;
	}
}

void Engine::scale(Quad& quad, double sx, double sy, double sz) {
	Engine::scale(quad.triangles[0], sx, sy, sz);
	Engine::scale(quad.triangles[1], sx, sy, sz);
}

void Engine::scale(Triangle& triangle, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	for (Mat& vertex : triangle.vertices) {
		vertex = scale_matrix * vertex;
	}

}

void Engine::scale(Mat& matrix, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	matrix = scale_matrix * matrix;
}

Mat Engine::euler_rotationX_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{1, 0, 0, 0},
			{0, cos(radians), -sin(radians), 0},
			{0, sin(radians), cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

Mat Engine::euler_rotationY_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{cos(radians), 0, -sin(radians), 0},
			{0, 1, 0, 0},
			{sin(radians), 0, cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

Mat Engine::euler_rotationZ_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{cos(radians), sin(radians), 0, 0},
			{sin(radians), cos(radians), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

/// <summary>
/// Calculates whether there is an intersection between the given line and plane, updating the `intersection_point` variable to the intersection point and returning 1 if that is the case, and 0 otherwise.
/// </summary>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <param name="intersection_point">Reference to a variable which will store the intersection point if it exists</param>
/// <returns>1 if the line intersects the plane, 0 otherwise</returns>
bool Engine::LinePlaneIntersection(const Mat* plane_point, const Mat* plane_normal, const Mat* line_start, const Mat* line_end, Mat& intersection_point) {
	// Plane normal should be normalized?
	const Mat line_vector = *line_end - *line_start;
	const double alpha = Mat::dot(*plane_point - *line_start, *plane_normal) / Mat::dot(line_vector, *plane_normal);
	if (alpha != 0) {
		intersection_point = *line_start + (alpha * line_vector);
		return 1;
	}

	return 0;
}

/// <summary>
/// Returns the distance between a given point and a given plane.
/// </summary>
/// <param name="point">The point to be tested against the plane</param>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <returns>The distance between the point and plane.</returns>
double Engine::PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal) {
	return Mat::dot(*point - *plane_point, *plane_normal);
}

/// <summary>
/// Tests whether a given triangle intersects a given plane and, if there is an intersection, stores the resulting clipped triangles in the given variables.
/// </summary>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <param name="input_triangle">The triangle to be tested against the plane</param>
/// <param name="clipped_triangle_a">Variable where a clipped triangle will be stored</param>
/// <param name="clipped_triangle_b">Variable where a clipped triangle will be stored</param>
/// <returns>How many new triangles it got split into</returns>
uint8_t Engine::ClipTriangleToPlane(const Mat* plane_point, const Mat* plane_normal, const Triangle* input_triangle, Triangle& clipped_triangle_a, Triangle& clipped_triangle_b) {
	uint8_t n_inside_points = 0;
	uint8_t n_outside_points = 0;

	const Mat* inside_points[3];
	const Mat* outside_points[3];
	int inside_indices[3];   // Keep track of original vertex indices
	int outside_indices[3];  // Keep track of original vertex indices

	double distances[3] = {
		Engine::PointDistanceToPlane(&input_triangle->vertices[0], plane_point, plane_normal),
		Engine::PointDistanceToPlane(&input_triangle->vertices[1], plane_point, plane_normal),
		Engine::PointDistanceToPlane(&input_triangle->vertices[2], plane_point, plane_normal)
	};

	// Classify points and maintain their original indices
	for (int i = 0; i < 3; i++) {
		if (distances[i] > 0) {
			inside_points[n_inside_points] = &input_triangle->vertices[i];
			inside_indices[n_inside_points] = i;
			n_inside_points++;
		}
		else {
			outside_points[n_outside_points] = &input_triangle->vertices[i];
			outside_indices[n_outside_points] = i;
			n_outside_points++;
		}
	}

	if (n_inside_points == 0) {
		return 0;
	}
	else if (n_inside_points == 3) {
		clipped_triangle_a = *input_triangle;
		return 1;
	}
	else if (n_inside_points == 1 && n_outside_points == 2) {
		Mat intersection_point_a = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
		Mat intersection_point_b = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[0], intersection_point_a);
		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[1], intersection_point_b);

		// Determine correct winding order based on original triangle
		int inside_idx = inside_indices[0];
		int next_idx = (inside_idx + 1) % 3;

		// If the next vertex in the original triangle matches first outside point
		if (next_idx == outside_indices[0]) {
			clipped_triangle_a = Triangle(*inside_points[0], intersection_point_a, intersection_point_b);
		}
		else {
			clipped_triangle_a = Triangle(*inside_points[0], intersection_point_b, intersection_point_a);
		}

		return 1;
	}
	else if (n_inside_points == 2 && n_outside_points == 1) {
		Mat intersection_point_a = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
		Mat intersection_point_b = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[0], intersection_point_a);
		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[1], outside_points[0], intersection_point_b);

		// Determine correct winding order based on original indices
		int first_inside_idx = inside_indices[0];
		int second_inside_idx = inside_indices[1];
		int outside_idx = outside_indices[0];

		// Check if first inside point comes before second inside point in original winding
		bool first_before_second = ((first_inside_idx + 1) % 3 == second_inside_idx) ||
			(first_inside_idx == 2 && second_inside_idx == 0);

		if (first_before_second) {
			clipped_triangle_a = Triangle(*inside_points[0], *inside_points[1], intersection_point_b);
			clipped_triangle_b = Triangle(*inside_points[0], intersection_point_b, intersection_point_a);
		}
		else {
			clipped_triangle_a = Triangle(*inside_points[1], *inside_points[0], intersection_point_a);
			clipped_triangle_b = Triangle(*inside_points[1], intersection_point_a, intersection_point_b);
		}

		return 2;

	}

	throw std::runtime_error("Invalid branch taken at function `ClipTriangleToPlane`");
}

// Sets yaw and pitch from a direction vector (the displacement in each axis from the default direction vector)
void Engine::Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch) {
	// Still need to handle gimbal lock edge case and also include roll

	yaw = atan2f(direction_vector.get(1, 1), direction_vector.get(3, 1));
	pitch = atan2f(-direction_vector.get(2, 1), sqrt(pow(direction_vector.get(1, 1), 2) + pow(direction_vector.get(3, 1), 2)));

	if (yaw < 0) yaw = (2 * M_PI) + yaw;
	if (pitch < 0) pitch = (2 * M_PI) + pitch;
}

// Sets yaw, pitch, and roll from a rotation matrix
void Engine::Euler_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll) {
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

// Frees the pixel buffer, destroys the texture, renderer, window, and cleans SDL subsystems.
void Engine::close() {
	free(this->pixel_buffer);
	free(this->depth_buffer);

	window_manager.close();

	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);

	SDL_Quit();
}