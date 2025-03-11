#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class Window {
protected:
	double window_alpha = 0.3;
public:
	void virtual draw() {
		static uint32_t current_frame = 1;
		static uint32_t total_clicks = 0;

		ImGui::Begin("Default window");
		ImGui::Text("Testing text...");

		char fmt_text[255];
		sprintf_s(fmt_text, 255, "Button clicks: %d", total_clicks);

		if (ImGui::Button("Click")) {
			total_clicks++;
		}

		ImGui::SameLine();
		ImGui::Text(fmt_text);

		char frames_text[255];
		sprintf_s(frames_text, 255, "Total frames: %d", current_frame);
		ImGui::Text(frames_text);

		current_frame++;

		ImGui::End();
	};
};

class General : Window {
private:
	void UpdateInstanceTranslation(Instance* instance) {
		instance->TRANSLATION_MATRIX = Mat::translation_matrix(instance->tx, instance->ty, instance->tz);
		instance->MODEL_TO_WORLD = instance->TRANSLATION_MATRIX * instance->ROTATION_MATRIX * instance->SCALING_MATRIX;
	}

	void UpdateInstanceRotation(Instance* instance, bool from_euler) {
		// If updating euler angles
		if (from_euler) {
			Mat default_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
			Mat default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
			Mat default_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

			Quaternion orientation = Quaternion::FromYawPitchRoll(this->rotation_orientation, instance->yaw, instance->pitch, instance->roll, default_right, default_up, default_forward);

			instance->orientation = orientation;

			this->display_qx = orientation.x;
			this->display_qy = orientation.y;
			this->display_qz = orientation.z;
			this->display_qw = orientation.w;
		}

		// If updating quaternion
		else {
			double qw = this->display_qw;
			double qx = this->display_qx;
			double qy = this->display_qy;
			double qz = this->display_qz;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			instance->orientation.w = qw;
			instance->orientation.x = qx;
			instance->orientation.y = qy;
			instance->orientation.z = qz;

			// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

			double new_yaw = 0;
			double new_pitch = 0;
			double new_roll = 0;

			instance->orientation.GetAngles(this->rotation_orientation, new_yaw, new_pitch, new_roll);

			double old_normalized_yaw = fmod(instance->yaw, 2 * M_PI);

			if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
			if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

			double old_normalized_pitch = fmod(instance->pitch, 2 * M_PI);

			if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
			if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

			double old_normalized_roll = fmod(instance->roll, 2 * M_PI);

			if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
			if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

			double yaw_difference = new_yaw - old_normalized_yaw;
			double pitch_difference = new_pitch - old_normalized_pitch;
			double roll_difference = new_roll - old_normalized_roll;

			instance->yaw = instance->yaw + yaw_difference;
			instance->pitch = instance->pitch + pitch_difference;
			instance->roll = instance->roll + roll_difference;
		}

		instance->ROTATION_MATRIX = instance->orientation.get_rotationmatrix();
		instance->MODEL_TO_WORLD = instance->TRANSLATION_MATRIX * instance->ROTATION_MATRIX * instance->SCALING_MATRIX;

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

	void UpdateLightTranslation(Light* light_source) {
		Mat TRANSLATION_MATRIX = Mat::translation_matrix(light_source->tx, light_source->ty, light_source->tz);

		if (light_source->has_model) {
			light_source->instance->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
			light_source->instance->MODEL_TO_WORLD = light_source->instance->TRANSLATION_MATRIX * light_source->instance->ROTATION_MATRIX * light_source->instance->SCALING_MATRIX;

			light_source->instance->tx = light_source->tx;
			light_source->instance->ty = light_source->ty;
			light_source->instance->tz = light_source->tz;
		}

		light_source->position = Mat(
			{
				{light_source->tx},
				{light_source->ty},
				{light_source->tz},
				{1}
			}
		, 4, 1);
	}

	void UpdateLightRotation(Light* light_source, bool from_euler) {
		// If updating euler angles
		if (from_euler) {
			Mat default_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
			Mat default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
			Mat default_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

			Quaternion orientation = Quaternion::FromYawPitchRoll(this->rotation_orientation, light_source->yaw, light_source->pitch, light_source->roll, default_right, default_up, default_forward);

			light_source->orientation = orientation;

			if (light_source->has_model) {
				light_source->instance->orientation = light_source->orientation;
				light_source->instance->yaw = light_source->yaw;
				light_source->instance->pitch = light_source->pitch;
				light_source->instance->roll = light_source->roll;
			}

			this->light_display_qx = orientation.x;
			this->light_display_qy = orientation.y;
			this->light_display_qz = orientation.z;
			this->light_display_qw = orientation.w;
		}

		// If updating quaternion
		else {
			double qw = this->light_display_qw;
			double qx = this->light_display_qx;
			double qy = this->light_display_qy;
			double qz = this->light_display_qz;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			light_source->orientation.w = qw;
			light_source->orientation.x = qx;
			light_source->orientation.y = qy;
			light_source->orientation.z = qz;

			if (light_source->has_model) {
				light_source->instance->orientation.w = qw;
				light_source->instance->orientation.x = qx;
				light_source->instance->orientation.y = qy;
				light_source->instance->orientation.z = qz;
			}

			// Normalizes the old yaw, pitch, and yaw values and adds the relative difference from the new angles, so that the values in the input box don't change to the normalized values of [-pi, pi]

			double new_yaw = 0;
			double new_pitch = 0;
			double new_roll = 0;

			light_source->orientation.GetAngles(this->rotation_orientation, new_yaw, new_pitch, new_roll);

			double old_normalized_yaw = fmod(light_source->yaw, 2 * M_PI);

			if (old_normalized_yaw > M_PI) old_normalized_yaw -= 2 * M_PI;
			if (old_normalized_yaw < -M_PI) old_normalized_yaw += 2 * M_PI;

			double old_normalized_pitch = fmod(light_source->pitch, 2 * M_PI);

			if (old_normalized_pitch > M_PI) old_normalized_pitch -= 2 * M_PI;
			if (old_normalized_pitch < -M_PI) old_normalized_pitch += 2 * M_PI;

			double old_normalized_roll = fmod(light_source->roll, 2 * M_PI);

			if (old_normalized_roll > M_PI) old_normalized_roll -= 2 * M_PI;
			if (old_normalized_roll < -M_PI) old_normalized_roll += 2 * M_PI;

			double yaw_difference = new_yaw - old_normalized_yaw;
			double pitch_difference = new_pitch - old_normalized_pitch;
			double roll_difference = new_roll - old_normalized_roll;

			light_source->yaw += yaw_difference;
			light_source->pitch += pitch_difference;
			light_source->roll += roll_difference;

			if (light_source->has_model) {
				light_source->instance->yaw = light_source->yaw;
				light_source->instance->pitch = light_source->pitch;
				light_source->instance->roll = light_source->roll;
			}
		}

		if (light_source->has_model) {
			light_source->instance->ROTATION_MATRIX = light_source->instance->orientation.get_rotationmatrix();
			light_source->instance->MODEL_TO_WORLD = light_source->instance->TRANSLATION_MATRIX * light_source->instance->ROTATION_MATRIX * light_source->instance->SCALING_MATRIX;
		}

		light_source->direction = light_source->orientation.get_rotationmatrix() * light_source->default_direction;
		light_source->up = light_source->orientation.get_rotationmatrix() * light_source->default_up;
	}

	bool* wireframe_renderer = nullptr;
	bool* rasterize = nullptr;
	bool* shade = nullptr;

	double* fps_update_interval = nullptr;
public:
	double zero = 0;
	double one = 1;

	Orientation rotation_orientation = Orientation::local;

	Instance* target_instance = nullptr;
	Light* light_source = nullptr;

	double translation_speed = 0.001;
	double minimum_translation_speed = zero;

	double light_translation_speed = 0.001;

	double rotation_speed = 0.01;
	double minimum_rotation_speed = zero;

	double light_rotation_speed = 0.01;


	double display_qx = 0;
	double display_qy = 0;
	double display_qz = 0;
	double display_qw = 0;

	double light_display_qx = 0;
	double light_display_qy = 0;
	double light_display_qz = 0;
	double light_display_qw = 0;

	double framerate = 0;

	

	General() {
		this->window_alpha = 0.3;
	};

	void initialize(Instance* target_instance, Light* light_source, bool* wireframe_renderer, bool* rasterize, bool* shade, double* fps_update_interval) {

		if (target_instance != nullptr) {
			this->target_instance = target_instance;
			this->light_source = light_source;

			this->display_qx = this->target_instance->orientation.x;
			this->display_qy = this->target_instance->orientation.y;
			this->display_qz = this->target_instance->orientation.z;
			this->display_qw = this->target_instance->orientation.w;

			this->light_display_qx = this->light_source->orientation.x;
			this->light_display_qy = this->light_source->orientation.y;
			this->light_display_qz = this->light_source->orientation.z;
			this->light_display_qw = this->light_source->orientation.w;
		}

		this->wireframe_renderer = wireframe_renderer;
		this->rasterize = rasterize;
		this->shade = shade;

		this->fps_update_interval = fps_update_interval;
	}

	void draw_debug_dropdown() {
		char fps_text[255];
		sprintf_s(fps_text, 255, "FPS: %.2lf", this->framerate);
		ImGui::Text(fps_text);
		ImGui::Spacing();
		ImGui::Text("FPS calculation interval (in ms): ");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::DragScalar("##FPS calculation interval (in ms): ", ImGuiDataType_Double, this->fps_update_interval, 1.0f, &zero, nullptr, "%.0lf", ImGuiSliderFlags_None);

		ImGui::Text("Window opacity:");
		ImGui::SameLine();
		ImGui::DragScalar("##Window opacity:", ImGuiDataType_Double, &this->window_alpha, 0.001f, &zero, &one, "%.2lf", ImGuiSliderFlags_None);

		static uint32_t current_frame = 1;
		static uint32_t total_clicks = 0;
		
		ImGui::Text("Testing text...");

		char fmt_text[255];
		sprintf_s(fmt_text, 255, "Button clicks: %d", total_clicks);

		if (ImGui::Button("Click")) {
			total_clicks++;
		}

		ImGui::SameLine();
		ImGui::Text(fmt_text);

		char frames_text[255];
		sprintf_s(frames_text, 255, "Total frames: %d", current_frame);
		ImGui::Text(frames_text);

		current_frame++;

		ImGui::Checkbox("Wireframe", wireframe_renderer);
		ImGui::Checkbox("Rasterize", rasterize);
		ImGui::Checkbox("Shade", shade);
	}

	void draw_scene_dropdown() {
		if (ImGui::TreeNode("Info")) {
			// Scene filepath (if loaded)
			// Number of instances, etc...
		}

		if (ImGui::TreeNode("Camera")) {
			// Position, direction, etc...
		}

		if (ImGui::TreeNode("Instances")) {
			// Selectable list of target instances, updates transform section
			// Double click or Right-click edit to transform? Dropdown?
		}

		if (ImGui::TreeNode("Meshes")) {
			// Also selectable but not "transformable", can only inspect data such as which file it's loaded from, number of triangles, etc...
		}

		if (ImGui::TreeNode("Light")) {
			// Light info such as type, direction vectors, rotations, position,
			// Also rendering type (i.e. phong, flat, gouraud)
		}
	}

	void draw_transform_dropdown() {
		ImGui::PushItemWidth(60);

		ImGui::Text("Translation speed: ");
		ImGui::SameLine();
		ImGui::DragScalar("##Translation speed: ", ImGuiDataType_Double, &translation_speed, 0.01, &minimum_translation_speed, nullptr, "%.3f", ImGuiSliderFlags_None);
		ImGui::Separator();

		ImGui::Text("Translation");
		ImGui::Spacing();
		ImGui::Text("X:");
		ImGui::SameLine();
		
		if (ImGui::DragScalar("##X:", ImGuiDataType_Double , (void*) &(target_instance->tx), translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceTranslation(target_instance);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Y:", ImGuiDataType_Double, (void*)&(target_instance->ty), translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceTranslation(target_instance);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Z:", ImGuiDataType_Double, (void*)&(target_instance->tz), translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceTranslation(target_instance);
		}

		ImGui::Separator();

		ImGui::Text("Rotation (YXZ)");
		ImGui::Spacing();
		ImGui::Text("Yaw:  ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Yaw:  ", ImGuiDataType_Double, (void*)&(target_instance->yaw), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, true);
		}

		ImGui::Text("Pitch:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Pitch:", ImGuiDataType_Double, (void*)&(target_instance->pitch), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, true);
		}

		ImGui::Text("Roll: ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Roll: ", ImGuiDataType_Double, (void*)&(target_instance->roll), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, true);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Quaternion");
		ImGui::Text("X:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QX:", ImGuiDataType_Double, &display_qx, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, false);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QY:", ImGuiDataType_Double, &display_qy, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, false);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QZ:", ImGuiDataType_Double, &display_qz, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, false);
		}

		ImGui::Text("W:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QW:", ImGuiDataType_Double, &display_qw, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateInstanceRotation(target_instance, false);
		}

		ImGui::Separator();
	}

	void draw_light_dropdown() {
		ImGui::PushItemWidth(60);

		ImGui::Text("Translation speed: ");
		ImGui::SameLine();
		ImGui::DragScalar("##Translation speed: ", ImGuiDataType_Double, &light_translation_speed, 0.01, &minimum_translation_speed, nullptr, "%.3f", ImGuiSliderFlags_None);
		ImGui::Separator();

		ImGui::Text("Translation");
		ImGui::Spacing();
		ImGui::Text("X:");
		ImGui::SameLine();

		if (ImGui::DragScalar("##X:", ImGuiDataType_Double, (void*)&(light_source->tx), light_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation(light_source);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Y:", ImGuiDataType_Double, (void*)&(light_source->ty), light_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation(light_source);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Z:", ImGuiDataType_Double, (void*)&(light_source->tz), light_translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightTranslation(light_source);
		}

		ImGui::Separator();

		ImGui::Text("Rotation (YXZ)");
		ImGui::Spacing();
		ImGui::Text("Yaw:  ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Yaw:  ", ImGuiDataType_Double, (void*)&(light_source->yaw), light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, true);
		}

		ImGui::Text("Pitch:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Pitch:", ImGuiDataType_Double, (void*)&(light_source->pitch), light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, true);
		}

		ImGui::Text("Roll: ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Roll: ", ImGuiDataType_Double, (void*)&(light_source->roll), light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, true);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Quaternion");
		ImGui::Text("X:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QX:", ImGuiDataType_Double, &light_display_qx, light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, false);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QY:", ImGuiDataType_Double, &light_display_qy, light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, false);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QZ:", ImGuiDataType_Double, &light_display_qz, light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, false);
		}

		ImGui::Text("W:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QW:", ImGuiDataType_Double, &light_display_qw, light_rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			UpdateLightRotation(light_source, false);
		}

		ImGui::Separator();
	}

	void draw() {
		ImGui::SetNextWindowBgAlpha(this->window_alpha);
		ImGui::Begin("General window");

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_None)) draw_debug_dropdown();
		if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_None)) draw_scene_dropdown();
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None)) draw_transform_dropdown();
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_None)) draw_light_dropdown();

		ImGui::End();
	}
};

class WindowManager {
private:
public:
	bool show_window = false;

	General general_window;

	Window* target_window = (Window*)&general_window;

	ImGuiIO io;

	void initialize(SDL_Window* window, SDL_Renderer* renderer) {
		this->general_window = General();
		this->target_window = (Window*)&general_window;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		this->io = ImGui::GetIO();
		this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer2_Init(renderer);
	};

	void handle_windows() {
		if (show_window && target_window != nullptr) {
			target_window->draw();
		}
	}

	void close() {
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	};
};