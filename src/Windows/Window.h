#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <cstdint>
#include <stdio.h>

class Window {
protected:
	double window_alpha = 0.3;
	const double zero = 0;
	const double one = 1;
public:
	Window() {};

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

	virtual ~Window() {

	}
};