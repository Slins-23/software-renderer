#include "Engine.h"
#include <cassert>

bool Engine::setup() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL video. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 0;
	}

	this->window = SDL_CreateWindow(this->TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->WIDTH, this->HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == nullptr) {
		std::cout << "Could not create window. Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 0;
	}

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
	if (this->renderer == nullptr) {
		std::cout << "Could not create renderer. Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(this->window);
		SDL_Quit();
		return 0;
	}

	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->WIDTH, this->HEIGHT);
	if (this->texture == nullptr) {
		std::cout << "Could not create texture. Error: " << SDL_GetError() << std::endl;
		SDL_DestroyRenderer(this->renderer);
		SDL_DestroyWindow(this->window);
		SDL_Quit();
		return 0;
	}

	this->pixel_buffer = (uint32_t*) malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);
	this->depth_buffer = (double*)malloc(sizeof(double) * this->WIDTH * this->HEIGHT);

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		this->pixel_buffer[i] = this->BG_COLOR;
		this->depth_buffer[i] = std::numeric_limits<double>::max();
	}

	return 1;
}

bool Engine::handle_events() {
	while (SDL_PollEvent(&this->event)) {
		switch (this->event.type) {
		case SDL_QUIT:
			close();
			return 1;
		case SDL_WINDOWEVENT:
			switch (this->event.window.event) {
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
			switch (this->event.key.keysym.scancode) {
			case SDL_SCANCODE_8:
				/*
			{
				Mat rot_axis = this->camera_up;
				this->light_source_pos = Quaternion::RotatePoint(this->light_source_pos, rot_axis, -rotation_angle, true);
				Instance* light = this->current_scene.get_instance_ptr("let");
				light->yaw += -rotation_angle;
				light->ROTATION_MATRIX = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), -rotation_angle).get_rotationmatrix() * light->ROTATION_MATRIX;
				light->orientation = Quaternion::AngleAxis(rot_axis.get(1, 1), rot_axis.get(2, 1), rot_axis.get(3, 1), -rotation_angle) * light->orientation;
				light->MODEL_TO_WORLD = light->ROTATION_MATRIX * light->TRANSLATION_MATRIX * light->SCALING_MATRIX;
			}
				*/
				break;
			case SDL_SCANCODE_9:
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
			case SDL_SCANCODE_KP_4:
				if (!this->playing) break;

				{

					// same as j
					Instance* obj = this->current_scene.get_instance_ptr(5);
					obj->tx += 0.01;
					obj->TRANSLATION_MATRIX = Mat::translation_matrix(obj->tx, obj->ty, obj->tz);
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;

					// same as i
					obj = this->current_scene.get_instance_ptr(5);
					obj->roll += 0.1;
					Mat obj_x_axis = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
					Mat obj_y_axis = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
					Mat obj_z_axis = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

					Quaternion rotationY = Quaternion::AngleAxis(obj_y_axis.get(1, 1), obj_y_axis.get(2, 1), obj_y_axis.get(3, 1), obj->yaw);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_y_axis, obj->yaw);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_y_axis, obj->yaw);

					Quaternion rotationX = Quaternion::AngleAxis(obj_x_axis.get(1, 1), obj_x_axis.get(2, 1), obj_x_axis.get(3, 1), obj->pitch);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_x_axis, obj->pitch);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_x_axis, obj->pitch);

					Quaternion rotationZ = Quaternion::AngleAxis(obj_z_axis.get(1, 1), obj_z_axis.get(2, 1), obj_z_axis.get(3, 1), obj->roll);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_z_axis, obj->roll);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_z_axis, obj->roll);

					Quaternion orientation = rotationZ * rotationX * rotationY;
					obj->orientation = orientation;
					obj->ROTATION_MATRIX = obj->orientation.get_rotationmatrix();
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_KP_6:
				if (!this->playing) break;
				{
					// same as l
					Instance* obj = this->current_scene.get_instance_ptr(5);
					obj->tx -= 0.01;
					obj->TRANSLATION_MATRIX = Mat::translation_matrix(obj->tx, obj->ty, obj->tz);
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_KP_8:
				if (!this->playing) break;
				{
					// same as i
					Instance* obj = this->current_scene.get_instance_ptr(0);
					obj->pitch += 0.1;
					Mat obj_x_axis = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
					Mat obj_y_axis = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
					Mat obj_z_axis = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

					Quaternion rotationY = Quaternion::AngleAxis(obj_y_axis.get(1, 1), obj_y_axis.get(2, 1), obj_y_axis.get(3, 1), obj->yaw);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_y_axis, obj->yaw);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_y_axis, obj->yaw);

					Quaternion rotationX = Quaternion::AngleAxis(obj_x_axis.get(1, 1), obj_x_axis.get(2, 1), obj_x_axis.get(3, 1), obj->pitch);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_x_axis, obj->pitch);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_x_axis, obj->pitch);

					Quaternion rotationZ = Quaternion::AngleAxis(obj_z_axis.get(1, 1), obj_z_axis.get(2, 1), obj_z_axis.get(3, 1), obj->roll);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_z_axis, obj->roll);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_z_axis, obj->roll);

					Quaternion orientation = rotationZ * rotationX * rotationY;
					obj->orientation = orientation;
					obj->ROTATION_MATRIX = obj->orientation.get_rotationmatrix();
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_KP_2:
				if (!this->playing) break;
				// same as k
				{
					Instance* obj = this->current_scene.get_instance_ptr(0);
					obj->pitch -= 0.1;
					Mat obj_x_axis = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
					Mat obj_y_axis = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
					Mat obj_z_axis = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

					Quaternion rotationY = Quaternion::AngleAxis(obj_y_axis.get(1, 1), obj_y_axis.get(2, 1), obj_y_axis.get(3, 1), obj->yaw);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_y_axis, obj->yaw);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_y_axis, obj->yaw);

					Quaternion rotationX = Quaternion::AngleAxis(obj_x_axis.get(1, 1), obj_x_axis.get(2, 1), obj_x_axis.get(3, 1), obj->pitch);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_x_axis, obj->pitch);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_x_axis, obj->pitch);

					Quaternion rotationZ = Quaternion::AngleAxis(obj_z_axis.get(1, 1), obj_z_axis.get(2, 1), obj_z_axis.get(3, 1), obj->roll);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_z_axis, obj->roll);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_z_axis, obj->roll);

					Quaternion orientation = rotationZ * rotationX * rotationY;
					obj->orientation = orientation;
					obj->ROTATION_MATRIX = obj->orientation.get_rotationmatrix();
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_KP_7:
				if (!this->playing) break;
				// same as q
				{
					Instance* obj = this->current_scene.get_instance_ptr(5);
					obj->sx -= 0.01;
					obj->sy -= 0.01;
					obj->sz -= 0.01;
					obj->SCALING_MATRIX = Mat::scale_matrix(obj->sx, obj->sy, obj->sz);
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_KP_9:
				if (!this->playing) break;
				// same as e
				{
					Instance* obj = this->current_scene.get_instance_ptr(5);
					obj->sx += 0.01;
					obj->sy += 0.01;
					obj->sz += 0.01;
					obj->SCALING_MATRIX = Mat::scale_matrix(obj->sx, obj->sy, obj->sz);
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;

					// same as i
					obj = this->current_scene.get_instance_ptr(5);
					obj->yaw += 0.1;
					Mat obj_x_axis = Mat({ {1}, {0}, {0}, {0} }, 4, 1);
					Mat obj_y_axis = Mat({ {0}, {1}, {0}, {0} }, 4, 1);
					Mat obj_z_axis = Mat({ {0}, {0}, {1}, {0} }, 4, 1);

					Quaternion rotationY = Quaternion::AngleAxis(obj_y_axis.get(1, 1), obj_y_axis.get(2, 1), obj_y_axis.get(3, 1), obj->yaw);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_y_axis, obj->yaw);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_y_axis, obj->yaw);

					Quaternion rotationX = Quaternion::AngleAxis(obj_x_axis.get(1, 1), obj_x_axis.get(2, 1), obj_x_axis.get(3, 1), obj->pitch);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_x_axis, obj->pitch);
					//obj_z_axis = Quaternion::RotatePoint(obj_z_axis, obj_x_axis, obj->pitch);

					Quaternion rotationZ = Quaternion::AngleAxis(obj_z_axis.get(1, 1), obj_z_axis.get(2, 1), obj_z_axis.get(3, 1), obj->roll);
					//obj_x_axis = Quaternion::RotatePoint(obj_x_axis, obj_z_axis, obj->roll);
					//obj_y_axis = Quaternion::RotatePoint(obj_y_axis, obj_z_axis, obj->roll);

					Quaternion orientation = rotationZ * rotationX * rotationY;
					obj->orientation = orientation;
					obj->ROTATION_MATRIX = obj->orientation.get_rotationmatrix();
					obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
				}
				break;
			case SDL_SCANCODE_PAGEUP:
				if (!this->playing) break;
				this->light_source_pos.set(this->light_source_pos.get(3, 1) + 0.1, 3, 1);
				std::cout << "Light source position: ";
				this->light_source_pos.print();
				break;
			case SDL_SCANCODE_PAGEDOWN:
				if (!this->playing) break;
				this->light_source_pos.set(this->light_source_pos.get(3, 1) - 0.1, 3, 1);
				std::cout << "Light source position: ";
				this->light_source_pos.print();
				break;
			case SDL_SCANCODE_KP_DIVIDE:
				if (!this->playing) break;
				if (this->light_intensity >= 0.199) this->light_intensity -= 0.1;
				std::cout << "Light intensity: " << this->light_intensity << std::endl;
				break;
			case SDL_SCANCODE_KP_MULTIPLY:
				if (!this->playing) break;
				this->light_intensity += 0.1;
				std::cout << "Light intensity: " << this->light_intensity << std::endl;
				break;
			case SDL_SCANCODE_KP_PERIOD:
				if (!this->playing) break;
				z_fighting_tolerance += 0.001;
				std::cout << "Z fighting tolerance: " << z_fighting_tolerance << std::endl;
				break;
			case SDL_SCANCODE_KP_ENTER:
				if (!this->playing) break;
				z_fighting_tolerance -= 0.001;
				std::cout << "Z fighting tolerance: " << z_fighting_tolerance << std::endl;
				break;

			case SDL_SCANCODE_1:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;
				this->wireframe_render = !this->wireframe_render;
				if (this->wireframe_render) {
					std::cout << "Wireframe rendering: Enabled" << std::endl;
				}
				else {
					std::cout << "Wireframe rendering: Disabled" << std::endl;
				}

				break;
			case SDL_SCANCODE_2:
				// Should I update the pixel buffer one last time to reflect the changes from the rendering mode instead of not allowing them to be changed if paused? Makes more sense for the "scene editor" behavior.

				if (!this->playing) break;
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
			case SDL_SCANCODE_P:
				this->playing = !this->playing;
				break;
			case SDL_SCANCODE_T:
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				std::cout << "Camera yaw: " << this->camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << this->camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << this->camera_roll * (180 / M_PI) << std::endl;
				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();
				std::cout << "Camera up: " << std::endl;
				this->camera_up.print();

				std::cout << "Camera position: " << std::endl;
				this->camera_position.print();

				break;
			case SDL_SCANCODE_G:
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				q_camera.GetAngles(camera_yaw, camera_pitch, camera_roll);
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

				std::cout << "Camera orientation quaternion x: " << q_camera.x << std::endl;
				std::cout << "Camera orientation quaternion y: " << q_camera.y << std::endl;
				std::cout << "Camera orientation quaternion z: " << q_camera.z << std::endl;
				std::cout << "Camera orientation quaternion w: " << q_camera.w << std::endl;

				this->current_scene.save_scene(this->scenes_folder, this->scene_save_name, this->models_folder, false, this->default_camera_position, this->camera_position, this->default_camera_direction, this->camera_direction, this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll);
				break;
			case SDL_SCANCODE_Z:
				if (!this->playing) break;
				{
				}
				break;
			case SDL_SCANCODE_X:
				if (!this->playing) break;
				{
					Instance* pilar = this->current_scene.get_instance_ptr("5");
					//double tx = pilar->MODEL_TO_WORLD.get(1, 4);
					//double ty = pilar->MODEL_TO_WORLD.get(2, 4);
					//double tz = pilar->MODEL_TO_WORLD.get(3, 4);
					//pilar->MODEL_TO_WORLD = Mat::translation_matrix(-tx, -ty, -tz) * pilar->MODEL_TO_WORLD;
					//pilar->MODEL_TO_WORLD = Engine::rotationX_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;
					//pilar->MODEL_TO_WORLD = Mat::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
					//break;

					pilar = this->current_scene.get_instance_ptr("5");
					pilar->yaw += 0.1;
					//pilar->pitch += 0.1;
					//pilar->roll += 0.1;
					//pilar->ty -= translation_amount;
					//pilar->tz -= translation_amount;
					pilar->ROTATION_MATRIX = Engine::quaternion_rotationZ_matrix(pilar->roll) * Engine::quaternion_rotationX_matrix(pilar->pitch) * Engine::quaternion_rotationY_matrix(pilar->yaw);
					pilar->MODEL_TO_WORLD = pilar->TRANSLATION_MATRIX * pilar->ROTATION_MATRIX * pilar->SCALING_MATRIX;
					break;
				}
				break;
			case SDL_SCANCODE_C:
				if (!this->playing) break;
				{
					Instance* pilar = this->current_scene.get_instance_ptr("5");
					double tx = pilar->MODEL_TO_WORLD.get(1, 4);
					double ty = pilar->MODEL_TO_WORLD.get(2, 4);
					double tz = pilar->MODEL_TO_WORLD.get(3, 4);
					//pilar->MODEL_TO_WORLD = Mat::translation_matrix(-tx, -ty, -tz) * pilar->MODEL_TO_WORLD;
					//pilar->MODEL_TO_WORLD = Engine::rotationX_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;
					//pilar->MODEL_TO_WORLD = Mat::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
					//break;

					pilar = this->current_scene.get_instance_ptr("5");
					//pilar->yaw -= 0.1;
					//pilar->pitch -= 0.1;
					pilar->roll += 0.1;
					//pilar->roll -= 0.1;
					//pilar->ty -= translation_amount;
					//pilar->tz -= translation_amount;
					pilar->ROTATION_MATRIX = Engine::quaternion_rotationZ_matrix(pilar->roll) * Engine::quaternion_rotationX_matrix(pilar->pitch) * Engine::quaternion_rotationY_matrix(pilar->yaw);
					pilar->MODEL_TO_WORLD = pilar->TRANSLATION_MATRIX * pilar->ROTATION_MATRIX * pilar->SCALING_MATRIX;
					break;
				}
				break;
			case SDL_SCANCODE_KP_MINUS:
				if (!this->playing) break;
				this->FOV--;
				FOVr = FOV * (M_PI / 180);
				printf("FOV: %f\n", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_KP_PLUS:
				if (!this->playing) break;
				this->FOV++;
				FOVr = FOV * (M_PI / 180);
				printf("FOV: %f\n", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_LEFTBRACKET:
				if (!this->playing) break;

				sx *= 1 - scale_factor;
				sy *= 1 - scale_factor;
				sz *= 1 - scale_factor;
				Engine::LookAt();
				break;
			case SDL_SCANCODE_RIGHTBRACKET:
				if (!this->playing) break;

				sx *= 1 + scale_factor;
				sy *= 1 + scale_factor;
				sz *= 1 + scale_factor;
				Engine::LookAt();
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
					Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), rotation_angle);
					Mat rotation = rotationY.get_rotationmatrix();
					this->camera_direction = rotation * this->camera_direction;
					this->q_camera = rotationY * this->q_camera;

					Engine::LookAt();
					Engine::Quaternion_GetAnglesFromDirection(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_L:
				if (!this->playing) break;
				{
					Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), -rotation_angle);
					Mat rotation = rotationY.get_rotationmatrix();
					this->camera_direction = rotation * this->camera_direction;
					this->q_camera = rotationY * this->q_camera;

					Engine::LookAt();
					Engine::Quaternion_GetAnglesFromDirection(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_I:
				if (!this->playing) break;
				{
					Mat camera_right = Engine::CrossProduct3D(camera_up, camera_direction);
					Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), rotation_angle);
					Mat rotation = rotationX.get_rotationmatrix();

					this->camera_direction = rotation * camera_direction;
					this->camera_up = rotation * camera_up;

					this->q_camera = rotationX * this->q_camera;

					Engine::LookAt();
					Engine::Quaternion_GetAnglesFromDirection(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_K:
				if (!this->playing) break;
				{
					Mat camera_right = Engine::CrossProduct3D(camera_up, camera_direction);

					Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), -rotation_angle);
					Mat rotation = rotationX.get_rotationmatrix();

					this->camera_direction = rotation * camera_direction;
					this->camera_up = rotation * camera_up;
					this->q_camera = rotationX * this->q_camera;

					Engine::LookAt();
					Engine::Quaternion_GetAnglesFromDirection(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_Q:
				if (!this->playing) break;
				{
					Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), rotation_angle);
					Mat rotation = rotationZ.get_rotationmatrix();
					this->camera_up = rotation * this->camera_up;
					this->q_camera = rotationZ * this->q_camera;

					Engine::LookAt();
					Engine::Quaternion_GetAnglesFromDirection(this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_E:
				if (!this->playing) break;
				{
					Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), -rotation_angle);
					Mat rotation = rotationZ.get_rotationmatrix();
					this->camera_up = rotation * this->camera_up;
					this->q_camera = rotationZ * this->q_camera;
					//Engine::rotateZ(this->camera_up, -this->rotation_angle);

					Engine::LookAt();
					this->q_camera.GetAngles(this->camera_yaw, this->camera_pitch, this->camera_roll);
				}
				break;
			case SDL_SCANCODE_A:
				if (!this->playing) break;
				{
					Mat camera_right = Engine::CrossProduct3D(this->camera_up, this->camera_direction);
					this->camera_position += tx * camera_right;
					Engine::LookAt();
				}
				break;
			case SDL_SCANCODE_D:
				if (!this->playing) break;
				{
					Mat camera_right = Engine::CrossProduct3D(this->camera_up, this->camera_direction);
					this->camera_position -= tx * camera_right;
					Engine::LookAt();
				}
				break;
			case SDL_SCANCODE_W:
				if (!this->playing) break;
				{
					this->camera_position += tz * this->camera_direction;
					Engine::LookAt();
				}
				break;
			case SDL_SCANCODE_S:
				if (!this->playing) break;
				{
					this->camera_position -= tz * this->camera_direction;
					Engine::LookAt();
				}
				break;
			case SDL_SCANCODE_UP:
				if (!this->playing) break;
				{
					this->camera_position += ty * this->camera_up;
					Engine::LookAt();
				}
				break;
			case SDL_SCANCODE_DOWN:
				if (!this->playing) break;
				{
					this->camera_position -= ty * this->camera_up;
					Engine::LookAt();
				}
				break;
			default:
				break;
			}

			break;
		}
	}

	return 0;
}

