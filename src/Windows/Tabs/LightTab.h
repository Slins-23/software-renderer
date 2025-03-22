#pragma once
#include "../../Utils.h"
#include "../Window.h"
#include "SceneTab.h"
#include <nfd.h>

class LightTab : Window {
public:
	/*
	Orientation* rotation_orientation = nullptr;

	// Affects WASD translation
	float* real_translation_speed = nullptr;

	// Affects menu translation
	float* menu_translation_speed = nullptr;

	// Affects mouse rotation
	float* real_rotation_speed = nullptr;

	// Affects menu rotation
	float* menu_rotation_speed = nullptr;

	// Affects menu scaling
	float* menu_scaling_speed = nullptr;

	double light_translation_speed = 0.001;
	double light_rotation_speed = 0.01;
	*/

	SceneTab* scene_tab = nullptr;

	double light_display_qx = 0;
	double light_display_qy = 0;
	double light_display_qz = 0;
	double light_display_qw = 0;

	double light_display_dir_x = 0;
	double light_display_dir_y = 0;
	double light_display_dir_z = 0;

	double light_display_up_x = 0;
	double light_display_up_y = 0;
	double light_display_up_z = 0;

	float display_LIGHT_COLOR[4] = { 0, 0, 0, 0 };

	const char* lighting_types[3] = { "Directional", "Point", "Spotlight" };
	const char* current_lighting_type = "Directional";
	uint8_t lighting_selected_idx = 0;

	const char* shading_types[3] = { "Flat", "Gouraud", "Phong" };
	const char* current_shading_type = "Flat";
	uint8_t shading_selected_idx = 0;

	std::string chosen_light_mesh_name = "";
	uint32_t chosen_light_mesh_id = 0;

	/*
	bool* show_transform_axes = nullptr;

	const char* models_folder = nullptr;

	Scene* current_scene = nullptr;
	*/

	LightTab(SceneTab* scene_tab) {
		this->scene_tab = scene_tab;

		if (this->scene_tab->current_scene.light_source.mesh != nullptr) {
			chosen_light_mesh_name = this->scene_tab->current_scene.light_source.mesh->mesh_filename.c_str();
			chosen_light_mesh_id = this->scene_tab->current_scene.light_source.mesh->mesh_id;
		}
		else {
			chosen_light_mesh_name = "";
			chosen_light_mesh_id = 0;
		}

		this->lighting_selected_idx = this->scene_tab->current_scene.light_source.lighting_type;
		this->current_lighting_type = this->lighting_types[this->scene_tab->current_scene.light_source.lighting_type];

		switch (this->scene_tab->current_scene.light_source.lighting_type) {
		case LightType::directional:
			this->current_lighting_type = "Directional";
			break;
		case LightType::point:
			this->current_lighting_type = "Point";
			break;
		case LightType::spotlight:
			this->current_lighting_type = "Spotlight";
			break;
		}

		switch (this->scene_tab->current_scene.light_source.shading_type) {
		case ShadingType::Flat:
			this->current_shading_type = "Flat";
			break;
		case ShadingType::Gouraud:
			this->current_shading_type = "Gouraud";
			break;
		case ShadingType::Phong:
			this->current_shading_type = "Phong";
			break;
		}

		this->light_display_qx = this->scene_tab->current_scene.light_source.orientation.x;
		this->light_display_qy = this->scene_tab->current_scene.light_source.orientation.y;
		this->light_display_qz = this->scene_tab->current_scene.light_source.orientation.z;
		this->light_display_qw = this->scene_tab->current_scene.light_source.orientation.w;

		double red = ((this->scene_tab->current_scene.light_source.color >> 24) & 0x000000FF) / 255.0;
		double green = ((this->scene_tab->current_scene.light_source.color >> 16) & 0x000000FF) / 255.0;
		double blue = ((this->scene_tab->current_scene.light_source.color >> 8) & 0x000000FF) / 255.0;
		double alpha = (this->scene_tab->current_scene.light_source.color & 0x000000FF) / 255.0;

		this->display_LIGHT_COLOR[0] = red;
		this->display_LIGHT_COLOR[1] = green;
		this->display_LIGHT_COLOR[2] = blue;
		this->display_LIGHT_COLOR[3] = alpha;
	}

