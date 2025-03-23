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

	float display_BG_COLOR[4] = {static_cast<float>(((BG_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((BG_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((BG_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((BG_COLOR & 0x000000FF) / 255.0) };

	float display_LINE_COLOR[4] = { static_cast<float>(((LINE_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((LINE_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((LINE_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((LINE_COLOR & 0x000000FF) / 255.0) };

	float display_FILL_COLOR[4] = { static_cast<float>(((FILL_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((FILL_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((FILL_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((FILL_COLOR & 0x000000FF) / 255.0) };

	double z_fighting_tolerance = 0.994;

	double* general_window_opacity = nullptr;

	SettingsTab(double* general_window_opacity) {
		this->general_window_opacity = general_window_opacity;
	};

	virtual void draw();
};