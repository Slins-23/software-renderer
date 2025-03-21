#pragma once
#include "../Window.h"
#include "SceneTab.h"

class InstancesTab : Window {
public:
	//Orientation* rotation_orientation = nullptr;

	SceneTab* scene_tab = nullptr;

	Instance* target_instance = nullptr;
	//uint32_t target_instance_idx = 0;

	double display_qx = 0;
	double display_qy = 0;
	double display_qz = 0;
	double display_qw = 0;

	std::string chosen_instance_name = "";
	uint32_t chosen_instance_id = 0;

	std::string chosen_mesh_name = "";
	uint32_t chosen_mesh_id = 0;

	/*
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

	bool* show_transform_axes = nullptr;

	Scene* current_scene = nullptr;

	const char* models_folder = nullptr;
	*/

	InstancesTab(SceneTab* scene_tab) {
		this->scene_tab = scene_tab;

		// Sets starting target instance to the first non-light source instance (currently there is only one light source and it is always the first instance)
		if (this->scene_tab->current_scene.scene_instances.size() > 0) {
			if (!this->scene_tab->current_scene.scene_instances[0].is_light_source) {
				this->target_instance = &this->scene_tab->current_scene.scene_instances[0];
				//this->target_instance_idx = 0;
			}
			else if (this->scene_tab->current_scene.scene_instances.size() > 1) {
				this->target_instance = &this->scene_tab->current_scene.scene_instances[1];
				//this->target_instance_idx = 1;
			}

			if (this->target_instance != nullptr) {
				this->chosen_instance_id = this->target_instance->instance_id;
				this->chosen_instance_name = this->target_instance->instance_name;

				if (this->target_instance->mesh != nullptr) {
					this->chosen_mesh_id = this->target_instance->mesh->mesh_id;
					this->chosen_mesh_name = this->target_instance->mesh->mesh_filename;
				}

				this->display_qx = this->target_instance->orientation.x;
				this->display_qy = this->target_instance->orientation.y;
				this->display_qz = this->target_instance->orientation.z;
				this->display_qw = this->target_instance->orientation.w;
			}
		}
	};


	/*
	void initialize(Scene* current_scene, Orientation* rotation_orientation, const char* models_folder, bool* show_transform_axes) {		
		this->scene_tab->show_transform_axes = show_transform_axes;
		this->current_scene = current_scene;
		this->scene_tab->models_folder = models_folder;

		// Sets the non light source target instance (which will be transformed when edit mode is enabled)
		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.total_instances > 1) {
			update(&this->scene_tab->current_scene.scene_instances[1], 1, rotation_orientation);
		}
		else if (!this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.total_instances > 0) {
			update(&this->scene_tab->current_scene.scene_instances[0], 0, rotation_orientation);
		}
		
		update_transform_axes();
	}
	*/

	/*
	void update(Instance* target_instance, uint8_t target_instance_idx, Orientation* rotation_orientation) {
		this->target_instance_idx = target_instance_idx;

		if (target_instance != nullptr) {
			this->target_instance = target_instance;

			this->display_qx = this->target_instance->orientation.x;
			this->display_qy = this->target_instance->orientation.y;
			this->display_qz = this->target_instance->orientation.z;
			this->display_qw = this->target_instance->orientation.w;
		}

		this->rotation_orientation = rotation_orientation;
	}
	*/

	/*
	void update(uint8_t target_instance_idx) {
		this->target_instance_idx = target_instance_idx;

		if (target_instance != nullptr) {
			this->target_instance = target_instance;

			this->display_qx = this->target_instance->orientation.x;
			this->display_qy = this->target_instance->orientation.y;
			this->display_qz = this->target_instance->orientation.z;
			this->display_qw = this->target_instance->orientation.w;
		}
	}
	*/

	void UpdateInstanceTranslation() {
		this->target_instance->TRANSLATION_MATRIX = Mat::translation_matrix(this->target_instance->tx, this->target_instance->ty, this->target_instance->tz);
		this->target_instance->MODEL_TO_WORLD = this->target_instance->TRANSLATION_MATRIX * this->target_instance->ROTATION_MATRIX * this->target_instance->SCALING_MATRIX;

		if (this->scene_tab->show_transform_axes) {
			update_transform_axes();
		}
	}

