#include "Scene.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// Default scene
Scene::Scene(const char* models_folder) {
	// Load transform axes mesh
	char axes_model_path[255];
	std::string axes_mesh_name = "axes.obj";
	sprintf_s(axes_model_path, 255, "%s%s", models_folder, axes_mesh_name.c_str());
	
	try {
		this->axes_mesh = Mesh(axes_model_path, axes_mesh_name.c_str(), this->total_ever_meshes);
	}
	catch (...) {
		this->load_error = SceneError::ModelLoad;
		this->errored_path = axes_model_path;
		return;
	}
	

	// Create instance if there is none
	Instance light_instance = Instance(this->total_ever_instances);
	light_instance.instance_id = this->total_ever_instances;
	light_instance.instance_name = "light_source";
	light_instance.is_light_source = true;

	
	this->scene_instances.push_back(std::move(light_instance));
	this->light_source.instance = &this->scene_instances.back();

	this->total_instances++;
}

Scene::Scene(const char* models_folder, Orientation rotation_orientation, bool verbose) {
	this->scene_meshes.clear();
	this->scene_instances.clear();

	this->total_ever_meshes = 0;
	this->total_meshes = 0;
	this->total_ever_instances = 0;
	this->total_instances = 0;
	this->total_triangles = 0;
	this->total_vertices = 0;

	this->rendered_meshes = 0;
	this->rendered_instances = 0;
	this->rendered_triangles = 0;
	this->rendered_vertices = 0;

	this->scene_filepath = "None";
	this->scene_data = nlohmann::ordered_json();

	char light_model_path[255];
	char cube_model_path[255];
	std::string light_mesh_name = "arrow.obj";
	std::string cube_mesh_name = "cube.obj";
	sprintf_s(light_model_path, 255, "%s%s", models_folder, light_mesh_name.c_str());
	sprintf_s(cube_model_path, 255, "%s%s", models_folder, cube_mesh_name.c_str());

	Mesh light_mesh = Mesh();

	try {
		light_mesh = Mesh(light_model_path, light_mesh_name.c_str(), this->total_ever_meshes);
	}
	catch (...) {
		this->load_error = SceneError::ModelLoad;
		this->errored_path = light_model_path;
		return;
	}

	this->total_instances++;
	this->scene_meshes.push_back(std::move(light_mesh));

	this->light_source = Light();

	this->light_source.lighting_type = LightType::directional;
	this->light_source.shading_type = ShadingType::Flat;
	this->light_source.mesh = &this->scene_meshes[0];
	this->light_source.tx = 0;
	this->light_source.ty = 0;
	this->light_source.tz = 0.5;
	this->light_source.yaw = 0;
	this->light_source.pitch = 0;
	this->light_source.roll = 0;
	this->light_source.has_model = true;

	Instance light_instance = Instance("light_source", rotation_orientation, &this->scene_meshes[0], this->light_source.tx, this->light_source.ty, this->light_source.tz, this->light_source.yaw, this->light_source.pitch, this->light_source.roll, 1, 1, 1, true, this->total_ever_instances);

	light_instance.is_light_source = true;
	this->light_source.instance = &light_instance;
	this->light_source.orientation = this->light_source.instance->orientation;

	// Assumes rotation relative to world space coordinate system
	Quaternion::RotatePoint(this->light_source.orientation, this->light_source.direction, false);
	Quaternion::RotatePoint(this->light_source.orientation, this->light_source.up, false);

	this->scene_instances.push_back(std::move(light_instance));
	this->total_instances++;

	Mesh cube_mesh = Mesh();

	try {
		cube_mesh = Mesh(cube_model_path, cube_mesh_name.c_str(), this->total_ever_meshes);
	}
	catch (...) {
		this->load_error = SceneError::ModelLoad;
		this->errored_path = cube_model_path;
		return;
	}

	this->scene_meshes.push_back(std::move(cube_mesh));
	this->total_meshes++;

	Instance cube_instance = Instance(rotation_orientation, &this->scene_meshes[1], 0, 0, 1, 0, 0, 0, 1, 1, 1, true, this->total_ever_instances);

	this->scene_instances.push_back(std::move(cube_instance));
	this->total_instances++;

	// Load transform axes mesh
	char axes_model_path[255];
	std::string axes_mesh_name = "axes.obj";
	sprintf_s(axes_model_path, 255, "%s%s", models_folder, axes_mesh_name.c_str());

	this->axes_mesh = Mesh(axes_model_path, axes_mesh_name.c_str(), this->total_ever_meshes);

	this->camera.VIEW_MATRIX = Camera::LookAt(this->camera.position, this->camera.direction, this->camera.up);
	this->camera.update_view_inverse();

	if (verbose) {
		std::cout << "Light dir: " << std::endl;
		this->light_source.direction.print();
		std::cout << "Light up: " << std::endl;
		this->light_source.up.print();

		std::cout << "Light yaw: " << this->light_source.yaw << std::endl;
		std::cout << "Light pitch: " << this->light_source.pitch << std::endl;
		std::cout << "Light roll: " << this->light_source.roll << std::endl;

		std::cout << "Camera position: " << std::endl;
		this->camera.position.print();

		std::cout << "Yaw: " << this->camera.yaw << std::endl;
		std::cout << "Pitch: " << this->camera.pitch << std::endl;
		std::cout << "Roll: " << this->camera.roll << std::endl;

		std::cout << "Camera direction: " << std::endl;
		this->camera.direction.print();

		std::cout << "Camera up: ";
		this->camera.up.print();
		std::cout << std::endl;

		std::cout << "View matrix: " << std::endl;
		this->camera.VIEW_MATRIX.print();
	}

	return;
}

