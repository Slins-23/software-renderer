#include "../Window.h"
#pragma once

class SettingsTab : public Window {
public:
	bool wireframe_render = false;
	bool rasterize = true;
	bool shade = true;
	bool depth_test = true;
	bool backface_cull = true;

	uint8_t FPS_LIMIT = 144;
	uint8_t MSPERFRAME = 1000.0 / FPS_LIMIT;

	double framerate = 0;

	// How often, in milliseconds, should the average FPS over the given time interval be printed to the console
	double fps_update_interval = 500;

	// SDL window clear/default color
	const uint32_t CLEAR_COLOR = 0xFFFFFFFF;

	// Background color
	uint32_t BG_COLOR = 0x000000FF;

	// Line/wireframe color
	uint32_t LINE_COLOR = 0x00FF00FF;

	// Rasterization color
	uint32_t FILL_COLOR = 0x66285CFF;

	float display_BG_COLOR[4] = { 0, 0, 0, 0 };
	float display_LINE_COLOR[4] = { 0, 0, 0, 0 };
	float display_FILL_COLOR[4] = { 0, 0, 0, 0 };

	double z_fighting_tolerance = 0.994;

	double* general_window_opacity = nullptr;

	SettingsTab(double* general_window_opacity) {
		this->general_window_opacity = general_window_opacity;
	};

	/*
	void update(bool wireframe_render, bool rasterize, bool shade, double framerate, double fps_update_interval) {
		this->wireframe_render = wireframe_render;
		this->rasterize = rasterize;
		this->shade = shade;
		this->framerate = framerate;
		this->fps_update_interval = fps_update_interval;
	}
	*/

	void draw() {
		ImGui::Text("Window opacity:");
		ImGui::SameLine();
		ImGui::DragScalar("##Window opacity:", ImGuiDataType_Double, this->general_window_opacity, 0.001f, &zero, &one, "%.2lf", ImGuiSliderFlags_None);

		char fps_text[255];
		sprintf_s(fps_text, 255, "FPS: %.2lf", this->framerate);
		ImGui::Text(fps_text);
		ImGui::Spacing();
		ImGui::Text("FPS calculation interval (in ms): ");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::DragScalar("##FPS calculation interval (in ms): ", ImGuiDataType_Double, &this->fps_update_interval, 1.0f, &zero, nullptr, "%.0lf", ImGuiSliderFlags_None);

		ImGui::Text("FPS Limit:");
		ImGui::SameLine();
		if (ImGui::DragScalar("##FPS Limit:", ImGuiDataType_U8, &this->FPS_LIMIT, 1, &zero, nullptr, 0, ImGuiSliderFlags_None)) {
			this->MSPERFRAME = 1000.0 / this->FPS_LIMIT;
		}

		ImGui::Checkbox("Wireframe", &this->wireframe_render);
		ImGui::Checkbox("Rasterize", &this->rasterize);
		ImGui::Checkbox("Shade", &this->shade);
		ImGui::Checkbox("Depth test:", &this->depth_test);
		ImGui::Checkbox("Backface cull:", &this->backface_cull);

		ImGui::Text("Background color:");
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##Color", this->display_BG_COLOR)) {
			uint8_t red = display_BG_COLOR[0] * 255.0;
			uint8_t green = display_BG_COLOR[1] * 255.0;
			uint8_t blue = display_BG_COLOR[2] * 255.0;
			uint8_t alpha = display_BG_COLOR[3] * 255.0;

			uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

			this->BG_COLOR = color;
		};

		ImGui::Text("Line/wireframe color:");
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##Color", this->display_LINE_COLOR)) {
			uint8_t red = display_LINE_COLOR[0] * 255.0;
			uint8_t green = display_LINE_COLOR[1] * 255.0;
			uint8_t blue = display_LINE_COLOR[2] * 255.0;
			uint8_t alpha = display_LINE_COLOR[3] * 255.0;

			uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

			this->LINE_COLOR = color;
		};

		ImGui::Text("Fill/ambient color:");
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##Color", this->display_FILL_COLOR)) {
			uint8_t red = display_FILL_COLOR[0] * 255.0;
			uint8_t green = display_FILL_COLOR[1] * 255.0;
			uint8_t blue = display_FILL_COLOR[2] * 255.0;
			uint8_t alpha = display_FILL_COLOR[3] * 255.0;

			uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

			this->FILL_COLOR = color;
		};

		ImGui::Text("Z fighting tolerance:");
		ImGui::SetItemTooltip("This tolerance value sets a treshold for avoiding z-fighting during depth testing. (higher = more precision but more z-fighting)");
		ImGui::SameLine();
		ImGui::DragScalar("##Z-fighting tolerance", ImGuiDataType_Double, &this->z_fighting_tolerance, 0.001, &zero, &one, "%.4f", ImGuiSliderFlags_None);
	}

};