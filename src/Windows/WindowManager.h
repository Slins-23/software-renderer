#pragma once
#include "Window.h"
#include "GeneralWindow.h"

class WindowManager {
private:
public:
	bool show_window = false;

	GeneralWindow general_window;

	Window* target_window = dynamic_cast<Window*>(&general_window);

	ImGuiIO io;

	WindowManager() {

	}

	void initialize(SDL_Window* window, SDL_Renderer* renderer) {
		this->target_window = dynamic_cast<Window*>(& general_window);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		this->io = ImGui::GetIO();
		this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer2_Init(renderer);
	}

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