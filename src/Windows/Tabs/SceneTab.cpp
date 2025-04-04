#include "SceneTab.h"
#include "CameraTab.h"
#include "InstancesTab.h"
#include "LightTab.h"

SceneTab::SceneTab() {
	this->camera_tab = new CameraTab(this);
	this->instances_tab = new InstancesTab(this);
	this->light_tab = new LightTab(this);
}

void SceneTab::initialize() {
	/*
	this->camera_tab.initialize(this, &this->current_scene);
	this->instances_tab.initialize(&this->current_scene, &this->rotation_orientation, this->models_folder, &this->show_transform_axes);
	this->light_tab.initialize(&this->current_scene, &this->rotation_orientation, this->models_folder, &this->show_transform_axes);

	this->camera_tab.menu_translation_speed = &this->menu_translation_speed;
	this->camera_tab.menu_rotation_speed = &this->menu_rotation_speed;
	this->camera_tab.real_rotation_speed = &this->real_rotation_speed;
	this->camera_tab.real_translation_speed = &this->real_translation_speed;

	this->instances_tab.menu_translation_speed = &this->menu_translation_speed;
	this->instances_tab.menu_rotation_speed = &this->menu_rotation_speed;
	this->instances_tab.menu_scaling_speed = &this->menu_scaling_speed;

	this->light_tab.menu_translation_speed = &this->menu_translation_speed;
	this->light_tab.menu_rotation_speed = &this->menu_rotation_speed;
	this->light_tab.menu_scaling_speed = &this->menu_scaling_speed;
	*/
}

void SceneTab::load_scene(const char* scene_folder, const char* scene_filename, const char* models_folder, bool update_camera_settings, bool verbose) {
	/*
	strcpy_s(this->scene_folder, sizeof(this->scene_folder), scene_folder);
	strcpy_s(this->scene_load_name, sizeof(this->scene_load_name), scene_filename);
	strcpy_s(this->scene_save_name, sizeof(this->scene_save_name), scene_filename);
	strcpy_s(this->models_folder, sizeof(this->models_folder), models_folder);
	*/

	double old_AR = this->current_scene.camera.AR;
	double old_near = this->current_scene.camera.near;
	double old_far = this->current_scene.camera.far;
	double old_FOV = this->current_scene.camera.FOV;
	double old_FOVr = this->current_scene.camera.FOVr;
	Mat old_PROJECTION_MATRIX = this->current_scene.camera.PROJECTION_MATRIX;
	Mat old_SCALE_MATRIX = this->current_scene.camera.SCALE_MATRIX;

	delete camera_tab;
	delete instances_tab;
	delete light_tab;

	this->current_scene = Scene(scene_folder, scene_filename, models_folder, this->rotation_orientation, update_camera_settings, verbose);

	this->current_scene.camera.AR = old_AR;
	this->current_scene.camera.near = old_near;
	this->current_scene.camera.far = old_far;
	this->current_scene.camera.FOV = old_FOV;
	this->current_scene.camera.FOVr = old_FOVr;
	this->current_scene.camera.PROJECTION_MATRIX = old_PROJECTION_MATRIX;
	this->current_scene.camera.SCALE_MATRIX = old_SCALE_MATRIX;

	camera_tab = new CameraTab(this);
	instances_tab = new InstancesTab(this);
	light_tab = new LightTab(this);

	//instances_tab->initialize(&this->current_scene, &this->rotation_orientation, this->models_folder, &this->show_transform_axes);
	//light_tab->initialize(&this->current_scene, &this->rotation_orientation, this->models_folder, &this->show_transform_axes);
}

