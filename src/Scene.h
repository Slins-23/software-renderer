#include <stdio.h>
#include <deque>
#include "Instance.h"
#include "json.hpp"
#include "Matrix.h"
#pragma once

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
	nlohmann::ordered_json scene_data;

	Scene();

	Scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_translation, Mat& default_camera_direction, Mat& new_camera_direction, Mat& default_camera_up, Mat& new_camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, bool& rotation_given, bool& direction_given, bool& up_given, bool use_scene_camera_settings);

	/// <summary>
	///  Load scene from a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	void load_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_translation, Mat& default_camera_direction, Mat& new_camera_direction, Mat& default_camera_up, Mat& new_camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, bool& rotation_given, bool& direction_given, bool& up_given, bool use_scene_camera_settings);

	/// <summary>
	///  Save scene to a JSON file in the defined scene folder, with the given name.
	///	(`scene_filename` is just the name of the scene, do not include the extension.)
	/// </summary>
	void save_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, const Mat& default_camera_position, const Mat& camera_position, const Mat& default_camera_direction, const Mat& camera_direction, const Mat& default_camera_up, const Mat& camera_up, double yaw, double pitch, double roll);

	Mesh get_mesh(uint32_t mesh_id);
	Mesh get_mesh(std::string mesh_filename);
	Mesh* get_mesh_ptr(uint32_t mesh_id);
	Mesh* get_mesh_ptr(std::string mesh_filename);

	Instance get_instance(uint32_t instance_id);
	Instance get_instance(std::string instance_name);
	Instance* get_instance_ptr(uint32_t instance_id);
	Instance* get_instance_ptr(std::string instance_name);
};