void Scene::save_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, const Mat& default_camera_position, const Mat& camera_position, const Mat& default_camera_direction, const Mat& camera_direction, const Mat& default_camera_up, const Mat& camera_up, double yaw, double pitch, double roll) {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scenes_folder, scene_filename);
	nlohmann::ordered_json json_object;

	json_object["scene"] = scene_filename;

	// The default camera position is considered to be the world coordinate in which the camera will be considered to be (0, 0, 0)
	json_object["camera"]["default_position"]["x"] = default_camera_position.get(1, 1);
	json_object["camera"]["default_position"]["y"] = default_camera_position.get(2, 1);
	json_object["camera"]["default_position"]["z"] = default_camera_position.get(3, 1);

	// Camera position relative to the default camera position
	json_object["camera"]["translation"]["x"] = camera_position.get(1, 1);
	json_object["camera"]["translation"]["y"] = camera_position.get(2, 1);
	json_object["camera"]["translation"]["z"] = camera_position.get(3, 1);

	json_object["camera"]["default_direction"]["x"] = default_camera_direction.get(1, 1);
	json_object["camera"]["default_direction"]["y"] = default_camera_direction.get(2, 1);
	json_object["camera"]["default_direction"]["z"] = default_camera_direction.get(3, 1);

	json_object["camera"]["direction"]["x"] = camera_direction.get(1, 1);
	json_object["camera"]["direction"]["y"] = camera_direction.get(2, 1);
	json_object["camera"]["direction"]["z"] = camera_direction.get(3, 1);

	json_object["camera"]["default_up"]["x"] = default_camera_up.get(1, 1);
	json_object["camera"]["default_up"]["y"] = default_camera_up.get(2, 1);
	json_object["camera"]["default_up"]["z"] = default_camera_up.get(3, 1);

	json_object["camera"]["up"]["x"] = camera_up.get(1, 1);
	json_object["camera"]["up"]["y"] = camera_up.get(2, 1);
	json_object["camera"]["up"]["z"] = camera_up.get(3, 1);

	json_object["camera"]["rotation"]["y"] = yaw * (180 / M_PI);
	json_object["camera"]["rotation"]["x"] = pitch * (180 / M_PI);
	json_object["camera"]["rotation"]["z"] = roll * (180 / M_PI);

	std::vector<std::string> models;
	std::vector<Instance*> instances;
	for (auto instance = this->scene_instances.begin(); instance != this->scene_instances.end(); instance++) {
		std::string mesh_filename = instance->mesh->mesh_filename;
		bool model_already_included = false;
		for (size_t model_n = 0; model_n < models.size(); model_n++) {
			std::string model_string = models[model_n];

			if (model_string == mesh_filename) {
				model_already_included = true;
				break;
			}
		}

		if (!model_already_included) {
			models.push_back(mesh_filename);
		}
		
		instances.push_back(&*instance);
	}

	for (auto model = models.begin(); model != models.end(); model++) {
		std::vector<std::string> txt_array;
		txt_array.push_back("instances");

		std::vector<std::string> instance_ids;
		for (size_t instance_id = 0; instance_id < this->total_instances; instance_id++) {
			Instance* instance = Scene::get_instance_ptr(instance_id);
			std::string mesh_filename = instance->mesh->mesh_filename;

			if (mesh_filename == *model) {
				json_object["models"][*model]["instances"][instance->instance_name]["show"] = instance->show;

				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["x"] = instance->tx;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["y"] = instance->ty;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["z"] = instance->tz;

				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["y"] = instance->yaw * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["x"] = instance->pitch * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["z"] = instance->roll * (180 / M_PI);

				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["x"] = instance->sx;
				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["y"] = instance->sy;
				json_object["models"][*model]["instances"][instance->instance_name]["scale"]["z"] = instance->sz;

				json_object["models"][*model]["instances"][instance->instance_name]["model_to_world"] = std::vector<std::vector<double>>
				{
					{instance->MODEL_TO_WORLD.get(1, 1), instance->MODEL_TO_WORLD.get(1, 2), instance->MODEL_TO_WORLD.get(1, 3), instance->MODEL_TO_WORLD.get(1, 4)},
					{instance->MODEL_TO_WORLD.get(2, 1), instance->MODEL_TO_WORLD.get(2, 2), instance->MODEL_TO_WORLD.get(2, 3), instance->MODEL_TO_WORLD.get(2, 4)},
					{instance->MODEL_TO_WORLD.get(3, 1), instance->MODEL_TO_WORLD.get(3, 2), instance->MODEL_TO_WORLD.get(3, 3), instance->MODEL_TO_WORLD.get(3, 4)},
					{instance->MODEL_TO_WORLD.get(4, 1), instance->MODEL_TO_WORLD.get(4, 2), instance->MODEL_TO_WORLD.get(4, 3), instance->MODEL_TO_WORLD.get(4, 4)}
				};
			}
			else {
				continue;
			}
		}
	}

	std::cout << json_object.dump(4) << std::endl;
	std::ofstream file(scene_filepath);
	file << json_object.dump(4);
	file.close();
}