Scene::Scene(const char* scene_folder, const char* scene_filename, const char* models_folder, Orientation rotation_orientation, bool update_camera_settings, bool verbose) {
	this->scene_meshes.clear();
	this->scene_instances.clear();

	this->total_ever_instances = 0;
	this->total_ever_meshes = 0;
	this->total_meshes = 0;
	this->total_instances = 0;
	this->total_triangles = 0;
	this->total_vertices = 0;

	this->rendered_meshes = 0;
	this->rendered_instances = 0;
	this->rendered_triangles = 0;
	this->rendered_vertices = 0;

	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scene_folder, scene_filename);
	std::ifstream file = std::ifstream(scene_filepath);

	// Loads default cube scene if could not load the given file
	if (file.fail()) {
		//printf("Error: Could not open scene file '%s' at path '%s'.\n", scene_filename, scene_filepath);
		//printf("Defaulting to the cube scene.\n");
		*this = Scene(models_folder);
		this->load_error = SceneError::SceneLoad;
		this->errored_path = scene_filepath;
		return;
		//throw std::runtime_error("Error: Could not load scene.");
	}

	this->scene_filepath = scene_filepath;

	try {
		this->scene_data = nlohmann::ordered_json::parse(file);
	}
	catch (...) {
		this->load_error = SceneError::JSONParsing;
		this->errored_path = this->scene_filepath;
		return;
	}

	bool rotation_given = false;
	bool direction_given = false;
	bool up_given = false;

	// Then translates the camera
	if (update_camera_settings && this->scene_data.contains("camera")) {
		if (this->scene_data["camera"].contains("default_position")) {
			double x = this->scene_data["camera"]["default_position"]["x"].get<double>();
			double y = this->scene_data["camera"]["default_position"]["y"].get<double>();
			double z = this->scene_data["camera"]["default_position"]["z"].get<double>();

			this->camera.default_position = Mat(
				{ 
					{x},
					{y},
					{z},
					{1}
				}, 4, 1);
		}

		// Loads scene camera parameters (position, direction, and orientation)
		if (this->scene_data["camera"].contains("translation")) {
			double tx = this->scene_data["camera"]["translation"]["x"].get<double>();
			double ty = this->scene_data["camera"]["translation"]["y"].get<double>();
			double tz = this->scene_data["camera"]["translation"]["z"].get<double>();

			this->camera.position = Mat(
				{ 
					{this->camera.default_position.get(1, 1) + tx},
					{this->camera.default_position.get(2, 1) + ty},
					{this->camera.default_position.get(3, 1) + tz},
					{1}
				}, 4, 1);
		}
	}

	// Then rotates the camera
	if (update_camera_settings) {

		if (this->scene_data["camera"].contains("default_direction")) {
			double default_x = this->scene_data["camera"]["default_direction"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_direction"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_direction"]["z"].get<double>();

			this->camera.default_direction = Mat(
				{
					{default_x},
					{default_y},
					{default_z},
					{0}
				}, 4, 1);
		}

		if (this->scene_data["camera"].contains("default_up")) {
			double default_x = this->scene_data["camera"]["default_up"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_up"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_up"]["z"].get<double>();

			this->camera.default_up = Mat(
				{
					{default_x},
					{default_y},
					{default_z},
					{0}
				}, 4, 1);
		}

		if (this->scene_data["camera"].contains("rotation")) {
			rotation_given = true;

			// Should be described in degrees

			if (this->scene_data["camera"]["rotation"].contains("y")) {
				this->camera.yaw = this->scene_data["camera"]["rotation"]["y"].get<double>() * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("x")) {
				this->camera.pitch = this->scene_data["camera"]["rotation"]["x"].get<double>() * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("z")) {
				this->camera.roll = this->scene_data["camera"]["rotation"]["z"].get<double>() * (M_PI / 180);
			}
		}

		// If direction was given, update the direction vector
		if (this->scene_data["camera"].contains("direction")) {
			direction_given = true;

			double x = this->scene_data["camera"]["direction"]["x"].get<double>();
			double y = this->scene_data["camera"]["direction"]["y"].get<double>();
			double z = this->scene_data["camera"]["direction"]["z"].get<double>();

			this->camera.direction = Mat(
				{
					{x},
					{y},
					{z},
					{0}
				}, 4, 1);
		}

		if (this->scene_data["camera"].contains("up")) {
			up_given = true;

			double x = this->scene_data["camera"]["up"]["x"].get<double>();
			double y = this->scene_data["camera"]["up"]["y"].get<double>();
			double z = this->scene_data["camera"]["up"]["z"].get<double>();

			this->camera.up = Mat(
				{
					{x},
					{y},
					{z},
					{0}
				}, 4, 1);
		}
	}

	if (this->scene_data.contains("bg_color")) {
		uint8_t red = this->scene_data["bg_color"]["r"].get<uint8_t>();
		uint8_t green = this->scene_data["bg_color"]["g"].get<uint8_t>();
		uint8_t blue = this->scene_data["bg_color"]["b"].get<uint8_t>();

		uint32_t BG_COLOR = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
		this->BG_COLOR = BG_COLOR;
	}

	if (this->scene_data.contains("line_color")) {
		uint8_t red = this->scene_data["line_color"]["r"].get<uint8_t>();
		uint8_t green = this->scene_data["line_color"]["g"].get<uint8_t>();
		uint8_t blue = this->scene_data["line_color"]["b"].get<uint8_t>();

		uint32_t LINE_COLOR = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
		this->LINE_COLOR = LINE_COLOR;
	}

	if (this->scene_data.contains("fill_color")) {
		uint8_t red = this->scene_data["fill_color"]["r"].get<uint8_t>();
		uint8_t green = this->scene_data["fill_color"]["g"].get<uint8_t>();
		uint8_t blue = this->scene_data["fill_color"]["b"].get<uint8_t>();

		uint32_t FILL_COLOR = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
		this->FILL_COLOR = FILL_COLOR;
	}

	bool light_given = false;
	bool light_rotation_given = false;
	bool light_direction_given = false;
	bool light_up_given = false;

	if (this->scene_data.contains("light")) {
		light_given = true;

		if (this->scene_data["light"].contains("model")) {
			this->light_source.has_model = true;
			char model_path[255];
			std::string light_mesh_name = this->scene_data["light"]["model"].get<std::string>();
			sprintf_s(model_path, 255, "%s%s", models_folder, light_mesh_name.c_str());

			Mesh light_mesh = Mesh();

			try {
				light_mesh = Mesh(model_path, light_mesh_name.c_str(), this->total_ever_meshes);
			}
			catch (...) {
				this->load_error = SceneError::ModelLoad;
				this->errored_path = model_path;
				return;
			}

			this->scene_meshes.push_back(std::move(light_mesh));
			this->total_meshes++;

			Instance light_instance = Instance("light_source", rotation_orientation, &this->scene_meshes[0], 0, 0, 0.5, 0, 0, 0, 1, 1, 1, true, this->total_ever_instances);

			this->total_instances++;
			light_instance.is_light_source = true;
			
			this->scene_instances.push_back(std::move(light_instance));

			this->light_source.mesh = &this->scene_meshes[0];
			this->light_source.instance = &this->scene_instances[0];
			this->light_source.tz = this->light_source.instance->tz;
		}

		if (this->scene_data["light"].contains("type")) {
			std::string light_type = this->scene_data["light"]["type"].get<std::string>();

			if (light_type == "point") {
				this->light_source.lighting_type = LightType::point;
			}
			else if (light_type == "directional") {
				this->light_source.lighting_type = LightType::directional;
			}
			else if (light_type == "spotlight") {
				this->light_source.lighting_type = LightType::spotlight;
			}
			else {
				std::cout << "Invalid light source type: '" << this->light_source.lighting_type << "', must be 'point', 'directional', or 'spotlight'." << std::endl;
				throw std::runtime_error("Invalid light source type.");
				exit(-1);
			}
			
		}

		if (this->scene_data["light"].contains("color")) {
			uint8_t red = this->scene_data["light"]["color"]["r"].get<uint8_t>();
			uint8_t green = this->scene_data["light"]["color"]["g"].get<uint8_t>();
			uint8_t blue = this->scene_data["light"]["color"]["b"].get<uint8_t>();

			uint32_t light_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
			this->light_source.color = light_color;
		}

		if (this->scene_data["light"].contains("enabled")) {
			this->light_source.enabled = this->scene_data["light"]["enabled"].get<bool>();
		}

		if (this->scene_data["light"].contains("intensity")) {
			this->light_source.intensity = this->scene_data["light"]["intensity"].get<double>();
		}

		if (this->scene_data["light"].contains("minimum_exposure")) {
			this->light_source.minimum_exposure = this->scene_data["light"]["minimum_exposure"].get<double>();
		}

		if (this->scene_data["light"].contains("position")) {
			double tx = this->scene_data["light"]["position"]["x"].get<double>();
			double ty = this->scene_data["light"]["position"]["y"].get<double>();
			double tz = this->scene_data["light"]["position"]["z"].get<double>();

			this->light_source.tx = tx;
			this->light_source.ty = ty;
			this->light_source.tz = tz;

			Mat position_vector = Mat({ {tx}, {ty}, {tz}, {1} }, 4, 1);

			this->light_source.position = position_vector;

			if (this->light_source.has_model) {
				this->light_source.instance->tx = tx;
				this->light_source.instance->ty = ty;
				this->light_source.instance->tz = tz;
				this->light_source.instance->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
				this->light_source.instance->MODEL_TO_WORLD = this->light_source.instance->TRANSLATION_MATRIX * this->light_source.instance->ROTATION_MATRIX * this->light_source.instance->SCALING_MATRIX;
			}
		}


		//bool light_rotation_given = false;
		//bool light_direction_given = false;
		double light_yaw = 0;
		double light_pitch = 0;
		double light_roll = 0;
		if (this->scene_data["light"].contains("rotation")) {
			light_rotation_given = true;

			// Should be described in degrees

			if (this->scene_data["light"]["rotation"].contains("y")) {
				this->light_source.yaw = this->scene_data["light"]["rotation"]["y"].get<double>() * (M_PI / 180);
			}

			if (this->scene_data["light"]["rotation"].contains("x")) {
				this->light_source.pitch = this->scene_data["light"]["rotation"]["x"].get<double>() * (M_PI / 180);
			}

			if (this->scene_data["light"]["rotation"].contains("z")) {
				this->light_source.roll = this->scene_data["light"]["rotation"]["z"].get<double>() * (M_PI / 180);
			}
		}

		if (this->scene_data["light"].contains("direction")) {
			light_direction_given = true;

			double x = this->scene_data["light"]["direction"]["x"].get<double>();
			double y = this->scene_data["light"]["direction"]["y"].get<double>();
			double z = this->scene_data["light"]["direction"]["z"].get<double>();

			this->light_source.direction.set(x, 1, 1);
			this->light_source.direction.set(y, 2, 1);
			this->light_source.direction.set(z, 3, 1);
			this->light_source.direction.set(0, 4, 1);
		}

		if (this->scene_data["light"].contains("up")) {
			light_up_given = true;

			double x = this->scene_data["light"]["up"]["x"].get<double>();
			double y = this->scene_data["light"]["up"]["y"].get<double>();
			double z = this->scene_data["light"]["up"]["z"].get<double>();

			this->light_source.up.set(x, 1, 1);
			this->light_source.up.set(y, 2, 1);
			this->light_source.up.set(z, 3, 1);
			this->light_source.up.set(0, 4, 1);
		}

		if (this->light_source.has_model && this->scene_data["light"].contains("scale")) {
			double sx = this->scene_data["light"]["scale"]["x"].get<double>();
			double sy = this->scene_data["light"]["scale"]["y"].get<double>();
			double sz = this->scene_data["light"]["scale"]["z"].get<double>();

			this->light_source.instance->sx = sx;
			this->light_source.instance->sy = sy;
			this->light_source.instance->sz = sz;

			this->light_source.instance->SCALING_MATRIX = Mat::scale_matrix(this->light_source.instance->sx, this->light_source.instance->sy, this->light_source.instance->sz);
			this->light_source.instance->MODEL_TO_WORLD = this->light_source.instance->TRANSLATION_MATRIX * this->light_source.instance->ROTATION_MATRIX * this->light_source.instance->SCALING_MATRIX;
		}
	}

	if (!light_given) {
		this->light_source = Light();
		this->light_source.enabled = false;

		if (this->light_source.has_model) {
			this->light_source.instance->show = false;
		}
	}

	// Updates camera position
	this->camera.default_right = Mat::CrossProduct3D(this->camera.default_up, this->camera.default_direction);

	Mat camera_right = this->camera.default_right;

	if (direction_given || up_given) {
		camera_right = Mat::CrossProduct3D(this->camera.up, this->camera.direction);
	}

	// If no rotation parameters were given, derive them from the direction vector
	if (direction_given && !rotation_given) {
		// Gets yaw and pitch representing the rotation from the default camera direction to the new camera direction
		Quaternion::GetAnglesFromDirection(Orientation::local, this->camera.default_direction, this->camera.direction, this->camera.yaw, this->camera.pitch, this->camera.roll);
	}

	if ((up_given && !rotation_given)) {
		// Gets roll representing the rotation from the default camera up to the camera up
		// Assumes default direction of (0, 0, 1)
		Quaternion::GetRoll(Orientation::local, this->camera.direction, this->camera.up, this->camera.yaw, this->camera.pitch, this->camera.roll);
	}

	this->camera.orientation = Quaternion::FromYawPitchRoll(Orientation::local, this->camera.yaw, this->camera.pitch, this->camera.roll, default_world_right, default_world_up, default_world_forward);

	// If direction was not given, but rotation was - rotates the default camera direction vectors by the rotation parameters
	if ((!direction_given || !up_given) && rotation_given) {
		this->camera.direction = this->camera.default_direction;
		this->camera.up = this->camera.default_up;

		Quaternion::RotatePoint(this->camera.orientation, this->camera.direction, false);
		Quaternion::RotatePoint(this->camera.orientation, this->camera.up, false);

		camera_right = Mat::CrossProduct3D(this->camera.up, this->camera.direction);
		/*
		this->camera.direction = Quaternion::RotatePoint(this->camera.direction, this->camera.up, this->camera.yaw, false);
		camera_right = Quaternion::RotatePoint(camera_right, this->camera.up, this->camera.yaw, false);
		this->camera.direction = Quaternion::RotatePoint(this->camera.direction, camera_right, this->camera.pitch, false);
		this->camera.up = Quaternion::RotatePoint(this->camera.up, camera_right, this->camera.pitch, false);
		this->camera.up = Quaternion::RotatePoint(this->camera.up, this->camera.direction, this->camera.roll, false);
		*/
	}

	std::cout << "R0: " << this->light_source.roll << std::endl;

	if (light_direction_given && !light_rotation_given) {
		Quaternion::GetAnglesFromDirection(rotation_orientation, this->light_source.default_direction, this->light_source.direction, this->light_source.yaw, this->light_source.pitch, this->light_source.roll);
	}


	if (light_up_given && !light_rotation_given) {
		// Gets roll representing the rotation from the default camera up to the camera up
		// Assumes default direction of (0, 0, 1)
		Quaternion::GetRoll(rotation_orientation, this->light_source.direction, this->light_source.up, this->light_source.yaw, this->light_source.pitch, this->light_source.roll);
	}

	this->light_source.orientation = Quaternion::FromYawPitchRoll(rotation_orientation, this->light_source.yaw, this->light_source.pitch, this->light_source.roll, default_world_right, default_world_up, default_world_forward);

	if ((!light_direction_given || !light_up_given) && light_rotation_given) {
		this->light_source.direction = this->light_source.default_direction;
		this->light_source.up = this->light_source.default_up;

		Quaternion::RotatePoint(this->light_source.orientation, this->light_source.direction, false);
		Quaternion::RotatePoint(this->light_source.orientation, this->light_source.up, false);
	}

	if (this->light_source.has_model) {
		this->light_source.instance->yaw = this->light_source.yaw;
		this->light_source.instance->pitch = this->light_source.pitch;
		this->light_source.instance->roll = this->light_source.roll;
		this->light_source.instance->orientation = this->light_source.orientation;
		this->light_source.instance->ROTATION_MATRIX = this->light_source.instance->orientation.get_rotationmatrix();
		this->light_source.instance->MODEL_TO_WORLD = this->light_source.instance->TRANSLATION_MATRIX * this->light_source.instance->ROTATION_MATRIX * this->light_source.instance->SCALING_MATRIX;
	}

	this->camera.VIEW_MATRIX = Camera::LookAt(this->camera.position, this->camera.direction, this->camera.up);
	this->camera.update_view_inverse();

	// Loads meshes and instances into the scene
	for (auto model = this->scene_data["models"].begin(); model != this->scene_data["models"].end(); model++) {
		const char* model_filename = model.key().c_str();
		char model_path[255];
		sprintf_s(model_path, 255, "%s%s", models_folder, model_filename);
		Mesh current_model = Mesh();

		try {
			current_model = Mesh(model_path, model_filename, this->total_ever_meshes);
		}
		catch (...) {
			this->load_error = SceneError::ModelLoad;
			this->errored_path = model_path;
			return;
		}

		this->scene_meshes.push_back(std::move(current_model));
		this->total_meshes++;

		

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
				translation_x = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["x"].get<double>();
				translation_y = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["y"].get<double>();
				translation_z = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["z"].get<double>();

				translation = Mat::translation_matrix(translation_x, translation_y, translation_z);
			}

			// Checks if scale has been given for this instance, updates scale matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("scale")) {
				has_scaling = true;
				scale_x = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["x"].get<double>();
				scale_y = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["y"].get<double>();
				scale_z = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["z"].get<double>();

				scale = Mat::scale_matrix(scale_x, scale_y, scale_z);
			}

			// Checks if rotation has been given for this instance, updates rotation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("rotation")) {
				has_rotation = true;
				yaw = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["y"].get<double>() * (M_PI / 180);
				pitch = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["x"].get<double>() * (M_PI / 180);
				roll = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["z"].get<double>() * (M_PI / 180);

				orientation = Quaternion::FromYawPitchRoll(rotation_orientation, yaw, pitch, roll, this->default_world_right, this->default_world_up, this->default_world_forward);
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
				MODEL_TO_WORLD = translation * rotation * scale;
			}

			bool show = true;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("show")) {
				show = this->scene_data["models"][model_filename]["instances"][instance_name]["show"].get<bool>();
			}

			Instance mesh_instance;

			// Loads from model-to-world if it's the only available information
			if (!has_translation && !has_scaling && !has_rotation && has_model_to_world) {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], MODEL_TO_WORLD, show, this->total_ever_instances);
			}

			// Loads from parameters
			else {
				mesh_instance = Instance(instance_name, rotation_orientation, &this->scene_meshes[this->total_meshes - 1], translation_x, translation_y, translation_z, yaw, pitch, roll, scale_x, scale_y, scale_z, show, this->total_ever_instances);
			}

			this->scene_instances.push_back(std::move(mesh_instance));
			this->total_instances++;

			//std::cout << "Total instances: " << this->total_instances << " | Total meshes: " << this->total_meshes << std::endl;
			//std::cout << "-----------------------" << std::endl;


		}

	}

	// Load transform axes mesh
	char axes_model_path[255];
	std::string axes_mesh_name = "axes.obj";
	sprintf_s(axes_model_path, 255, "%s%s", models_folder, axes_mesh_name.c_str());


	this->axes_mesh = Mesh(axes_model_path, axes_mesh_name.c_str(), this->total_ever_meshes);

	// Logs camera/view information if enabled (might also include other scene relevant information such as number of meshes, instances, vertices, etc...)
	if (verbose) {
		std::cout << "Light dir: " << std::endl;
		this->light_source.direction.print();
		std::cout << "Light up: " << std::endl;
		this->light_source.up.print();

		std::cout << "Light yaw: " << this->light_source.yaw << std::endl;
		std::cout << "Light pitch: " << this->light_source.pitch << std::endl;
		std::cout << "Light roll: " << this->light_source.roll << std::endl;

		std::cout << "Camera position: " << std::endl;
		this->camera.position.print();

		std::cout << "Yaw: " << this->camera.yaw << std::endl;
		std::cout << "Pitch: " << this->camera.pitch << std::endl;
		std::cout << "Roll: " << this->camera.roll << std::endl;

		std::cout << "Camera direction: " << std::endl;
		this->camera.direction.print();

		std::cout << "Camera up: ";
		this->camera.up.print();
		std::cout << std::endl;

		std::cout << "View matrix: " << std::endl;
		this->camera.VIEW_MATRIX.print();
	}
}

