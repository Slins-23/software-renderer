#include "Engine.h"

int main() {
	Engine engine;
	bool loaded = engine.setup();

	if (!loaded) {
		std::cout << "Could not load engine. Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	char models_folder[255] = "D:/Programming/Graphics/Prototyping/models/";
	char scene_folder[255] = "D:/Programming/Graphics/Prototyping/scenes/";
	char scene_load_name[255] = "hallway.json";
	char scene_save_name[255] = "tst.json";

	//engine.window_manager.general_window.scene_tab.load_scene(scene_folder, scene_load_name, models_folder, true, true);
	//engine.window_manager.general_window.scene_tab.load_scene();
		
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

		if (engine.window_manager.show_window) {
			engine.window_manager.handle_windows();
			ImGui::ShowDemoWindow();

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