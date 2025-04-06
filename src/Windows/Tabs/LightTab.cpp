#include "LightTab.h"

LightTab::LightTab(SceneTab* scene_tab) {
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

	this->light_display_dir_x = this->scene_tab->current_scene.light_source.direction.get(1, 1);
	this->light_display_dir_y = this->scene_tab->current_scene.light_source.direction.get(2, 1);
	this->light_display_dir_z = this->scene_tab->current_scene.light_source.direction.get(3, 1);

	this->light_display_up_x = this->scene_tab->current_scene.light_source.up.get(1, 1);
	this->light_display_up_y = this->scene_tab->current_scene.light_source.up.get(2, 1);
	this->light_display_up_z = this->scene_tab->current_scene.light_source.up.get(3, 1);
}

void LightTab::UpdateLightTranslation() {
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

void LightTab::UpdateLightScaling() {
	if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
		this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX = Mat::scale_matrix(this->scene_tab->current_scene.light_source.instance->sx, this->scene_tab->current_scene.light_source.instance->sy, this->scene_tab->current_scene.light_source.instance->sz);
		this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX * this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX * this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX;
	}
}

void LightTab::UpdateLightRotation(uint8_t rotation_type) {
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

		this->scene_tab->current_scene.light_source.direction.normalize();

		this->scene_tab->current_scene.light_source.up.set(this->light_display_up_x, 1, 1);
		this->scene_tab->current_scene.light_source.up.set(this->light_display_up_y, 2, 1);
		this->scene_tab->current_scene.light_source.up.set(this->light_display_up_z, 3, 1);

		this->scene_tab->current_scene.light_source.up.normalize();

		//Mat light_right = Mat::CrossProduct3D(this->scene_tab->current_scene.light_source.up, this->scene_tab->current_scene.light_source.direction);

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
		this->scene_tab->current_scene.light_source.instance->yaw = this->scene_tab->current_scene.light_source.yaw;
		this->scene_tab->current_scene.light_source.instance->pitch = this->scene_tab->current_scene.light_source.pitch;
		this->scene_tab->current_scene.light_source.instance->roll = this->scene_tab->current_scene.light_source.roll;

		this->scene_tab->current_scene.light_source.instance->orientation = this->scene_tab->current_scene.light_source.orientation;

		this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX = this->scene_tab->current_scene.light_source.instance->orientation.get_rotationmatrix();
		this->scene_tab->current_scene.light_source.instance->MODEL_TO_WORLD = this->scene_tab->current_scene.light_source.instance->TRANSLATION_MATRIX * this->scene_tab->current_scene.light_source.instance->ROTATION_MATRIX * this->scene_tab->current_scene.light_source.instance->SCALING_MATRIX;
	}

	if (this->scene_tab->show_transform_axes) {
		update_transform_axes();
	}
}

void LightTab::update_transform_axes() {
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

		if (this->scene_tab->rotation_orientation == Orientation::local) {
			this->scene_tab->current_scene.axes_instance.MODEL_TO_WORLD = this->scene_tab->current_scene.axes_instance.TRANSLATION_MATRIX * this->scene_tab->current_scene.axes_instance.ROTATION_MATRIX * this->scene_tab->current_scene.axes_instance.SCALING_MATRIX;
		}
		else if (this->scene_tab->rotation_orientation == Orientation::world) {
			this->scene_tab->current_scene.axes_instance.MODEL_TO_WORLD = this->scene_tab->current_scene.axes_instance.TRANSLATION_MATRIX;
		}

		
		this->scene_tab->current_scene.axes_instance.show = true;
	}

}