Scene& Scene::operator=(Scene&& original_scene) noexcept {
	if (this != &original_scene) {
		std::swap(this->total_ever_instances, original_scene.total_ever_instances);
		std::swap(this->total_ever_meshes, original_scene.total_ever_meshes);

		std::swap(this->total_meshes, original_scene.total_meshes);
		std::swap(this->total_instances, original_scene.total_instances);
		std::swap(this->total_triangles, original_scene.total_triangles);
		std::swap(this->total_vertices, original_scene.total_vertices);

		std::swap(this->rendered_meshes, original_scene.rendered_meshes);
		std::swap(this->rendered_instances, original_scene.rendered_instances);
		std::swap(this->rendered_triangles, original_scene.rendered_triangles);
		std::swap(this->rendered_vertices, original_scene.rendered_vertices);

		std::swap(this->scene_meshes, original_scene.scene_meshes);
		std::swap(this->scene_instances, original_scene.scene_instances);

		std::swap(this->scene_filepath, original_scene.scene_filepath);
		std::swap(this->scene_data, original_scene.scene_data);

		std::swap(this->default_world_up, original_scene.default_world_up);
		std::swap(this->default_world_right, original_scene.default_world_right);
		std::swap(this->default_world_forward, original_scene.default_world_forward);

		std::swap(this->camera, original_scene.camera);

		std::swap(this->light_source, original_scene.light_source);

		std::swap(this->axes_instance, original_scene.axes_instance);
		std::swap(this->axes_mesh, original_scene.axes_mesh);

		std::swap(this->load_error, original_scene.load_error);
		std::swap(this->errored_path, original_scene.errored_path);

		std::swap(this->BG_COLOR, original_scene.BG_COLOR);
		std::swap(this->LINE_COLOR, original_scene.LINE_COLOR);
		std::swap(this->FILL_COLOR, original_scene.FILL_COLOR);
	}

	return *this;
}