void Scene::load_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_position, Mat& default_camera_direction, Mat& camera_direction, Mat& default_camera_up, Mat& camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, Mat& VIEW_MATRIX, bool use_scene_camera_settings) {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scenes_folder, scene_filename);

	std::ifstream file = std::ifstream(scene_filepath);

	this->scene_filepath = scene_filepath;
	this->scene_data = nlohmann::json::parse(file);

	this->scene_meshes.clear();
	this->scene_instances.clear();

	this->total_meshes = 0;
	this->total_instances = 0;
	this->total_triangles = 0;
	this->total_vertices = 0;

	this->rendered_meshes = 0;
	this->rendered_instances = 0;
	this->rendered_triangles = 0;
	this->rendered_vertices = 0;

	Mat tmp_dir = default_camera_direction;
	Mat tmp_up = default_camera_up;

	// Reset view matrix
	VIEW_MATRIX = Mat::identity_matrix(4);

	camera_position = default_camera_position;
	camera_direction = default_camera_direction;
	camera_up = default_camera_up;
	camera_yaw = 0;
	camera_pitch = 0;
	camera_roll = 0;

	Mat camera_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);

		// Then translates the camera
	if (use_scene_camera_settings) {
		if (this->scene_data["camera"].contains("default_position")) {
			double x = this->scene_data["camera"]["default_position"]["x"].get<double>();
			double y = this->scene_data["camera"]["default_position"]["y"].get<double>();
			double z = this->scene_data["camera"]["default_position"]["z"].get<double>();

			default_camera_position.set(x, 1, 1);
			default_camera_position.set(y, 2, 1);
			default_camera_position.set(z, 3, 1);

			camera_position = default_camera_position;
		}

		// Loads scene camera parameters (position, direction, and orientation)
		if (this->scene_data["camera"].contains("translation")) {
			double tx = this->scene_data["camera"]["translation"]["x"].get<double>();
			double ty = this->scene_data["camera"]["translation"]["y"].get<double>();
			double tz = this->scene_data["camera"]["translation"]["z"].get<double>();

			camera_position.set(tx + default_camera_position.get(1, 1), 1, 1);
			camera_position.set(ty + default_camera_position.get(2, 1), 2, 1);
			camera_position.set(tz + default_camera_position.get(3, 1), 3, 1);
		}
	}

	// Then rotates the camera
	if (use_scene_camera_settings) {

		if (this->scene_data["camera"].contains("default_direction")) {
			double default_x = this->scene_data["camera"]["default_direction"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_direction"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_direction"]["z"].get<double>();

			default_camera_direction.set(default_x, 1, 1);
			default_camera_direction.set(default_y, 2, 1);
			default_camera_direction.set(default_z, 3, 1);

			camera_direction = default_camera_direction;
		}

		if (this->scene_data["camera"].contains("default_up")) {
			double default_x = this->scene_data["camera"]["default_up"]["x"].get<double>();
			double default_y = this->scene_data["camera"]["default_up"]["y"].get<double>();
			double default_z = this->scene_data["camera"]["default_up"]["z"].get<double>();

			default_camera_up.set(default_x, 1, 1);
			default_camera_up.set(default_y, 2, 1);
			default_camera_up.set(default_z, 3, 1);

			camera_up = default_camera_up;
		}

		bool rotation_given = false;
		if (this->scene_data["camera"].contains("rotation")) {
			rotation_given = true;

			// Should be described in degrees

			if (this->scene_data["camera"]["rotation"].contains("y")) {
				camera_yaw = this->scene_data["camera"]["rotation"]["y"] * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("x")) {
				camera_pitch = this->scene_data["camera"]["rotation"]["x"] * (M_PI / 180);
			}

			if (this->scene_data["camera"]["rotation"].contains("z")) {
				camera_roll = this->scene_data["camera"]["rotation"]["z"] * (M_PI / 180);
			}
		}
		

		// If direction was given, update the direction vector
		if (this->scene_data["camera"].contains("direction")) {
			double x = this->scene_data["camera"]["direction"]["x"].get<double>();
			double y = this->scene_data["camera"]["direction"]["y"].get<double>();
			double z = this->scene_data["camera"]["direction"]["z"].get<double>();

			camera_direction.set(x, 1, 1);
			camera_direction.set(y, 2, 1);
			camera_direction.set(z, 3, 1);

			// If no rotation parameters were given, derive them from the direction vector
			if (!rotation_given) {
				// Gets yaw and pitch representing the rotation from the default camera direction to the camera direction
				
				Engine::Quaternion_GetAnglesFromDirection(default_camera_direction, camera_direction, camera_yaw, camera_pitch, camera_roll);
				Engine::rotateY(tmp_dir, camera_yaw);
				Engine::rotateX(tmp_dir, camera_pitch);
			}
		}
		
		// If direction was not given, but rotation was - rotates the default camera direction vectors by the rotation parameters
		else if (rotation_given) {
			camera_direction = Quaternion::RotatePoint(camera_direction, camera_up, camera_yaw, false);
			camera_right = Quaternion::RotatePoint(camera_right, camera_up, camera_yaw, false);
			camera_direction = Quaternion::RotatePoint(camera_direction, camera_right, camera_pitch, false);
			camera_up = Quaternion::RotatePoint(camera_up, camera_right, camera_pitch, false);
			camera_up = Quaternion::RotatePoint(camera_up, camera_direction, camera_roll, false);
		}

		if (this->scene_data["camera"].contains("up")) {
			double x = this->scene_data["camera"]["up"]["x"].get<double>();
			double y = this->scene_data["camera"]["up"]["y"].get<double>();
			double z = this->scene_data["camera"]["up"]["z"].get<double>();

			camera_up.set(x, 1, 1);
			camera_up.set(y, 2, 1);
			camera_up.set(z, 3, 1);

			if (!rotation_given) {
				// Gets roll representing the rotation from the default camera up to the camera up
				Engine::GetRoll(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				Engine::rotateX(tmp_up, camera_pitch);
				Engine::rotateZ(tmp_up, camera_roll);
			}
		}
	}

	std::cout << "Camera position: " << std::endl;
	camera_position.print();

	std::cout << "Yaw: " << camera_yaw << std::endl;
	std::cout << "Pitch: " << camera_pitch << std::endl;
	std::cout << "Roll: " << camera_roll << std::endl;

	std::cout << "Camera direction: " << std::endl;
	camera_direction.print();

	std::cout << "Camera up: ";
	camera_up.print();
	std::cout << std::endl;

	//VIEW_MATRIX =  SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
	VIEW_MATRIX = Engine::LookAt(camera_position, camera_direction, camera_up);
	std::cout << "View matrix: " << std::endl;
	VIEW_MATRIX.print();
	printf("\n\n");

	// Loads meshes and instances into the scene
	for (auto model = this->scene_data["models"].begin(); model != this->scene_data["models"].end(); model++) {
		const char* model_filename = model.key().c_str();
		char model_path[255];
		sprintf_s(model_path, 255, "%s%s", models_folder, model_filename);
		Mesh current_model = Mesh(model_path, model_filename, this->total_meshes);

		this->scene_meshes.push_back(std::move(current_model));

		for (auto instance = this->scene_data["models"][model_filename]["instances"].begin(); instance != this->scene_data["models"][model_filename]["instances"].end(); instance++) {
			const char* instance_name = instance.key().c_str();
			//printf("%s: %s\n", model_filename, instance_name);

			Mat translation = Mat::identity_matrix(4);
			Mat scale = Mat::identity_matrix(4);
			Mat rotation = Mat::identity_matrix(4);

			Quaternion orientation = Quaternion(0, 0, 0, 1);

			double translation_x = 0;
			double translation_y = 0;
			double translation_z = 0;

			double scale_x = 1;
			double scale_y = 1;
			double scale_z = 1;

			double yaw = 0;
			double pitch = 0;
			double roll = 0;

			bool has_translation = false;
			bool has_scaling = false;
			bool has_rotation = false;

			// Checks if translation has been given for this instance, updates translation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("translation")) {
				has_translation = true;
				translation_x = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["x"];
				translation_y = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["y"];
				translation_z = this->scene_data["models"][model_filename]["instances"][instance_name]["translation"]["z"];

				translation = Mat::translation_matrix(translation_x, translation_y, translation_z);
			}

			// Checks if scale has been given for this instance, updates scale matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("scale")) {
				has_scaling = true;
				scale_x = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["x"];
				scale_y = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["y"];
				scale_z = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["z"];

				scale = Mat::scale_matrix(scale_x, scale_y, scale_z);
			}

			// Checks if rotation has been given for this instance, updates rotation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("rotation")) {
				has_rotation = true;
				yaw = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["y"] * (M_PI / 180);
				pitch = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["x"] * (M_PI / 180);
				roll = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["z"] * (M_PI / 180);

				orientation = Quaternion::FromYawPitchRoll(yaw, pitch, roll);
				rotation = orientation.get_rotationmatrix();
			}

			Mat MODEL_TO_WORLD = Mat::identity_matrix(4);
			bool has_model_to_world = false;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("model_to_world")) {
				
				const auto model_matrix_obj = this->scene_data["models"][model_filename]["instances"][instance_name]["model_to_world"];
				if (model_matrix_obj.size() == 4) {
					const auto first_row = model_matrix_obj[0];
					if (first_row.size() == 4) {
						has_model_to_world = true;

						MODEL_TO_WORLD = Mat(
							{
								{model_matrix_obj[0][0], model_matrix_obj[0][1], model_matrix_obj[0][2], model_matrix_obj[0][3]},
								{model_matrix_obj[1][0], model_matrix_obj[1][1], model_matrix_obj[1][2], model_matrix_obj[1][3]},
								{model_matrix_obj[2][0], model_matrix_obj[2][1], model_matrix_obj[2][2], model_matrix_obj[2][3]},
								{model_matrix_obj[3][0], model_matrix_obj[3][1], model_matrix_obj[3][2], model_matrix_obj[3][3]}
							}, 4, 4);
					}
					else {
						throw std::runtime_error("Model-to-world matrix of invalid size given for scene '" + std::string(scene_filename) + "', model '" + std::string(model_filename) + "', instance '" + std::string(instance_name) + "'. Must be of size 16 (for a 4x4 matrix), but parsed size is " + std::to_string(model_matrix_obj.size() * first_row.size()));
					}

				}
				else {
					throw std::runtime_error("Model-to-world matrix of invalid size given for scene '" + std::string(scene_filename) + "', model '" + std::string(model_filename) + "', instance '" + std::string(instance_name) + "'. Must be of size 16 (for a 4x4 matrix), but parsed size is " + std::to_string(model_matrix_obj.size() * model_matrix_obj[0].size()));
				}
			}
			else {
				MODEL_TO_WORLD = translation * scale * rotation;
			}

			bool show = true;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("show")) {
				show = this->scene_data["models"][model_filename]["instances"][instance_name]["show"].get<bool>();
			}

			Instance mesh_instance;

			// Loads from model-to-world if it's the only available information
			if (!has_translation && !has_scaling && !has_rotation && has_model_to_world) {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], MODEL_TO_WORLD, show, this->total_instances);
			}

			// Loads from parameters
			else {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], translation_x, translation_y, translation_z, yaw, pitch, roll, scale_x, scale_y, scale_z, show, this->total_instances);
			}

			this->scene_instances.push_back(std::move(mesh_instance));

			//std::cout << "Total instances: " << this->total_instances << " | Total meshes: " << this->total_meshes << std::endl;
			//std::cout << "-----------------------" << std::endl;


		}

	}

	

	for (int i = 0; i < this->total_instances; i++) {
		Instance* instance = &this->scene_instances[i];

		//printf("(%s: %d) - (%s : %d) | Show? %d\n", instance->mesh->mesh_filename.c_str(), instance->mesh->mesh_id, instance->instance_name.c_str(), instance->instance_id, instance->show);
	}
}

