#include "Scene.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

Scene::Scene() {

}

Scene::Scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_translation, Mat& default_camera_direction, Mat& new_camera_direction, Mat& default_camera_up, Mat& new_camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, bool& rotation_given, bool& direction_given, bool& up_given, bool use_scene_camera_settings) {
	this->load_scene(scenes_folder, scene_filename, models_folder, verbose, default_camera_position, camera_translation, default_camera_direction, new_camera_direction, default_camera_up, new_camera_up, camera_yaw, camera_pitch, camera_roll, rotation_given, direction_given, up_given, use_scene_camera_settings);
}

void Scene::load_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_translation, Mat& default_camera_direction, Mat& new_camera_direction, Mat& default_camera_up, Mat& new_camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, bool& rotation_given, bool& direction_given, bool& up_given, bool use_scene_camera_settings) {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scenes_folder, scene_filename);

	std::ifstream file = std::ifstream(scene_filepath);

	this->scene_filepath = scene_filepath;
	this->scene_data = nlohmann::json::parse(file);

	this->scene_meshes.clear();
	this->scene_instances.clear();

	this->total_meshes = 0;
	this->total_instances = 0;
	this->total_triangles = 0;
	this->total_vertices = 0;

	this->rendered_meshes = 0;
	this->rendered_instances = 0;
	this->rendered_triangles = 0;
	this->rendered_vertices = 0;

	// Then translates the camera
	if (use_scene_camera_settings) {
		if (this->scene_data["camera"].contains("default_position")) {
			double x = this->scene_data["camera"]["default_position"]["x"].get<double>();
			double y = this->scene_data["camera"]["default_position"]["y"].get<double>();
			double z = this->scene_data["camera"]["default_position"]["z"].get<double>();

			default_camera_position.set(x, 1, 1);
			default_camera_position.set(y, 2, 1);
			default_camera_position.set(z, 3, 1);
		}

		// Loads scene camera parameters (position, direction, and orientation)
		if (this->scene_data["camera"].contains("translation")) {
			double tx = this->scene_data["camera"]["translation"]["x"].get<double>();
			double ty = this->scene_data["camera"]["translation"]["y"].get<double>();
			double tz = this->scene_data["camera"]["translation"]["z"].get<double>();

			camera_translation.set(tx, 1, 1);
			camera_translation.set(ty, 2, 1);
			camera_translation.set(tz, 3, 1);
		}
	}

	// Then rotates the camera
	if (use_scene_camera_settings) {

		if (this->scene_data["camera"].contains("default_direction")) {
			double default_x = this->scene_data["camera"]["default_direction"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_direction"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_direction"]["z"].get<double>();

			default_camera_direction.set(default_x, 1, 1);
			default_camera_direction.set(default_y, 2, 1);
			default_camera_direction.set(default_z, 3, 1);
		}

		if (this->scene_data["camera"].contains("default_up")) {
			double default_x = this->scene_data["camera"]["default_up"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_up"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_up"]["z"].get<double>();

			default_camera_up.set(default_x, 1, 1);
			default_camera_up.set(default_y, 2, 1);
			default_camera_up.set(default_z, 3, 1);
		}

		if (this->scene_data["camera"].contains("rotation")) {
			rotation_given = true;

			// Should be described in degrees

			if (this->scene_data["camera"]["rotation"].contains("y")) {
				camera_yaw = this->scene_data["camera"]["rotation"]["y"] * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("x")) {
				camera_pitch = this->scene_data["camera"]["rotation"]["x"] * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("z")) {
				camera_roll = this->scene_data["camera"]["rotation"]["z"] * (M_PI / 180);
			}
		}

		// If direction was given, update the direction vector
		if (this->scene_data["camera"].contains("direction")) {
			direction_given = true;

			double x = this->scene_data["camera"]["direction"]["x"].get<double>();
			double y = this->scene_data["camera"]["direction"]["y"].get<double>();
			double z = this->scene_data["camera"]["direction"]["z"].get<double>();

			new_camera_direction.set(x, 1, 1);
			new_camera_direction.set(y, 2, 1);
			new_camera_direction.set(z, 3, 1);
		}

		if (this->scene_data["camera"].contains("up")) {
			up_given = true;

			double x = this->scene_data["camera"]["up"]["x"].get<double>();
			double y = this->scene_data["camera"]["up"]["y"].get<double>();
			double z = this->scene_data["camera"]["up"]["z"].get<double>();

			new_camera_up.set(x, 1, 1);
			new_camera_up.set(y, 2, 1);
			new_camera_up.set(z, 3, 1);
		}
	}

	// Loads meshes and instances into the scene
	for (auto model = this->scene_data["models"].begin(); model != this->scene_data["models"].end(); model++) {
		const char* model_filename = model.key().c_str();
		char model_path[255];
		sprintf_s(model_path, 255, "%s%s", models_folder, model_filename);
		Mesh current_model = Mesh(model_path, model_filename, this->total_meshes);

		this->scene_meshes.push_back(std::move(current_model));

		for (auto instance = this->scene_data["models"][model_filename]["instances"].begin(); instance != this->scene_data["models"][model_filename]["instances"].end(); instance++) {
			const char* instance_name = instance.key().c_str();
			//printf("%s: %s\n", model_filename, instance_name);

			Mat translation = Mat::identity_matrix(4);
			Mat scale = Mat::identity_matrix(4);
			Mat rotation = Mat::identity_matrix(4);

			Quaternion orientation = Quaternion(0, 0, 0, 1);

			double translation_x = 0;
			double translation_y = 0;
			double translation_z = 0;

			double scale_x = 1;
			double scale_y = 1;
			double scale_z = 1;

			double yaw = 0;
			double pitch = 0;
			double roll = 0;

			bool has_translation = false;
			bool has_scaling = false;
			bool has_rotation = false;

			// Checks if translation has been given for this instance, updates translation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("translation")) {
				has_translation = true;
				translation_x = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["x"];
				translation_y = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["y"];
				translation_z = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["z"];

				translation = Mat::translation_matrix(translation_x, translation_y, translation_z);
			}

			// Checks if scale has been given for this instance, updates scale matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("scale")) {
				has_scaling = true;
				scale_x = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["x"];
				scale_y = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["y"];
				scale_z = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["z"];

				scale = Mat::scale_matrix(scale_x, scale_y, scale_z);
			}

			// Checks if rotation has been given for this instance, updates rotation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("rotation")) {
				has_rotation = true;
				yaw = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["y"] * (M_PI / 180);
				pitch = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["x"] * (M_PI / 180);
				roll = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["z"] * (M_PI / 180);

				orientation = Quaternion::FromYawPitchRoll(yaw, pitch, roll);
				rotation = orientation.get_rotationmatrix();
			}

			Mat MODEL_TO_WORLD = Mat::identity_matrix(4);
			bool has_model_to_world = false;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("model_to_world")) {

				const auto model_matrix_obj = this->scene_data["models"][model_filename]["instances"][instance_name]["model_to_world"];
				if (model_matrix_obj.size() == 4) {
					const auto first_row = model_matrix_obj[0];
					if (first_row.size() == 4) {
						has_model_to_world = true;

						MODEL_TO_WORLD = Mat(
							{
								{model_matrix_obj[0][0], model_matrix_obj[0][1], model_matrix_obj[0][2], model_matrix_obj[0][3]},
								{model_matrix_obj[1][0], model_matrix_obj[1][1], model_matrix_obj[1][2], model_matrix_obj[1][3]},
								{model_matrix_obj[2][0], model_matrix_obj[2][1], model_matrix_obj[2][2], model_matrix_obj[2][3]},
								{model_matrix_obj[3][0], model_matrix_obj[3][1], model_matrix_obj[3][2], model_matrix_obj[3][3]}
							}, 4, 4);
					}
					else {
						throw std::runtime_error("Model-to-world matrix of invalid size given for scene '" + std::string(scene_filename) + "', model '" + std::string(model_filename) + "', instance '" + std::string(instance_name) + "'. Must be of size 16 (for a 4x4 matrix), but parsed size is " + std::to_string(model_matrix_obj.size() * first_row.size()));
					}

				}
				else {
					throw std::runtime_error("Model-to-world matrix of invalid size given for scene '" + std::string(scene_filename) + "', model '" + std::string(model_filename) + "', instance '" + std::string(instance_name) + "'. Must be of size 16 (for a 4x4 matrix), but parsed size is " + std::to_string(model_matrix_obj.size() * model_matrix_obj[0].size()));
				}
			}
			else {
				MODEL_TO_WORLD = translation * scale * rotation;
			}

			bool show = true;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("show")) {
				show = this->scene_data["models"][model_filename]["instances"][instance_name]["show"].get<bool>();
			}

			Instance mesh_instance;

			// Loads from model-to-world if it's the only available information
			if (!has_translation && !has_scaling && !has_rotation && has_model_to_world) {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], MODEL_TO_WORLD, show, this->total_instances);
			}

			// Loads from parameters
			else {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], translation_x, translation_y, translation_z, yaw, pitch, roll, scale_x, scale_y, scale_z, show, this->total_instances);
			}

			this->scene_instances.push_back(std::move(mesh_instance));

			//std::cout << "Total instances: " << this->total_instances << " | Total meshes: " << this->total_meshes << std::endl;
			//std::cout << "-----------------------" << std::endl;


		}

	}
}


