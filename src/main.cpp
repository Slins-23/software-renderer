#include "Engine.h"

int main() {
	Engine engine;
	bool loaded = engine.setup();

	if (!loaded) {
		std::cout << "Could not load engine. Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	engine.load_scene(true);
		
	bool quit = false;
	std::vector<double> frametimes;
	double last_tick = SDL_GetTicks();

	

	while (true) {
		uint32_t frame_start = SDL_GetTicks();

		quit = engine.handle_events();
		if (quit) break;

		if (engine.playing) {
			engine.draw();
		}

		if (engine.window_manager.show_window) {
			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();
			engine.window_manager.handle_windows();
			ImGui::ShowDemoWindow();
			ImGui::Render();
		}


		engine.render();

		uint32_t msperframe = SDL_GetTicks() - frame_start;

		if (msperframe < engine.MSPERFRAME) {
			SDL_Delay(engine.MSPERFRAME - msperframe);
		}	

		double frametime = SDL_GetTicks() - frame_start;
		frametimes.push_back(frametime);

		if (SDL_GetTicks() - last_tick >= engine.fps_update_interval) {
			double average = 0;
			for (int i = 0; i < frametimes.size(); i++) {
				average += frametimes[i];
			}

			average /= frametimes.size();
			frametimes.clear();

			double framerate = 1000.0 / average;

			std::cout << "Rendered @ " << framerate << " fps average" << std::endl;

			engine.window_manager.general_window.framerate = framerate;

			last_tick = SDL_GetTicks();
		}
	}
	return 0;
}