	/*
	void initialize(Scene* current_scene, Orientation* rotation_orientation, const char* models_folder, bool* show_transform_axes) {
		this->scene_tab->show_transform_axes = show_transform_axes;
		//this->scene_tab->current_scene = current_scene;

		update(&this->scene_tab->current_scene.light_source, rotation_orientation);
	}
	*/

	/*
	//void update(Light* light_source, Orientation* rotation_orientation) {
	void update() {
		this->lighting_selected_idx = this->scene_tab->current_scene.light_source.lighting_type;
		this->current_lighting_type = this->lighting_types[this->scene_tab->current_scene.light_source.lighting_type];

		switch (this->scene_tab->current_scene.light_source.lighting_type) {
		case LightType::directional:
			this->current_lighting_type = "Directional";
			break;
		case LightType::point:
			this->current_lighting_type = "Point";
			break;
		case LightType::spotlight:
			this->current_lighting_type = "Spotlight";
			break;
		}

		switch (this->scene_tab->current_scene.light_source.shading_type) {
		case ShadingType::Flat:
			this->current_lighting_type = "Flat";
			break;
		case ShadingType::Gouraud:
			this->current_lighting_type = "Gouraud";
			break;
		case ShadingType::Phong:
			this->current_lighting_type = "Phong";
			break;
		}

		this->light_display_qx = this->scene_tab->current_scene.light_source.orientation.x;
		this->light_display_qy = this->scene_tab->current_scene.light_source.orientation.y;
		this->light_display_qz = this->scene_tab->current_scene.light_source.orientation.z;
		this->light_display_qw = this->scene_tab->current_scene.light_source.orientation.w;

		//this->scene_tab->rotation_orientation = rotation_orientation;
	}
	*/

	void UpdateLightTranslation() {
		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
			this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX = Mat::translation_matrix(this->scene_tab->current_scene.light_source.tx, this->scene_tab->current_scene.light_source.ty, this->scene_tab->current_scene.light_source.tz);

			this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX * this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX * this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX;

			this->scene_tab->current_scene.light_source.instance->tx = this->scene_tab->current_scene.light_source.tx;
			this->scene_tab->current_scene.light_source.instance->ty = this->scene_tab->current_scene.light_source.ty;
			this->scene_tab->current_scene.light_source.instance->tz = this->scene_tab->current_scene.light_source.tz;
		}

		this->scene_tab->current_scene.light_source.position = Mat(
			{
				{this->scene_tab->current_scene.light_source.tx},
				{this->scene_tab->current_scene.light_source.ty},
				{this->scene_tab->current_scene.light_source.tz},
				{1}
			}
		, 4, 1);

