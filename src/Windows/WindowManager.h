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

	WindowManager(uint16_t* WIDTH, uint16_t* HEIGHT) {
		general_window.WIDTH = WIDTH;
		general_window.HEIGHT = HEIGHT;

		general_window.scene_tab.WIDTH = WIDTH;
		general_window.scene_tab.HEIGHT = HEIGHT;
	};

	void initialize(SDL_Window* window, SDL_Renderer* renderer);
	void handle_windows();
	void close();
};