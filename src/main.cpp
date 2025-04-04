#include "Engine.h"

int main() {
	Engine engine;
	bool loaded = engine.setup();

	if (!loaded) {
		std::cout << "Could not load engine. Error: " << SDL_GetError() << std::endl;
		return -1;
	}


	bool quit = false;
	std::vector<double> frametimes;
	double last_tick = SDL_GetTicks();

	while (true) {
		uint32_t frame_start = SDL_GetTicks();

		quit = engine.handle_events();
		if (quit) break;

		engine.draw();		

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 10), IM_COL32(255, 255, 255, 255), "Press '1' to open/close the settings", 0);

		char fps_text[255];
		sprintf_s(fps_text, 255, "Framerate: %.2lf", engine.window_manager.general_window.settings_tab.framerate);

		ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 30), IM_COL32(255, 255, 255, 255), fps_text, 0);

		if (engine.window_manager.general_window.scene_tab.current_scene.load_error != SceneError::None) {
			char error_string[512] = "";

			switch (engine.window_manager.general_window.scene_tab.current_scene.load_error) {
			case SceneError::ModelLoad:
				sprintf_s(error_string, sizeof(error_string), "Error loading scene: Could not load model file at %s\nMake sure to set the model folder in the menu accordingly\nAnd that the model file can be found within it, then reload the scene.", engine.window_manager.general_window.scene_tab.current_scene.errored_path.c_str());
				break;
			case SceneError::SceneLoad:
				sprintf_s(error_string, sizeof(error_string), "Error loading scene: Could not load scene file at %s\nMake sure to set the scene folder in the menu accordingly\nAlso make sure that the scene filename is correct and its file is within the scene folder\nThen reload the scene.", engine.window_manager.general_window.scene_tab.current_scene.errored_path.c_str());
				break;
			case SceneError::JSONParsing:
				sprintf_s(error_string, sizeof(error_string), "Error loading scene: Could not parse the JSON for the scene configuration file.\nAt %s.\nMake sure that the file is in a valid JSON format with no missing or extra commas, quotations, braces, etc...", engine.window_manager.general_window.scene_tab.current_scene.errored_path.c_str());
				break;
			default:
				break;
			}
			

			ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 50), IM_COL32(255, 0, 0, 255), error_string, nullptr);
		}

		if (engine.window_manager.show_window) {
			engine.window_manager.handle_windows();
		}

		ImGui::Render();
		engine.render();

		uint32_t msperframe = SDL_GetTicks() - frame_start;

		if (msperframe < engine.window_manager.general_window.settings_tab.MSPERFRAME) {
			SDL_Delay(engine.window_manager.general_window.settings_tab.MSPERFRAME - msperframe);
		}	

		double frametime = SDL_GetTicks() - frame_start;
		frametimes.push_back(frametime);

		if (SDL_GetTicks() - last_tick >= engine.window_manager.general_window.settings_tab.fps_update_interval) {
			double average = 0;
			for (int i = 0; i < frametimes.size(); i++) {
				average += frametimes[i];
			}

			average /= frametimes.size();
			frametimes.clear();

			double framerate = 1000.0 / average;

			std::cout << "Rendered @ " << framerate << " fps average" << std::endl;

			engine.window_manager.general_window.settings_tab.framerate = framerate;

			last_tick = SDL_GetTicks();
		}
	}
	return 0;
}