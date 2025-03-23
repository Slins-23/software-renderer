#include "WindowManager.h"

void WindowManager::initialize(SDL_Window* window, SDL_Renderer* renderer) {
	this->target_window = dynamic_cast<Window*>(&general_window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	this->io = ImGui::GetIO();
	this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

void WindowManager::handle_windows() {
	if (show_window && target_window != nullptr) {
		target_window->draw();
	}
}

void WindowManager::close() {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
};