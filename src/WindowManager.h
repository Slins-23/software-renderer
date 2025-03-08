#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class Window {
protected:
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

	void UpdateLightTranslation(Light* light_source, Instance* instance) {
		UpdateInstanceTranslation(instance);

		light_source->tx = instance->tx;
		light_source->ty = instance->ty;
		light_source->tz = instance->tz;

		light_source->position = Mat(
			{
				{light_source->tx},
				{light_source->ty},
				{light_source->tz},
				{1}
			}
		, 4, 1);
	}

	void UpdateInstanceRotation(Instance* instance, bool from_euler) {
		// If updating euler angles
		if (from_euler) {
			Mat default_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
			Mat default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
			Mat default_forward = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

			Quaternion orientation = Quaternion::FromYawPitchRoll(this->rotation_orientation, instance->yaw, instance->pitch, instance->roll, default_right, default_up, default_forward);

			instance->orientation = orientation;
		}

		// If updating quaternion
		else {
			double qw = instance->orientation.w;
			double qx = instance->orientation.x;
			double qy = instance->orientation.y;
			double qz = instance->orientation.z;

			double mag = sqrt((qw * qw) + (qx * qx) + (qy * qy) + (qz * qz));
			qw /= mag;
			qx /= mag;
			qy /= mag;
			qz /= mag;

			instance->orientation.w = qw;
			instance->orientation.x = qx;
			instance->orientation.y = qy;
			instance->orientation.z = qz;

			instance->orientation.GetAngles(this->rotation_orientation, instance->yaw, instance->pitch, instance->roll);
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

	void UpdateLightRotation(Light* light_source, Instance* instance, bool from_euler) {
		UpdateInstanceRotation(instance, from_euler);

		light_source->yaw = instance->yaw;
		light_source->pitch = instance->pitch;
		light_source->roll = instance->roll;

		light_source->direction = instance->ROTATION_MATRIX * light_source->default_direction;
		light_source->up = instance->ROTATION_MATRIX * light_source->default_up;


	}

	bool* wireframe_renderer = nullptr;
	bool* rasterize = nullptr;
	bool* shade = nullptr;

	double* fps_update_interval = nullptr;
public:
	double translation_speed = 0.001;
	double minimum_translation_speed = zero;

	double rotation_speed = 0.01;
	double minimum_rotation_speed = zero;

	double zero = 0;

	Instance* target_instance = nullptr;
	Light* light_source = nullptr;

	Orientation rotation_orientation = Orientation::local;

	double display_qx = 0;
	double display_qy = 0;
	double display_qz = 0;
	double display_qw = 0;

	double framerate = 0;

	General() {};

	void initialize(Instance* target_instance, bool* wireframe_renderer, bool* rasterize, bool* shade, double* fps_update_interval) {

		if (target_instance != nullptr) {
			this->target_instance = target_instance;
			this->display_qx = this->target_instance->orientation.x;
			this->display_qy = this->target_instance->orientation.y;
			this->display_qz = this->target_instance->orientation.z;
			this->display_qw = this->target_instance->orientation.w;
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
			if (!this->target_instance->is_light_source) UpdateInstanceTranslation(target_instance);
			else UpdateLightTranslation(light_source, target_instance);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Y:", ImGuiDataType_Double, (void*)&(target_instance->ty), translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			if (!this->target_instance->is_light_source) UpdateInstanceTranslation(target_instance);
			else UpdateLightTranslation(light_source, target_instance);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Z:", ImGuiDataType_Double, (void*)&(target_instance->tz), translation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			if (!this->target_instance->is_light_source) UpdateInstanceTranslation(target_instance);
			else UpdateLightTranslation(light_source, target_instance);
		}

		ImGui::Separator();

		ImGui::Text("Rotation (YXZ)");
		ImGui::Spacing();
		ImGui::Text("Yaw:  ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Yaw:  ", ImGuiDataType_Double, (void*)&(target_instance->yaw), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, true);
			else UpdateLightRotation(light_source, target_instance, true);
		}

		ImGui::Text("Pitch:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Pitch:", ImGuiDataType_Double, (void*)&(target_instance->pitch), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, true);
			else UpdateLightRotation(light_source, target_instance, true);
		}

		ImGui::Text("Roll: ");
		ImGui::SameLine();
		if (ImGui::DragScalar("##Roll: ", ImGuiDataType_Double, (void*)&(target_instance->roll), rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, true);
			else UpdateLightRotation(light_source, target_instance, true);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Quaternion");
		ImGui::Text("X:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QX:", ImGuiDataType_Double, &display_qx, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			target_instance->orientation.x = display_qx;

			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, false);
			else UpdateLightRotation(light_source, target_instance, false);
		}

		ImGui::Text("Y:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QY:", ImGuiDataType_Double, &display_qy, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			target_instance->orientation.y = display_qy;

			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, false);
			else UpdateLightRotation(light_source, target_instance, false);
		}

		ImGui::Text("Z:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QZ:", ImGuiDataType_Double, &display_qz, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			target_instance->orientation.z = display_qz;

			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, false);
			else UpdateLightRotation(light_source, target_instance, false);
		}

		ImGui::Text("W:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##QW:", ImGuiDataType_Double, &display_qw, rotation_speed, nullptr, nullptr, "%.3f", ImGuiSliderFlags_None)) {
			target_instance->orientation.w = display_qw;

			if (!this->target_instance->is_light_source) UpdateInstanceRotation(target_instance, false);
			else UpdateLightRotation(light_source, target_instance, false);
		}

		ImGui::Separator();
	}

	void draw() {
		ImGui::Begin("General window");

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_None)) draw_debug_dropdown();
		if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_None)) draw_scene_dropdown();
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None)) draw_transform_dropdown();

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