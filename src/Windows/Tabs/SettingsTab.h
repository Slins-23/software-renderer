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

	double z_fighting_tolerance = 0.994;

	double* general_window_opacity = nullptr;

	SettingsTab(double* general_window_opacity) {
		this->general_window_opacity = general_window_opacity;
	};

	virtual void draw();
};