Mesh Scene::get_mesh(uint32_t mesh_id) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			return *current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

Mesh Scene::get_mesh(std::string mesh_filename) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			return *current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

Mesh* Scene::get_mesh_ptr(uint32_t mesh_id) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_id == mesh_id) {
			return current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by id for the given mesh id of: " + std::to_string(mesh_id) + ".");
}

Mesh* Scene::get_mesh_ptr(std::string mesh_filename) {
	for (int i = 0; i < this->scene_meshes.size(); i++) {
		Mesh* current_mesh = &this->scene_meshes[i];

		if (current_mesh->mesh_filename == mesh_filename) {
			return current_mesh;
		}
	}

	throw std::runtime_error("Error: Could not find mesh by filename for the given mesh filename of: " + mesh_filename + ".");
}

Instance Scene::get_instance(uint32_t instance_id) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			return *current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

Instance Scene::get_instance(std::string instance_name) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			return *current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}

Instance* Scene::get_instance_ptr(uint32_t instance_id) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_id == instance_id) {
			return current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by id for the given instance id of: " + std::to_string(instance_id) + ".");
}

Instance* Scene::get_instance_ptr(std::string instance_name) {
	for (int i = 0; i < this->scene_instances.size(); i++) {
		Instance* current_instance = &this->scene_instances[i];

		if (current_instance->instance_name == instance_name) {
			return current_instance;
		}
	}

	throw std::runtime_error("Error: Could not find instance by name for the given instance name of: " + instance_name + ".");
}

void Engine::draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	draw_mesh(*instance.mesh, instance.MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
}