	void UpdateInstanceScaling() {
		this->target_instance->SCALING_MATRIX = Mat::scale_matrix(this->target_instance->sx, this->target_instance->sy, this->target_instance->sz);
		this->target_instance->MODEL_TO_WORLD = this->target_instance->TRANSLATION_MATRIX * this->target_instance->ROTATION_MATRIX * this->target_instance->SCALING_MATRIX;
	}

	void UpdateInstanceRotation(uint8_t rotation_type) {
		// If updating euler angles
		if (rotation_type == RotationType_Euler) {
			Mat default_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
			Mat default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
			Mat default_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

			Quaternion orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, this->target_instance->yaw, this->target_instance->pitch, this->target_instance->roll, default_right, default_up, default_forward);

			this->target_instance->orientation = orientation;

			this->display_qx = orientation.x;
			this->display_qy = orientation.y;
			this->display_qz = orientation.z;
			this->display_qw = orientation.w;
		}

		// If updating quaternion
		else if (rotation_type == RotationType_Quaternion) {
			double qw = this->display_qw;
			double qx = this->display_qx;
			double qy = this->display_qy;
			double qz = this->display_qz;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			this->target_instance->orientation.w = qw;
			this->target_instance->orientation.x = qx;
			this->target_instance->orientation.y = qy;
			this->target_instance->orientation.z = qz;

			// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

			double new_yaw = 0;
			double new_pitch = 0;
			double new_roll = 0;

			this->target_instance->orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

			double old_normalized_yaw = fmod(this->target_instance->yaw, 2 * M_PI);

			if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
			if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

			double old_normalized_pitch = fmod(this->target_instance->pitch, 2 * M_PI);

			if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
			if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

			double old_normalized_roll = fmod(this->target_instance->roll, 2 * M_PI);

			if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
			if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

			double yaw_difference = new_yaw - old_normalized_yaw;
			double pitch_difference = new_pitch - old_normalized_pitch;
			double roll_difference = new_roll - old_normalized_roll;

			this->target_instance->yaw += yaw_difference;
			this->target_instance->pitch += pitch_difference;
			this->target_instance->roll += roll_difference;
		}

		this->target_instance->ROTATION_MATRIX = this->target_instance->orientation.get_rotationmatrix();
		this->target_instance->MODEL_TO_WORLD = this->target_instance->TRANSLATION_MATRIX * this->target_instance->ROTATION_MATRIX * this->target_instance->SCALING_MATRIX;

		if (this->scene_tab->show_transform_axes) {
			update_transform_axes();
		}