void Scene::save_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, const Mat& default_camera_position, const Mat& camera_position, const Mat& default_camera_direction, const Mat& camera_direction, const Mat& default_camera_up, const Mat& camera_up, double yaw, double pitch, double roll) {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scenes_folder, scene_filename);
	nlohmann::ordered_json json_object;

	json_object["scene"] = scene_filename;

	// The default camera position is considered to be the world coordinate in which the camera will be considered to be (0, 0, 0)
	json_object["camera"]["default_position"]["x"] = default_camera_position.get(1, 1);
	json_object["camera"]["default_position"]["y"] = default_camera_position.get(2, 1);
	json_object["camera"]["default_position"]["z"] = default_camera_position.get(3, 1);

	// Camera position relative to the default camera position
	json_object["camera"]["translation"]["x"] = camera_position.get(1, 1);
	json_object["camera"]["translation"]["y"] = camera_position.get(2, 1);
	json_object["camera"]["translation"]["z"] = camera_position.get(3, 1);

	json_object["camera"]["default_direction"]["x"] = default_camera_direction.get(1, 1);
	json_object["camera"]["default_direction"]["y"] = default_camera_direction.get(2, 1);
	json_object["camera"]["default_direction"]["z"] = default_camera_direction.get(3, 1);

	json_object["camera"]["direction"]["x"] = camera_direction.get(1, 1);
	json_object["camera"]["direction"]["y"] = camera_direction.get(2, 1);
	json_object["camera"]["direction"]["z"] = camera_direction.get(3, 1);

	json_object["camera"]["default_up"]["x"] = default_camera_up.get(1, 1);
	json_object["camera"]["default_up"]["y"] = default_camera_up.get(2, 1);
	json_object["camera"]["default_up"]["z"] = default_camera_up.get(3, 1);

	json_object["camera"]["up"]["x"] = camera_up.get(1, 1);
	json_object["camera"]["up"]["y"] = camera_up.get(2, 1);
	json_object["camera"]["up"]["z"] = camera_up.get(3, 1);

	json_object["camera"]["rotation"]["x"] = pitch * (180 / M_PI);
	json_object["camera"]["rotation"]["y"] = yaw * (180 / M_PI);
	json_object["camera"]["rotation"]["z"] = roll * (180 / M_PI);

	std::vector<std::string> models;
	std::vector<Instance*> instances;
	for (auto instance = this->scene_instances.begin(); instance != this->scene_instances.end(); instance++) {
		std::string mesh_filename = instance->mesh->mesh_filename;
		bool model_already_included = false;
		for (size_t model_n = 0; model_n < models.size(); model_n++) {
			std::string model_string = models[model_n];

			if (model_string == mesh_filename) {
				model_already_included = true;
				break;
			}
		}

		if (!model_already_included) {
			models.push_back(mesh_filename);
		}

		instances.push_back(&*instance);
	}

	for (auto model = models.begin(); model != models.end(); model++) {
		std::vector<std::string> txt_array;
		txt_array.push_back("instances");

		std::vector<std::string> instance_ids;
		for (size_t instance_id = 0; instance_id < this->total_instances; instance_id++) {
			Instance* instance = Scene::get_instance_ptr(instance_id);
			std::string mesh_filename = instance->mesh->mesh_filename;

			if (mesh_filename == *model) {
				json_object["models"][*model]["instances"][instance->instance_name]["show"] = instance->show;

				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["x"] = instance->tx;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["y"] = instance->ty;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["z"] = instance->tz;

				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["y"] = instance->yaw * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["x"] = instance->pitch * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["z"] = instance->roll * (180 / M_PI);

				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["x"] = instance->sx;
				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["y"] = instance->sy;
				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["z"] = instance->sz;

				json_object["models"][*model]["instances"][instance->instance_name]["model_to_world"] = std::vector<std::vector<double>>
				{
					{instance->MODEL_TO_WORLD.get(1, 1), instance->MODEL_TO_WORLD.get(1, 2), instance->MODEL_TO_WORLD.get(1, 3), instance->MODEL_TO_WORLD.get(1, 4)},
					{instance->MODEL_TO_WORLD.get(2, 1), instance->MODEL_TO_WORLD.get(2, 2), instance->MODEL_TO_WORLD.get(2, 3), instance->MODEL_TO_WORLD.get(2, 4)},
					{instance->MODEL_TO_WORLD.get(3, 1), instance->MODEL_TO_WORLD.get(3, 2), instance->MODEL_TO_WORLD.get(3, 3), instance->MODEL_TO_WORLD.get(3, 4)},
					{instance->MODEL_TO_WORLD.get(4, 1), instance->MODEL_TO_WORLD.get(4, 2), instance->MODEL_TO_WORLD.get(4, 3), instance->MODEL_TO_WORLD.get(4, 4)}
				};
			}
			else {
				continue;
			}
		}
	}

	std::cout << json_object.dump(4) << std::endl;
	std::ofstream file(scene_filepath);
	file << json_object.dump(4);
	file.close();
}

Mesh Scene::get_mesh(uint32_t mesh_id) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			return *current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

Mesh Scene::get_mesh(std::string mesh_filename) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			return *current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

Mesh* Scene::get_mesh_ptr(uint32_t mesh_id) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			return current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

Mesh* Scene::get_mesh_ptr(std::string mesh_filename) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			return current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

Instance Scene::get_instance(uint32_t instance_id) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			return *current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

Instance Scene::get_instance(std::string instance_name) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			return *current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}

Instance* Scene::get_instance_ptr(uint32_t instance_id) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			return current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

Instance* Scene::get_instance_ptr(std::string instance_name) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			return current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}
