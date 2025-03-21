#include "Engine.h"
#include "Windows/Tabs/CameraTab.h"
#include "Windows/Tabs/LightTab.h"
#include "Windows/Tabs/InstancesTab.h"

bool Engine::handle_events() {
	const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

	if (keyboard_state[SDL_SCANCODE_W] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		window_manager.general_window.scene_tab.current_scene.camera.position += window_manager.general_window.scene_tab.real_translation_speed * window_manager.general_window.scene_tab.current_scene.camera.direction;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}

	if (keyboard_state[SDL_SCANCODE_S] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		window_manager.general_window.scene_tab.current_scene.camera.position -= window_manager.general_window.scene_tab.real_translation_speed * window_manager.general_window.scene_tab.current_scene.camera.direction;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}
	
	if (keyboard_state[SDL_SCANCODE_A] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		Mat camera_right = Mat::CrossProduct3D(window_manager.general_window.scene_tab.current_scene.camera.up, window_manager.general_window.scene_tab.current_scene.camera.direction);
		window_manager.general_window.scene_tab.current_scene.camera.position += window_manager.general_window.scene_tab.real_translation_speed * camera_right;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}



	if (keyboard_state[SDL_SCANCODE_D] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		Mat camera_right = Mat::CrossProduct3D(window_manager.general_window.scene_tab.current_scene.camera.up, window_manager.general_window.scene_tab.current_scene.camera.direction);
		window_manager.general_window.scene_tab.current_scene.camera.position -= window_manager.general_window.scene_tab.real_translation_speed * camera_right;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}

	if (keyboard_state[SDL_SCANCODE_UP] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		window_manager.general_window.scene_tab.current_scene.camera.position += window_manager.general_window.scene_tab.real_translation_speed * window_manager.general_window.scene_tab.current_scene.camera.up;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}

	if (keyboard_state[SDL_SCANCODE_DOWN] && !window_manager.show_window) {
		// Transforms camera and updates view matrix
		window_manager.general_window.scene_tab.current_scene.camera.position -= window_manager.general_window.scene_tab.real_translation_speed * window_manager.general_window.scene_tab.current_scene.camera.up;
		window_manager.general_window.scene_tab.camera_tab->UpdateCameraTranslation(true);
		//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
		//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
	}


	while (SDL_PollEvent(&this->event)) {
		ImGui_ImplSDL2_ProcessEvent(&this->event);

		switch (this->event.type) {
		case SDL_QUIT:
			close();
			return 1;
		case SDL_WINDOWEVENT:
			switch (this->event.window.event) {
			case SDL_WINDOWEVENT_CLOSE:
			{
				if (SDL_GetWindowID(this->window) == this->event.window.windowID) {
					close();
					return 1;
				}
			}
			break;
			case SDL_WINDOWEVENT_RESIZED:
				free(this->pixel_buffer);
				free(this->depth_buffer);

				SDL_DestroyTexture(this->texture);

				this->WIDTH = this->event.window.data1;
				this->HEIGHT = this->event.window.data2;

				std::cout << "Window width: " << this->WIDTH << std::endl;
				std::cout << "Window height: " << this->HEIGHT << std::endl;

				this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->WIDTH, this->HEIGHT);
				if (this->texture == nullptr) {
					std::cout << "Could not create texture during window resizing. Error: " << SDL_GetError() << std::endl;
					SDL_DestroyRenderer(this->renderer);
					SDL_DestroyWindow(this->window);
					SDL_Quit();
					return 0;
				}

				this->pixel_buffer = (uint32_t*)malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);
				this->depth_buffer = (double*)malloc(sizeof(double) * this->WIDTH * this->HEIGHT);
				for (size_t i = 0; i < this->WIDTH * this->HEIGHT; i++) {
					this->pixel_buffer[i] = window_manager.general_window.settings_tab.BG_COLOR;
					this->depth_buffer[i] = std::numeric_limits<double>::max();
				}

				window_manager.general_window.scene_tab.current_scene.camera.update_window_resized(this->WIDTH, this->HEIGHT);
				break;
			}

			break;
		case SDL_KEYDOWN:
			// Ignores keyboard key presses if user is inputting text into any ImGUI input (avoids camera from moving or window from closing when typing in values for ex)
			if (ImGui::GetIO().WantTextInput) break;

			switch (this->event.key.keysym.scancode) {
			case SDL_SCANCODE_1:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				window_manager.show_window = !window_manager.show_window;

				
				if (window_manager.show_window) {
					if ((window_manager.general_window.scene_tab.is_instances_open || window_manager.general_window.scene_tab.is_light_open) && window_manager.general_window.scene_tab.show_transform_axes) {
						if (window_manager.general_window.scene_tab.current_scene.axes_instance.instance_name == "light_source_axes") {
							window_manager.general_window.scene_tab.light_tab->update_transform_axes();

							if (window_manager.general_window.scene_tab.instances_tab->target_instance != nullptr) {
								window_manager.general_window.scene_tab.instances_tab->target_instance->has_axes = false;
							}
						}
						else {
							window_manager.general_window.scene_tab.instances_tab->update_transform_axes();

							if (window_manager.general_window.scene_tab.current_scene.light_source.instance != nullptr) {
								window_manager.general_window.scene_tab.current_scene.light_source.instance->has_axes = false;
							}
						}

						window_manager.general_window.scene_tab.current_scene.axes_instance.show = true;

						//window_manager.general_window.scene_tab.instances_tab->target_instance->has_axes = true;
						//window_manager.general_window.scene_tab.instances_tab->update_transform_axes();
					}

					// Disables relative mouse tracking
					// (This disables the mouse pointer and camera relative movement)
					SDL_SetRelativeMouseMode(SDL_FALSE);
				}
				else {
					if (window_manager.general_window.scene_tab.current_scene.axes_instance.show) {
						window_manager.general_window.scene_tab.current_scene.axes_instance.show = false;
					}

					//window_manager.general_window.scene_tab.instances_tab->target_instance->has_axes = false;
					// Enables relative mouse tracking
					// (This enables the mouse pointer and camera relative movement)
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}

				/*
				this->wireframe_render = !this->wireframe_render;
				if (this->wireframe_render) {
					std::cout << "Wireframe rendering: Enabled" << std::endl;
				}
				else {
					std::cout << "Wireframe rendering: Disabled" << std::endl;
				}
				*/

				break;
			case SDL_SCANCODE_T:
				std::cout << "View matrix: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.VIEW_MATRIX.print();

				std::cout << "Camera direction: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.direction.print();
				std::cout << "Camera up: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.up.print();
				std::cout << "Camera yaw: " << window_manager.general_window.scene_tab.current_scene.camera.yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << window_manager.general_window.scene_tab.current_scene.camera.pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << window_manager.general_window.scene_tab.current_scene.camera.roll * (180 / M_PI) << std::endl;
				/*

				std::cout << "Camera position: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.position.print();

				std::cout << "Light position: " << std::endl;
				light_source.position.print();

				std::cout << "Light yaw: " << light_source.yaw << std::endl;
				std::cout << "Light pitch: " << light_source.pitch << std::endl;
				std::cout << "Light roll: " << light_source.roll << std::endl;
				std::cout << "Light direction: " << std::endl;
				light_source.direction.print();
				std::cout << "Light up: " << std::endl;
				light_source.up.print();
				*/

				break;
				/*
			case SDL_SCANCODE_Y:
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				camera_orientation.GetAngles(Orientation::local, camera_yaw, camera_pitch, camera_roll);
				std::cout << "Camera yaw: " << camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << camera_roll * (180 / M_PI) << std::endl;
				std::cout << "Camera direction: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.direction.print();
				std::cout << "Camera up: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.up.print();
				std::cout << "Camera position: " << std::endl;
				window_manager.general_window.scene_tab.current_scene.camera.position.print();

				printf("\n\n");

				std::cout << "Camera orientation quaternion x: " << camera_orientation.x << std::endl;
				std::cout << "Camera orientation quaternion y: " << camera_orientation.y << std::endl;
				std::cout << "Camera orientation quaternion z: " << camera_orientation.z << std::endl;
				std::cout << "Camera orientation quaternion w: " << camera_orientation.w << std::endl;

				window_manager.general_window.current_scene.save_scene(window_manager.general_window.scenes_folder, window_manager.general_window.scene_save_name, window_manager.general_window.models_folder, false, this->default_camera_position, window_manager.general_window.scene_tab.current_scene.camera.position, this->default_camera_direction, window_manager.general_window.scene_tab.current_scene.camera.direction, this->default_camera_up, window_manager.general_window.scene_tab.current_scene.camera.up, window_manager.general_window.scene_tab.current_scene.camera.yaw, window_manager.general_window.scene_tab.current_scene.camera.pitch, window_manager.general_window.scene_tab.current_scene.camera.roll, this->light_source);
				break;
				*/
			}
			
			break;
		case SDL_MOUSEMOTION:
		{
			if (SDL_GetRelativeMouseMode()) {
				double dx = event.motion.xrel;
				double dy = event.motion.yrel;

				double yaw_angle = 0;
				double pitch_angle = 0;

				if (dy != 0) {
					double pitch_angle = Utils::normalize(abs(dy), 0, this->HEIGHT, 0, M_PI);

					// Moving up
					if (dy < 0) {
						pitch_angle = -pitch_angle;
					}

					window_manager.general_window.scene_tab.current_scene.camera.pitch += pitch_angle * window_manager.general_window.scene_tab.real_rotation_speed;
				}

				if (dx != 0) {
					double yaw_angle = Utils::normalize(abs(dx), 0, this->WIDTH, 0, M_PI);

					// Moving right
					if (dx > 0) {
						yaw_angle = -yaw_angle;
					}

					window_manager.general_window.scene_tab.current_scene.camera.yaw += yaw_angle * window_manager.general_window.scene_tab.real_rotation_speed;
				}

				// Updates camera orientation quaternion, rotates the camera vectors, and updates the view matrix (and its inverse)
				if (dy != 0 || dx != 0) {
					Quaternion rotation = Quaternion::FromYawPitchRoll(Orientation::local, window_manager.general_window.scene_tab.current_scene.camera.yaw, window_manager.general_window.scene_tab.current_scene.camera.pitch, window_manager.general_window.scene_tab.current_scene.camera.roll, window_manager.general_window.scene_tab.current_scene.camera.default_right, window_manager.general_window.scene_tab.current_scene.camera.default_up, window_manager.general_window.scene_tab.current_scene.camera.default_direction);

					window_manager.general_window.scene_tab.current_scene.camera.orientation = rotation;

					window_manager.general_window.scene_tab.current_scene.camera.direction = rotation.get_rotationmatrix() * window_manager.general_window.scene_tab.current_scene.camera.default_direction;
					window_manager.general_window.scene_tab.current_scene.camera.up = rotation.get_rotationmatrix() * window_manager.general_window.scene_tab.current_scene.camera.default_up;

					window_manager.general_window.scene_tab.camera_tab->UpdateCameraRotation(RotationType_Quaternion, true);

					//window_manager.general_window.scene_tab.current_scene.camera.LookAt();
					//window_manager.general_window.scene_tab.current_scene.camera.update_view_inverse();
				}

			}
		}
		break;
		}
	}
	return 0;
}