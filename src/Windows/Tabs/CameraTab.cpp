#include "CameraTab.h"

CameraTab::CameraTab(SceneTab* scene_tab) {
	this->scene_tab = scene_tab;

	this->camera_display_qx = this->scene_tab->current_scene.camera.orientation.x;
	this->camera_display_qy = this->scene_tab->current_scene.camera.orientation.y;
	this->camera_display_qz = this->scene_tab->current_scene.camera.orientation.z;
	this->camera_display_qw = this->scene_tab->current_scene.camera.orientation.w;

	this->camera_display_dir_x = this->scene_tab->current_scene.camera.direction.get(1, 1);
	this->camera_display_dir_y = this->scene_tab->current_scene.camera.direction.get(2, 1);
	this->camera_display_dir_z = this->scene_tab->current_scene.camera.direction.get(3, 1);

	this->camera_display_up_x = this->scene_tab->current_scene.camera.up.get(1, 1);
	this->camera_display_up_y = this->scene_tab->current_scene.camera.up.get(2, 1);
	this->camera_display_up_z = this->scene_tab->current_scene.camera.up.get(3, 1);

	this->camera_display_tx = this->scene_tab->current_scene.camera.position.get(1, 1);
	this->camera_display_ty = this->scene_tab->current_scene.camera.position.get(2, 1);
	this->camera_display_tz = this->scene_tab->current_scene.camera.position.get(3, 1);
}

void CameraTab::UpdateCameraTranslation(bool display_only) {
	if (display_only) {
		this->camera_display_tx = this->scene_tab->current_scene.camera.position.get(1, 1);
		this->camera_display_ty = this->scene_tab->current_scene.camera.position.get(2, 1);
		this->camera_display_tz = this->scene_tab->current_scene.camera.position.get(3, 1);
	}
	else {
		this->scene_tab->current_scene.camera.position = Mat(
			{
				{this->camera_display_tx},
				{this->camera_display_ty},
				{this->camera_display_tz},
				{1}
			}
		, 4, 1);
	}

	this->scene_tab->current_scene.camera.LookAt();
	this->scene_tab->current_scene.camera.update_view_inverse();
}