void Engine::draw_mesh(const Mesh& mesh, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {	
	for (int face = 0; face < mesh.total_faces(); face++) {
		uint32_t idx_a = mesh.faces_indices[face][0] - 1;
		uint32_t idx_b = mesh.faces_indices[face][1] - 1;
		uint32_t idx_c = mesh.faces_indices[face][2] - 1;

		uint32_t total_vertices_count = mesh.total_vertices() - 1;

		// Skip if not a valid vertex
		if ((idx_a > total_vertices_count || idx_b > total_vertices_count || idx_c > total_vertices_count)
			||
			(idx_a < 0 || idx_b < 0 || idx_c < 0)
			) {
			continue;
		}

		Mat vertices[4] = {mesh.vertices[idx_a], mesh.vertices[idx_b], mesh.vertices[idx_c], mesh.vertices[idx_c]};

		// Include 4th vertex if face is given as a quad
		if (mesh.faces_indices[face].size() == 4) {
			uint32_t idx_d = mesh.faces_indices[face][3] - 1;

			if (idx_d > total_vertices_count || idx_d < 0) {
				continue;
			}

			vertices[3] = mesh.vertices[idx_d];
		}

		// Draw quad if face is rectangular
		if (mesh.faces_indices[face].size() == 4) {
			draw_quad(vertices[0], vertices[1], vertices[2], vertices[3], MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
		}

		// Draw triangle if face is triangular
		else if (mesh.faces_indices[face].size() == 3) {
			draw_triangle(vertices[0], vertices[1], vertices[2], MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
		}

	}
}


void Engine::draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	draw_triangle(v0, v1, v2, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
	draw_triangle(v0, v2, v3, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
}

void Engine::draw_triangle(Mat v0, Mat v1, Mat v2, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	Mat world_v0 = MODEL_TO_WORLD * v0;
	Mat world_v1 = MODEL_TO_WORLD * v1;
	Mat world_v2 = MODEL_TO_WORLD * v2;


	v0 =  this->VIEW_MATRIX * MODEL_TO_WORLD * v0;
	v1 =  this->VIEW_MATRIX * MODEL_TO_WORLD * v1;
	v2 =  this->VIEW_MATRIX * MODEL_TO_WORLD * v2;

	// MODEL SPACE -> WORLD SPACE ->  NOW IN CAMERA SPACE

	// Cull triangle if all vertices are further than the far plane
	if (abs(v0.get(3, 1)) > this->far || abs(v1.get(3, 1)) > this->far || abs(v2.get(3, 1)) > this->far) {
		return;
	}

	// Should include the z flip in the view or projection matrix instead of when loading meshes?

	//v0.set(-v0.get(3, 1), 3, 1);
	//v1.set(-v1.get(3, 1), 3, 1);
	//v2.set(-v2.get(3, 1), 3, 1);

	Triangle current_triangle = Triangle(v0, v1, v2);
	Triangle clipped_triangles[2];

	const Mat near_plane_point = Mat(
		{ 
			{0},
			{0},
			{this->near},
			{0}
		}
	, 4, 1);

	const Mat near_plane_normal = Mat(
		{
			{0},
			{0},
			{1},
			{0}
		}
		, 4, 1
	);

	uint8_t n_clipped_triangles = Engine::ClipTriangleToPlane(&near_plane_point, &near_plane_normal, &current_triangle, clipped_triangles[0], clipped_triangles[1]);

	if (n_clipped_triangles == 0) return;
	
	double new_near = near;

	double xright = new_near * tan(FOVr / 2);
	double xleft = -xright;
	double ytop = xright / AR;
	double ybottom = -ytop;

	Mat left_plane_normal = Mat(
		{
			{near / xright},
			{0},
			{1},
			{0}
		}
	, 4, 1);

	left_plane_normal = left_plane_normal / left_plane_normal.norm();


	Mat right_plane_normal = Mat(
		{
			{-near / xright},
			{0},
			{1},
			{0}
		}
	, 4, 1);

	right_plane_normal = right_plane_normal / right_plane_normal.norm();

	Mat top_plane_normal = Mat(
		{
			{0},
			{-near/ ytop},
			{1},
			{0}
		}
	, 4, 1);

	top_plane_normal = top_plane_normal / top_plane_normal.norm();

	Mat bottom_plane_normal = Mat(
		{
			{0},
			{near / ytop},
			{1},
			{0}
		}
	, 4, 1);

	bottom_plane_normal = bottom_plane_normal / bottom_plane_normal.norm();

	const Mat left_plane_point = Mat(
		{
			{xleft},
			{0},
			{this->near},
			{0}
		}
	, 4, 1);

	const Mat right_plane_point = Mat(
		{
			{xright},
			{0},
			{this->near},
			{0}
		}
	, 4, 1);

	const Mat top_plane_point = Mat(
		{
			{0},
			{ytop},
			{near},
			{0}
		}
	, 4, 1);

	const Mat bottom_plane_point = Mat(
		{
			{0},
			{ybottom},
			{near},
			{0}
		}
	, 4, 1);

	// Triangles get clipped, processed, then rendered
	if (draw_outline || fill || shade) {
		for (uint8_t n_clipped_triangle = 0; n_clipped_triangle < n_clipped_triangles; n_clipped_triangle++) {
			Triangle starting_clipped_triangle = clipped_triangles[n_clipped_triangle];
			Mat v0 = starting_clipped_triangle.vertices[0];
			Mat v1 = starting_clipped_triangle.vertices[1];
			Mat v2 = starting_clipped_triangle.vertices[2];

			starting_clipped_triangle.vertices[0] = v0;
			starting_clipped_triangle.vertices[1] = v1;
			starting_clipped_triangle.vertices[2] = v2;

			std::deque<Triangle> sub_clipped_triangles;
			sub_clipped_triangles.push_back(starting_clipped_triangle);
			static Triangle current_sub_clipped_triangles[2];

			for (uint8_t plane = 0; plane < 4; plane++) {
				size_t n_current_plane_triangles = sub_clipped_triangles.size();
				for (int i = 0; i < n_current_plane_triangles; i++) {
					Triangle current_sub_clipped_triangle = sub_clipped_triangles.front();
					sub_clipped_triangles.pop_front();

					uint8_t n_clipped_sub_triangles = 0;

					switch (plane) {
					case 0:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&bottom_plane_point, &bottom_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 1:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&left_plane_point, &left_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 2:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&top_plane_point, &top_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					case 3:
						n_clipped_sub_triangles = Engine::ClipTriangleToPlane(&right_plane_point, &right_plane_normal, &current_sub_clipped_triangle, current_sub_clipped_triangles[0], current_sub_clipped_triangles[1]);
						break;
					}

					for (n_clipped_sub_triangles; n_clipped_sub_triangles > 0; n_clipped_sub_triangles--) {
						sub_clipped_triangles.push_back(current_sub_clipped_triangles[n_clipped_sub_triangles - 1]);
					}
				}
			}

			for (Triangle current_triangle : sub_clipped_triangles) {
				Mat v0 = current_triangle.vertices[0];
				Mat v1 = current_triangle.vertices[1];
				Mat v2 = current_triangle.vertices[2];

				bool visible = true;

				// This projection matrix's `z` coordinate gets multiplied by the `far` plane value
				// So, the range after the projection of the `z` coordinate is (0, far), not (0, 1)
				// This is so that after the perspective divide, which happens in the sequence, it becomes (0, 1), already including it

				v0 = this->PROJECTION_MATRIX * v0;
				v1 = this->PROJECTION_MATRIX * v1;
				v2 = this->PROJECTION_MATRIX * v2;

				// NOW IN CLIP SPACE FOR SOME GRAPHICS LIBRARIES

				double v0_originalz = v0.get(4, 1);
				double v1_originalz = v1.get(4, 1);
				double v2_originalz = v2.get(4, 1);

				// Cull triangle if all vertices are further than the far plane
				//if (v0_originalz > this->far && v1_originalz > this->far && v2_originalz > this->far) continue;



				// Perspective divide
				v0 /= v0_originalz;
				v1 /= v1_originalz;
				v2 /= v2_originalz;


				bool cull_triangle = false;

				if (this->backface_cull) {
					Mat vec_a = v0 - v1;
					vec_a.normalize();
					Mat vec_b = v2 - v1;
					vec_b.normalize();
					Mat triangle_normal = Engine::CrossProduct3D(vec_a, vec_b);
					triangle_normal.normalize();

					//Mat middle_point = v1 + (0.5 * (v0 - v1)) + (0.5 * (v2 - v1));

					//Mat eye_to_triangle_dist = middle_point - camera_position;
					Mat eye_to_triangle_dist = v1;
					eye_to_triangle_dist.normalize();

					double correlation = Mat::dot(triangle_normal, eye_to_triangle_dist);
					if (correlation >= 0) {
						//eye_to_triangle_dist.print();
						cull_triangle = true;
						continue;
					}
				}


				// Triangle lighting/shading happens here
				if (shade) {
					Mat vec_a = v0 - v1;
					vec_a.normalize();
					Mat vec_b = v2 - v1;
					vec_b.normalize();
					Mat triangle_normal = Engine::CrossProduct3D(vec_a, vec_b);

					Mat a_vec_a = world_v0 - world_v1;
					a_vec_a.normalize();
					Mat a_vec_b = world_v2 - world_v1;
					a_vec_b.normalize();
					Mat world_normal = Engine::CrossProduct3D(a_vec_a, a_vec_b);
					world_normal.normalize();

					triangle_normal.normalize();
					//Mat normalized_light_source = light_source_dir / light_source_dir.norm();
					Mat normalized_light_source = light_source_pos - world_v0;
					normalized_light_source = normalized_light_source / normalized_light_source.norm();

					//Mat dist_vertex_to_light = v0 - light_source_pos;
					//double distance = abs(Engine::PointDistanceToPlane(&v0, &light_source_pos, &normalized_light_source));
					//double distance = sqrt(pow(world_v0.get(1, 1) - light_source_pos.get(1, 1), 2) + pow(world_v0.get(2, 1) - light_source_pos.get(2, 1), 2) + pow(world_v0.get(3, 1) - light_source_pos.get(3, 1), 2));
					double distance = sqrt(pow(world_v0.get(1, 1) - light_source_pos.get(1, 1), 2) + pow(world_v0.get(2, 1) - light_source_pos.get(2, 1), 2) + pow(world_v0.get(3, 1) - light_source_pos.get(3, 1), 2));
					double world_similarity = Mat::dot(normalized_light_source, world_normal);
					//double view_similarity = Mat::dot(normalized_light_source, triangle_normal);
					double similarity = world_similarity;

					//std::cout << "Similarity: " << similarity << " | Distance: " << distance << std::endl;

					//std::cout << light_intensity << std::endl;
					if (similarity > 0) {
						//double light_intensity = (1 / pow(this->light_reach, 2)) * pow(distance, 2);

						//double light_intensity = abs((distance - light_reach)) * -similarity;
						//double light_intensity = -similarity;
						//double light_intensity = -similarity;
						double light_intensity = similarity;
						double base_intensity = this->light_intensity;
						double attenuation = 1.f / (distance * distance);
						//double distance_multiplier = light_intensity * attenuation * base_intensity;
						//distance_multiplier = Utils::clamp(distance_multiplier, 0, 1);

						//light_intensity = light_intensity * distance_multiplier;
						light_intensity = light_intensity * base_intensity * attenuation;
						light_intensity = Utils::clamp(light_intensity, 0, 1);

						uint8_t red = (uint8_t)(light_intensity * ((this->light_color >> 24) & 0x000000FF));
						uint8_t green = (uint8_t)(light_intensity * ((this->light_color >> 16) & 0x000000FF));
						uint8_t blue = (uint8_t)(light_intensity * ((this->light_color >> 8) & 0x000000FF));

						//uint8_t color = (uint8_t)(light_intensity * 255.0);

						//color /= pow(distance, 1/3);
						//fill_color = 0x000000FF | (color << 24) | (color << 16) | (color << 8);
						fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);

						if (light_intensity < 0.001) {
							visible = false;
							fill_color = BG_COLOR;

							light_intensity = this->light_intensity;
							double light_intensity = similarity;
							double base_intensity = this->light_intensity;
							double attenuation = 1.f / (distance * distance);

							light_intensity = light_intensity * base_intensity * attenuation;
							light_intensity = Utils::clamp(light_intensity, 0, 1);

							light_intensity = this->light_intensity;

							uint8_t red = (uint8_t)(light_intensity * ((this->light_color >> 24) & 0x000000FF));
							uint8_t green = (uint8_t)(light_intensity * ((this->light_color >> 16) & 0x000000FF));
							uint8_t blue = (uint8_t)(light_intensity * ((this->light_color >> 8) & 0x000000FF));
							//fill_color = 0x000000FF | (color << 24) | (color << 16) | (color << 8);
							fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
						}
					}
					else {
						visible = false;
						fill_color = BG_COLOR;
						//uint8_t color = (uint8_t)(this->light_intensity * 0xFF);

						light_intensity = this->light_intensity;
						double light_intensity = similarity;
						double base_intensity = this->light_intensity;
						double attenuation = 1.f / (distance * distance);

						light_intensity = light_intensity * base_intensity * attenuation;
						light_intensity = Utils::clamp(light_intensity, 0, 1);

						uint8_t red = (uint8_t)(light_intensity * ((this->light_color >> 24) & 0x000000FF));
						uint8_t green = (uint8_t)(light_intensity * ((this->light_color >> 16) & 0x000000FF));
						uint8_t blue = (uint8_t)(light_intensity * ((this->light_color >> 8) & 0x000000FF));
						//fill_color = 0x000000FF | (color << 24) | (color << 16) | (color << 8);
						fill_color = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);
					}
				}


				// FLIP X AND Y
				v0.set(-v0.get(1, 1), 1, 1);
				v0.set(-v0.get(2, 1), 2, 1);

				v1.set(-v1.get(1, 1), 1, 1);
				v1.set(-v1.get(2, 1), 2, 1);

				v2.set(-v2.get(1, 1), 1, 1);
				v2.set(-v2.get(2, 1), 2, 1);

				// NOW IN NDC SPACE
				
				
				
				
				// Transformation into screen space coordinates (i.e. 2D window x-y)
				// Positive x is to the right
				// Positive y is down (i.e. top is 0 and bottom is window height - 1)
				v0 = this->SCALE_MATRIX * v0;
				v1 = this->SCALE_MATRIX * v1;
				v2 = this->SCALE_MATRIX * v2;

				// Gives triangles random colors each frame
				/*
				uint8_t alpha = 0xFF;
				uint8_t red = rand() % 0xFF;
				uint8_t blue = rand() % 0xFF;
				uint8_t green = rand() % 0xFF;
				uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;


				outline_color = color;
				*/


				//fill_color = color;

				bool is_v0_equal_v1 = v0 == v1;
				bool is_v0_equal_v2 = v0 == v2;
				bool is_v1_equal_v2 = v1 == v2;

				//draw_outline = true;
				//fill = true;

				// Clamps pixels to window boundaries if exceeding
				if (v0.get(1, 1) < 0) v0.set(0, 1, 1);
				if (v0.get(2, 1) < 0) v0.set(0, 2, 1);

				if (v1.get(1, 1) < 0) v1.set(0, 1, 1);
				if (v1.get(2, 1) < 0) v1.set(0, 2, 1);

				if (v2.get(1, 1) < 0) v2.set(0, 1, 1);
				if (v2.get(2, 1) < 0) v2.set(0, 2, 1);

				if (v0.get(1, 1) >= WIDTH - 1) v0.set(WIDTH - 1, 1, 1);
				if (v0.get(2, 1) >= HEIGHT - 1) v0.set(HEIGHT - 1, 2, 1);

				if (v1.get(1, 1) >= WIDTH - 1) v1.set(WIDTH - 1, 1, 1);
				if (v1.get(2, 1) >= HEIGHT - 1) v1.set(HEIGHT - 1, 2, 1);

				if (v2.get(1, 1) >= WIDTH - 1) v2.set(WIDTH - 1, 1, 1);
				if (v2.get(2, 1) >= HEIGHT - 1) v2.set(HEIGHT - 1, 2, 1);

				if ((fill || shade)) {
					// Ignore triangle rasterization if 2 of the vertices are the same (meaning the triangle is basically a line, which is already handled when the lines are drawn)
					if (is_v0_equal_v1 || is_v0_equal_v2 || is_v1_equal_v2) {
						return;
					}

					fill_triangle(v0, v1, v2, v0_originalz, v1_originalz, v2_originalz, fill_color, shade);
				}

				if (draw_outline) {
					// Checks whether 2 of the triangle's vertices are the same, and only draws line if that is not the case (avoids drawing the same line more than once)
					if (!is_v0_equal_v1) {
						draw_line(
							v0.get(1, 1), v0.get(2, 1),
							v1.get(1, 1), v1.get(2, 1),
							v0, v1,
							v0_originalz, v1_originalz,
							outline_color
						);
					}
					
					if (!is_v1_equal_v2 && v2 != v0) {
						draw_line(
							v1.get(1, 1), v1.get(2, 1),
							v2.get(1, 1), v2.get(2, 1),
							v1, v2,
							v1_originalz, v2_originalz,
							outline_color
						);
					}

					if (!is_v0_equal_v2 && v0 != v1 && v2 != v1) {
						draw_line(
							v2.get(1, 1), v2.get(2, 1),
							v0.get(1, 1), v0.get(2, 1),
							v2, v0,
							v2_originalz, v0_originalz,
							outline_color
						);
					}
				}


			}
		}
	}
}

void Engine::draw_line(double x1, double y1, double x2, double y2, const Mat& vec_a, const Mat& vec_b, const double& vec_a_originalz, const double& vec_b_originalz, uint32_t outline_color) {
	// If not a vertical line
	// (if x's are too close it's considered a vertical line, this accounts for small floating point errors)

	// Ignore lines if they are outside of the window

	/*
	if ((x1 < 0 && x2 < 0) || (x1 >= WIDTH && x2 >= WIDTH)) {
		return;
	}
	if ((y1 < 0 && y2 < 0) || (y1 >= HEIGHT && y2 >= HEIGHT)) {
		return;
	}
	*/


	Mat start_vector = vec_a;
	Mat end_vector = vec_b;
	double start_vector_original_z = vec_a_originalz;
	double end_vector_original_z = vec_b_originalz;

	if (abs(x1 - x2) > 0.5) {
		double original_x = fmin(x1, x2);
		double y = (original_x == x1) ? y1 : y2;

		bool starts_at_vec_a = true;
		if (original_x == x2) {
			starts_at_vec_a = false;
			std::swap(start_vector, end_vector);
			std::swap(start_vector_original_z, end_vector_original_z);
		}

		//double dy = (y2 - y1) / (x2 - x1);
		double dy = (end_vector.get(2, 1) - start_vector.get(2, 1)) / (end_vector.get(1, 1) - start_vector.get(1, 1));
		double abs_dx = abs(1 / dy);

		// Clip edges of the line if they're out of the screen to optimize (check intersections with screen) x and y must be within screen boundaries
		if (abs(dy) <= abs_dx) {
			uint16_t x = round(fmax(0, original_x));
			if (x != original_x) {
				y += (x - original_x) * dy;
			}

			uint16_t rounded_y = round(y);
			//start_vector.set(x, 1, 1);
			//start_vector.set(rounded_y, 2, 1);

			double start_vector_x = start_vector.get(1, 1);
			double start_vector_y = start_vector.get(2, 1);
			double end_vector_x = end_vector.get(1, 1);
			double end_vector_y = end_vector.get(2, 1);

			Mat direction_vector = end_vector - start_vector;
			double direction_x = direction_vector.get(1, 1);
			double direction_y = direction_vector.get(2, 1);

			double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

			for (x;
				x < (uint16_t) round(fmax(x1, x2)); x++) {

				// Check and update depth buffer, or only do so when rasterizing?

				if (!this->depth_test) {
					this->pixel_buffer[(WIDTH * rounded_y) + x] = outline_color;
				}
				else {
					double walked_length = sqrt(pow(x - start_vector_x, 2) + pow(rounded_y - start_vector_y, 2));
					double alpha = walked_length / total_length;

					if (total_length == 0) {
						//return;
						throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
					}

					double a = alpha;
					double b = 1 - alpha;

					
					//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
					double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);
					if (this->shade || this->rasterize) interpolated_z *= z_fighting_tolerance;
					//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));

					if (interpolated_z <= this->depth_buffer[(this->WIDTH * rounded_y) + x]) {
						this->pixel_buffer[(this->WIDTH * rounded_y) + x] = outline_color;
					}
				}

				y += dy;
				rounded_y = round(y);
			}
		}
		else {
			double x = fmax(0, original_x);
			double original_y = y;
			double target_y = original_y == y1 ? y2 : y1;

			if (x != original_x) {
				original_y += (x - original_x) * dy;
			}

			uint16_t rounded_x = round(x);
			int16_t y = round(original_y);
			//start_vector.set(rounded_x, 1, 1);
			//start_vector.set(y, 2, 1);

			double start_vector_x = start_vector.get(1, 1);
			double start_vector_y = start_vector.get(2, 1);
			double end_vector_x = end_vector.get(1, 1);
			double end_vector_y = end_vector.get(2, 1);

			Mat direction_vector = end_vector - start_vector;
			double direction_x = direction_vector.get(1, 1);
			double direction_y = direction_vector.get(2, 1);

			double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

			for (y;
				y != round(target_y); y += dy > 0 ? 1 : -1) {

				// Check and update depth buffer, or only do so when rasterizing?

				//this->pixel_buffer[(WIDTH * (int16_t)y) + rounded_x] = outline_color;

				if (!this->depth_test) {
					this->pixel_buffer[(WIDTH * y) + rounded_x] = outline_color;
				}
				else {
					double walked_length = sqrt(pow(rounded_x - start_vector_x, 2) + pow(y - start_vector_y, 2));
					double alpha = walked_length / total_length;

					if (total_length == 0) {
						//return;
						throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
					}

					double a = alpha;
					double b = 1 - alpha; // always on v1

					//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
					double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);
					if (this->shade || this->rasterize) interpolated_z *= z_fighting_tolerance;
					//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));
					//double interpolated_z = ((start_vector_original_z) + (alpha * ((end_vector_original_z) - (start_vector_original_z))));

					if (interpolated_z <= this->depth_buffer[(this->WIDTH * y) + rounded_x]) {
						this->pixel_buffer[(this->WIDTH * y) + rounded_x] = outline_color;
					}
				}

				x += abs_dx;
				rounded_x = round(x);

			}
		}


	}

	// If a vertical line
	else {
		uint16_t x = round(x1);
		uint16_t y = round(fmax(0, fmin(y1, y2)));

		if (y2 < y1) {
			std::swap(start_vector, end_vector);
			std::swap(start_vector_original_z, end_vector_original_z);
		}

		double start_vector_x = x;
		double start_vector_y = start_vector.get(2, 1);
		double end_vector_x = x;
		double end_vector_y = end_vector.get(2, 1);

		Mat direction_vector = end_vector - start_vector;
		double direction_x = x;
		double direction_y = direction_vector.get(2, 1);

		double total_length = sqrt(pow(direction_x, 2) + pow(direction_y, 2));

		for (y; y < (uint16_t)round(fmax(y1, y2)); y++) {
			// Check and update depth buffer, or only do so when rasterizing?

			if (!this->depth_test) {
				this->pixel_buffer[(WIDTH * y) + x] = outline_color;
			}
			else {
				double walked_length = sqrt(pow(x - start_vector_x, 2) + pow(y - start_vector_y, 2));
				double alpha = walked_length / total_length;

				if (total_length == 0) {
					//return;
					throw std::runtime_error("Error: A direction vector of length 0 caused a division by 0 during line depth testing.");
				}

				double a = alpha;
				double b = 1 - alpha; // always on v1

				//double interpolated_z = 1 / (((1 / start_vector_original_z) * b) + ((1 / end_vector_original_z) * a));
				double interpolated_z = (start_vector_original_z * b) + (end_vector_original_z * a);

				if (this->shade || this->rasterize) interpolated_z *= z_fighting_tolerance;
				//double interpolated_z = 1 / (((1 / start_vector.get(3, 1)) * b) + ((1 / end_vector.get(3, 1) * a)));

				if (interpolated_z <= this->depth_buffer[(this->WIDTH * y) + x]) {
					this->pixel_buffer[(this->WIDTH * y) + x] = outline_color;
				}
			}
		}

	}

}


