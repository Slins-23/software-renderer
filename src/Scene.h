#pragma once
#include <stdio.h>
#include <deque>
#include "Instance.h"
#include "json.hpp"
#include "Matrix.h"
#include "Light.h"
#include "Camera.h"
#include "Quaternion.h"


class Scene {
private:
public:
	uint32_t total_ever_instances = 0;
	uint32_t total_ever_meshes = 0;

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
	nlohmann::ordered_json scene_data;

	// The default vectors here represent the world coordinate system, not the camera/view space
	Mat default_world_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
	Mat default_world_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
	Mat default_world_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

	Camera camera;

	Light light_source;

	Mesh axes_mesh = Mesh(this->total_ever_meshes);
	Instance axes_instance = Instance(this->total_ever_instances);

	// Background color
	uint32_t BG_COLOR = 0x000000FF;

	// Line/wireframe color
	uint32_t LINE_COLOR = 0x00FF00FF;

	// Rasterization color
	uint32_t FILL_COLOR = 0x66285CFF;

	bool load_error = false;

	Scene() {};

	~Scene() {};

	Scene(const char* models_folder);

	Scene(const char* models_folder, Orientation rotation_orientation, bool verbose);

	/// <summary>
	///  Load scene from a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	Scene(const char* scene_folder, const char* scene_filename, const char* models_folder, Orientation rotation_orientation, bool update_camera_settings, bool verbose);

	// Explicitly using the default copy constructor for initializing the scene object. This needs to be done since overloading the move operator triggers the compiler to require a custom implementation for the copy constructor.
	Scene(const Scene& original_scene) = default;

	// Move operator overload for updating the scene once a new one is loaded since pointers would otherwise get freed
	Scene& operator=(Scene&& original_scene) noexcept;

	/// <summary>
	///  Save scene to a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	void save(const char* scenes_folder, const char* scene_filename) const;

	bool get_mesh(uint32_t mesh_id, Mesh& mesh);
	bool get_mesh(std::string mesh_filename, Mesh& mesh);
	Mesh* get_mesh_ptr(uint32_t mesh_id);
	Mesh* get_mesh_ptr(std::string mesh_filename);

	bool get_instance(uint32_t instance_id, Instance& instance);
	bool get_instance(std::string instance_name, Instance& instance);
	Instance* get_instance_ptr(uint32_t instance_id);
	Instance* get_instance_ptr(std::string instance_name);
};