void CameraTab::UpdateCameraRotation(uint8_t rotation_type, bool display_only) {
	// If updating euler angles
	if (rotation_type == RotationType_Euler) {
		Quaternion orientation = Quaternion::FromYawPitchRoll(Orientation::local, this->scene_tab->current_scene.camera.yaw, this->scene_tab->current_scene.camera.pitch, this->scene_tab->current_scene.camera.roll, this->scene_tab->current_scene.camera.default_right, this->scene_tab->current_scene.camera.default_up, this->scene_tab->current_scene.camera.default_direction);

		this->scene_tab->current_scene.camera.orientation = orientation;

		this->scene_tab->current_scene.camera.direction = this->scene_tab->current_scene.camera.orientation.get_rotationmatrix() * this->scene_tab->current_scene.camera.default_direction;
		this->scene_tab->current_scene.camera.up = this->scene_tab->current_scene.camera.orientation.get_rotationmatrix() * this->scene_tab->current_scene.camera.default_up;

		this->camera_display_qx = orientation.x;
		this->camera_display_qy = orientation.y;
		this->camera_display_qz = orientation.z;
		this->camera_display_qw = orientation.w;

		this->camera_display_dir_x = this->scene_tab->current_scene.camera.direction.get(1, 1);
		this->camera_display_dir_y = this->scene_tab->current_scene.camera.direction.get(2, 1);
		this->camera_display_dir_z = this->scene_tab->current_scene.camera.direction.get(3, 1);

		this->camera_display_up_x = this->scene_tab->current_scene.camera.up.get(1, 1);
		this->camera_display_up_y = this->scene_tab->current_scene.camera.up.get(2, 1);
		this->camera_display_up_z = this->scene_tab->current_scene.camera.up.get(3, 1);
	}

	// If updating quaternion
	else if (rotation_type == RotationType_Quaternion) {
		if (!display_only) {
			double qw = this->camera_display_qw;
			double qx = this->camera_display_qx;
			double qy = this->camera_display_qy;
			double qz = this->camera_display_qz;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			this->scene_tab->current_scene.camera.orientation.w = qw;
			this->scene_tab->current_scene.camera.orientation.x = qx;
			this->scene_tab->current_scene.camera.orientation.y = qy;
			this->scene_tab->current_scene.camera.orientation.z = qz;
		}
		else {
			this->camera_display_qx = this->scene_tab->current_scene.camera.orientation.x;
			this->camera_display_qy = this->scene_tab->current_scene.camera.orientation.y;
			this->camera_display_qz = this->scene_tab->current_scene.camera.orientation.z;
			this->camera_display_qw = this->scene_tab->current_scene.camera.orientation.w;
		}

		if (!display_only) {

			// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

			double new_yaw = 0;
			double new_pitch = 0;
			double new_roll = 0;

			this->scene_tab->current_scene.camera.orientation.GetAngles(Orientation::local, new_yaw, new_pitch, new_roll);

			double old_normalized_yaw = fmod(this->scene_tab->current_scene.camera.yaw, 2 * M_PI);

			if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
			if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

			double old_normalized_pitch = fmod(this->scene_tab->current_scene.camera.pitch, 2 * M_PI);

			if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
			if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

			double old_normalized_roll = fmod(this->scene_tab->current_scene.camera.roll, 2 * M_PI);

			if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
			if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

			double yaw_difference = new_yaw - old_normalized_yaw;
			double pitch_difference = new_pitch - old_normalized_pitch;
			double roll_difference = new_roll - old_normalized_roll;

			this->scene_tab->current_scene.camera.yaw += yaw_difference;
			this->scene_tab->current_scene.camera.pitch += pitch_difference;
			this->scene_tab->current_scene.camera.roll += roll_difference;

			this->scene_tab->current_scene.camera.direction = this->scene_tab->current_scene.camera.orientation.get_rotationmatrix() * this->scene_tab->current_scene.camera.default_direction;
			this->scene_tab->current_scene.camera.up = this->scene_tab->current_scene.camera.orientation.get_rotationmatrix() * this->scene_tab->current_scene.camera.default_up;

		}


		this->camera_display_dir_x = this->scene_tab->current_scene.camera.direction.get(1, 1);
		this->camera_display_dir_y = this->scene_tab->current_scene.camera.direction.get(2, 1);
		this->camera_display_dir_z = this->scene_tab->current_scene.camera.direction.get(3, 1);

		this->camera_display_up_x = this->scene_tab->current_scene.camera.up.get(1, 1);
		this->camera_display_up_y = this->scene_tab->current_scene.camera.up.get(2, 1);
		this->camera_display_up_z = this->scene_tab->current_scene.camera.up.get(3, 1);
	}

	else if (rotation_type == RotationType_Direction) {
		if (!display_only) {
			this->scene_tab->current_scene.camera.direction.set(this->camera_display_dir_x, 1, 1);
			this->scene_tab->current_scene.camera.direction.set(this->camera_display_dir_y, 2, 1);
			this->scene_tab->current_scene.camera.direction.set(this->camera_display_dir_z, 3, 1);

			this->scene_tab->current_scene.camera.direction.normalize();

			this->scene_tab->current_scene.camera.up.set(this->camera_display_up_x, 1, 1);
			this->scene_tab->current_scene.camera.up.set(this->camera_display_up_y, 2, 1);
			this->scene_tab->current_scene.camera.up.set(this->camera_display_up_z, 3, 1);

			this->scene_tab->current_scene.camera.up.normalize();
		}


		// Gets yaw and pitch representing the rotation from the default camera direction to the new camera direction
		Quaternion::GetAnglesFromDirection(Orientation::local, this->scene_tab->current_scene.camera.default_direction, this->scene_tab->current_scene.camera.direction, this->scene_tab->current_scene.camera.yaw, this->scene_tab->current_scene.camera.pitch, this->scene_tab->current_scene.camera.roll);

		// Gets roll representing the rotation from the default camera up to the camera up
		// Assumes default direction of (0, 0, 1)
		Quaternion::GetRoll(Orientation::local, this->scene_tab->current_scene.camera.direction, this->scene_tab->current_scene.camera.up, this->scene_tab->current_scene.camera.yaw, this->scene_tab->current_scene.camera.pitch, this->scene_tab->current_scene.camera.roll);

		if (!display_only) {
			this->scene_tab->current_scene.camera.orientation = Quaternion::FromYawPitchRoll(Orientation::local, this->scene_tab->current_scene.camera.yaw, this->scene_tab->current_scene.camera.pitch, this->scene_tab->current_scene.camera.roll, this->scene_tab->current_scene.camera.default_right, this->scene_tab->current_scene.camera.default_up, this->scene_tab->current_scene.camera.default_direction);
		}

		this->camera_display_qx = this->scene_tab->current_scene.camera.orientation.x;
		this->camera_display_qy = this->scene_tab->current_scene.camera.orientation.y;
		this->camera_display_qz = this->scene_tab->current_scene.camera.orientation.z;
		this->camera_display_qw = this->scene_tab->current_scene.camera.orientation.w;

		/*
		this->camera_display_dir_x = this->scene_tab->current_scene.camera.direction.get(1, 1);
		this->camera_display_dir_y = this->scene_tab->current_scene.camera.direction.get(2, 1);
		this->camera_display_dir_z = this->scene_tab->current_scene.camera.direction.get(3, 1);

		this->camera_display_up_x = this->scene_tab->current_scene.camera.up.get(1, 1);
		this->camera_display_up_y = this->scene_tab->current_scene.camera.up.get(2, 1);
		this->camera_display_up_z = this->scene_tab->current_scene.camera.up.get(3, 1);
		*/
	}

	this->scene_tab->current_scene.camera.LookAt();
	this->scene_tab->current_scene.camera.update_view_inverse();
}