void Scene::save(const char* scene_folder, const char* scene_filename) const {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scene_folder, scene_filename);
	nlohmann::ordered_json json_object;

	json_object["scene"] = scene_filename;

	// The default camera position is considered to be the world coordinate in which the camera will be considered to be (0, 0, 0)
	json_object["camera"]["default_position"]["x"] = this->camera.default_position.get(1, 1);
	json_object["camera"]["default_position"]["y"] = this->camera.default_position.get(2, 1);
	json_object["camera"]["default_position"]["z"] = this->camera.default_position.get(3, 1);

	// Camera position relative to the default camera position
	json_object["camera"]["translation"]["x"] = this->camera.position.get(1, 1);
	json_object["camera"]["translation"]["y"] = this->camera.position.get(2, 1);
	json_object["camera"]["translation"]["z"] = this->camera.position.get(3, 1);

	json_object["camera"]["default_direction"]["x"] = this->camera.default_direction.get(1, 1);
	json_object["camera"]["default_direction"]["y"] = this->camera.default_direction.get(2, 1);
	json_object["camera"]["default_direction"]["z"] = this->camera.default_direction.get(3, 1);

	json_object["camera"]["direction"]["x"] = this->camera.direction.get(1, 1);
	json_object["camera"]["direction"]["y"] = this->camera.direction.get(2, 1);
	json_object["camera"]["direction"]["z"] = this->camera.direction.get(3, 1);

	json_object["camera"]["default_up"]["x"] = this->camera.default_up.get(1, 1);
	json_object["camera"]["default_up"]["y"] = this->camera.default_up.get(2, 1);
	json_object["camera"]["default_up"]["z"] = this->camera.default_up.get(3, 1);

	json_object["camera"]["up"]["x"] = this->camera.up.get(1, 1);
	json_object["camera"]["up"]["y"] = this->camera.up.get(2, 1);
	json_object["camera"]["up"]["z"] = this->camera.up.get(3, 1);

	json_object["camera"]["rotation"]["x"] = this->camera.pitch * (180 / M_PI);
	json_object["camera"]["rotation"]["y"] = this->camera.yaw * (180 / M_PI);
	json_object["camera"]["rotation"]["z"] = this->camera.roll * (180 / M_PI);

	json_object["bg_color"]["r"] = (uint8_t)(this->BG_COLOR >> 24);
	json_object["bg_color"]["g"] = (uint8_t)(this->BG_COLOR >> 16);
	json_object["bg_color"]["b"] = (uint8_t)(this->BG_COLOR >> 8);

	json_object["line_color"]["r"] = (uint8_t)(this->LINE_COLOR >> 24);
	json_object["line_color"]["g"] = (uint8_t)(this->LINE_COLOR >> 16);
	json_object["line_color"]["b"] = (uint8_t)(this->LINE_COLOR >> 8);

	json_object["fill_color"]["r"] = (uint8_t)(this->FILL_COLOR >> 24);
	json_object["fill_color"]["g"] = (uint8_t)(this->FILL_COLOR >> 16);
	json_object["fill_color"]["b"] = (uint8_t)(this->FILL_COLOR >> 8);

	if (this->light_source.lighting_type == LightType::point) {
		json_object["light"]["type"] = "point";
	}
	else if (this->light_source.lighting_type == LightType::directional) {
		json_object["light"]["type"] = "directional";
	}
	else if (this->light_source.lighting_type == LightType::spotlight) {
		json_object["light"]["type"] = "spotlight";
	}

	json_object["light"]["enabled"] = this->light_source.enabled;
	json_object["light"]["color"]["r"] = (uint8_t)(this->light_source.color >> 24);
	json_object["light"]["color"]["g"] = (uint8_t)(this->light_source.color >> 16);
	json_object["light"]["color"]["b"] = (uint8_t)(this->light_source.color >> 8);

	json_object["light"]["intensity"] = this->light_source.intensity;
	json_object["light"]["minimum_exposure"] = this->light_source.minimum_exposure;

	if (this->light_source.has_model) {
		json_object["light"]["model"] = this->light_source.mesh->mesh_filename;
	}

	json_object["light"]["position"]["x"] = this->light_source.tx;
	json_object["light"]["position"]["y"] = this->light_source.ty;
	json_object["light"]["position"]["z"] = this->light_source.tz;

	json_object["light"]["rotation"]["x"] = this->light_source.pitch * (180 / M_PI);
	json_object["light"]["rotation"]["y"] = this->light_source.yaw * (180 / M_PI);
	json_object["light"]["rotation"]["z"] = this->light_source.roll * (180 / M_PI);

	json_object["light"]["direction"]["x"] = this->light_source.direction.get(1, 1);
	json_object["light"]["direction"]["y"] = this->light_source.direction.get(2, 1);
	json_object["light"]["direction"]["z"] = this->light_source.direction.get(3, 1);

	json_object["light"]["up"]["x"] = this->light_source.up.get(1, 1);
	json_object["light"]["up"]["y"] = this->light_source.up.get(2, 1);
	json_object["light"]["up"]["z"] = this->light_source.up.get(3, 1);

	if (this->light_source.has_model) {
		json_object["light"]["scale"]["x"] = this->light_source.instance->sx;
		json_object["light"]["scale"]["y"] = this->light_source.instance->sy;
		json_object["light"]["scale"]["z"] = this->light_source.instance->sz;
	}

	std::vector<std::string> models;
	for (auto instance = this->scene_instances.begin(); instance != this->scene_instances.end(); instance++) {
		if (instance->is_light_source) continue;

		std::string mesh_filename = instance->mesh->mesh_filename;

		// Ignore if instance represents light source
		if (mesh_filename == this->light_source.mesh->mesh_filename) continue;

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
	}

	for (auto model = models.begin(); model != models.end(); model++) {
		std::vector<std::string> txt_array;
		txt_array.push_back("instances");

		std::vector<std::string> instance_ids;
		for (size_t n_instance = 0; n_instance < this->total_instances; n_instance++) {
			const Instance* instance = &this->scene_instances[n_instance];
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

bool Scene::get_mesh(uint32_t mesh_id, Mesh& mesh) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		const Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			mesh = *current_mesh;
			return true;
		}
	}

	return false;
	//throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

bool Scene::get_mesh(std::string mesh_filename, Mesh& mesh) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		const Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			mesh = *current_mesh;
			return true;
		}
	}

	return false;
	//throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

Mesh* Scene::get_mesh_ptr(uint32_t mesh_id) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			return current_mesh;
		}
	}

	return nullptr;
	//throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

Mesh* Scene::get_mesh_ptr(std::string mesh_filename) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			return current_mesh;
		}
	}

	return nullptr;
	//throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

bool Scene::get_instance(uint32_t instance_id, Instance& instance) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		const Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			instance = *current_instance;
			return true;
		}
	}

	return false;
	//throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

bool Scene::get_instance(std::string instance_name, Instance& instance) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		const Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			instance = *current_instance;
			return true;
		}
	}

	return false;
	//throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}

Instance* Scene::get_instance_ptr(uint32_t instance_id) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			return current_instance;
		}
	}

	return nullptr;
	//throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

Instance* Scene::get_instance_ptr(std::string instance_name) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			return current_instance;
		}
	}

	return nullptr;
	//throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}