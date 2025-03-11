#include "Engine.h"

bool Engine::handle_events() {
	const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

	if (keyboard_state[SDL_SCANCODE_W] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		if (!this->editing_mode) {
			this->camera_position += tz * this->camera_direction;
			Engine::LookAt();
			Engine::update_view_inverse();
		}
		else {
			if (!this->transform_light) {
				// Transforms target instance
				if (this->target_instance != nullptr) {
					Engine::translate(*this->target_instance, 0, 0, tz);
				}
			}
			// Transforms light source
			else {
				this->light_source.tz += tz;
				Engine::translate(this->light_source.position, 0, 0, tz);

				if (this->light_source.has_model) {
					Engine::translate(*this->light_source.instance, 0, 0, tz);
				}
			}
		}
	}

	if (keyboard_state[SDL_SCANCODE_S] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		this->camera_position -= tz * this->camera_direction;
		Engine::LookAt();
		Engine::update_view_inverse();

		if (!this->transform_light) {
			// Transforms target instance
			if (this->target_instance != nullptr) {
				Engine::translate(*this->target_instance, 0, 0, -tz);
			}
		}
		// Transforms light source
		else {
			this->light_source.tz += -tz;
			Engine::translate(this->light_source.position, 0, 0, -tz);

			if (this->light_source.has_model) {
				Engine::translate(*this->light_source.instance, 0, 0, -tz);
			}
		}
	}
	
	if (keyboard_state[SDL_SCANCODE_A] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		if (!this->editing_mode) {
			Mat camera_right = Mat::CrossProduct3D(this->camera_up, this->camera_direction);
			this->camera_position += tx * camera_right;
			Engine::LookAt();
			Engine::update_view_inverse();
		}
		else {
			if (!this->transform_light) {
				// Transforms target instance
				if (this->target_instance != nullptr) {
					Engine::translate(*this->target_instance, tx, 0, 0);
				}
			}
			// Transforms light source
			else {
				this->light_source.tx += tx;
				Engine::translate(this->light_source.position, tx, 0, 0);

				if (this->light_source.has_model) {
					Engine::translate(*this->light_source.instance, tx, 0, 0);
				}
			}
		}
	}



	if (keyboard_state[SDL_SCANCODE_D] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		if (!this->editing_mode) {
			Mat camera_right = Mat::CrossProduct3D(this->camera_up, this->camera_direction);
			this->camera_position -= tx * camera_right;
			Engine::LookAt();
			Engine::update_view_inverse();
		}
		else {
			if (!this->transform_light) {
				// Transforms target instance
				if (this->target_instance != nullptr) {
					Engine::translate(*this->target_instance, -tx, 0, 0);
				}
			}
			// Transforms light source
			else {
				this->light_source.tx += -tx;
				Engine::translate(this->light_source.position, -tx, 0, 0);

				if (this->light_source.has_model) {
					Engine::translate(*this->light_source.instance, -tx, 0, 0);
				}
			}
		}
	}

	if (keyboard_state[SDL_SCANCODE_UP] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		if (!this->editing_mode) {
			this->camera_position += ty * this->camera_up;
			Engine::LookAt();
			Engine::update_view_inverse();
		}
		else {
			if (!this->transform_light) {
				// Transforms target instance
				if (this->target_instance != nullptr) {
					Engine::translate(*this->target_instance, 0, ty, 0);
				}
			}
			// Transforms light source
			else {
				this->light_source.ty += ty;
				Engine::translate(this->light_source.position, 0, ty, 0);

				if (this->light_source.has_model) {
					Engine::translate(*this->light_source.instance, 0, ty, 0);
				}
			}
		}
	}

	if (keyboard_state[SDL_SCANCODE_DOWN] && !this->window_manager.show_window) {
		// Transforms camera and updates view matrix
		if (!this->editing_mode) {
			this->camera_position -= ty * this->camera_up;
			Engine::LookAt();
			Engine::update_view_inverse();
		}
		else {
			if (!this->transform_light) {
				// Transforms target instance
				if (this->target_instance != nullptr) {
					Engine::translate(*this->target_instance, 0, -ty, 0);
				}
			}
			// Transforms light source
			else {
				this->light_source.ty += -ty;

				Engine::translate(this->light_source.position, 0, -ty, 0);

				if (this->light_source.has_model) {
					Engine::translate(*this->light_source.instance, 0, -ty, 0);
				}
			}
		}
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

				this->WIDTH = this->event.window.data1;
				this->HEIGHT = this->event.window.data2;

				std::cout << "Window width: " << this->WIDTH << std::endl;
				std::cout << "Window height: " << this->HEIGHT << std::endl;

				this->pixel_buffer = (uint32_t*)malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);
				this->depth_buffer = (double*)malloc(sizeof(double) * this->WIDTH * this->HEIGHT);
				for (size_t i = 0; i < this->WIDTH * this->HEIGHT; i++) {
					this->pixel_buffer[i] = this->BG_COLOR;
					this->depth_buffer[i] = std::numeric_limits<double>::max();
				}

				this->AR = (double)this->WIDTH / (double)this->HEIGHT;
				//this->AR = 16 / 9.f;

				SDL_DestroyTexture(this->texture);
				this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->WIDTH, this->HEIGHT);
				if (this->texture == nullptr) {
					std::cout << "Could not create texture. Error: " << SDL_GetError() << std::endl;
					SDL_DestroyRenderer(this->renderer);
					SDL_DestroyWindow(this->window);
					SDL_Quit();
					return 0;
				}

				this->SCALE_MATRIX = Mat(
					{
						{this->WIDTH / 2., 0, 0, this->WIDTH / 2.},
						{0, this->HEIGHT / 2., 0, this->HEIGHT / 2.},
						{0, 0, 1, 0},
						{0, 0, 0, 1}
					}, 4, 4);

				this->update_projection_matrix();

				break;
			}

			break;
		case SDL_KEYDOWN:
			// Ignores keyboard key presses if user is inputting text into any ImGUI input
			if (ImGui::GetIO().WantTextInput) break;

			switch (this->event.key.keysym.scancode) {
			case SDL_SCANCODE_KP_ENTER:
				this->transform_light = !this->transform_light;
				if (this->transform_light) {
					std::cout << "Transforming light source: Enabled" << std::endl;
				}
				else {
					std::cout << "Transforming light source: Disabled (now key shortcuts apply them to the active instance)" << std::endl;
				}
				break;
			case SDL_SCANCODE_N:
				// D
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
			case SDL_SCANCODE_B:
				// S
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
			case SDL_SCANCODE_V:
				// A
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
			case SDL_SCANCODE_G:
				// W
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
			case SDL_SCANCODE_F:
				// ARROW DOWN
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
			case SDL_SCANCODE_H:
				// ARROW UP
				if (this->transform_light) {
					// light
				}
				else {
					// instance
				}
				break;
			case SDL_SCANCODE_KP_0:
				if (!this->playing) break;
				light_source.minimum_exposure -= 0.1;
				if (light_source.minimum_exposure < 0) light_source.minimum_exposure = 0;

				std::cout << "Minimum exposure (light): " << light_source.minimum_exposure << std::endl;
				break;
				/*
			{
				Mat rot_axis = this->camera_up;
				this->light_source_pos = Quaternion::RotatePoint(this->light_source_pos, rot_axis, -rotation_angle, true);
				Instance* light = this->current_scene.get_instance_ptr("let");
				light->yaw += -rotation_angle;
				light->ROTATION_MATRIX = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), -rotation_angle).get_rotationmatrix() * light->ROTATION_MATRIX;	light->orientation = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), -rotation_angle) * light->orientation;
				light->MODEL_TO_WORLD = light->ROTATION_MATRIX * light->TRANSLATION_MATRIX * light->SCALING_MATRIX;
			}
				*/
				break;
			case SDL_SCANCODE_KP_PERIOD:
				if (!this->playing) break;
				light_source.minimum_exposure += 0.1;
				if (light_source.minimum_exposure > 1) light_source.minimum_exposure = 1;

				std::cout << "Minimum exposure (light): " << light_source.minimum_exposure << std::endl;
				break;
				/*
			{
				Mat rot_axis = this->camera_up;
				this->light_source_pos = Quaternion::RotatePoint(this->light_source_pos, rot_axis, rotation_angle, true);
				Instance* light = this->current_scene.get_instance_ptr("let");
				light->yaw += rotation_angle;
				light->ROTATION_MATRIX = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), rotation_angle).get_rotationmatrix() * light->ROTATION_MATRIX;
				light->orientation = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), rotation_angle) * light->orientation;
				light->MODEL_TO_WORLD = light->ROTATION_MATRIX * light->TRANSLATION_MATRIX * light->SCALING_MATRIX;
			}
				*/
				break;
			case SDL_SCANCODE_PAGEUP:
				if (!this->playing) break;
				light_source.position.set(light_source.position.get(3, 1) + this->translation_amount, 3, 1);
				if (light_source.has_model) {
					light_source.instance->tz += this->translation_amount;
					light_source.instance->TRANSLATION_MATRIX = Mat::translation_matrix(light_source.instance->tx, light_source.instance->ty, light_source.instance->tz);
					light_source.instance->MODEL_TO_WORLD = light_source.instance->TRANSLATION_MATRIX * light_source.instance->ROTATION_MATRIX * light_source.instance->SCALING_MATRIX;
				}

				std::cout << "Light source position: ";
				this->light_source.position.print();
				break;
			case SDL_SCANCODE_PAGEDOWN:
				if (!this->playing) break;
				light_source.position.set(light_source.position.get(3, 1) - this->translation_amount, 3, 1);
				if (light_source.has_model) {
					light_source.instance->tz -= this->translation_amount;
					light_source.instance->TRANSLATION_MATRIX = Mat::translation_matrix(light_source.instance->tx, light_source.instance->ty, light_source.instance->tz);
					light_source.instance->MODEL_TO_WORLD = light_source.instance->TRANSLATION_MATRIX * light_source.instance->ROTATION_MATRIX * light_source.instance->SCALING_MATRIX;
				}
				std::cout << "Light source position: ";
				this->light_source.position.print();
				break;
			case SDL_SCANCODE_KP_PLUS:
				if (!this->playing) break;
				if (light_source.intensity >= 0.1) light_source.intensity -= 0.1;
				else light_source.intensity = 0;
				std::cout << "Light intensity: " << light_source.intensity << std::endl;
				break;
			case SDL_SCANCODE_KP_MINUS:
				if (!this->playing) break;
				light_source.intensity += 0.1;
				std::cout << "Light intensity: " << light_source.intensity << std::endl;
				break;
			case SDL_SCANCODE_KP_MULTIPLY:
				if (!this->playing) break;
				z_fighting_tolerance += 0.001;
				std::cout << "Z fighting tolerance: " << z_fighting_tolerance << std::endl;
				break;
			case SDL_SCANCODE_KP_DIVIDE:
				if (!this->playing) break;
				z_fighting_tolerance -= 0.001;
				std::cout << "Z fighting tolerance: " << z_fighting_tolerance << std::endl;
				break;

			case SDL_SCANCODE_1:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				this->window_manager.show_window = !this->window_manager.show_window;

				// Disables relative mouse tracking when the menu is open and re-enables once closed
				if (this->window_manager.show_window) {
					SDL_SetRelativeMouseMode(SDL_FALSE);
				}
				else {
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}

				if (!this->playing) break;



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
			case SDL_SCANCODE_2:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;

				/*
				this->rasterize = !this->rasterize;
				if (this->rasterize && this->wireframe_render) {
					this->wireframe_render = false;
				}

				if (this->rasterize) {
					std::cout << "Rasterization: Enabled" << std::endl;
				}
				else {
					std::cout << "Rasterization: Disabled" << std::endl;
				}
				break;
				*/
				break;
			case SDL_SCANCODE_3:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;
				this->shade = !this->shade;
				if (this->shade && this->wireframe_render) {
					this->wireframe_render = false;
				}

				if (this->shade) {
					std::cout << "Shading: Enabled" << std::endl;
				}
				else {
					std::cout << "Shading: Disabled" << std::endl;
				}
				break;
			case SDL_SCANCODE_4:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;
				this->backface_cull = !this->backface_cull;

				if (this->backface_cull) {
					std::cout << "Backface culling: Enabled" << std::endl;
				}
				else {
					std::cout << "Backface culling: Disabled" << std::endl;
				}
				break;
			case SDL_SCANCODE_5:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;
				this->depth_test = !this->depth_test;

				if (this->depth_test) {
					std::cout << "Depth testing: Enabled" << std::endl;
				}
				else {
					std::cout << "Depth testing: Disabled" << std::endl;
				}

				break;
			case SDL_SCANCODE_6:
				if (!this->playing) break;
				this->editing_mode = !this->editing_mode;
				this->window_manager.show_window = !this->window_manager.show_window;

				if (this->editing_mode) {
					std::cout << "Editing mode: Enabled" << std::endl;
				}
				else {
					std::cout << "Editing mode: Disabled" << std::endl;
				}
				break;

			case SDL_SCANCODE_7:
				if (!this->playing) break;
				this->transform_light = !this->transform_light;

				if (this->transform_light) {
					std::cout << "Light source transformation: Enabled" << std::endl;

					this->window_manager.general_window.light_source = &this->light_source;
					this->window_manager.general_window.target_instance = light_source.instance;
				}
				else {
					std::cout << "Light source transformation: Disabled" << std::endl;
				}
				break;
			case SDL_SCANCODE_8:
				/*
				if (!this->playing) break;
				if (this->using_instance_local_orientation) {
					this->using_instance_local_orientation = false;
					this->using_instance_world_orientation = true;
					std::cout << "Transformation orientation: World coordinate system" << std::endl;
				}
				else if (this->using_instance_world_orientation) {
					this->using_instance_world_orientation = false;
					this->using_instance_local_orientation = true;
					std::cout << "Transformation orientation: Local coordinate system" << std::endl;
				}
				*/
				break;
			case SDL_SCANCODE_9:
				if (this->light_source.type == LightType::point) {
					this->light_source.type = LightType::directional;
					std::cout << "Light source type: directional" << std::endl;
				}
				else if (this->light_source.type == LightType::directional) {
					this->light_source.type = LightType::spotlight;
					std::cout << "Light source type: spotlight" << std::endl;
				}
				else if (this->light_source.type == LightType::spotlight) {
					this->light_source.type = LightType::point;
					std::cout << "Light source type: point" << std::endl;
				}
				break;
			case SDL_SCANCODE_0:
				if (this->shading_type == ShadingType::Flat) {
					this->shading_type = ShadingType::Gouraud;
					std::cout << "Shading type: Gouraud" << std::endl;
				}
				else if (this->shading_type == ShadingType::Gouraud) {
					this->shading_type = ShadingType::Phong;
					std::cout << "Shading type: Phong" << std::endl;
				}
				else if (this->shading_type == ShadingType::Phong) {
					this->shading_type = ShadingType::Flat;
					std::cout << "Shading type: Flat" << std::endl;
				}
				break;
			case SDL_SCANCODE_P:
				this->playing = !this->playing;
				break;
			case SDL_SCANCODE_T:
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();

				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();
				std::cout << "Camera up: " << std::endl;
				this->camera_up.print();
				std::cout << "Camera yaw: " << this->camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << this->camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << this->camera_roll * (180 / M_PI) << std::endl;
				/*

				std::cout << "Camera position: " << std::endl;
				this->camera_position.print();

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
			case SDL_SCANCODE_Y:
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				camera_orientation.GetAngles(Orientation::local, camera_yaw, camera_pitch, camera_roll);
				std::cout << "Camera yaw: " << camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << camera_roll * (180 / M_PI) << std::endl;
				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();
				std::cout << "Camera up: " << std::endl;
				this->camera_up.print();
				std::cout << "Camera position: " << std::endl;
				this->camera_position.print();

				printf("\n\n");

				std::cout << "Camera orientation quaternion x: " << camera_orientation.x << std::endl;
				std::cout << "Camera orientation quaternion y: " << camera_orientation.y << std::endl;
				std::cout << "Camera orientation quaternion z: " << camera_orientation.z << std::endl;
				std::cout << "Camera orientation quaternion w: " << camera_orientation.w << std::endl;

				this->current_scene.save_scene(this->scenes_folder, this->scene_save_name, this->models_folder, false, this->default_camera_position, this->camera_position, this->default_camera_direction, this->camera_direction, this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll, this->light_source);
				break;
			case SDL_SCANCODE_Z:
				if (!this->playing) break;
				break;
			case SDL_SCANCODE_X:
				if (!this->playing) break;
				break;
			case SDL_SCANCODE_C:
				if (!this->playing) break;
				break;
			case SDL_SCANCODE_HOME:
				break;
			case SDL_SCANCODE_END:
				break;
			case SDL_SCANCODE_LEFTBRACKET:
				if (!this->playing) break;
				this->FOV--;
				FOVr = FOV * (M_PI / 180);
				printf("FOV: %f\n", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_RIGHTBRACKET:
				if (!this->playing) break;
				this->FOV++;
				FOVr = FOV * (M_PI / 180);
				printf("FOV: %f\n", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_SEMICOLON:
				if (!this->playing) break;

				// Limits the far plane to the near plane, so the far plane cannot get behind the near plane
				if (this->far - 0.5 >= this->near) {
					this->far -= 0.5;
				}
				else {
					this->far = this->near + 0.5;
				}

				printf("Far plane: %f\n", this->far);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_APOSTROPHE:
				if (!this->playing) break;
				// Far plane has no upper limit
				this->far += 0.5;

				printf("Far plane: %f\n", this->far);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_COMMA:
				if (!this->playing) break;
				// Least amount for near plane is 0.1
				if (this->near - 0.01 >= 0.01) {
					this->near -= 0.01;
				}
				else {
					this->near = 0.01;
				}

				this->update_projection_matrix();

				printf("Near plane: %f\n", this->near);
				break;
			case SDL_SCANCODE_PERIOD:
				if (!this->playing) break;
				// Limits the near plane to the far plane, so the near plane cannot get in front the far plane
				if (this->near + 0.01 < this->far) {
					this->near += 0.01;
				}
				else {
					this->near = this->far;
				}

				this->update_projection_matrix();

				printf("Near plane: %f\n", this->near);
				break;
			case SDL_SCANCODE_J:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), rotation_angle);
						Mat rotation = rotationY.get_rotationmatrix();
						this->camera_direction = rotation * this->camera_direction;
						this->camera_orientation = rotationY * this->camera_orientation;

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateY(*target_instance, this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateY(*target_instance, this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;

							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateY(this->light_source, this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}

					/*
					Engine::translate(*obj, 0.01, -0.01, 0);

					if (obj->roll < 1.5) Engine::rotateZ(*obj, 0.1);
					else if (obj->roll >= 1.5 && obj->pitch >= 1.5) Engine::rotateZ(*obj, 0.1);
					else Engine::rotateX(*obj, 0.1);
					*/

					/*

					std::cout << "Yaw: " << obj->yaw << std::endl;
					std::cout << "Pitch: " << obj->pitch << std::endl;
					std::cout << "Roll: " << obj->roll << std::endl;
					std::cout << "Rotation matrix: " << obj->ROTATION_MATRIX << std::endl;
					std::cout << "Quaternion x: " << obj->orientation.x << std::endl;
					std::cout << "Quaternion y: " << obj->orientation.y << std::endl;
					std::cout << "Quaternion z: " << obj->orientation.z << std::endl;
					std::cout << "Quaternion w: " << obj->orientation.w << std::endl;
					std::cout << "Tx: " << obj->tx << std::endl;
					std::cout << "Ty: " << obj->ty << std::endl;
					std::cout << "Tz: " << obj->tz << std::endl;
					*/
				}
				break;
			case SDL_SCANCODE_L:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), -rotation_angle);
						Mat rotation = rotationY.get_rotationmatrix();
						this->camera_direction = rotation * this->camera_direction;
						this->camera_orientation = rotationY * this->camera_orientation;

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateY(*target_instance, -this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateY(*target_instance, -this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;
							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateY(this->light_source, -this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}
				}
				break;
			case SDL_SCANCODE_I:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Mat camera_right = Mat::CrossProduct3D(camera_up, camera_direction);
						Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), rotation_angle);
						Mat rotation = rotationX.get_rotationmatrix();

						this->camera_direction = rotation * camera_direction;
						this->camera_up = rotation * camera_up;

						this->camera_orientation = rotationX * this->camera_orientation;

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateX(*target_instance, this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateX(*target_instance, this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;
							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateX(this->light_source, this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}
				}
				break;
			case SDL_SCANCODE_K:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Mat camera_right = Mat::CrossProduct3D(camera_up, camera_direction);

						Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), -rotation_angle);
						Mat rotation = rotationX.get_rotationmatrix();

						this->camera_direction = rotation * camera_direction;
						this->camera_up = rotation * camera_up;
						this->camera_orientation = rotationX * this->camera_orientation;

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateX(*target_instance, -this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateX(*target_instance, -this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;
							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateX(this->light_source, -this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}
				}
				break;
			case SDL_SCANCODE_Q:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), rotation_angle);
						Mat rotation = rotationZ.get_rotationmatrix();
						this->camera_up = rotation * this->camera_up;
						this->camera_orientation = rotationZ * this->camera_orientation;

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateZ(*target_instance, this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateZ(*target_instance, this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;
							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateZ(this->light_source, this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}
				}
				break;
			case SDL_SCANCODE_E:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), -rotation_angle);
						Mat rotation = rotationZ.get_rotationmatrix();
						this->camera_up = rotation * this->camera_up;
						this->camera_orientation = rotationZ * this->camera_orientation;
						//Engine::rotateZ(this->camera_up, -this->rotation_angle);

						Engine::LookAt();
						Engine::update_view_inverse();

						this->camera_orientation.GetAngles(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll);
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								if (this->using_instance_world_orientation) {
									Engine::rotateZ(*target_instance, -this->rotation_angle, Orientation::world, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
								else if (this->using_instance_local_orientation) {
									Engine::rotateZ(*target_instance, -this->rotation_angle, Orientation::local, this->default_camera_direction, this->default_camera_up, this->default_camera_right);
								}
							}
						}
						// Transforms light source
						else {
							Orientation rotation_orientation = Orientation::world;
							if (this->using_instance_world_orientation) {
								rotation_orientation = Orientation::world;
							}
							else if (this->using_instance_local_orientation) {
								rotation_orientation = Orientation::local;
							}

							Engine::rotateZ(this->light_source, -this->rotation_angle, rotation_orientation, default_camera_direction, default_camera_up, default_camera_right);

							/*
							this->light_source.orientation.GetAngles(this->light_source.yaw, this->light_source.pitch, this->light_source.roll);

							if (this->light_source.has_model) {
								this->light_source.orientation.GetAngles(this->light_source.instance->yaw, this->light_source.instance->pitch, this->light_source.instance->roll);
							}
							*/
						}
					}
				}
				break;
			/*
			case SDL_SCANCODE_A:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Mat camera_right = Mat::CrossProduct3D(this->camera_up, this->camera_direction);
						this->camera_position += tx * camera_right;
						Engine::LookAt();
						Engine::update_view_inverse();
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								Engine::translate(*this->target_instance, tx, 0, 0);
							}
						}
						// Transforms light source
						else {
							this->light_source.tx += tx;
							Engine::translate(this->light_source.position, tx, 0, 0);

							if (this->light_source.has_model) {
								Engine::translate(*this->light_source.instance, tx, 0, 0);
							}
						}
					}
				}
				break;
			case SDL_SCANCODE_D:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						Mat camera_right = Mat::CrossProduct3D(this->camera_up, this->camera_direction);
						this->camera_position -= tx * camera_right;
						Engine::LookAt();
						Engine::update_view_inverse();
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								Engine::translate(*this->target_instance, -tx, 0, 0);
							}
						}
						// Transforms light source
						else {
							this->light_source.tx += -tx;
							Engine::translate(this->light_source.position, -tx, 0, 0);

							if (this->light_source.has_model) {
								Engine::translate(*this->light_source.instance, -tx, 0, 0);
							}
						}
					}
				}
				break;
			case SDL_SCANCODE_W:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						this->camera_position += tz * this->camera_direction;
						Engine::LookAt();
						Engine::update_view_inverse();
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								Engine::translate(*this->target_instance, 0, 0, tz);
							}
						}
						// Transforms light source
						else {
							this->light_source.tz += tz;
							Engine::translate(this->light_source.position, 0, 0, tz);

							if (this->light_source.has_model) {
								Engine::translate(*this->light_source.instance, 0, 0, tz);
							}
						}
					}
				}
				break;
			case SDL_SCANCODE_S:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					this->camera_position -= tz * this->camera_direction;
					Engine::LookAt();
					Engine::update_view_inverse();
					
					if (!this->transform_light) {
						// Transforms target instance
						if (this->target_instance != nullptr) {
							Engine::translate(*this->target_instance, 0, 0, -tz);
						}
					}
					// Transforms light source
					else {
						this->light_source.tz += -tz;
						Engine::translate(this->light_source.position, 0, 0, -tz);

						if (this->light_source.has_model) {
							Engine::translate(*this->light_source.instance, 0, 0, -tz);
						}
					}
				}
				break;
			case SDL_SCANCODE_UP:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						this->camera_position += ty * this->camera_up;
						Engine::LookAt();
						Engine::update_view_inverse();
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								Engine::translate(*this->target_instance, 0, ty, 0);
							}
						}
						// Transforms light source
						else {
							this->light_source.ty += ty;
							Engine::translate(this->light_source.position, 0, ty, 0);

							if (this->light_source.has_model) {
								Engine::translate(*this->light_source.instance, 0, ty, 0);
							}
						}
					}
				}
				break;
			case SDL_SCANCODE_DOWN:
				if (!this->playing) break;
				{
					// Transforms camera and updates view matrix
					if (!this->editing_mode) {
						this->camera_position -= ty * this->camera_up;
						Engine::LookAt();
						Engine::update_view_inverse();
					}
					else {
						if (!this->transform_light) {
							// Transforms target instance
							if (this->target_instance != nullptr) {
								Engine::translate(*this->target_instance, 0, -ty, 0);
							}
						}
							// Transforms light source
						else {
							this->light_source.ty += -ty;
							
							Engine::translate(this->light_source.position, 0, -ty, 0);

							if (this->light_source.has_model) {
								Engine::translate(*this->light_source.instance, 0, -ty, 0);
							}
						}
					}
				}
				break;
				*/
			default:
				break;
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

					this->camera_pitch += pitch_angle * this->camera_rotation_speed_factor;
				}

				if (dx != 0) {
					double yaw_angle = Utils::normalize(abs(dx), 0, this->WIDTH, 0, M_PI);

					// Moving right
					if (dx > 0) {
						yaw_angle = -yaw_angle;
					}

					this->camera_yaw += yaw_angle * this->camera_rotation_speed_factor;
				}

				// Updates camera orientation quaternion, rotates the camera vectors, and updates the view matrix (and its inverse)
				if (dy != 0 || dx != 0) {
					Quaternion rotation = Quaternion::FromYawPitchRoll(Orientation::local, this->camera_yaw, this->camera_pitch, this->camera_roll, this->default_camera_right, this->default_camera_up, this->default_camera_direction);

					this->camera_orientation = rotation;

					this->camera_direction = rotation.get_rotationmatrix() * this->default_camera_direction;
					this->camera_up = rotation.get_rotationmatrix() * this->default_camera_up;

					Engine::LookAt();
					Engine::update_view_inverse();
				}

			}
		}
			break;
		}
	}

	return 0;
}