void LightTab::draw() {
	// Toggle rendering of transform axes
	
	ImGui::Checkbox("##Enabled", &this->scene_tab->current_scene.light_source.enabled);
	ImGui::SameLine();
	ImGui::Text("Enabled");

	if (this->scene_tab->current_scene.light_source.enabled) {
		if (ImGui::BeginCombo("##Rotation orientation", this->scene_tab->display_rotation_orientation, ImGuiComboFlags_None)) {
			for (uint8_t rotation_orientation = 0; rotation_orientation < 2; rotation_orientation++) {
				Orientation orientation = (Orientation)rotation_orientation;

				const bool is_selected = orientation == this->scene_tab->rotation_orientation;

				char label[255] = "";

				if (orientation == Orientation::local) {
					sprintf_s(label, sizeof(label), "Local##orientation_%d", rotation_orientation);
				}
				else if (orientation == Orientation::world) {
					sprintf_s(label, sizeof(label), "World##orientation_%d", rotation_orientation);
				}

				if (ImGui::Selectable(label, is_selected)) {
					switch (orientation) {
					case Orientation::local:
						this->scene_tab->rotation_orientation = Orientation::local;
						strcpy_s(this->scene_tab->display_rotation_orientation, sizeof(this->scene_tab->display_rotation_orientation), "Local");
						break;
					case Orientation::world:
						this->scene_tab->rotation_orientation = Orientation::world;
						strcpy_s(this->scene_tab->display_rotation_orientation, sizeof(this->scene_tab->display_rotation_orientation), "World");
						break;
					default:
						break;
					}
				}

				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}

				
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Text("Rotation orientation");
		

		if (ImGui::Checkbox("##Show transform axes", &this->scene_tab->show_transform_axes)) {
			if (this->scene_tab->show_transform_axes) {
				update_transform_axes();
			}
		}
		ImGui::SameLine();
		ImGui::Text("Show transform axes");

		if (ImGui::Checkbox("##Attach model", &this->scene_tab->current_scene.light_source.has_model)) {
			if (this->scene_tab->current_scene.light_source.instance != nullptr) {
				if (!this->scene_tab->current_scene.light_source.has_model) {
					this->scene_tab->current_scene.light_source.instance->show = false;
				}
				else {
					this->scene_tab->current_scene.light_source.instance->show = true;
				}
			}

		}
		ImGui::SetItemTooltip("If enabled, you can choose a mesh to be rendered at the light source position and orientation.");
		ImGui::SameLine();
		ImGui::Text("Attach model");

		if (ImGui::ColorEdit4("##LColor", this->display_LIGHT_COLOR, ImGuiColorEditFlags_NoInputs)) {
			uint8_t red = this->display_LIGHT_COLOR[0] * 255.0;
			uint8_t green = this->display_LIGHT_COLOR[1] * 255.0;
			uint8_t blue = this->display_LIGHT_COLOR[2] * 255.0;
			uint8_t alpha = this->display_LIGHT_COLOR[3] * 255.0;

			uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

			this->scene_tab->current_scene.light_source.color = color;
		};
		ImGui::SameLine();
		ImGui::Text("Color");


		ImGui::PushItemWidth(100);
		if (ImGui::BeginCombo("##Lighting type", this->current_lighting_type, ImGuiComboFlags_None)) {
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
		ImGui::SameLine();
		ImGui::Text("Lighting type");

		ImGui::Spacing();
		
		if (ImGui::BeginCombo("##Shading type", this->current_shading_type, ImGuiComboFlags_None)) {
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
		ImGui::SameLine();
		ImGui::Text("Shading type");


		ImGui::PushItemWidth(50);

		ImGui::DragScalar("##Intensity", ImGuiDataType_Double, &this->scene_tab->current_scene.light_source.intensity, 0.005, &zero, nullptr, "%.3lf", ImGuiSliderFlags_None);
		ImGui::SetItemTooltip("The light intensity, the higher the brighter it is and vice-versa.");
		ImGui::SameLine();
		ImGui::Text("Intensity");

		ImGui::DragScalar("##Minimum exposure", ImGuiDataType_Double, &this->scene_tab->current_scene.light_source.minimum_exposure, 0.005, &zero, &one, "%.3lf", ImGuiSliderFlags_None);
		ImGui::SetItemTooltip("If the final light intensity (within the program) is lower than this\nSets the triangle/vertex/pixel color to the scene's ambient/fill color.\nThis is essentially the 'minimum' color treshold so that it isn't invisible.");
		ImGui::SameLine();
		ImGui::Text("Minimum exposure");

		ImGui::Spacing();

		ImGui::Separator();

		if (this->scene_tab->current_scene.light_source.has_model) {
			// Load mesh from file and send it to scene meshes if not already there
			
			static bool errored = 0;
			if (ImGui::Button("Load mesh")) {
				static const nfdchar_t* obj_filter = "obj";
				nfdchar_t* mesh_path = nullptr;
				nfdresult_t result = NFD_OpenDialog(obj_filter, this->scene_tab->models_folder, &mesh_path);

				if (result == NFD_OKAY) {
					std::string tmp_path = mesh_path;

					std::string mesh_folder = tmp_path.substr(0, tmp_path.find_last_of("\\")) + '\\';
					std::string mesh_filename = tmp_path.substr(tmp_path.find_last_of("\\") + 1);

					char mesh_path[255] = "";
					sprintf_s(mesh_path, sizeof(mesh_path), "%s%s", mesh_folder.c_str(), mesh_filename.c_str());

					// Makes sure that the mesh folder matches the set models folder
					if (!strcmp(mesh_folder.c_str(), this->scene_tab->models_folder)) {
						// Load mesh only if a mesh with the same filename isn't loaded (to avoid duplication)
						if (this->scene_tab->current_scene.get_mesh_ptr(mesh_filename) == nullptr) {
							Mesh loaded_mesh = Mesh(mesh_path, mesh_filename.c_str(), this->scene_tab->current_scene.total_ever_meshes);
							this->scene_tab->current_scene.total_meshes++;
							this->chosen_light_mesh_id = loaded_mesh.mesh_id;
							this->chosen_light_mesh_name = loaded_mesh.mesh_filename;
							this->scene_tab->current_scene.scene_meshes.push_back(std::move(loaded_mesh));

							this->scene_tab->current_scene.light_source.mesh = &this->scene_tab->current_scene.scene_meshes.back();
							this->scene_tab->current_scene.light_source.instance->mesh = &this->scene_tab->current_scene.scene_meshes.back();
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
			ImGui::SetItemTooltip("The .obj mesh file MUST be within the MODELS folder! (This is because scenes are saved with only the filename, not path, as it is assumed to be relative to the models folder)");

			if (errored) {
				ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "Mesh could not be loaded!\Make sure the mesh is within the set models folder.");
			}
			
			ImGui::PushItemWidth(100);
			// Make sure that mesh is not a nullptr here in the case that no mesh is attached for a scene without a light source in principle
			if (ImGui::BeginCombo("##Choose mesh", this->chosen_light_mesh_name.c_str(), ImGuiComboFlags_None)) {
				for (uint8_t mesh = 0; mesh < this->scene_tab->current_scene.scene_meshes.size(); mesh++) {
					Mesh* current_mesh = &this->scene_tab->current_scene.scene_meshes[mesh];

					bool is_selected = false;
					if (this->scene_tab->current_scene.light_source.mesh != nullptr) {
						is_selected = this->scene_tab->current_scene.light_source.mesh->mesh_id == this->chosen_light_mesh_id;
					}

					if (ImGui::Selectable(current_mesh->mesh_filename.c_str(), is_selected)) {
						this->scene_tab->current_scene.light_source.mesh = current_mesh;
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
			ImGui::SameLine();
			ImGui::Text("Choose mesh");

			ImGui::PushItemWidth(50);
		}

		ImGui::Separator();
		
		ImGui::DragScalar("##LTranslation speed (menu)", ImGuiDataType_Double, &this->scene_tab->menu_translation_speed, 0.005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
		ImGui::SetItemTooltip("How sensitive the menu translation slider is to clicking and dragging (higher = more change)");
		ImGui::SameLine();
		ImGui::Text("Translation speed");

		ImGui::Separator();

		ImGui::Separator();
		
		
		
		ImGui::DragScalar("##LRotation speed (menu)", ImGuiDataType_Double, &this->scene_tab->menu_rotation_speed, 0.0005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
		ImGui::SetItemTooltip("(Menu) How sensitive the menu rotation slider is to clicking and dragging (higher = more change): ");
		ImGui::SameLine();
		ImGui::Text("Rotation speed");


		ImGui::Separator();
		
		ImGui::DragScalar("##LScaling speed", ImGuiDataType_Double, &this->scene_tab->menu_scaling_speed, 0.0005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
		ImGui::SetItemTooltip("(Menu) How sensitive the menu scaling slider is to clicking and dragging (higher = more change): ");
		ImGui::SameLine();
		ImGui::Text("Scaling speed");

		ImGui::Separator();

		ImGui::Text("Direction");
		ImGui::Spacing();

		
		

		if (ImGui::DragScalar("##LDX", ImGuiDataType_Double, (void*)&(this->light_display_dir_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("X");
		
		
		if (ImGui::DragScalar("##LDY", ImGuiDataType_Double, (void*)&(this->light_display_dir_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("Y");

		
		
		if (ImGui::DragScalar("##LDZ", ImGuiDataType_Double, (void*)&(this->light_display_dir_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("Z");

		ImGui::Spacing();

		ImGui::Text("Up");
		ImGui::Spacing();

		
		

		if (ImGui::DragScalar("##LUX", ImGuiDataType_Double, (void*)&(this->light_display_up_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("X");

		
		
		if (ImGui::DragScalar("##LUY", ImGuiDataType_Double, (void*)&(this->light_display_up_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("Y");

		
		
		if (ImGui::DragScalar("##LUZ", ImGuiDataType_Double, (void*)&(this->light_display_up_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Direction);
		}
		ImGui::SameLine();
		ImGui::Text("Z");

		ImGui::Separator();

		ImGui::Text("Translation");
		ImGui::Spacing();

		
		

		if (ImGui::DragScalar("##LX", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.tx), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation();
		}
		ImGui::SameLine();
		ImGui::Text("X");

		
		
		if (ImGui::DragScalar("##LY", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.ty), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation();
		}
		ImGui::SameLine();
		ImGui::Text("Y");

		
		
		if (ImGui::DragScalar("##LZ", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.tz), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation();
		}
		ImGui::SameLine();
		ImGui::Text("Z");

		ImGui::Separator();

		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
			ImGui::Text("Scaling");
			ImGui::Spacing();
			
			

			if (ImGui::DragScalar("##LSX", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sx), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightScaling();
			}
			ImGui::SameLine();
			ImGui::Text("X");

			
			
			if (ImGui::DragScalar("##LSY", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sy), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightScaling();
			}
			ImGui::SameLine();
			ImGui::Text("Y");

			
			
			if (ImGui::DragScalar("##LSZ", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.instance->sz), 0.05, &zero, nullptr, "%.3f", ImGuiSliderFlags_None)) {
				UpdateLightScaling();
			}
			ImGui::SameLine();
			ImGui::Text("Z");

			ImGui::Separator();
		}

		ImGui::Text("Rotation (YXZ)");
		ImGui::Spacing();
		
		
		if (ImGui::DragScalar("##LYaw", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.yaw), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Euler);
		}
		ImGui::SameLine();
		ImGui::Text("Yaw");

		
		
		if (ImGui::DragScalar("##LPitch", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.pitch), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Euler);
		}
		ImGui::SameLine();
		ImGui::Text("Pitch");

		
		
		if (ImGui::DragScalar("##LRoll", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.light_source.roll), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Euler);
		}
		ImGui::SameLine();
		ImGui::Text("Roll");

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Quaternion");
		
		
		if (ImGui::DragScalar("##LQX", ImGuiDataType_Double, &this->light_display_qx, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Quaternion);
		}
		ImGui::SameLine();
		ImGui::Text("X");

		
		
		if (ImGui::DragScalar("##LQY", ImGuiDataType_Double, &this->light_display_qy, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Quaternion);
		}
		ImGui::SameLine();
		ImGui::Text("Y");

		
		
		if (ImGui::DragScalar("##LQZ", ImGuiDataType_Double, &this->light_display_qz, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Quaternion);
		}
		ImGui::SameLine();
		ImGui::Text("Z");
		
		if (ImGui::DragScalar("##LQW", ImGuiDataType_Double, &this->light_display_qw, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(RotationType_Quaternion);
		}
		ImGui::SameLine();
		ImGui::Text("W");

		ImGui::Separator();

		if (this->scene_tab->current_scene.light_source.has_model && this->scene_tab->current_scene.light_source.instance != nullptr) {
			ImGui::Text("Model-to-world matrix");
			if (ImGui::BeginTable("Model-to-world matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
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

			ImGui::Text("Translation matrix");
			if (ImGui::BeginTable("Translation matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
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

			ImGui::Text("Rotation matrix");
			if (ImGui::BeginTable("Rotation matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
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

			ImGui::Text("Scaling matrix");
			if (ImGui::BeginTable("Scaling matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
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

