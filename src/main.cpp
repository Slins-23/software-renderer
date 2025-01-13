#include "Engine.h"

int main() {
	Engine engine;
	bool loaded = engine.setup();

	if (!loaded) {
		std::cout << "Could not load engine. Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	engine.current_scene = Scene(engine.scenes_folder, engine.scene_filename, engine.models_folder, true, engine.default_camera_position, engine.camera_position, engine.default_camera_direction, engine.camera_direction, engine.default_camera_up, engine.camera_up, engine.camera_yaw, engine.camera_pitch, engine.camera_roll, engine.VIEW_MATRIX, engine.use_scene_camera_settings);
		
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

			std::cout << "Rendered @ " << 1000 / average << " fps average" << std::endl;

			last_tick = SDL_GetTicks();
		}
	}
	return 0;
}