		/*
		Mat direction = instance->ROTATION_MATRIX * Mat({ {0}, {0}, {1}, {0} }, 4, 1);
		Mat up = instance->ROTATION_MATRIX * Mat({ {0}, {1}, {0}, {0} }, 4, 1);

		if (abs(direction.get(2, 1)) > 0.9999999) {
			// In gimbal lock - collapse roll into yaw
			instance->roll = 0.0;
		}
		else {
			// Normal case - extract roll from up vector
			instance->roll = -atan2(up.get(1, 1) * cos(instance->yaw) - up.get(3, 1) * sin(instance->yaw), up.get(2, 1) / cos(instance->pitch));
		}
		*/
	}

	void draw() {
		// SPAWN AXES
		ImGui::Text("Show transform axes: ");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Show transform axes: ", &this->scene_tab->show_transform_axes)) {
			if (this->scene_tab->show_transform_axes) {
				update_transform_axes();
			}
		}
		ImGui::Separator();

		if (ImGui::Button("Add instance/load mesh")) {
			ImGui::OpenPopup("popup");
		}

		if (ImGui::BeginPopupModal("popup")) {
			Instance instance = Instance(this->scene_tab->current_scene.total_ever_instances);
			Mesh* chosen_mesh = nullptr;

			double display_qx = instance.orientation.x;
			double display_qy = instance.orientation.y;
			double display_qz = instance.orientation.z;
			double display_qw = instance.orientation.w;

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
			if (ImGui::BeginCombo("##Choose mesh:", this->chosen_mesh_name.c_str(), ImGuiComboFlags_None)) {
				for (uint8_t mesh = 0; mesh < this->scene_tab->current_scene.scene_meshes.size(); mesh++) {
					Mesh* current_mesh = &this->scene_tab->current_scene.scene_meshes[mesh];

					const bool is_selected = current_mesh->mesh_id == this->chosen_mesh_id;

					if (ImGui::Selectable(current_mesh->mesh_filename.c_str(), is_selected)) {
						instance.mesh = current_mesh;
						chosen_mesh = current_mesh;
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Separator();

			char instance_name[255];

			ImGui::Text("Instance name:");
			ImGui::SameLine();
			ImGui::InputText("##Instance name:", instance_name, sizeof(instance_name));

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Translation");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			ImGui::InputScalar("##X:", ImGuiDataType_Double, (void*)&(instance.tx), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::InputScalar("##Y:", ImGuiDataType_Double, (void*)&(instance.ty), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::InputScalar("##Z:", ImGuiDataType_Double, (void*)&(instance.tz), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Text("Scaling");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			ImGui::InputScalar("##SX:", ImGuiDataType_Double, (void*)&(instance.sx), &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::InputScalar("##SY:", ImGuiDataType_Double, (void*)&(instance.sy), &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::InputScalar("##SZ:", ImGuiDataType_Double, (void*)&(instance.sz), &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Text("Rotation (YXZ)");
			ImGui::Spacing();
			ImGui::Text("Yaw:  ");
			ImGui::SameLine();
			if (ImGui::InputScalar("##Yaw:  ", ImGuiDataType_Double, (void*)&(instance.yaw), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				Quaternion orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, instance.yaw, instance.pitch, instance.roll, this->scene_tab->current_scene.default_world_right, this->scene_tab->current_scene.default_world_up, this->scene_tab->current_scene.default_world_forward);

				instance.orientation = orientation;
			}

			ImGui::Text("Pitch:");
			ImGui::SameLine();
			if (ImGui::InputScalar("##Pitch:", ImGuiDataType_Double, (void*)&(instance.pitch), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				Quaternion orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, instance.yaw, instance.pitch, instance.roll, this->scene_tab->current_scene.default_world_right, this->scene_tab->current_scene.default_world_up, this->scene_tab->current_scene.default_world_forward);

				instance.orientation = orientation;
			}

			ImGui::Text("Roll: ");
			ImGui::SameLine();
			if (ImGui::InputScalar("##Roll: ", ImGuiDataType_Double, (void*)&(instance.roll), nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				Quaternion orientation = Quaternion::FromYawPitchRoll(this->scene_tab->rotation_orientation, instance.yaw, instance.pitch, instance.roll, this->scene_tab->current_scene.default_world_right, this->scene_tab->current_scene.default_world_up, this->scene_tab->current_scene.default_world_forward);

				instance.orientation = orientation;
			}

			ImGui::Text("Quaternion");
			ImGui::Text("X:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QX:", ImGuiDataType_Double, &instance.orientation.x, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				double qw = instance.orientation.w;
				double qx = instance.orientation.x;
				double qy = instance.orientation.y;
				double qz = instance.orientation.z;

				double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
				qw /= mag;
				qx /= mag;
				qy /= mag;
				qz /= mag;

				// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

				double new_yaw = 0;
				double new_pitch = 0;
				double new_roll = 0;

				instance.orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

				double old_normalized_yaw = fmod(instance.yaw, 2 * M_PI);

				if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
				if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

				double old_normalized_pitch = fmod(instance.pitch, 2 * M_PI);

				if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
				if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

				double old_normalized_roll = fmod(instance.roll, 2 * M_PI);

				if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
				if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

				double yaw_difference = new_yaw - old_normalized_yaw;
				double pitch_difference = new_pitch - old_normalized_pitch;
				double roll_difference = new_roll - old_normalized_roll;

				instance.yaw += yaw_difference;
				instance.pitch += pitch_difference;
				instance.roll += roll_difference;
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QY:", ImGuiDataType_Double, &instance.orientation.y, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				double qw = instance.orientation.w;
				double qx = instance.orientation.x;
				double qy = instance.orientation.y;
				double qz = instance.orientation.z;

				double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
				qw /= mag;
				qx /= mag;
				qy /= mag;
				qz /= mag;

				// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

				double new_yaw = 0;
				double new_pitch = 0;
				double new_roll = 0;

				instance.orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

				double old_normalized_yaw = fmod(instance.yaw, 2 * M_PI);

				if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
				if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

				double old_normalized_pitch = fmod(instance.pitch, 2 * M_PI);

				if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
				if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

				double old_normalized_roll = fmod(instance.roll, 2 * M_PI);

				if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
				if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

				double yaw_difference = new_yaw - old_normalized_yaw;
				double pitch_difference = new_pitch - old_normalized_pitch;
				double roll_difference = new_roll - old_normalized_roll;

				instance.yaw += yaw_difference;
				instance.pitch += pitch_difference;
				instance.roll += roll_difference;
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QZ:", ImGuiDataType_Double, &instance.orientation.z, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				double qw = instance.orientation.w;
				double qx = instance.orientation.x;
				double qy = instance.orientation.y;
				double qz = instance.orientation.z;

				double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
				qw /= mag;
				qx /= mag;
				qy /= mag;
				qz /= mag;

				// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

				double new_yaw = 0;
				double new_pitch = 0;
				double new_roll = 0;

				instance.orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

				double old_normalized_yaw = fmod(instance.yaw, 2 * M_PI);

				if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
				if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

				double old_normalized_pitch = fmod(instance.pitch, 2 * M_PI);

				if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
				if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

				double old_normalized_roll = fmod(instance.roll, 2 * M_PI);

				if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
				if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

				double yaw_difference = new_yaw - old_normalized_yaw;
				double pitch_difference = new_pitch - old_normalized_pitch;
				double roll_difference = new_roll - old_normalized_roll;

				instance.yaw += yaw_difference;
				instance.pitch += pitch_difference;
				instance.roll += roll_difference;
			}

			ImGui::Text("W:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QW:", ImGuiDataType_Double, &instance.orientation.w, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				double qw = instance.orientation.w;
				double qx = instance.orientation.x;
				double qy = instance.orientation.y;
				double qz = instance.orientation.z;

				double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
				qw /= mag;
				qx /= mag;
				qy /= mag;
				qz /= mag;

				// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

				double new_yaw = 0;
				double new_pitch = 0;
				double new_roll = 0;

				instance.orientation.GetAngles(this->scene_tab->rotation_orientation, new_yaw, new_pitch, new_roll);

				double old_normalized_yaw = fmod(instance.yaw, 2 * M_PI);

				if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
				if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

				double old_normalized_pitch = fmod(instance.pitch, 2 * M_PI);

				if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
				if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

				double old_normalized_roll = fmod(instance.roll, 2 * M_PI);

				if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
				if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

				double yaw_difference = new_yaw - old_normalized_yaw;
				double pitch_difference = new_pitch - old_normalized_pitch;
				double roll_difference = new_roll - old_normalized_roll;

				instance.yaw += yaw_difference;
				instance.pitch += pitch_difference;
				instance.roll += roll_difference;
			}

			// Position at center somehow?
			if (ImGui::Button("Add")) {
				this->target_instance = &instance;
				this->scene_tab->current_scene.scene_instances.push_back(std::move(instance));

				this->scene_tab->current_scene.total_instances++;
				this->target_instance->instance_name = std::string(instance_name);

				this->chosen_instance_id = this->target_instance->instance_id;
				this->chosen_instance_name = this->target_instance->instance_name;

				if (chosen_mesh != nullptr) {
					this->target_instance->mesh = chosen_mesh;
					this->chosen_mesh_id = this->target_instance->mesh->mesh_id;
					this->chosen_mesh_name = this->target_instance->mesh->mesh_filename;
				}
				else {
					std::cout << "No mesh was chosen. The instance will be added but not rendered, since there is no mesh." << std::endl;
				}

				this->target_instance->ROTATION_MATRIX = this->target_instance->orientation.get_rotationmatrix();
				this->target_instance->SCALING_MATRIX = Mat::scale_matrix(1, 1, 1);
				this->target_instance->TRANSLATION_MATRIX = Mat::translation_matrix(this->target_instance->tx, this->target_instance->ty, this->target_instance->tz);
				this->target_instance->MODEL_TO_WORLD = this->target_instance->TRANSLATION_MATRIX * this->target_instance->ROTATION_MATRIX * this->target_instance->SCALING_MATRIX;

				this->display_qx = this->target_instance->orientation.x;
				this->display_qy = this->target_instance->orientation.y;
				this->display_qz = this->target_instance->orientation.z;
				this->display_qw = this->target_instance->orientation.w;

				if (this->scene_tab->show_transform_axes) {
					update_transform_axes();
				}

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}


		}



		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Separator();

		static ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable;

		ImGui::BeginTable("##Tableee", 4, table_flags);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Visible", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_NoHide);
		ImGui::TableHeadersRow();

		for (uint8_t instance = 0; instance < this->scene_tab->current_scene.scene_instances.size(); instance++) {
			Instance* current_instance = &this->scene_tab->current_scene.scene_instances.at(instance);
			if (current_instance->is_light_source || current_instance->is_axes) continue;

			ImGui::TableNextRow();
			ImGui::TableNextColumn();



			bool is_selected = (current_instance->instance_id == this->target_instance->instance_id);

			char selectable_id[255] = "";
			sprintf_s(selectable_id, sizeof(selectable_id), "##selectable_id_%d", current_instance->instance_id);

			ImVec2 selectable_render_pos = ImGui::GetCursorPos();
			if (ImGui::Selectable(selectable_id, is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(FLT_MAX, 25))) {
				this->target_instance->has_axes = false;
				this->target_instance = current_instance;
				this->target_instance->has_axes = true;

				this->chosen_instance_id = this->target_instance->instance_id;
				this->chosen_instance_name = this->target_instance->instance_name;

				this->chosen_mesh_id = this->target_instance->mesh->mesh_id;
				this->chosen_mesh_name = this->target_instance->mesh->mesh_filename;

				this->display_qx = this->target_instance->orientation.x;
				this->display_qy = this->target_instance->orientation.y;
				this->display_qz = this->target_instance->orientation.z;
				this->display_qw = this->target_instance->orientation.w;

				if (this->target_instance->show && this->scene_tab->show_transform_axes) {
					update_transform_axes();
				}
			}

			ImGui::SameLine();

			/*
			const char* is_visible;
			if (current_instance->show) {
				is_visible = "Yes";
			}
			else {
				is_visible = "No";
			}
			*/

			char instance_name[255];
			strcpy_s(instance_name, sizeof(instance_name), current_instance->instance_name.c_str());

			char instance_name_label[255];
			sprintf_s(instance_name_label, sizeof(instance_name_label), "##name_%d", current_instance->instance_id);


			//char instance_label[255];
			//sprintf_s(instance_label, sizeof(instance_label), "%s##instance_%d", instance_name, std::to_string(current_instance->instance_id).c_str());

			ImGui::SetCursorPos(selectable_render_pos);
			if (ImGui::InputText(instance_name_label, instance_name, sizeof(instance_name), ImGuiInputTextFlags_None, nullptr, nullptr)) {
				current_instance->instance_name = std::string(instance_name);
			}

			//ImGui::Text(name)

			ImGui::TableNextColumn();
			ImGui::SameLine();

			ImGui::Text("%d", current_instance->instance_id);
			//strcpy_s(selectable_id + 4, sizeof(selectable_id) - 4, std::to_string(current_instance->instance_id).c_str());

			ImGui::TableNextColumn();
			ImGui::SameLine();

			char visibility_label[255];
			sprintf_s(visibility_label, sizeof(visibility_label), "##is_visible_%d", current_instance->instance_id);

			if (ImGui::Checkbox(visibility_label, &current_instance->show)) {
				if (current_instance->show && this->scene_tab->show_transform_axes) {
					update_transform_axes();
				}
			}

			ImGui::TableNextColumn(); //ImGui::Text(is_visible); 
			ImGui::SameLine();

			char delete_label[255] = "Delete##delete_";
			sprintf_s(delete_label, sizeof(delete_label), "Delete##delete_%d", current_instance->instance_id);

			if (ImGui::Button(delete_label)) {
				this->scene_tab->current_scene.scene_instances.erase(this->scene_tab->current_scene.scene_instances.begin() + instance);
				this->scene_tab->current_scene.total_instances--;

				update_instances_queue_reconstruction(is_selected);
			}

			/*
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
			*/
		}

		ImGui::EndTable();
		//ImGui::TreePop();

		ImGui::Separator();

		if (this->target_instance != nullptr) {
			ImGui::Text("Choose mesh:");
			ImGui::SameLine();
			// Make sure that mesh is not a nullptr here in the case that no mesh is attached for a scene without a light source in principle
			if (ImGui::BeginCombo("##Choose mesh:", this->chosen_mesh_name.c_str(), ImGuiComboFlags_None)) {
				for (uint8_t mesh = 0; mesh < this->scene_tab->current_scene.scene_meshes.size(); mesh++) {
					Mesh* current_mesh = &this->scene_tab->current_scene.scene_meshes[mesh];

					const bool is_selected = current_mesh->mesh_id == this->chosen_mesh_id;

					if (ImGui::Selectable(current_mesh->mesh_filename.c_str(), is_selected)) {
						this->target_instance->mesh = current_mesh;
						this->chosen_mesh_name = current_mesh->mesh_filename;
						this->chosen_mesh_id = current_mesh->mesh_id;
					}

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Text("Translation speed:");
			ImGui::SetItemTooltip("How sensitive the menu translation slider is to clicking and dragging (higher = more change)");
			ImGui::SameLine();
			ImGui::DragScalar("##ITranslation speed (menu):", ImGuiDataType_Double, &this->scene_tab->menu_translation_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();



			ImGui::Separator();
			ImGui::Text("Rotation speed:");
			ImGui::SetItemTooltip("(Menu) How sensitive the menu rotation slider is to clicking and dragging (higher = more change): ");
			ImGui::SameLine();
			ImGui::DragScalar("##IRotation speed (menu):", ImGuiDataType_Double, &this->scene_tab->menu_rotation_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();

			ImGui::Text("Scaling speed:");
			ImGui::SetItemTooltip("(Menu) How sensitive the menu scaling slider is to clicking and dragging (higher = more change): ");
			ImGui::SameLine();
			ImGui::DragScalar("##IScaling speed:", ImGuiDataType_Double, &this->scene_tab->menu_scaling_speed, 0.01, &zero, nullptr, "%.3f", ImGuiSliderFlags_None);

			ImGui::Separator();



			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();




			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::PushItemWidth(60);

			ImGui::Text("Translation");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			if (ImGui::DragScalar("##X:", ImGuiDataType_Double, (void*)&(this->target_instance->tx), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceTranslation();
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##Y:", ImGuiDataType_Double, (void*)&(this->target_instance->ty), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceTranslation();
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##Z:", ImGuiDataType_Double, (void*)&(this->target_instance->tz), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceTranslation();
			}

			ImGui::Separator();

			ImGui::Text("Scaling");
			ImGui::Spacing();
			ImGui::Text("X:");
			ImGui::SameLine();

			if (ImGui::DragScalar("##SX:", ImGuiDataType_Double, (void*)&(this->target_instance->sx), this->scene_tab->menu_scaling_speed, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceScaling();
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##SY:", ImGuiDataType_Double, (void*)&(this->target_instance->sy), this->scene_tab->menu_scaling_speed, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceScaling();
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##SZ:", ImGuiDataType_Double, (void*)&(this->target_instance->sz), this->scene_tab->menu_scaling_speed, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceScaling();
			}

			ImGui::Separator();

			ImGui::Text("Rotation (YXZ)");
			ImGui::Spacing();
			ImGui::Text("Yaw:  ");
			ImGui::SameLine();
			if (ImGui::DragScalar("##Yaw:  ", ImGuiDataType_Double, (void*)&(this->target_instance->yaw), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(true);
			}

			ImGui::Text("Pitch:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##Pitch:", ImGuiDataType_Double, (void*)&(this->target_instance->pitch), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(true);
			}

			ImGui::Text("Roll: ");
			ImGui::SameLine();
			if (ImGui::DragScalar("##Roll: ", ImGuiDataType_Double, (void*)&(this->target_instance->roll), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(true);
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Quaternion");
			ImGui::Text("X:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QX:", ImGuiDataType_Double, &this->display_qx, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(false);
			}

			ImGui::Text("Y:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QY:", ImGuiDataType_Double, &this->display_qy, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(false);
			}

			ImGui::Text("Z:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QZ:", ImGuiDataType_Double, &this->display_qz, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(false);
			}

			ImGui::Text("W:");
			ImGui::SameLine();
			if (ImGui::DragScalar("##QW:", ImGuiDataType_Double, &this->display_qw, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateInstanceRotation(false);
			}

			ImGui::Separator();

			ImGui::Text("Model-to-world matrix:");
			if (ImGui::BeginTable("Model-to-world matrix", 4, ImGuiTableFlags_None)) {
				for (uint8_t row = 1; row < 5; row++) {
					ImGui::TableNextRow();
					for (uint8_t col = 1; col < 5; col++) {
						ImGui::TableNextColumn();

						double number = this->target_instance->MODEL_TO_WORLD.get(row, col);

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

						double number = this->target_instance->TRANSLATION_MATRIX.get(row, col);

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

						double number = this->target_instance->ROTATION_MATRIX.get(row, col);

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

						double number = this->target_instance->SCALING_MATRIX.get(row, col);

						ImGui::Text(std::to_string(number).c_str());
					}
				}

			}

			ImGui::EndTable();

			ImGui::Separator();
		}

		// DELETE AXES
	}


	void update_transform_axes() {
		// If not false, make light_source->instance->has_axes = false to disable the axes for the light source
		if (this->scene_tab->show_transform_axes && this->target_instance != nullptr) {
			this->target_instance->has_axes = true;
			this->scene_tab->current_scene.axes_instance = *this->target_instance;
			this->scene_tab->current_scene.axes_instance.is_axes = true;
			this->scene_tab->current_scene.axes_instance.has_axes = false;
			this->scene_tab->current_scene.axes_instance.instance_id = 13377331;
			this->scene_tab->current_scene.axes_instance.instance_name = "instance_axes";
			this->scene_tab->current_scene.axes_instance.is_light_source = false;
			this->scene_tab->current_scene.axes_instance.mesh = &this->scene_tab->current_scene.axes_mesh;
			this->scene_tab->current_scene.axes_instance.sx = 1;
			this->scene_tab->current_scene.axes_instance.sy = 1;
			this->scene_tab->current_scene.axes_instance.sz = 1;
			this->scene_tab->current_scene.axes_instance.SCALING_MATRIX = Mat::scale_matrix(1, 1, 1);
			this->scene_tab->current_scene.axes_instance.MODEL_TO_WORLD = this->scene_tab->current_scene.axes_instance.TRANSLATION_MATRIX * this->scene_tab->current_scene.axes_instance.ROTATION_MATRIX * this->scene_tab->current_scene.axes_instance.SCALING_MATRIX;
			this->scene_tab->current_scene.axes_instance.show = true;
		}

	}

	void update_instances_queue_reconstruction(bool selected_deleted) {
		bool target_set = false;
		bool light_set = false;
		for (uint32_t instance = 0; instance < this->scene_tab->current_scene.scene_instances.size(); instance++) {
			if (target_set && light_set) break;

			Instance* current_instance = &this->scene_tab->current_scene.scene_instances[instance];

			if (!current_instance->is_light_source) {
				if (selected_deleted) {
					this->target_instance = current_instance;

					this->chosen_instance_id = this->target_instance->instance_id;
					this->chosen_instance_name = this->target_instance->instance_name;

					this->chosen_mesh_id = this->target_instance->mesh->mesh_id;
					this->chosen_mesh_name = this->target_instance->mesh->mesh_filename;

					this->display_qx = this->target_instance->orientation.x;
					this->display_qy = this->target_instance->orientation.y;
					this->display_qz = this->target_instance->orientation.z;
					this->display_qw = this->target_instance->orientation.w;

					this->target_instance->has_axes = true;
					if (this->scene_tab->show_transform_axes) {
						update_transform_axes();
					}
					target_set = true;
				}
				else {
					this->target_instance = current_instance;
					target_set = true;
					//update_transform_axes();
				}
				
			}
			else {
				this->scene_tab->current_scene.light_source.instance = current_instance;
				light_set = true;
				//update_transform_axes();

			}

		}

		if (!target_set) {
			this->target_instance = nullptr;

			this->chosen_instance_id = 0;
			this->chosen_instance_name = "";

			this->chosen_mesh_id = 0;
			this->chosen_mesh_name = "";

			this->display_qx = 0;
			this->display_qy = 0;
			this->display_qz = 0;
			this->display_qw = 1;
		}
	}
};