void SceneTab::load_scene() {
	double old_AR = this->current_scene.camera.AR;
	double old_near = this->current_scene.camera.near;
	double old_far = this->current_scene.camera.far;
	double old_FOV = this->current_scene.camera.FOV;
	double old_FOVr = this->current_scene.camera.FOVr;
	Mat old_PROJECTION_MATRIX = this->current_scene.camera.PROJECTION_MATRIX;
	Mat old_SCALE_MATRIX = this->current_scene.camera.SCALE_MATRIX;

	delete light_tab;
	delete instances_tab;
	delete camera_tab;

	this->current_scene = Scene(this->scene_folder, this->scene_load_name, this->models_folder, this->rotation_orientation, this->update_camera_settings, this->verbose);

	this->display_BG_COLOR[0] = static_cast<float>(((this->current_scene.BG_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[1] = static_cast<float>(((this->current_scene.BG_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[2] = static_cast<float>(((this->current_scene.BG_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[3] = static_cast<float>((this->current_scene.BG_COLOR & 0x000000FF) / 255.0);

	this->display_LINE_COLOR[0] = static_cast<float>(((this->current_scene.LINE_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[1] = static_cast<float>(((this->current_scene.LINE_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[2] = static_cast<float>(((this->current_scene.LINE_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[3] = static_cast<float>((this->current_scene.LINE_COLOR & 0x000000FF) / 255.0);

	this->display_FILL_COLOR[0] = static_cast<float>(((this->current_scene.FILL_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[1] = static_cast<float>(((this->current_scene.FILL_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[2] = static_cast<float>(((this->current_scene.FILL_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[3] = static_cast<float>((this->current_scene.FILL_COLOR & 0x000000FF) / 255.0);

	this->current_scene.camera.AR = old_AR;
	this->current_scene.camera.near = old_near;
	this->current_scene.camera.far = old_far;
	this->current_scene.camera.FOV = old_FOV;
	this->current_scene.camera.FOVr = old_FOVr;
	this->current_scene.camera.PROJECTION_MATRIX = old_PROJECTION_MATRIX;
	this->current_scene.camera.SCALE_MATRIX = old_SCALE_MATRIX;

	camera_tab = new CameraTab(this);
	instances_tab = new InstancesTab(this);
	light_tab = new LightTab(this);

	if (this->is_instances_open) {
		this->instances_tab->update_transform_axes();
	}
	else if (this->is_light_open) {
		this->light_tab->update_transform_axes();
	}
}

void SceneTab::load_empty_scene() {
	delete light_tab;
	delete instances_tab;
	delete camera_tab;

	this->current_scene = Scene(this->models_folder);

	this->display_BG_COLOR[0] = static_cast<float>(((this->current_scene.BG_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[1] = static_cast<float>(((this->current_scene.BG_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[2] = static_cast<float>(((this->current_scene.BG_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_BG_COLOR[3] = static_cast<float>((this->current_scene.BG_COLOR & 0x000000FF) / 255.0);

	this->display_LINE_COLOR[0] = static_cast<float>(((this->current_scene.LINE_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[1] = static_cast<float>(((this->current_scene.LINE_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[2] = static_cast<float>(((this->current_scene.LINE_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_LINE_COLOR[3] = static_cast<float>((this->current_scene.LINE_COLOR & 0x000000FF) / 255.0);

	this->display_FILL_COLOR[0] = static_cast<float>(((this->current_scene.FILL_COLOR >> 24) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[1] = static_cast<float>(((this->current_scene.FILL_COLOR >> 16) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[2] = static_cast<float>(((this->current_scene.FILL_COLOR >> 8) & 0x000000FF) / 255.0);
	this->display_FILL_COLOR[3] = static_cast<float>((this->current_scene.FILL_COLOR & 0x000000FF) / 255.0);

	camera_tab = new CameraTab(this);
	instances_tab = new InstancesTab(this);
	light_tab = new LightTab(this);

	if (this->is_instances_open) {
		this->instances_tab->update_transform_axes();
	}
	else if (this->is_light_open) {
		this->light_tab->update_transform_axes();
	}
}

void SceneTab::draw() {
	ImGui::Text("Load scene camera settings");
	ImGui::SameLine();
	ImGui::Checkbox("##load_camera", &this->update_camera_settings);
	ImGui::SetItemTooltip("Enabled: Camera starts at the camera position and orientation within the file.\nDisabled: Camera starts at the default position and orientation.");

	if (ImGui::Button("New")) {
		load_empty_scene();
	}
	ImGui::SetItemTooltip("Creates and empty scene.");

	ImGui::Text("Scene file:");
	ImGui::SameLine();
	ImGui::PushItemWidth(90);
	ImGui::InputText("##Scene filename:", this->scene_load_name, 255, ImGuiInputTextFlags_None);
	ImGui::SetItemTooltip("Filename of the scene configuration file. (Including the extension)\nIt must be within the given scene folder");

	ImGui::SameLine();

	if (ImGui::Button("Load")) {
		//load_scene(this->scene_folder, this->scene_load_name, this->models_folder, this->update_camera_settings, true);
		load_scene();
	}

	ImGui::Separator();

	ImGui::Spacing();

	ImGui::Text("Scene file:");
	ImGui::SameLine();
	ImGui::InputText("##LScene filename (including extension):", this->scene_save_name, 255, ImGuiInputTextFlags_None);
	ImGui::SetItemTooltip("Filename of the scene configuration file. (Including the extension)\nIt will be saved in the given scene folder");
	ImGui::SameLine();

	if (ImGui::Button("Save")) {
		current_scene.save(this->scene_folder, this->scene_save_name);
	}

	ImGui::Separator();

	if (ImGui::Button("Browse##scene_folder")) {
		nfdchar_t* scenes_path = nullptr;
		nfdresult_t result = NFD_PickFolder(this->scene_folder, &scenes_path);

		if (result == NFD_OKAY) {
			sprintf_s(this->scene_folder, sizeof(this->scene_folder), "%s\\", scenes_path);
		}
		else if (result == NFD_CANCEL) {

		}
		else {
			std::cout << "NFD loading scene folder error: " << NFD_GetError() << std::endl;
			exit(-1);
		}

		free(scenes_path);
	}
	ImGui::SetItemTooltip("Choose the folder in which your scenes are saved.\nThe program will crash if it cannot find the mesh!\nMake sure the given model file is within the given model folder.");
	ImGui::SameLine();

	ImGui::Text("Scene folder:");
	ImGui::SameLine();
	ImGui::Text(this->scene_folder);

	if (ImGui::Button("Browse##models_folder")) {
		nfdchar_t* models_path = nullptr;
		nfdresult_t result = NFD_PickFolder(this->models_folder, &models_path);

		if (result == NFD_OKAY) {
			sprintf_s(this->models_folder, sizeof(this->models_folder), "%s\\", models_path);
		}
		else if (result == NFD_CANCEL) {

		}
		else {
			std::cout << "NFD loading models folder error: " << NFD_GetError() << std::endl;
			exit(-1);
		}

		free(models_path);
	}

	ImGui::SetItemTooltip("Choose the folder in which your models are saved.\nAll models within the scene must be in this folder.");
	ImGui::SameLine();
	ImGui::Text("Models folder:");
	ImGui::SameLine();
	ImGui::Text(this->models_folder);
	
	ImGui::Separator();

	if (ImGui::ColorEdit4("##BGColor", this->display_BG_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_BG_COLOR[0] * 255.0;
		uint8_t green = display_BG_COLOR[1] * 255.0;
		uint8_t blue = display_BG_COLOR[2] * 255.0;
		uint8_t alpha = display_BG_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->current_scene.BG_COLOR = color;
	};
	ImGui::SameLine();
	ImGui::Text("Background color");
	

	if (ImGui::ColorEdit4("##LColor", this->display_LINE_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_LINE_COLOR[0] * 255.0;
		uint8_t green = display_LINE_COLOR[1] * 255.0;
		uint8_t blue = display_LINE_COLOR[2] * 255.0;
		uint8_t alpha = display_LINE_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->current_scene.LINE_COLOR = color;
	};
	ImGui::SameLine();
	ImGui::Text("Line/wireframe color");
	

	if (ImGui::ColorEdit4("##FColor", this->display_FILL_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_FILL_COLOR[0] * 255.0;
		uint8_t green = display_FILL_COLOR[1] * 255.0;
		uint8_t blue = display_FILL_COLOR[2] * 255.0;
		uint8_t alpha = display_FILL_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->current_scene.FILL_COLOR = color;
	};
	ImGui::SameLine();
	ImGui::Text("Fill/ambient color");
	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::TreeNode("Camera")) {

		camera_tab->draw();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Instances")) {
		if (!is_instances_open) {
			is_instances_open = true;

			if (show_transform_axes) instances_tab->update_transform_axes();
		}

		//if (show_transform_axes && this->target_instance->show) update_transform_axes();

		instances_tab->draw();

		ImGui::TreePop();
	}
	else if (is_instances_open) {
		is_instances_open = false;
		//instances_tab->target_instance->has_axes = false;
	}

	if (ImGui::TreeNode("Light")) {
		if (!is_light_open) {
			is_light_open = true;

			if (show_transform_axes) light_tab->update_transform_axes();
		}

		light_tab->draw();

		ImGui::TreePop();
	}
	else if (is_light_open) {
		is_light_open = false;
		//this->current_scene.light_source.instance->has_axes = false;
	}
}

SceneTab::~SceneTab() {
	delete this->camera_tab;
	delete this->instances_tab;
	delete this->light_tab;
}