// 2D Cross product
double Engine::CrossProduct2D(const Mat& v1, const Mat& v2) {
	return (v1.get(1, 1) * v2.get(2, 1)) - (v2.get(1, 1) * v1.get(2, 1));
}

// 3D Cross product
Mat Engine::CrossProduct3D(const Mat& v1, const Mat& v2) {
	double v1_x = v1.get(1, 1);
	double v1_y = v1.get(2, 1);
	double v1_z = v1.get(3, 1);

	double v2_x = v2.get(1, 1);
	double v2_y = v2.get(2, 1);
	double v2_z = v2.get(3, 1);

	Mat result = Mat(
		{ 
			{(v1_y * v2_z) - (v1_z * v2_y)},
			{(v1_z * v2_x) - (v1_x * v2_z)},
			{(v1_x * v2_y) - (v1_y * v2_x)},
			{0}
		}
	, 4, 1);

	// If the given vectors point in opposite directions (resulting in an empty zero vector)
	if (result.get(1, 1) == 0 && result.get(2, 1) == 0 && result.get(3, 1) == 0) {

	}

	return result;
}

void Engine::fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const double& v0_originalz, const double& v1_originalz, const double& v2_originalz, uint32_t fill_color, bool shade) {

	double v0_x = v0.get(1, 1);
	double v0_y = v0.get(2, 1);

	double v1_x = v1.get(1, 1);
	double v1_y = v1.get(2, 1);

	double v2_x = v2.get(1, 1);
	double v2_y = v2.get(2, 1);

	uint16_t bounding_box_left_x = round(fmin(fmin(v0_x, v1_x), v2_x));
	uint16_t bounding_box_right_x = round(fmax(fmax(v0_x, v1_x), v2_x));
	uint16_t bounding_box_top_y = round(fmin(fmin(v0_y, v1_y), v2_y));
	uint16_t bounding_box_bottom_y = round(fmax(fmax(v0_y, v1_y), v2_y));

	const Mat vec_a = v0 - v1;
	const Mat vec_b = v2 - v1;

	double vec_a_x = vec_a.get(1, 1);
	double vec_a_y = vec_a.get(2, 1);

	double vec_b_x = vec_b.get(1, 1);
	double vec_b_y = vec_b.get(2, 1);

	for (uint16_t pixel_y = bounding_box_top_y; pixel_y <= bounding_box_bottom_y; pixel_y++) {
		if (pixel_y >= this->HEIGHT || pixel_y < 0) return;
		for (uint16_t pixel_x = bounding_box_left_x; pixel_x <= bounding_box_right_x; pixel_x++) {
			if (pixel_x >= this->WIDTH || pixel_x < 0) return;
			// C = V1 (Red) 
			// A = V0 (Green)
			// B = V2 (Blue)
			// Picturing a flat bottom triangle, the bottom left vertex would V1 (red), top middle vertex would be V0 (green), and bottom right vertex would be V2 (blue)
			//                 (V0, green)
			//                    /\
			//                   /  \
 			//                  /    \
			// 			       /      \
			//                /        \
			//               /          \
			//   (V1, red)  /____________\  (V2, blue)
			// 
			// 
			//           * V0 and V2 are swapped (and consequently their colors) if V1 and V2 have the same y (as in this triangle), becoming:
			// 
			// 			       (V2, blue)
			//                    /\
			//                   /  \
 			//                  /    \
			// 			       /      \
			//                /        \
			//               /          \
			//   (V1, red)  /____________\  (V0, green)          
			// 

			double division = (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));

			if (division == 0) {
				return;
				throw std::runtime_error("Error: Division by 0 during barycentric coordinates calculation (rasterization).");
			}

			double alpha = (((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / division;
			double beta = (((v0_x - v1_x) * (pixel_y - v1_y)) - ((v0_y - v1_y) * (pixel_x - v1_x))) / division;

			// Checks whether is within triangle/edge
			//if ((pixel_t0 >= 0 && pixel_t1 >= 0) || (pixel_t0 <= 1 && pixel_t1 <= 1)) {

			if (alpha >= 0 && beta >= 0 && alpha + beta <= 1) {
				double c = 1 - alpha - beta; // always on v1
				double b = beta; // always on v2, unless v2 is at the same height as v1, then v0 and v2 swap and this is now at v0
				double a = alpha; // always on v0, unless same case as beta above, in this case it stays at v2

				bool left_edge = true;
				bool top_edge = true;

				// Skips pixel if sits at bottom/right edge
				// Ignore t0 == 0 && t1 == 0 as well as

				/*
				if ((alpha == 0 || beta == 0) && !left_edge && !top_edge) {
					continue;
				}
				*/

				// Interpolates each vertex for red, green, and blue (if not shading)
				if (!shade) {
					uint8_t alpha = 0xFF;
					uint8_t red = c * 0xFF; // v1
					uint8_t green = a * 0xFF; // v0 (when v2 at same height as v1 swaps with v2)
					uint8_t blue = b * 0xFF; // v2 (when at same height as v1 swaps with v0)
					fill_color = (red << 24) | (green << 16) | (blue << 8) | alpha;
				}


				if (pixel_y < this->HEIGHT && pixel_y >= 0 && pixel_x >= 0 && pixel_x < this->WIDTH) {
					if (this->depth_test) {
						//double interpolated_z = 1 / (((1 / v0_originalz) * a) + ((1 / v1_originalz) * c) + ((1 / v2_originalz) * b));
						double interpolated_z = (v0_originalz * a) + (v1_originalz * c) + (v2_originalz * b);


						if (interpolated_z < this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x]) {
							this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x] = interpolated_z;
							this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
						}
						else if (interpolated_z == this->depth_buffer[(this->WIDTH * pixel_y) + pixel_x]) {
							this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
						}
					}
					else {
						this->pixel_buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
					}
				}
			}
			else {
				//std::cout << "Alpha: " << alpha << std::endl;
				//std::cout << "Beta: " << beta << std::endl;
			}
		}
	}

	return;
}