void CameraTab::draw() {

	ImGui::PushItemWidth(60);

	ImGui::DragScalar("##CTranslation speed (keyboard)", ImGuiDataType_Double, &this->scene_tab->real_translation_speed, 0.005, &zero, nullptr, "%.4f",
		ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("How much translation occurs from moving the camera through the WASD keys (higher = faster)");
	ImGui::SameLine();
	ImGui::Text("Translation speed (keyboard)");

	ImGui::Spacing();

	ImGui::DragScalar("##CTranslation speed (menu)", ImGuiDataType_Double, &this->scene_tab->menu_translation_speed, 0.005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("How sensitive the menu translation slider is to clicking and dragging (higher = more change)");
	ImGui::SameLine();
	ImGui::Text("Translation speed (menu)");

	ImGui::Separator();

	ImGui::DragScalar("##CRotation speed (mouse)", ImGuiDataType_Double, &this->scene_tab->real_rotation_speed, 0.005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("How sensitive the camera is to mouse movement (higher = faster)");
	ImGui::SameLine();
	ImGui::Text("Rotation speed (mouse)");

	ImGui::Spacing();

	ImGui::DragScalar("##CRotation speed (menu)", ImGuiDataType_Double, &this->scene_tab->menu_rotation_speed, 0.005, &zero, nullptr, "%.4f", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("(Menu) How sensitive the menu rotation slider is to clicking and dragging (higher = more change): ");
	ImGui::SameLine();
	ImGui::Text("Rotation speed (menu)");

	ImGui::Separator();

	

	if (ImGui::DragScalar("##FOV (degrees)", ImGuiDataType_Double, &this->scene_tab->current_scene.camera.FOV, 0.1, &zero, nullptr, "%.2f", ImGuiSliderFlags_None)) {
		this->scene_tab->current_scene.camera.FOVr = this->scene_tab->current_scene.camera.FOV * (M_PI / 180.0);

		this->scene_tab->current_scene.camera.update_projection_matrix();
	}
	ImGui::SameLine();
	ImGui::Text("FOV (degrees)");	
	
	if (ImGui::DragScalar("##Near plane", ImGuiDataType_Double, &this->scene_tab->current_scene.camera.near, 0.001, &zero, &this->scene_tab->current_scene.camera.far, "%.4f", ImGuiSliderFlags_None)) {
		this->scene_tab->current_scene.camera.update_projection_matrix();
	}
	ImGui::SameLine();
	ImGui::Text("Near plane");
	
	if (ImGui::DragScalar("##Far plane", ImGuiDataType_Double, &this->scene_tab->current_scene.camera.far, 0.001, &this->scene_tab->current_scene.camera.near, nullptr, "%.4f", ImGuiSliderFlags_None)) {
		this->scene_tab->current_scene.camera.update_projection_matrix();
	}
	ImGui::SameLine();
	ImGui::Text("Far plane");

	ImGui::Separator();

	ImGui::Text("Direction");
	ImGui::Spacing();

	if (ImGui::DragScalar("##CDX", ImGuiDataType_Double, (void*)&(this->camera_display_dir_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("X");
	
	if (ImGui::DragScalar("##CDY", ImGuiDataType_Double, (void*)&(this->camera_display_dir_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("Y");

	if (ImGui::DragScalar("##CDZ", ImGuiDataType_Double, (void*)&(this->camera_display_dir_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("Z");

	ImGui::Separator();

	ImGui::Text("Up");
	ImGui::Spacing();
	
	if (ImGui::DragScalar("##CUX", ImGuiDataType_Double, (void*)&(this->camera_display_up_x), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("X");

	if (ImGui::DragScalar("##CUY", ImGuiDataType_Double, (void*)&(this->camera_display_up_y), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("Y");

	
	
	if (ImGui::DragScalar("##CUZ", ImGuiDataType_Double, (void*)&(this->camera_display_up_z), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Direction, false);
	}
	ImGui::SameLine();
	ImGui::Text("Z");

	ImGui::Separator();

	ImGui::Text("Translation");
	ImGui::Spacing();
	
	

	if (ImGui::DragScalar("##CX", ImGuiDataType_Double, (void*)&(this->camera_display_tx), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraTranslation(false);
	}
	ImGui::SameLine();
	ImGui::Text("X");

	if (ImGui::DragScalar("##CY", ImGuiDataType_Double, (void*)&(this->camera_display_ty), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraTranslation(false);
	}
	ImGui::SameLine();
	ImGui::Text("Y");

	if (ImGui::DragScalar("##CZ", ImGuiDataType_Double, (void*)&(this->camera_display_tz), this->scene_tab->menu_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraTranslation(false);
	}
	ImGui::SameLine();
	ImGui::Text("Z");

	ImGui::Separator();

	ImGui::Text("Rotation (YXZ)");
	ImGui::Spacing();
	
	if (ImGui::DragScalar("##CYaw", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.camera.yaw), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Euler, false);
	}
	ImGui::SameLine();
	ImGui::Text("Yaw");

	
	
	if (ImGui::DragScalar("##CPitch", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.camera.pitch), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Euler, false);
	}
	ImGui::SameLine();
	ImGui::Text("Pitch");

	
	
	if (ImGui::DragScalar("##CRoll", ImGuiDataType_Double, (void*)&(this->scene_tab->current_scene.camera.roll), this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Euler, false);
	}
	ImGui::SameLine();
	ImGui::Text("Roll");

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Quaternion");
	
	if (ImGui::DragScalar("##CQX", ImGuiDataType_Double, &camera_display_qx, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Quaternion, false);
	}
	ImGui::SameLine();
	ImGui::Text("X");

	if (ImGui::DragScalar("##CQY", ImGuiDataType_Double, &camera_display_qy, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Quaternion, false);
	}
	ImGui::SameLine();
	ImGui::Text("Y");

	
	
	if (ImGui::DragScalar("##CQZ", ImGuiDataType_Double, &camera_display_qz, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Quaternion, false);
	}
	ImGui::SameLine();
	ImGui::Text("Z");

	if (ImGui::DragScalar("##CQW", ImGuiDataType_Double, &camera_display_qw, this->scene_tab->menu_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
		UpdateCameraRotation(RotationType_Quaternion, false);
	}
	ImGui::SameLine();
	ImGui::Text("W");

	ImGui::Separator();

	ImGui::Text("Projection matrix");
	if (ImGui::BeginTable("Projection matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
		for (uint8_t row = 1; row < 5; row++) {
			ImGui::TableNextRow();
			for (uint8_t col = 1; col < 5; col++) {
				ImGui::TableNextColumn();

				double number = this->scene_tab->current_scene.camera.PROJECTION_MATRIX.get(row, col);

				ImGui::Text(std::to_string(number).c_str());
			}
		}

	}

	ImGui::EndTable();

	ImGui::Spacing();
	ImGui::Text("View matrix");
	if (ImGui::BeginTable("View matrix", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX)) {
		for (uint8_t row = 1; row < 5; row++) {
			ImGui::TableNextRow();
			for (uint8_t col = 1; col < 5; col++) {
				ImGui::TableNextColumn();

				double number = this->scene_tab->current_scene.camera.VIEW_MATRIX.get(row, col);

				ImGui::Text(std::to_string(number).c_str());
			}
		}

	}

	ImGui::EndTable();
}