		if (this->scene_tab->show_transform_axes) {
			update_transform_axes();
		}
	}

	void UpdateLightScaling() {
		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
			this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX = Mat::scale_matrix(this->scene_tab->current_scene.light_source.instance->sx, this->scene_tab->current_scene.light_source.instance->sy, this->scene_tab->current_scene.light_source.instance->sz);
			this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX * this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX * this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX;
		}
	}

	void UpdateLightRotation(uint8_t rotation_type) {
		// If updating euler angles
		if (rotation_type == RotationType_Euler) {
			Mat default_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
			Mat default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
			Mat default_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

			Quaternion orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, this->scene_tab->current_scene.light_source.yaw, this->scene_tab->current_scene.light_source.pitch, this->scene_tab->current_scene.light_source.roll, default_right, default_up, default_forward);

			this->scene_tab->current_scene.light_source.orientation = orientation;

			if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
				this->scene_tab->current_scene.light_source.instance->orientation = this->scene_tab->current_scene.light_source.orientation;
				this->scene_tab->current_scene.light_source.instance->yaw = this->scene_tab->current_scene.light_source.yaw;
				this->scene_tab->current_scene.light_source.instance->pitch = this->scene_tab->current_scene.light_source.pitch;
				this->scene_tab->current_scene.light_source.instance->roll = this->scene_tab->current_scene.light_source.roll;
			}

			this->light_display_qx = orientation.x;
			this->light_display_qy = orientation.y;
			this->light_display_qz = orientation.z;
			this->light_display_qw = orientation.w;

			this->scene_tab->current_scene.light_source.direction = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix() * this->scene_tab->current_scene.light_source.default_direction;
			this->scene_tab->current_scene.light_source.up = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix() * this->scene_tab->current_scene.light_source.default_up;

			this->light_display_dir_x = this->scene_tab->current_scene.light_source.direction.get(1, 1);
			this->light_display_dir_y = this->scene_tab->current_scene.light_source.direction.get(2, 1);
			this->light_display_dir_z = this->scene_tab->current_scene.light_source.direction.get(3, 1);

			this->light_display_up_x = this->scene_tab->current_scene.light_source.up.get(1, 1);
			this->light_display_up_y = this->scene_tab->current_scene.light_source.up.get(2, 1);
			this->light_display_up_z = this->scene_tab->current_scene.light_source.up.get(3, 1);

		}

		// If updating quaternion
		else if (rotation_type == RotationType_Quaternion) {
			double qw = this->light_display_qw;
			double qx = this->light_display_qx;
			double qy = this->light_display_qy;
			double qz = this->light_display_qz;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			this->scene_tab->current_scene.light_source.orientation.w = qw;
			this->scene_tab->current_scene.light_source.orientation.x = qx;
			this->scene_tab->current_scene.light_source.orientation.y = qy;
			this->scene_tab->current_scene.light_source.orientation.z = qz;

			if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
				this->scene_tab->current_scene.light_source.instance->orientation.w = qw;
				this->scene_tab->current_scene.light_source.instance->orientation.x = qx;
				this->scene_tab->current_scene.light_source.instance->orientation.y = qy;
				this->scene_tab->current_scene.light_source.instance->orientation.z = qz;
			}

			// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

			double new_yaw = 0;
			double new_pitch = 0;
			double new_roll = 0;

			this->scene_tab->current_scene.light_source.orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

			double old_normalized_yaw = fmod(this->scene_tab->current_scene.light_source.yaw, 2 * M_PI);

			if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
			if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

			double old_normalized_pitch = fmod(this->scene_tab->current_scene.light_source.pitch, 2 * M_PI);

			if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
			if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

			double old_normalized_roll = fmod(this->scene_tab->current_scene.light_source.roll, 2 * M_PI);

			if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
			if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

			double yaw_difference = new_yaw - old_normalized_yaw;
			double pitch_difference = new_pitch - old_normalized_pitch;
			double roll_difference = new_roll - old_normalized_roll;

			this->scene_tab->current_scene.light_source.yaw += yaw_difference;
			this->scene_tab->current_scene.light_source.pitch += pitch_difference;
			this->scene_tab->current_scene.light_source.roll += roll_difference;

			if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
				this->scene_tab->current_scene.light_source.instance->yaw = this->scene_tab->current_scene.light_source.yaw;
				this->scene_tab->current_scene.light_source.instance->pitch = this->scene_tab->current_scene.light_source.pitch;
				this->scene_tab->current_scene.light_source.instance->roll = this->scene_tab->current_scene.light_source.roll;
			}

			this->scene_tab->current_scene.light_source.direction = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix() * this->scene_tab->current_scene.light_source.default_direction;
			this->scene_tab->current_scene.light_source.up = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix() * this->scene_tab->current_scene.light_source.default_up;

			this->light_display_dir_x = this->scene_tab->current_scene.light_source.direction.get(1, 1);
			this->light_display_dir_y = this->scene_tab->current_scene.light_source.direction.get(2, 1);
			this->light_display_dir_z = this->scene_tab->current_scene.light_source.direction.get(3, 1);

			this->light_display_up_x = this->scene_tab->current_scene.light_source.up.get(1, 1);
			this->light_display_up_y = this->scene_tab->current_scene.light_source.up.get(2, 1);
			this->light_display_up_z = this->scene_tab->current_scene.light_source.up.get(3, 1);
		}
		else if (rotation_type == RotationType_Direction) {
			this->scene_tab->current_scene.light_source.direction.set(this->light_display_dir_x, 1, 1);
			this->scene_tab->current_scene.light_source.direction.set(this->light_display_dir_y, 2, 1);
			this->scene_tab->current_scene.light_source.direction.set(this->light_display_dir_z, 3, 1);

			this->scene_tab->current_scene.light_source.up.set(this->light_display_up_x, 1, 1);
			this->scene_tab->current_scene.light_source.up.set(this->light_display_up_y, 2, 1);
			this->scene_tab->current_scene.light_source.up.set(this->light_display_up_z, 3, 1);

			Mat light_right = Mat::CrossProduct3D(this->scene_tab->current_scene.light_source.up, this->scene_tab->current_scene.light_source.direction);

			// Gets yaw and pitch representing the rotation from the default camera direction to the new camera direction
			Quaternion::GetAnglesFromDirection(this->scene_tab->rotation_orientation, this->scene_tab->current_scene.light_source.default_direction, this->scene_tab->current_scene.light_source.direction, this->scene_tab->current_scene.light_source.yaw, this->scene_tab->current_scene.light_source.pitch, this->scene_tab->current_scene.light_source.roll);

			// Gets roll representing the rotation from the default camera up to the camera up
			// Assumes default direction of (0, 0, 1)
			Quaternion::GetRoll(this->scene_tab->rotation_orientation, this->scene_tab->current_scene.light_source.direction, this->scene_tab->current_scene.light_source.up, this->scene_tab->current_scene.light_source.yaw, this->scene_tab->current_scene.light_source.pitch, this->scene_tab->current_scene.light_source.roll);

			this->scene_tab->current_scene.light_source.orientation = Quaternion::FromYawPitchRoll(Orientation::local, this->scene_tab->current_scene.light_source.yaw, this->scene_tab->current_scene.light_source.pitch, this->scene_tab->current_scene.light_source.roll, this->scene_tab->current_scene.light_source.default_right, this->scene_tab->current_scene.light_source.default_up, this->scene_tab->current_scene.light_source.default_direction);

			this->light_display_qx = this->scene_tab->current_scene.light_source.orientation.x;
			this->light_display_qy = this->scene_tab->current_scene.light_source.orientation.y;
			this->light_display_qz = this->scene_tab->current_scene.light_source.orientation.z;
			this->light_display_qw = this->scene_tab->current_scene.light_source.orientation.w;

			/*
			this->light_display_dir_x = this->scene_tab->current_scene.light_source.direction.get(1, 1);
			this->light_display_dir_y = this->scene_tab->current_scene.light_source.direction.get(2, 1);
			this->light_display_dir_z = this->scene_tab->current_scene.light_source.direction.get(3, 1);

			this->light_display_up_x = this->scene_tab->current_scene.light_source.up.get(1, 1);
			this->light_display_up_y = this->scene_tab->current_scene.light_source.up.get(2, 1);
			this->light_display_up_z = this->scene_tab->current_scene.light_source.up.get(3, 1);
			*/
		}

		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
			this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX = this->scene_tab->current_scene.light_source.instance->orientation.get_rotationmatrix();
			this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX * this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX * this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX;
		}

		if (this->scene_tab->show_transform_axes) {
			update_transform_axes();
		}
	}

	void draw() {
		ImGui::PushItemWidth(100);

		ImGui::Text("Enabled:");
		ImGui::SameLine();
		ImGui::Checkbox("##Enabled:", &this->scene_tab->current_scene.light_source.enabled);

		if (this->scene_tab->current_scene.light_source.enabled) {
			ImGui::Text("Lighting type:");
			ImGui::SameLine();
			if (ImGui::BeginCombo("##Lighting type:", this->current_lighting_type, ImGuiComboFlags_None)) {
				for (uint8_t lighting_type = 0; lighting_type < sizeof(this->lighting_types) / sizeof(const char*); lighting_type++) {

					const bool is_selected = (lighting_selected_idx == lighting_type);
					if (ImGui::Selectable(lighting_types[lighting_type], is_selected)) {
						lighting_selected_idx = lighting_type;

						switch (lighting_selected_idx) {
						case LightType::directional:
							this->scene_tab->current_scene.light_source.lighting_type = LightType::directional;
							current_lighting_type = "Directional";
							break;
						case LightType::point:
							this->scene_tab->current_scene.light_source.lighting_type = LightType::point;
							current_lighting_type = "Point";
							break;
						case LightType::spotlight:
							this->scene_tab->current_scene.light_source.lighting_type = LightType::spotlight;
							current_lighting_type = "Spotlight";
							break;
						}
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Spacing();

			ImGui::Text("Shading type:");
			ImGui::SameLine();
			if (ImGui::BeginCombo("##Shading type:", this->current_shading_type, ImGuiComboFlags_None)) {
				for (uint8_t shading_type = 0; shading_type < sizeof(this->shading_types) / sizeof(const char*); shading_type++) {

					const bool is_selected = (this->shading_selected_idx == shading_type);
					if (ImGui::Selectable(this->shading_types[shading_type], is_selected)) {
						this->shading_selected_idx = shading_type;

						switch (this->shading_selected_idx) {
						case ShadingType::Flat:
							this->scene_tab->current_scene.light_source.shading_type = ShadingType::Flat;
							this->current_shading_type = "Flat";
							break;
						case ShadingType::Gouraud:
							this->scene_tab->current_scene.light_source.shading_type = ShadingType::Gouraud;
							this->current_shading_type = "Gouraud";
							break;
						case ShadingType::Phong:
							this->scene_tab->current_scene.light_source.shading_type = ShadingType::Phong;
							this->current_shading_type = "Phong";
							break;
						}
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::PushItemWidth(60);

			ImGui::Text("Intensity:");
			ImGui::SameLine();
			ImGui::DragScalar("##Intensity:", ImGuiDataType_Double, &this->scene_tab->current_scene.light_source.intensity, 0.005, &zero, nullptr, "%.4lf", ImGuiSliderFlags_None);

			ImGui::Text("Minimum exposure:");
			ImGui::SameLine();
			ImGui::DragScalar("##Minimum exposure:", ImGuiDataType_Double, &this->scene_tab->current_scene.light_source.minimum_exposure, 0.005, &zero, nullptr, "%.4lf", ImGuiSliderFlags_None);

			ImGui::Spacing();

			ImGui::Text("Color:");
			ImGui::SameLine();
			if (ImGui::ColorEdit4("##LColor:", this->display_LIGHT_COLOR, ImGuiColorEditFlags_NoInputs)) {
				uint8_t red = this->display_LIGHT_COLOR[0] * 255.0;
				uint8_t green = this->display_LIGHT_COLOR[1] * 255.0;
				uint8_t blue = this->display_LIGHT_COLOR[2] * 255.0;
				uint8_t alpha = this->display_LIGHT_COLOR[3] * 255.0;

				uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

				this->scene_tab->current_scene.light_source.color = color;
			};

			ImGui::Separator();

			ImGui::Text("Attach model:");
			ImGui::SameLine();
			if (ImGui::Checkbox("##Attach model? ", &this->scene_tab->current_scene.light_source.has_model)) {
				if (!this->scene_tab->current_scene.light_source.has_model) {
					this->scene_tab->current_scene.light_source.instance->show = false;
				}
				else {
					this->scene_tab->current_scene.light_source.instance->show = true;
				}
			}

			if (this->scene_tab->current_scene.light_source.has_model) {
				// Load mesh from file and send it to scene meshes if not already there
				ImGui::SetItemTooltip("The .obj mesh file MUST be within the MODELS folder! (This is because scenes are saved with only the filename, not path, as it is assumed to be relative to the models folder)");
				static bool errored = 0;
				if (ImGui::Button("Load mesh")) {
					static const nfdchar_t* obj_filter = "obj";
					nfdchar_t* mesh_path = nullptr;
					nfdresult_t result = NFD_OpenDialog(obj_filter, this->scene_tab->models_folder, &mesh_path);

					if (result == NFD_OKAY) {
						std::string tmp_path = mesh_path;


						std::string mesh_folder = tmp_path.substr(0, tmp_path.find_last_of("\\"));
						std::string mesh_filename = tmp_path.substr(tmp_path.find_last_of("\\") + 1);

						// Makes sure that the mesh folder matches the set models folder
						if (!strcmp(mesh_folder.c_str(), this->scene_tab->models_folder)) {
							// Load mesh only if a mesh with the same filename isn't loaded (to avoid duplication)
							if (this->scene_tab->current_scene.get_mesh_ptr(mesh_filename) == nullptr) {
								Mesh loaded_mesh = Mesh(mesh_folder.c_str(), mesh_filename.c_str(), this->scene_tab->current_scene.total_ever_meshes);
								this->scene_tab->current_scene.total_meshes++;
								this->scene_tab->current_scene.light_source.mesh = &loaded_mesh;
								this->scene_tab->current_scene.scene_meshes.push_back(std::move(loaded_mesh));
							}
							errored = 0;
						}
						else {
							errored = 1;	
						}

					}
					else if (result == NFD_CANCEL) {

					}
					else {
						std::cout << "NFD Error: " << NFD_GetError() << std::endl;
						exit(-1);
					}

					free(mesh_path);
				}

				if (errored) {
					ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "Mesh could not be loaded: The folder from which the mesh is chosen must be the same models folder that contains all other meshes. (It is seen and can be updated above)");
				}

				ImGui::Text("Choose mesh:");
				ImGui::SameLine();
				// Make sure that mesh is not a nullptr here in the case that no mesh is attached for a scene without a light source in principle
				if (ImGui::BeginCombo("##Choose mesh:", this->chosen_light_mesh_name.c_str(), ImGuiComboFlags_None)) {
					for (uint8_t mesh = 0; mesh < this->scene_tab->current_scene.scene_meshes.size(); mesh++) {
						Mesh* current_mesh = &this->scene_tab->current_scene.scene_meshes[mesh];

						bool is_selected = false;
						if (this->scene_tab->current_scene.light_source.mesh != nullptr) {
							is_selected = this->scene_tab->current_scene.light_source.mesh->mesh_id == this->chosen_light_mesh_id;
						}

						if (ImGui::Selectable(current_mesh->mesh_filename.c_str(), is_selected)) {
							this->scene_tab->current_scene.light_source.mesh = current_mesh;

							if (this->scene_tab->current_scene.light_source.instance == nullptr) {
								// Create instance if there is none
								Instance light_instance = Instance(this->scene_tab->current_scene.total_ever_instances);
								light_instance.instance_id = this->scene_tab->current_scene.total_ever_instances;
								light_instance.instance_name = "light_source";

								light_instance.orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, this->scene_tab->current_scene.light_source.yaw, this->scene_tab->current_scene.light_source.pitch, this->scene_tab->current_scene.light_source.roll, this->scene_tab->current_scene.default_world_right, this->scene_tab->current_scene.default_world_up, this->scene_tab->current_scene.default_world_forward);

								light_instance.ROTATION_MATRIX = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix();
								light_instance.SCALING_MATRIX = Mat::scale_matrix(1, 1, 1);
								light_instance.TRANSLATION_MATRIX = Mat::translation_matrix(this->scene_tab->current_scene.light_source.tx, this->scene_tab->current_scene.light_source.ty, this->scene_tab->current_scene.light_source.tz);
								light_instance.MODEL_TO_WORLD = light_instance.TRANSLATION_MATRIX * light_instance.ROTATION_MATRIX * light_instance.SCALING_MATRIX;

								this->scene_tab->current_scene.light_source.instance = &light_instance;
								this->scene_tab->current_scene.scene_instances.push_back(std::move(light_instance));

								this->scene_tab->current_scene.total_instances++;
							}

							this->scene_tab->current_scene.light_source.instance->mesh = current_mesh;
							
							this->chosen_light_mesh_name = current_mesh->mesh_filename;
							this->chosen_light_mesh_id = current_mesh->mesh_id;
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}
			}

			ImGui::Separator();

			ImGui::Text("Translation speed:");
			ImGui::SetItemTooltip("How sensitive the menu translation slider is to clicking and dragging (higher = more change)");
			ImGui::SameLine();
			ImGui::DragScalar("##LTranslation speed (menu):", ImGuiDataType_Double, &this->scene_tab->menu_translation_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Separator();
			ImGui::Text("Rotation speed:");
			ImGui::SetItemTooltip("(Menu) How sensitive the menu rotation slider is to clicking and dragging (higher = more change): ");
			ImGui::SameLine();
			ImGui::DragScalar("##LRotation speed (menu):", ImGuiDataType_Double, &this->scene_tab->menu_rotation_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Text("Scaling speed:");
			ImGui::SetItemTooltip("(Menu) How sensitive the menu scaling slider is to clicking and dragging (higher = more change): ");
			ImGui::SameLine();
			ImGui::DragScalar("##LScaling speed:", ImGuiDataType_Double, &this->scene_tab->menu_scaling_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Text("Direction");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			if (ImGui::DragScalar("##LDX:", ImGuiDataType_Double, (void*)&(this->light_display_dir_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LDY:", ImGuiDataType_Double, (void*)&(this->light_display_dir_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LDZ:", ImGuiDataType_Double, (void*)&(this->light_display_dir_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Spacing();

			ImGui::Text("Up");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			if (ImGui::DragScalar("##LUX:", ImGuiDataType_Double, (void*)&(this->light_display_up_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LUY:", ImGuiDataType_Double, (void*)&(this->light_display_up_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LUZ:", ImGuiDataType_Double, (void*)&(this->light_display_up_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Direction);
			}

			ImGui::Separator();

			ImGui::Text("Translation");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			if (ImGui::DragScalar("##LX:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.tx), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightTranslation();
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LY:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.ty), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightTranslation();
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LZ:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.tz), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightTranslation();
			}

			ImGui::Separator();

			if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
				ImGui::Text("Scaling");
				ImGui::Spacing();
				ImGui::Text("X:");
				ImGui::SameLine();

				if (ImGui::DragScalar("##LSX:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sx), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
					UpdateLightScaling();
				}

				ImGui::Text("Y:");
				ImGui::SameLine();
				if (ImGui::DragScalar("##LSY:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sy), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
					UpdateLightScaling();
				}

				ImGui::Text("Z:");
				ImGui::SameLine();
				if (ImGui::DragScalar("##LSZ:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sz), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
					UpdateLightScaling();
				}

				ImGui::Separator();
			}

			ImGui::Text("Rotation (YXZ)");
			ImGui::Spacing();
			ImGui::Text("Yaw:  ");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LYaw:  ", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.yaw), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Euler);
			}

			ImGui::Text("Pitch:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LPitch:", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.pitch), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Euler);
			}

			ImGui::Text("Roll: ");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LRoll: ", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.roll), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Euler);
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Quaternion");
			ImGui::Text("X:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LQX:", ImGuiDataType_Double, &this->light_display_qx, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Quaternion);
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LQY:", ImGuiDataType_Double, &this->light_display_qy, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Quaternion);
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LQZ:", ImGuiDataType_Double, &this->light_display_qz, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Quaternion);
			}

			ImGui::Text("W:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##LQW:", ImGuiDataType_Double, &this->light_display_qw, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightRotation(RotationType_Quaternion);
			}

			ImGui::Separator();

			if (this->scene_tab->current_scene.light_source.has_model) {
				ImGui::Text("Model-to-world matrix:");
				if (ImGui::BeginTable("Model-to-world matrix", 4, ImGuiTableFlags_None)) {
					for (uint8_t row = 1; row < 5; row++) {
						ImGui::TableNextRow();
						for (uint8_t col = 1; col < 5; col++) {
							ImGui::TableNextColumn();

							double number = this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD.get(row, col);

							ImGui::Text(std::to_string(number).c_str());
						}
					}

				}

				ImGui::EndTable();

				ImGui::Spacing();

				ImGui::Text("Translation matrix:");
				if (ImGui::BeginTable("Translation matrix", 4, ImGuiTableFlags_None)) {
					for (uint8_t row = 1; row < 5; row++) {
						ImGui::TableNextRow();
						for (uint8_t col = 1; col < 5; col++) {
							ImGui::TableNextColumn();

							double number = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX.get(row, col);

							ImGui::Text(std::to_string(number).c_str());
						}
					}

				}

				ImGui::EndTable();

				ImGui::Spacing();

				ImGui::Text("Rotation matrix:");
				if (ImGui::BeginTable("Rotation matrix", 4, ImGuiTableFlags_None)) {
					for (uint8_t row = 1; row < 5; row++) {
						ImGui::TableNextRow();
						for (uint8_t col = 1; col < 5; col++) {
							ImGui::TableNextColumn();

							double number = this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX.get(row, col);

							ImGui::Text(std::to_string(number).c_str());
						}
					}

				}

				ImGui::EndTable();

				ImGui::Spacing();

				ImGui::Text("Scaling matrix:");
				if (ImGui::BeginTable("Scaling matrix", 4, ImGuiTableFlags_None)) {
					for (uint8_t row = 1; row < 5; row++) {
						ImGui::TableNextRow();
						for (uint8_t col = 1; col < 5; col++) {
							ImGui::TableNextColumn();

							double number = this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX.get(row, col);

							ImGui::Text(std::to_string(number).c_str());
						}
					}

				}

				ImGui::EndTable();

				ImGui::Separator();
			}
		}


	}

	void update_transform_axes() {
		// If not false, make target_instance->has_axes = false to disable the axes for the non-light source instance
		if (this->scene_tab->show_transform_axes) {
			if (this->scene_tab->current_scene.light_source.instance != nullptr) {
				this->scene_tab->current_scene.light_source.instance->has_axes = true;
			}
			
			this->scene_tab->current_scene.axes_instance.is_axes = true;
			this->scene_tab->current_scene.axes_instance.instance_id = 13377331;
			this->scene_tab->current_scene.axes_instance.instance_name = "light_source_axes";
			this->scene_tab->current_scene.axes_instance.is_light_source = false;
			this->scene_tab->current_scene.axes_instance.mesh = &this->scene_tab->current_scene.axes_mesh;

			this->scene_tab->current_scene.axes_instance.tx = this->scene_tab->current_scene.light_source.tx;
			this->scene_tab->current_scene.axes_instance.ty = this->scene_tab->current_scene.light_source.ty;
			this->scene_tab->current_scene.axes_instance.tz = this->scene_tab->current_scene.light_source.tz;

			this->scene_tab->current_scene.axes_instance.yaw = this->scene_tab->current_scene.light_source.yaw;
			this->scene_tab->current_scene.axes_instance.pitch = this->scene_tab->current_scene.light_source.pitch;
			this->scene_tab->current_scene.axes_instance.roll = this->scene_tab->current_scene.light_source.roll;

			this->scene_tab->current_scene.axes_instance.sx = 1;
			this->scene_tab->current_scene.axes_instance.sy = 1;
			this->scene_tab->current_scene.axes_instance.sz = 1;

			this->scene_tab->current_scene.axes_instance.TRANSLATION_MATRIX = Mat::translation_matrix(this->scene_tab->current_scene.light_source.tx, this->scene_tab->current_scene.light_source.ty, this->scene_tab->current_scene.light_source.tz);
			this->scene_tab->current_scene.axes_instance.ROTATION_MATRIX = this->scene_tab->current_scene.light_source.orientation.get_rotationmatrix();
			this->scene_tab->current_scene.axes_instance.SCALING_MATRIX = Mat::scale_matrix(1, 1, 1);
			this->scene_tab->current_scene.axes_instance.MODEL_TO_WORLD = this->scene_tab->current_scene.axes_instance.TRANSLATION_MATRIX * this->scene_tab->current_scene.axes_instance.ROTATION_MATRIX * this->scene_tab->current_scene.axes_instance.SCALING_MATRIX;
			this->scene_tab->current_scene.axes_instance.show = true;
		}

	}
};