// Returns the vertex that represents the center point of the instance in world space
Mat Engine::Instance_GetCenterVertex(const Instance& instance) {
	Mat center = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
	
	double min_x = std::numeric_limits<double>::max();
	double max_x = std::numeric_limits<double>::lowest();
	double min_y = std::numeric_limits<double>::max();
	double max_y = std::numeric_limits<double>::lowest();
	double min_z = std::numeric_limits<double>::max();
	double max_z = std::numeric_limits<double>::lowest();

	for (const Mat& vertex : instance.mesh->vertices) {
		Mat current_instance_vertex = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX * vertex;
		double x = current_instance_vertex.get(1, 1);
		double y = current_instance_vertex.get(2, 1);
		double z = current_instance_vertex.get(3, 1);

		min_x = std::min(x, min_x);
		max_x = std::max(x, max_x);
		min_y = std::min(y, min_y);
		max_y = std::max(y, max_y);
		min_z = std::min(z, min_z);
		max_z = std::max(z, max_z);
	}

	double x = (min_x + max_x) / 2;
	double y = (min_y + max_y) / 2;
	double z = (min_z + max_z) / 2;

	center.set(x, 1, 1);
	center.set(y, 2, 1);
	center.set(z, 3, 1);

	return center;
}

void Engine::draw() {
	// Clears pixel buffer to the background/clear color
	for (int i = 0; i < this->WIDTH * this->HEIGHT; i++) {
		this->pixel_buffer[i] = this->BG_COLOR;
		this->depth_buffer[i] = std::numeric_limits<double>::max();
	}

	for (const Instance& instance : this->current_scene.scene_instances) {
		if (instance.show) draw_instance(instance, this->wireframe_render, this->LINE_COLOR, this->rasterize, this->FILL_COLOR, this->shade);
	}
}

void Engine::render() {
	SDL_SetRenderDrawColor(this->renderer, this->CLEAR_COLOR >> 24, this->CLEAR_COLOR >> 16, this->CLEAR_COLOR >> 8, this->CLEAR_COLOR);
	SDL_RenderClear(this->renderer);
	SDL_UpdateTexture(this->texture, nullptr, this->pixel_buffer, this->WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	SDL_RenderPresent(this->renderer);
}

void Engine::rotateX(Instance& instance, double radians) {
	Mat rotation = Engine::quaternion_rotationX_matrix(radians);

	instance.ROTATION_MATRIX = rotation * instance.ROTATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::rotateX(Mesh& mesh, double radians) {
	Mat rotation = Engine::quaternion_rotationX_matrix(radians);

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateX(Quad& quad, double radians) {
	rotateX(quad.triangles[0], radians);
	rotateX(quad.triangles[1], radians);
}

void Engine::rotateX(Triangle& triangle, double radians) {
	Mat rotation = Engine::quaternion_rotationX_matrix(radians);

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateX(Mat& matrix, double radians) {
	Mat rotation = Engine::quaternion_rotationX_matrix(radians);

	matrix = rotation * matrix;
}

void Engine::rotateY(Instance& instance, double radians) {
	Mat rotation = Engine::quaternion_rotationY_matrix(radians);

	instance.ROTATION_MATRIX = rotation * instance.ROTATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::rotateY(Mesh& mesh, double radians) {
	Mat rotation = Engine::quaternion_rotationY_matrix(radians);

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateY(Quad& quad, double radians) {
	rotateY(quad.triangles[0], radians);
	rotateY(quad.triangles[1], radians);
}

void Engine::rotateY(Triangle& triangle, double radians) {
	Mat rotation = Engine::quaternion_rotationY_matrix(radians);

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateY(Mat& matrix, double radians) {
	Mat rotation = Engine::quaternion_rotationY_matrix(radians);

	matrix = rotation * matrix;
}

void Engine::rotateZ(Instance& instance, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	instance.ROTATION_MATRIX = rotation * instance.ROTATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::rotateZ(Mesh& mesh, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	for (Mat& vertex : mesh.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateZ(Quad& quad, double radians) {
	rotateZ(quad.triangles[0], radians);
	rotateZ(quad.triangles[1], radians);
}

void Engine::rotateZ(Triangle& triangle, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	for (Mat& vertex : triangle.vertices) {
		vertex = rotation * vertex;
	}
}

void Engine::rotateZ(Mat& matrix, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	matrix = rotation * matrix;
}

void Engine::translate(Instance& instance, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	instance.TRANSLATION_MATRIX = translation_matrix * instance.TRANSLATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::translate(Mesh& mesh, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	for (Mat& vertex : mesh.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Quad& quad, double tx, double ty, double tz) {
	Engine::translate(quad.triangles[0], tx, ty, tz);
	Engine::translate(quad.triangles[1], tx, ty, tz);
}

void Engine::translate(Triangle& triangle, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	for (Mat& vertex : triangle.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Mat& matrix, double tx, double ty, double tz) {
	Mat translation_matrix = Mat::translation_matrix(tx, ty, tz);

	matrix = translation_matrix * matrix;
}

void Engine::scale(Instance& instance, double sx, double sy, double sz) {
	Mat scaling_matrix = Mat::scale_matrix(sx, sy, sz);

	instance.SCALING_MATRIX = scaling_matrix * instance.SCALING_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
}

void Engine::scale(Mesh& mesh, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	for (Mat& vertex : mesh.vertices) {
		vertex = scale_matrix * vertex;
	}
}

void Engine::scale(Quad& quad, double sx, double sy, double sz) {
	Engine::scale(quad.triangles[0], sx, sy, sz);
	Engine::scale(quad.triangles[1], sx, sy, sz);
}

void Engine::scale(Triangle& triangle, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	for (Mat& vertex : triangle.vertices) {
		vertex = scale_matrix * vertex;
	}

}

void Engine::scale(Mat& matrix, double sx, double sy, double sz) {
	Mat scale_matrix = Mat::scale_matrix(sx, sy, sz);

	matrix = scale_matrix * matrix;
}

Mat Engine::euler_rotationX_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{1, 0, 0, 0},
			{0, cos(radians), -sin(radians), 0},
			{0, sin(radians), cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

Mat Engine::quaternion_rotationX_matrix(double radians) {
	Quaternion quaternion = Quaternion::AngleAxis(1, 0, 0, radians);
	return quaternion.get_rotationmatrix();
}

Mat Engine::euler_rotationY_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{cos(radians), 0, sin(radians), 0},
			{0, 1, 0, 0},
			{-sin(radians), 0, cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

Mat Engine::quaternion_rotationY_matrix(double radians) {
	Quaternion quaternion = Quaternion::AngleAxis(0, 1, 0, radians);
	return quaternion.get_rotationmatrix();
}

Mat Engine::euler_rotationZ_matrix(double radians) {
	Mat rotation_matrix = Mat(
		{
			{cos(radians), -sin(radians), 0, 0},
			{sin(radians), cos(radians), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		},
		4, 4
	);

	return rotation_matrix;
}

Mat Engine::quaternion_rotationZ_matrix(double radians) {
	Quaternion quaternion = Quaternion::AngleAxis(0, 0, 1, radians);
	return quaternion.get_rotationmatrix();
}

/// <summary>
/// Calculates whether there is an intersection between the given line and plane, updating the `intersection_point` variable to the intersection point and returning 1 if that is the case, and 0 otherwise.
/// </summary>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <param name="intersection_point">Reference to a variable which will store the intersection point if it exists</param>
/// <returns>1 if the line intersects the plane, 0 otherwise</returns>
bool Engine::LinePlaneIntersection(const Mat* plane_point, const Mat* plane_normal, const Mat* line_start, const Mat* line_end, Mat& intersection_point) {
	// Plane normal should be normalized?
	const Mat line_vector = *line_end - *line_start;
	const double alpha = Mat::dot(*plane_point - *line_start, *plane_normal) / Mat::dot(line_vector, *plane_normal);
	if (alpha != 0) {
		intersection_point = *line_start + (alpha * line_vector);
		return 1;
	}

	return 0;
}

/// <summary>
/// Returns the distance between a given point and a given plane.
/// </summary>
/// <param name="point">The point to be tested against the plane</param>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <returns>The distance between the point and plane.</returns>
double Engine::PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal) {
	return Mat::dot(*point - *plane_point, *plane_normal);
}

/// <summary>
/// Tests whether a given triangle intersects a given plane and, if there is an intersection, stores the resulting clipped triangles in the given variables.
/// </summary>
/// <param name="plane_point">A point on the plane</param>
/// <param name="plane_normal">A vector orthonormal to the plane</param>
/// <param name="input_triangle">The triangle to be tested against the plane</param>
/// <param name="clipped_triangle_a">Variable where a clipped triangle will be stored</param>
/// <param name="clipped_triangle_b">Variable where a clipped triangle will be stored</param>
/// <returns>How many new triangles it got split into</returns>
uint8_t Engine::ClipTriangleToPlane(const Mat* plane_point, const Mat* plane_normal, const Triangle* input_triangle, Triangle& clipped_triangle_a, Triangle& clipped_triangle_b) {
	uint8_t n_inside_points = 0;
	uint8_t n_outside_points = 0;

	const Mat* inside_points[3];
	const Mat* outside_points[3];
	int inside_indices[3];   // Keep track of original vertex indices
	int outside_indices[3];  // Keep track of original vertex indices

	double distances[3] = {
		Engine::PointDistanceToPlane(&input_triangle->vertices[0], plane_point, plane_normal),
		Engine::PointDistanceToPlane(&input_triangle->vertices[1], plane_point, plane_normal),
		Engine::PointDistanceToPlane(&input_triangle->vertices[2], plane_point, plane_normal)
	};

	// Classify points and maintain their original indices
	for (int i = 0; i < 3; i++) {
		if (distances[i] > 0) {
			inside_points[n_inside_points] = &input_triangle->vertices[i];
			inside_indices[n_inside_points] = i;
			n_inside_points++;
		}
		else {
			outside_points[n_outside_points] = &input_triangle->vertices[i];
			outside_indices[n_outside_points] = i;
			n_outside_points++;
		}
	}

	if (n_inside_points == 0) {
		return 0;
	}
	else if (n_inside_points == 3) {
		clipped_triangle_a = *input_triangle;
		return 1;
	}
	else if (n_inside_points == 1 && n_outside_points == 2) {
		Mat intersection_point_a = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
		Mat intersection_point_b = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[0], intersection_point_a);
		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[1], intersection_point_b);

		// Determine correct winding order based on original triangle
		int inside_idx = inside_indices[0];
		int next_idx = (inside_idx + 1) % 3;

		// If the next vertex in the original triangle matches first outside point
		if (next_idx == outside_indices[0]) {
			clipped_triangle_a = Triangle(*inside_points[0], intersection_point_a, intersection_point_b);
		}
		else {
			clipped_triangle_a = Triangle(*inside_points[0], intersection_point_b, intersection_point_a);
		}

		return 1;
	}
	else if (n_inside_points == 2 && n_outside_points == 1) {
		Mat intersection_point_a = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
		Mat intersection_point_b = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[0], outside_points[0], intersection_point_a);
		Engine::LinePlaneIntersection(plane_point, plane_normal, inside_points[1], outside_points[0], intersection_point_b);

		// Determine correct winding order based on original indices
		int first_inside_idx = inside_indices[0];
		int second_inside_idx = inside_indices[1];
		int outside_idx = outside_indices[0];

		// Check if first inside point comes before second inside point in original winding
		bool first_before_second = ((first_inside_idx + 1) % 3 == second_inside_idx) ||
			(first_inside_idx == 2 && second_inside_idx == 0);

		if (first_before_second) {
			clipped_triangle_a = Triangle(*inside_points[0], *inside_points[1], intersection_point_b);
			clipped_triangle_b = Triangle(*inside_points[0], intersection_point_b, intersection_point_a);
		}
		else {
			clipped_triangle_a = Triangle(*inside_points[1], *inside_points[0], intersection_point_a);
			clipped_triangle_b = Triangle(*inside_points[1], intersection_point_a, intersection_point_b);
		}

		return 2;
	}

	throw std::runtime_error("Invalid branch taken at function `ClipTriangleToPlane`");
}

// Sets yaw and pitch from a direction vector (the displacement in each axis from the default direction vector)
void Engine::Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch) {
	// Still need to handle gimbal lock edge case and also include roll

	yaw = atan2f(direction_vector.get(1, 1), direction_vector.get(3, 1));
	pitch = atan2f(-direction_vector.get(2, 1), sqrt(pow(direction_vector.get(1, 1), 2) + pow(direction_vector.get(3, 1), 2)));

	if (yaw < 0) yaw = (2 * M_PI) + yaw;
	if (pitch < 0) pitch = (2 * M_PI) + pitch;
}

// Sets yaw, pitch, and roll from a rotation matrix
void Engine::Euler_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll) {
	pitch = asin(-Utils::clamp(rotation_matrix.get(2, 3), -1, 1));

	if (abs(rotation_matrix.get(2, 3) < 0.9999999)) {
		yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
		roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
	}
	else {
		yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
		roll = 0;
	}
}

// Sets yaw, pitch, and roll from a direction vector (the displacement in each axis from the default direction vector)
void Engine::Quaternion_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll) {
	const Mat normalized_v1 = default_direction_vector / default_direction_vector.norm();
	const Mat normalized_v2 = direction_vector / direction_vector.norm();

	Mat rotation_axis = Engine::CrossProduct3D(normalized_v1, normalized_v2);
	rotation_axis.normalize();

	double rotation_angle = acos(Mat::dot(normalized_v1, normalized_v2));
	const Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

	rotation.GetAngles(yaw, pitch, roll);
}

// Frees the pixel buffer, destroys the texture, renderer, window, and cleans SDL subsystems.
void Engine::close() {
	free(this->pixel_buffer);
	free(this->depth_buffer);

	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);

	SDL_Quit();
}

/// <summary>
/// Updates the view matrix to look at the current camera direction vector in the current camera position, with the current camera up vector.
/// </summary>
void Engine::LookAt() {

	// Setting 4th dimension to 0 for dot product
	Mat camera_position = this->camera_position;
	camera_position.set(0, 4, 1);

	Mat cam_z_axis = ((camera_direction + camera_position) - camera_position);
	cam_z_axis.set(0, 4, 1);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	Mat cam_y_axis = Engine::CrossProduct3D(cam_z_axis, cam_x_axis);

	this->VIEW_MATRIX = Mat::identity_matrix(4);

	this->VIEW_MATRIX.set(cam_x_axis.get(1, 1), 1, 1);
	this->VIEW_MATRIX.set(cam_x_axis.get(2, 1), 1, 2);
	this->VIEW_MATRIX.set(cam_x_axis.get(3, 1), 1, 3);

	this->VIEW_MATRIX.set(cam_y_axis.get(1, 1), 2, 1);
	this->VIEW_MATRIX.set(cam_y_axis.get(2, 1), 2, 2);
	this->VIEW_MATRIX.set(cam_y_axis.get(3, 1), 2, 3);

	this->VIEW_MATRIX.set(cam_z_axis.get(1, 1), 3, 1);
	this->VIEW_MATRIX.set(cam_z_axis.get(2, 1), 3, 2);
	this->VIEW_MATRIX.set(cam_z_axis.get(3, 1), 3, 3);

	this->VIEW_MATRIX.set(-Mat::dot(cam_x_axis, camera_position), 1, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_y_axis, camera_position), 2, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_z_axis, camera_position), 3, 4);

	this->VIEW_MATRIX.set(0, 4, 1);
	this->VIEW_MATRIX.set(0, 4, 2);
	this->VIEW_MATRIX.set(0, 4, 3);
	this->VIEW_MATRIX.set(1, 4, 4);
}

/// <summary>
/// Updates the view matrix to look at the target vector.
/// </summary>
/// <param name="target_vector">Vector to look at</param>
void Engine::LookAt(const Mat& target_vector) {

	// Setting 4th dimension to 0 for dot product
	Mat camera_position = this->camera_position;
	camera_position.set(0, 4, 1);

	Mat tmp_target_vector = target_vector;
	tmp_target_vector.set(0, 4, 1);

	Mat cam_z_axis = target_vector - camera_position;
	cam_z_axis.set(0, 4, 1);
	cam_z_axis.normalize();
	this->camera_direction = cam_z_axis;

	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	Mat cam_y_axis = Engine::CrossProduct3D(cam_z_axis, cam_x_axis);
	this->camera_up = cam_y_axis;

	this->VIEW_MATRIX.set(cam_x_axis.get(1, 1), 1, 1);
	this->VIEW_MATRIX.set(cam_x_axis.get(2, 1), 1, 2);
	this->VIEW_MATRIX.set(cam_x_axis.get(3, 1), 1, 3);

	this->VIEW_MATRIX.set(cam_y_axis.get(1, 1), 2, 1);
	this->VIEW_MATRIX.set(cam_y_axis.get(2, 1), 2, 2);
	this->VIEW_MATRIX.set(cam_y_axis.get(3, 1), 2, 3);

	this->VIEW_MATRIX.set(cam_z_axis.get(1, 1), 3, 1);
	this->VIEW_MATRIX.set(cam_z_axis.get(2, 1), 3, 2);
	this->VIEW_MATRIX.set(cam_z_axis.get(3, 1), 3, 3);

	this->VIEW_MATRIX.set(-Mat::dot(cam_x_axis, camera_position), 1, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_y_axis, camera_position), 2, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_z_axis, camera_position), 3, 4);

	this->VIEW_MATRIX.set(0, 4, 1);
	this->VIEW_MATRIX.set(0, 4, 2);
	this->VIEW_MATRIX.set(0, 4, 3);
	this->VIEW_MATRIX.set(1, 4, 4);
}

/// <summary>
/// Returns a view matrix which looks at the given camera direction vector in the given camera position, with the given camera up vector.
/// </summary>
/// <param name="camera_position">Camera position vector</param>
/// <param name="camera_direction">Camera direction vector</param>
/// <param name="camera_up">Camera up vector</param>
/// <returns>A 4x4 view matrix looking at the camera direction in the camera position, with the camera up vector.</returns>
Mat Engine::LookAt(const Mat& camera_position, const Mat& camera_direction, const Mat& camera_up) {

	// Setting 4th dimension to 0 for dot product
	Mat tmp_camera_position = camera_position;
	tmp_camera_position.set(0, 4, 1);

	Mat cam_z_axis = ((camera_direction + camera_position) - tmp_camera_position);

	cam_z_axis.set(0, 4, 1);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	Mat cam_y_axis = Engine::CrossProduct3D(cam_z_axis, cam_x_axis);

	Mat VIEW_MATRIX = Mat::identity_matrix(4);

	VIEW_MATRIX.set(cam_x_axis.get(1, 1), 1, 1);
	VIEW_MATRIX.set(cam_x_axis.get(2, 1), 1, 2);
	VIEW_MATRIX.set(cam_x_axis.get(3, 1), 1, 3);

	VIEW_MATRIX.set(cam_y_axis.get(1, 1), 2, 1);
	VIEW_MATRIX.set(cam_y_axis.get(2, 1), 2, 2);
	VIEW_MATRIX.set(cam_y_axis.get(3, 1), 2, 3);

	VIEW_MATRIX.set(cam_z_axis.get(1, 1), 3, 1);
	VIEW_MATRIX.set(cam_z_axis.get(2, 1), 3, 2);
	VIEW_MATRIX.set(cam_z_axis.get(3, 1), 3, 3);

	VIEW_MATRIX.set(-Mat::dot(cam_x_axis, tmp_camera_position), 1, 4);
	VIEW_MATRIX.set(-Mat::dot(cam_y_axis, tmp_camera_position), 2, 4);
	VIEW_MATRIX.set(-Mat::dot(cam_z_axis, tmp_camera_position), 3, 4);

	VIEW_MATRIX.set(0, 4, 1);
	VIEW_MATRIX.set(0, 4, 2);
	VIEW_MATRIX.set(0, 4, 3);
	VIEW_MATRIX.set(1, 4, 4);

	return VIEW_MATRIX;
}

/// <summary>
/// Returns a view matrix which looks at the given target vector, and updates the camera direction and camera up vectors accordingly.
/// </summary>
/// <param name="camera_position">Camera position vector</param>
/// <param name="camera_direction">Camera direction vector</param>
/// <param name="target_vector">Vector to look at</param>
/// <param name="camera_up">Camera up vector</param>
/// <returns>A 4x4 view matrix looking at the camera direction in the camera position, with the camera up vector.</returns>
Mat Engine::LookAt(const Mat& camera_position, Mat& camera_direction, const Mat& target_vector, Mat& camera_up) {

	// Setting 4th dimension to 0 for dot product
	Mat tmp_camera_position = camera_position;
	tmp_camera_position.set(0, 4, 1);
	Mat tmp_target_vector = target_vector;
	tmp_target_vector.set(0, 4, 1);

	Mat cam_z_axis = target_vector - tmp_camera_position;

	cam_z_axis.set(0, 4, 1);
	cam_z_axis.normalize();
	camera_direction = cam_z_axis;

	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	Mat cam_y_axis = Engine::CrossProduct3D(cam_z_axis, cam_x_axis);
	camera_up = cam_y_axis;

	Mat VIEW_MATRIX = Mat::identity_matrix(4);

	VIEW_MATRIX.set(cam_x_axis.get(1, 1), 1, 1);
	VIEW_MATRIX.set(cam_x_axis.get(2, 1), 1, 2);
	VIEW_MATRIX.set(cam_x_axis.get(3, 1), 1, 3);

	VIEW_MATRIX.set(cam_y_axis.get(1, 1), 2, 1);
	VIEW_MATRIX.set(cam_y_axis.get(2, 1), 2, 2);
	VIEW_MATRIX.set(cam_y_axis.get(3, 1), 2, 3);

	VIEW_MATRIX.set(cam_z_axis.get(1, 1), 3, 1);
	VIEW_MATRIX.set(cam_z_axis.get(2, 1), 3, 2);
	VIEW_MATRIX.set(cam_z_axis.get(3, 1), 3, 3);

	VIEW_MATRIX.set(-Mat::dot(cam_x_axis, tmp_camera_position), 1, 4);
	VIEW_MATRIX.set(-Mat::dot(cam_y_axis, tmp_camera_position), 2, 4);
	VIEW_MATRIX.set(-Mat::dot(cam_z_axis, tmp_camera_position), 3, 4);

	VIEW_MATRIX.set(0, 4, 1);
	VIEW_MATRIX.set(0, 4, 2);
	VIEW_MATRIX.set(0, 4, 3);
	VIEW_MATRIX.set(1, 4, 4);

	return VIEW_MATRIX;
}

void Engine::GetRoll(const Mat& camera_direction, const Mat& camera_up, const double& yaw, const double& pitch, double& roll) {

	if (abs(camera_direction.get(2, 1)) > 0.9999999) {
		// In gimbal lock - collapse roll into yaw
		roll = 0.0;
	}
	else {
		// Normal case - extract roll from up vector
		roll = -atan2(camera_up.get(1, 1) * cos(yaw) - camera_up.get(3, 1) * sin(yaw), camera_up.get(2, 1) / cos(pitch));
	}


	//roll = -atan2(new_vector.get(1, 1) * cos(yaw) - new_vector.get(3, 1) * sin(yaw), new_vector.get(2, 1) / cos(pitch));
}