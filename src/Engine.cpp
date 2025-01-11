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

	this->buffer = (uint32_t*) malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		this->buffer[i] = this->BG_COLOR;
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
					free(this->buffer);
					//std::cout << "Old width: " << this->WIDTH << std::endl;
					//std::cout << "Old height: " << this->HEIGHT << std::endl;
					this->WIDTH = this->event.window.data1;
					this->HEIGHT = this->event.window.data2;

					//std::cout << "New width: " << this->WIDTH << std::endl;
					//std::cout << "New height: " << this->HEIGHT << std::endl;

					this->buffer = (uint32_t*)malloc(sizeof(uint32_t) * this->WIDTH * this->HEIGHT);
					for (size_t i = 0; i < this->WIDTH * this->HEIGHT; i++) {
						this->buffer[i] = this->BG_COLOR;
					}

					this->AR = (double) this->WIDTH / (double) this->HEIGHT;
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
							{0, 0, 0, 0},
							{0, 0, 0, 0}
						}, 4, 4);

					this->update_projection_matrix();

					break;
			}

			break;
		case SDL_KEYDOWN:
			switch (this->event.key.keysym.scancode) {
			case SDL_SCANCODE_KP_4:
			{
				this->z_sorted = false;

				// same as j
				Instance* obj = this->current_scene.get_instance_ptr(5);
				obj->tx += 0.01;
				obj->TRANSLATION_MATRIX = Engine::translation_matrix(obj->tx, obj->ty, obj->tz);
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
			{
				this->z_sorted = false;
				// same as l
				Instance* obj = this->current_scene.get_instance_ptr(5);
				obj->tx -= 0.01;
				obj->TRANSLATION_MATRIX = Engine::translation_matrix(obj->tx, obj->ty, obj->tz);
				obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
			}
				break;
			case SDL_SCANCODE_KP_8:
			{
				this->z_sorted = false;
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
				// same as k
			{
				this->z_sorted = false;
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
				// same as q
			{
				this->z_sorted = false;
				Instance* obj = this->current_scene.get_instance_ptr(5);
				obj->sx -= 0.01;
				obj->sy -= 0.01;
				obj->sz -= 0.01;
				obj->SCALING_MATRIX = Engine::scale_matrix(obj->sx, obj->sy, obj->sz);
				obj->MODEL_TO_WORLD = obj->TRANSLATION_MATRIX * obj->ROTATION_MATRIX * obj->SCALING_MATRIX;
			}
				break;
			case SDL_SCANCODE_KP_9:
				// same as e
			{
				this->z_sorted = false;
				Instance* obj = this->current_scene.get_instance_ptr(5);
				obj->sx += 0.01;
				obj->sy += 0.01;
				obj->sz += 0.01;
				obj->SCALING_MATRIX = Engine::scale_matrix(obj->sx, obj->sy, obj->sz);
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
			case SDL_SCANCODE_KP_PERIOD:
				light_reach -= 0.1;
				break;
			case SDL_SCANCODE_KP_ENTER:
				light_reach += 0.1;
				break;

			case SDL_SCANCODE_1:
				this->wireframe_triangles = !this->wireframe_triangles;
				break;
			case SDL_SCANCODE_2:
				this->rasterize_triangles = !this->rasterize_triangles;
				if (this->rasterize_triangles) {
					this->shade_triangles = false;
				}
				break;
			case SDL_SCANCODE_3:
				this->shade_triangles = !this->shade_triangles;
				if (this->shade_triangles) {
					this->rasterize_triangles = false;
				}
				break;
			case SDL_SCANCODE_T:
				//std::cout << "Camera position:" << std::endl;
				//(this->camera_position - this->default_camera_position).print();
				//this->camera_position.print();
				//std::cout << "Default camera direction: " << std::endl;
				//this->default_camera_direction.print();
				std::cout << "Camera yaw: " << this->camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << this->camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << this->camera_roll * (180 / M_PI) << std::endl;

				{

					//double new_yaw = -atan(this->VIEW_MATRIX.get(1, 3) / this->VIEW_MATRIX.get(3, 3));
					//double new_pitch = -asin(-this->VIEW_MATRIX.get(2, 3));
					//double new_roll = -atan(this->VIEW_MATRIX.get(2, 1) / this->VIEW_MATRIX.get(2, 2));

					double new_yaw = atan2f(this->VIEW_MATRIX.get(3, 1), this->VIEW_MATRIX.get(3, 3));
					


					double new_roll = atan2f(this->VIEW_MATRIX.get(1, 2), this->VIEW_MATRIX.get(2, 2));

					double new_pitch = 0;

					if (cos(new_yaw) == 0) {
						new_pitch = atan2f(-this->VIEW_MATRIX.get(3, 2), this->VIEW_MATRIX.get(3, 1) / sin(new_yaw));
					}
					else {
						new_pitch = atan2f(-this->VIEW_MATRIX.get(3, 2), this->VIEW_MATRIX.get(3, 3) / cos(new_yaw));
					}

					//double new_pitch = asin(-this->VIEW_MATRIX.get(3, 2));

					if (new_yaw < 0) new_yaw += 2 * M_PI;
					if (new_pitch < 0) new_pitch += 2 * M_PI;
					if (new_roll < 0) new_roll += 2 * M_PI;

					//std::cout << "Camera yaw 2: " << new_yaw * (180 / M_PI) << std::endl;
					//std::cout << "Camera pitch 2: " << new_pitch * (180 / M_PI) << std::endl;
					//std::cout << "Camera roll 2: " << new_roll * (180 / M_PI) << std::endl;
				}

				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();
				std::cout << "Camera up: " << std::endl;
				this->camera_up.print();
				//std::cout << "View: " << std::endl;
				//this->VIEW_MATRIX.print();

				std::cout << "Camera position: " << std::endl;
				this->camera_position.print();


				{
					/*
					Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), camera_yaw);
					Mat rotation = rotationY.get_rotationmatrix();
					camera_direction = rotation * camera_direction;
					camera_right = rotation * camera_right;

					Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), camera_pitch);
					rotation = rotationX.get_rotationmatrix();

					camera_direction = rotation * camera_direction;

					//camera_up = Engine::CrossProduct3D(camera_direction, camera_right);
					camera_up = rotation * camera_up;

					Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), camera_roll);
					rotation = rotationZ.get_rotationmatrix();
					camera_up = rotation * camera_up;
					*/

					double tmp_yaw = 0;
					double tmp_pitch = 0;
					double tmp_roll = 0;
					Engine::Quaternion_GetAnglesFromQuaternionYP(q_camera, tmp_yaw, tmp_pitch, tmp_roll);
					

					Mat rotated_direction = default_camera_direction;
					Mat rotated_up = default_camera_up;
					Mat cam_right = Mat({ {1}, {0}, {0}, {0} }, 4, 1);

					rotated_direction = Quaternion::RotatePoint(rotated_direction, Mat({ {0}, {1}, {0}, {0} }, 4, 1), tmp_yaw);
					rotated_direction.set(0, 4, 1);

					//cam_right = Quaternion::RotatePoint(cam_right, Mat({ {0}, {1}, {0}, {0} }, 4, 1), tmp_yaw);
					//cam_right.set(0, 4, 1);

					cam_right = Engine::CrossProduct3D(rotated_up, rotated_direction);
					std::cout << "Right vector: " << std::endl;
					cam_right.print();
					rotated_direction = Quaternion::RotatePoint(rotated_direction, cam_right, tmp_pitch);
					rotated_direction.set(0, 4, 1);
					rotated_up = Quaternion::RotatePoint(rotated_up, cam_right, tmp_pitch);
					rotated_up.set(0, 4, 1);

					rotated_up = Quaternion::RotatePoint(rotated_up, rotated_direction, tmp_roll);
					rotated_up.set(0, 4, 1);

					camera_direction.set(0, 4, 1);
					camera_up.set(0, 4, 1);
					double similarity_direction = Mat::dot(camera_direction, rotated_direction) / (camera_direction.norm() * rotated_direction.norm());
					double similarity_up = Mat::dot(camera_up, rotated_up) / (camera_up.norm() * rotated_up.norm());

					std::cout << "Similarity dir: " << similarity_direction << std::endl;
					std::cout << "Similarity up: " << similarity_up << std::endl;

					///*
					if (similarity_direction < 0.999999) {
						std::cout << "Direction differs, current: " << std::endl;
						camera_direction.print();

						std::cout << "Rerotated direction: " << std::endl;
						rotated_direction.print();
					}

					if (similarity_up < 0.999999) {
						std::cout << "Up differs, current: " << std::endl;
						camera_up.print();

						std::cout << "Rerotated up: " << std::endl;
						rotated_up.print();
					}
					//*/
				}

				//std::cout << "View matrix: " << std::endl;
				//this->VIEW_MATRIX.print();
				//std::cout << "TRANSLATION: " << std::endl;
				//this->TRANSLATION_MATRIX.print();
				//std::cout << "ROTATION: " << std::endl;
				//this->ROTATION_MATRIX.print();
				//std::cout << "SCALE: " << std::endl;
				//this->SCALING_MATRIX.print();
				break;
			case SDL_SCANCODE_P:
			{
				//Quaternion_GetAnglesFromQuaternionYP(this->q_camera_direction, camera_yaw, camera_pitch, camera_roll);
				//Quaternion_GetAnglesFromQuaternionYR(this->q_camera_up, camera_yaw, camera_pitch, camera_roll);
				//Quaternion_GetAnglesFromQuaternionYP(this->q_camera, camera_yaw, camera_pitch, camera_roll);
			}

				std::cout << "Camera position:" << std::endl;
				(this->camera_position - this->default_camera_position).print();
				std::cout << "Default camera direction: " << std::endl;
				this->default_camera_direction.print();
				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();
				std::cout << "Camera yaw: " << this->camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "Camera pitch: " << this->camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "Camera roll: " << this->camera_roll * (180 / M_PI) << std::endl;
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				std::cout << "TRANSLATION: " << std::endl;
				this->TRANSLATION_MATRIX.print();
				std::cout << "ROTATION: " << std::endl;
				this->ROTATION_MATRIX.print();
				std::cout << "SCALE: " << std::endl;
				this->SCALING_MATRIX.print();

				printf("\n\n");

				{
					std::cout << "From specifics: " << std::endl;
					double tmp_yaw = 0;
					double tmp_pitch = 0;
					double tmp_roll = 0;
					Engine::Quaternion_GetAnglesFromDirectionYP(default_camera_direction, camera_direction, tmp_yaw, tmp_pitch, tmp_roll);
					std::cout << "Direction yaw: " << tmp_yaw << std::endl;
					std::cout << "Direction pitch: " << tmp_pitch << std::endl;
					Engine::Quaternion_GetAnglesFromDirectionYP(default_camera_up, camera_up, tmp_yaw, tmp_pitch, tmp_roll);
					std::cout << "Up pitch: " << tmp_pitch << std::endl;
					std::cout << "Up roll: " << tmp_roll << std::endl;


					std::cout << "Previous compounded: " << std::endl;
					std::cout << "Camera yaw: " << camera_yaw << std::endl;
					std::cout << "Camera pitch: " << camera_pitch << std::endl;
					std::cout << "Camera roll: " << camera_roll << std::endl;

					Engine::Quaternion_GetAnglesFromQuaternionYP(q_camera, camera_yaw, camera_pitch, camera_roll);

					std::cout << "New from quaternion: " << std::endl;
					std::cout << "Camera yaw: " << camera_yaw << std::endl;
					std::cout << "Camera pitch: " << camera_pitch << std::endl;
					std::cout << "Camera roll: " << camera_roll << std::endl;

					std::cout << "Quaternion x: " << q_camera.x << std::endl;
					std::cout << "Quaternion y: " << q_camera.y << std::endl;
					std::cout << "Quaternion z: " << q_camera.z << std::endl;
					std::cout << "Quaternion w: " << q_camera.w << std::endl;
				}

				this->current_scene.save_scene(this->scenes_folder, "tst.json", this->models_folder, false, this->default_camera_position, this->camera_position, this->default_camera_direction, this->camera_direction, this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll);
				break;

				this->current_scene = Scene(this->scenes_folder, "hallway.json", this->models_folder, false, this->default_camera_position, this->camera_position, this->default_camera_direction, this->camera_direction, this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll, this->TRANSLATION_MATRIX, this->ROTATION_MATRIX, this->SCALING_MATRIX, this->VIEW_MATRIX, this->use_scene_camera_settings);

				std::cout << "Camera position:" << std::endl;
				this->camera_position.print();
				std::cout << "Default camera direction: " << std::endl;
				this->default_camera_direction.print();
				std::cout << "Camera direction: " << std::endl;
				this->camera_direction.print();

				{

				double new_yaw = atan2f(this->VIEW_MATRIX.get(3, 1), this->VIEW_MATRIX.get(3, 3));



				double new_roll = atan2f(this->VIEW_MATRIX.get(1, 2), this->VIEW_MATRIX.get(2, 2));

				double new_pitch = 0;

				if (cos(new_yaw) == 0) {
					new_pitch = atan2f(-this->VIEW_MATRIX.get(3, 2), this->VIEW_MATRIX.get(3, 1) / sin(new_yaw));
				}
				else {
					new_pitch = atan2f(-this->VIEW_MATRIX.get(3, 2), this->VIEW_MATRIX.get(3, 3) / cos(new_yaw));
				}

				//double new_pitch = asin(-this->VIEW_MATRIX.get(3, 2));

				if (new_yaw < 0) new_yaw += 2 * M_PI;
				if (new_pitch < 0) new_pitch += 2 * M_PI;
				if (new_roll < 0) new_roll += 2 * M_PI;

				//this->camera_yaw = new_yaw;
				//this->camera_pitch = new_pitch;
				//this->camera_roll = new_roll;
				}

				std::cout << "Camera yaw: " << this->camera_yaw << std::endl;
				std::cout << "Camera pitch: " << this->camera_pitch << std::endl;
				std::cout << "Camera roll: " << this->camera_roll << std::endl;
				std::cout << "View matrix: " << std::endl;
				this->VIEW_MATRIX.print();
				break;
				//this->playing = !this->playing;
				//this->current_scene.scene_instances[0].show = !this->current_scene.scene_instances[0].show;
				//this->VIEW_MATRIX.print();

				std::cout << "Camera direction: ";  this->camera_direction.print();
				std::cout << "PREVIOUS CAMERA YAW: " << this->camera_yaw * (180 / M_PI) << std::endl;
				std::cout << "PREVIOUS CAMERA PITCH: " << this->camera_pitch * (180 / M_PI) << std::endl;
				std::cout << "PREVIOUS CAMERA ROLL: " << this->camera_roll * (180 / M_PI) << std::endl;
				//std::cout << "Camera position: "; this->camera_position.print();

				//std::cout << "Yaw: " << this->camera_yaw * (180 / M_PI) << " degrees." << std::endl;
				//std::cout << "Pitch: " << this->camera_pitch * (180 / M_PI) << " degrees." << std::endl;
				//std::cout << "Roll: " << this->camera_roll * (180 / M_PI) << " degrees." << std::endl;
				{
					/*
					double correct_yaw = (atan2f(-camera_direction.get(3, 1), camera_direction.get(1, 1)) - (M_PI / 2)) * (180 / M_PI);

					correct_yaw = Utils::round_to(correct_yaw, 2);

					double correct_pitch = atan2f(-camera_direction.get(2, 1), -camera_direction.get(3, 1)) * (180 / M_PI);

					correct_pitch = Utils::round_to(correct_pitch, 2);

					if (camera_yaw == Utils::round_to(M_PI, 2) && camera_yaw == camera_pitch) {
						camera_pitch = 0;
						camera_yaw = M_PI;
					}

					

					if (correct_yaw < 0) correct_yaw += 360;
					if (correct_pitch < 0) correct_pitch += 360;

					std::cout << "New camera yaw 1 (EULER): " << correct_yaw << std::endl;
					std::cout << "New camera pitch 1 (EULER): " << correct_pitch << std::endl;
					*/

					//double new_yaw = (atan2f(camera_direction.get(3, 1), camera_direction.get(1, 1)) * (180 / M_PI)) - 90;
					//if (new_yaw < 0) new_yaw += (2 * M_PI) * (180 / M_PI);
					//double new_pitch = (atan2f(camera_direction.get(2, 1), camera_direction.get(3, 1)) * (180 / M_PI));
					//if (new_pitch < 0) new_pitch += (2 * M_PI) * (180 / M_PI);
					//std::cout << "New yaw 1: " << correct_yaw << std::endl;
					//std::cout << "New pitch 1: " << correct_pitch << std::endl;
				}

				{
					//double correct_yaw = atan2f(-camera_direction.get(3, 1), camera_direction.get(1, 1));


					//double new_adjacent = -camera_direction.get(3, 1);
					//double new_adjacent = sqrt((camera_direction.norm() * camera_direction.norm()) - (camera_direction.get(2, 1) + camera_direction.get(2, 1)));
					//double correct_pitch = atan2f(camera_direction.get(2, 1), new_adjacent);
					double start = SDL_GetTicks();

					for (int i = 0; i < 13248; i++) {
						double correct_yaw = atan2f(camera_direction.get(1, 1), camera_direction.get(3, 1));
						//double correct_pitch = asinf(-camera_direction.get(2, 1));
						//double correct_pitch = atan2f(-camera_direction.get(2, 1), camera_direction.get(3, 1));
						double correct_pitch = atan2f(-camera_direction.get(2, 1), sqrt(pow(camera_direction.get(1, 1), 2) + pow(camera_direction.get(3, 1), 2)));

						if (correct_yaw < 0) correct_yaw = (2 * M_PI) + correct_yaw;

						if (correct_pitch < 0) correct_pitch = (2 * M_PI) + correct_pitch;

						//if (correct_yaw >= 2 * M_PI) correct_yaw = fmod(correct_yaw, M_PI);
						//if (correct_pitch >= 2 * M_PI) correct_pitch = fmod(correct_pitch, M_PI);

						//correct_yaw = Utils::round_to(correct_yaw, 5);
						//correct_pitch = Utils::round_to(correct_pitch, 5);

						if (i == 10000) {
							std::cout << "NEW CAMERA YAW (EULER): " << correct_yaw * (180 / M_PI) << " degrees." << std::endl;
							std::cout << "NEW CAMERA PITCH (EULER): " << correct_pitch * (180 / M_PI) << " degrees." << std::endl;
						}
					}

					double end = SDL_GetTicks();
					std::cout << "Euler elapsed time: " << end - start << std::endl;

					//std::cout << "New camera yaw 2 (EULER): " << correct_yaw * (180 / M_PI) << std::endl;
					//std::cout << "New camera pitch 2 (EULER): " << correct_pitch * (180 / M_PI) << std::endl;

					//camera_yaw = correct_yaw;
					//camera_pitch = correct_pitch;


	
					
					

					/*
					double one_degree_in_radians = 1 * (M_PI / 180);

					// If yaw is roughly equal to PI, and yaw is roughly equal to pitch, set pitch equal to 0 in order to avoid gimbal lock
					if (abs(camera_yaw - Utils::round_to(M_PI, 3)) <= one_degree_in_radians && abs(camera_yaw - camera_pitch) == one_degree_in_radians) {
						camera_pitch = 0;
						camera_yaw = Utils::round_to(M_PI, 3);
					}
					*/

					//std::cout << "New yaw 2: " << Utils::round_to(correct_yaw * (180 / M_PI), 3) << std::endl;
					//std::cout << "New pitch 2: " << Utils::round_to(correct_pitch * (180 / M_PI), 3) << std::endl;
				}

				{
					double start = SDL_GetTicks();

					for (int i = 0; i < 13248; i++) {
						Mat original_direction = Mat(
							{
								{0},
								{0},
								{1},
								{0}
							}, 4, 1
						);
						original_direction.normalize();

						Mat cam_dir = this->camera_direction;
						cam_dir.set(0, 4, 1);
						cam_dir.normalize();


						double similarity = Mat::dot(original_direction, cam_dir);

						Mat rotation_axis = Engine::CrossProduct3D(original_direction, cam_dir);
						rotation_axis.normalize();


						double rotation_angle = acos(similarity);

						Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

						double yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z),
							1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));

						// Extract pitch (X rotation, around x-axis, second)
						double pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));

						if (yaw < 0) yaw = 2 * M_PI + yaw;
						if (pitch < 0) pitch = 2 * M_PI + pitch;

						double c_yaw = yaw;
						double c_pitch = pitch;


					}

					double end = SDL_GetTicks();
					std::cout << "Quaternion elapsed time: " << end - start << std::endl;
					//break;
					///*
					Mat original_direction = Mat(
						{
							{0},
							{0},
							{1},
							{0}
						}, 4, 1
					);
					original_direction.normalize();

					Mat cam_dir = this->camera_direction;
					cam_dir.set(0, 4, 1);
					cam_dir.normalize();


					double similarity = Mat::dot(original_direction, cam_dir);

					//std::cout << "Similarity: " << similarity << std::endl;

					/*
					if (similarity > 0.99) {
						std::cout << "Both camera direction vectors (previous and current) are too similar to differentiate." << std::endl;
						std::cout << "Camera yaw: " << 0 << " degrees." << std::endl;
						std::cout << "Camera pitch: " << 0 << " degrees." << std::endl;
						std::cout << "Camera roll: " << 0 << " degrees." << std::endl;
						break;
					}
					*/

					Mat rotation_axis = Engine::CrossProduct3D(original_direction, cam_dir);
					rotation_axis.normalize();


					double rotation_angle = acos(similarity);

					Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);
					///*
					//double c_yaw = asin(2 * ((rotation.w * rotation.y) - (rotation.z * rotation.x)));
					//double c_pitch = asin(2 * ((rotation.x * rotation.w) - (rotation.y * rotation.z)));
					//double c_pitch = atan2(2 * ((rotation.x * rotation.w) + (rotation.y * rotation.z)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.y + rotation.y))));
					//if (c_pitch < 0) {
						//c_pitch = (2 * M_PI) + c_pitch;
					//}

					//double c_roll = atan2(2 * ((rotation.x * rotation.y) + (rotation.z * rotation.w)), 1 - (2 * ((rotation.y * rotation.y) + (rotation.z + rotation.z))));
					//*/

					double yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z),
						1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));

					// Extract pitch (X rotation, around x-axis, second)
					double pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));

					// Extract roll (Z rotation, around z-axis, last)
					//double roll = atan2(2 * (rotation.w * rotation.z + rotation.y * rotation.x),
						//1 - 2 * (rotation.x * rotation.x + rotation.z * rotation.z));


					// Normalize to [0, 2π) range
					if (yaw < 0) yaw = 2 * M_PI + yaw;
					if (pitch < 0) pitch = 2 * M_PI + pitch;
					//if (roll < 0) roll = 2 * M_PI + roll;

					double c_yaw = yaw;
					double c_pitch = pitch;
					//double c_roll = roll;

					// Given quaternion components w, x, y, z:

					std::cout << "NEW CAMERA YAW (QUATERNION): " << c_yaw * (180 / M_PI) << " degrees." << std::endl;
					std::cout << "NEW CAMERA PITCH (QUATERNION): " << c_pitch * (180 / M_PI) << " degrees." << std::endl;
					//std::cout << "NEW CAMERA ROLL (QUATERNION): " << c_roll * (180 / M_PI) << " degrees." << std::endl;

					//*/
				}
				
			{
				/*
				// Mesh new_mesh(camera);

				camera.print();
				*/

				//Mesh new_mesh = Mesh();
				//scene_meshes.push_back(new_mesh);
			}
				break;
			case SDL_SCANCODE_Z:
			{
				Instance* pilar = this->current_scene.get_instance_ptr("5");
				double tx = pilar->MODEL_TO_WORLD.get(1, 4);
				double ty = pilar->MODEL_TO_WORLD.get(2, 4);
				double tz = pilar->MODEL_TO_WORLD.get(3, 4);
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::rotationX_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
				//break;

				pilar = this->current_scene.get_instance_ptr("22");
				pilar->tx += translation_amount;
				pilar->ty += translation_amount;
				pilar->tz += translation_amount;
				pilar->TRANSLATION_MATRIX = Engine::translation_matrix(-pilar->tx, -pilar->ty, -pilar->tz);
				pilar->MODEL_TO_WORLD = pilar->TRANSLATION_MATRIX * pilar->ROTATION_MATRIX * pilar->SCALING_MATRIX;
				break;

				// Rotate about camera direction (0, 0, -1)
				// By this->rotation_angle

				/*
				q = cos(2 * this->rotation_angle / 2) + (sin(2 * this->rotation_angle / 2) * ());
				p = 2;
				qinv = inv(q)

				ROTATED_POINT = q * p * qinv;
				ROTATED_MODEL_TO_WORLD = ROTATE(pilar->MODEL_TO_WORLD, ROTATED_POINT);
				*/

				double a = cos(8 * (this->rotation_angle / 2));
				double b = sin(8 * (this->rotation_angle / 2)) * 0;
				double c = sin(8 * (this->rotation_angle / 2)) * 0;
				double d = sin(8 * (this->rotation_angle / 2)) * 1;

				a /= sqrt((a * a) + (b * b) + (c * c) + (d * d));
				b /= sqrt((a * a) + (b * b) + (c * c) + (d * d));
				c /= sqrt((a * a) + (b * b) + (c * c) + (d * d));
				d /= sqrt((a * a) + (b * b) + (c * c) + (d * d));


				/*
				Mat ROTATE_MODEL_TO_WORLD = Mat(
					{ 
						{1 - (c * c) - (d * d), (b * c) - (a * d), (b * d) + (a * c), 0},
						{(b * c) + (a * d), 1 - (b * b) - (d * d), (c * d) - (a * b), 0},
						{(b * d) - (a * c), (c * d) + (a * b), 1 - (b * b) - (c * c), 0},
						{0, 0, 0, 1}
					}
				, 4, 4);
				*/

				Mat ROTATE_MODEL_TO_WORLD = Mat(
					{
					   {1 - 2 * (c * c + d * d), 2 * (b * c - a * d), 2 * (b * d + a * c), 0},
						{2 * (b * c + a * d), 1 - 2 * (b * b + d * d), 2 * (c * d - a * b), 0},
						{2 * (b * d - a * c), 2 * (c * d + a * b), 1 - 2 * (b * b + c * c), 0},
						{0, 0, 0, 1}
					}
				, 4, 4);

				Quaternion q = Quaternion::AngleAxis(0, 0, 1, this->rotation_angle);


				std::cout << "Length before: " << q.get_magnitude() << std::endl;
				q.normalize();
				std::cout << "Length after: " << q.get_magnitude() << std::endl;

				ROTATE_MODEL_TO_WORLD = q.get_rotationmatrix();

				Quaternion rr = Quaternion::AngleAxis(1, 0, 0, this->rotation_angle);
				Quaternion point = Quaternion(0, 0, -5, 0);
				Quaternion rr_cj = rr.get_complexconjugate();
				Quaternion result = rr * point * rr_cj;
				Mat new_point = result.get_3dvector();
				new_point.print();

				/*
				Mat ROTATE_MODEL_TO_WORLD = Mat(
					{
					   {(a * a) + (b * b) - (c * c) - (d * d), 2 * (b * c - a * d), 2 * (b * d + a * c), 0},
						{2 * (b * c + a * d), (a * a) - (b * b) + (c * c) - (d * d), 2 * (c * d - a * b), 0},
						{2 * (b * d - a * c), 2 * (c * d + a * b), (a * a) - (b * b) - (c * c) + (d * d), 0},
						{0, 0, 0, 1}
					}
				, 4, 4);
				*/

				pilar->MODEL_TO_WORLD = ROTATE_MODEL_TO_WORLD * pilar->MODEL_TO_WORLD;

				//pilar->MODEL_TO_WORLD = Engine::rotationZ_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;

				pilar->MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
			}
				break;
			case SDL_SCANCODE_X:
			{
				Instance* pilar = this->current_scene.get_instance_ptr("5");
				//double tx = pilar->MODEL_TO_WORLD.get(1, 4);
				//double ty = pilar->MODEL_TO_WORLD.get(2, 4);
				//double tz = pilar->MODEL_TO_WORLD.get(3, 4);
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::rotationX_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
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
			{
				Instance* pilar = this->current_scene.get_instance_ptr("5");
				double tx = pilar->MODEL_TO_WORLD.get(1, 4);
				double ty = pilar->MODEL_TO_WORLD.get(2, 4);
				double tz = pilar->MODEL_TO_WORLD.get(3, 4);
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::rotationX_matrix(this->rotation_angle) * pilar->MODEL_TO_WORLD;
				//pilar->MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * pilar->MODEL_TO_WORLD;
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
				this->FOV--;
				FOVr = FOV * (M_PI / 180);
				//printf("FOV: %f", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_KP_PLUS:
				this->FOV++;
				FOVr = FOV * (M_PI / 180);
				//printf("FOV: %f", FOV);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_LEFTBRACKET:
				/*
				for (Mesh& mesh : scene_meshes) {
					scale(mesh, sx - scale_factor, sy - scale_factor, 1);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					scale(instance, sx - scale_factor, sy - scale_factor, 1);
				}
				*/

				SCALING_MATRIX = Engine::scale_matrix(sx - scale_factor, sy - scale_factor, 1) * SCALING_MATRIX;
				this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				//this->VIEW_MATRIX = Engine::scale_matrix(sx - scale_factor, sy - scale_factor, 1) * this->VIEW_MATRIX;

				/*
				for (Mesh& mesh : loaded_meshes) {
					scale(mesh, sx - scale_factor, sy - scale_factor, 1);
				}
				*/

				/*
				if (this->near >= 0.5) {
					this->near -= 0.5;
				}
				*/
				
				/*
				for (Triangle& triangle : this->main_cube.triangles) {
					for (Mat& vertex : triangle.vertices) {
						vertex.set(vertex.get(3, 1) - 0.1, 3, 1);
					}
					//rotateZ(triangle);
					//rotate(triangle);
					//rotateX(triangle);
					//rotateY(triangle);
				}
				*/
				//this->FOV++;
				break;
			case SDL_SCANCODE_RIGHTBRACKET:
				/*
				for (Mesh& mesh : loaded_meshes) {
					scale(mesh, sx + scale_factor, sy + scale_factor, 1);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					scale(instance, sx + scale_factor, sy + scale_factor, 1);
				}
				*/


				SCALING_MATRIX = Engine::scale_matrix(sx + scale_factor, sy + scale_factor, 1) * SCALING_MATRIX;
				this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				//this->VIEW_MATRIX = Engine::scale_matrix(sx + scale_factor, sy + scale_factor, 1) * this->VIEW_MATRIX;

				//translate(mesh, 0.15, 0, 0);
				/*
				if (this->near + 0.5 >= 0) {
					this->near += 0.5;
				}
				*/
				
				/*
				for (Triangle& triangle : this->main_cube.triangles) {
					for (Mat& vertex : triangle.vertices) {
						vertex.set(vertex.get(3, 1) + 0.1, 3, 1);
					}
					//rotateZ(triangle);
					//rotate(triangle);
					//rotateX(triangle);
					//rotateY(triangle);
				}
				*/
				//this->FOV--;
				//printf("FOV: %f", FOV);
				break;
			case SDL_SCANCODE_SEMICOLON:
				// Limits the far plane to the near plane, so the far plane cannot get behind the near plane
				if (this->far - 0.5 >= this->near) {
					this->far -= 0.5;
				}
				else {
					this->far = this->near + 0.5;
				}

				printf("New far plane value: %f\n", this->far);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_APOSTROPHE:
				// Far plane has no upper limit
				this->far += 0.5;

				printf("New far plane value: %f\n", this->far);

				this->update_projection_matrix();
				break;
			case SDL_SCANCODE_COMMA:
				// Least amount for near plane is 0.1
				if (this->near - 0.01 >= 0.01) {
					this->near -= 0.01;
				}
				else {
					this->near = 0.01;
				}

				this->update_projection_matrix();

				printf("New near plane value: %f\n", this->near);
				break;
			case SDL_SCANCODE_PERIOD:
				// Limits the near plane to the far plane, so the near plane cannot get in front the far plane
				if (this->near + 0.01 < this->far) {
					this->near += 0.01;
				}
				else {
					this->near = this->far;
				}

				this->update_projection_matrix();

				printf("New near plane value: %f\n", this->near);
				break;
			case SDL_SCANCODE_J:
				/*
				for (Triangle& triangle : this->cube.triangles) {
					draw_triangle(triangle);
					rotateY(triangle, -0.01);
					//rotateZ(triangle);
					//rotate(triangle);
					//rotateX(triangle);
					//rotateY(triangle);
				}
				*/

				// rotateY(camera, -rotation_angle);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateY(mesh, rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateY(instance, rotation_angle);
				}
				*/

			{
				//Mat rotation = Engine::rotationY_matrix(rotation_angle);
				//Mat rotation_inv = Engine::rotationY_matrix(-rotation_angle);
				
				//this->camera_direction = rotation * this->camera_direction;
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;


				///*
				Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), rotation_angle);
				Mat rotation = rotationY.get_rotationmatrix();
				this->camera_direction = rotation * this->camera_direction;
				this->q_camera_direction = rotationY * this->q_camera_direction;
				this->q_camera = rotationY * this->q_camera;
				//*/

				//this->camera_direction = Quaternion::RotatePoint(this->camera_direction, this->camera_up, rotation_angle);

				//Engine::rotateY(this->camera_direction, rotation_angle);
				
				//Engine::rotateY(this->camera_up, rotation_angle);


				//Engine::rotateY(this->ROTATION_MATRIX, -this->rotation_angle);
				Engine::LookAt();

				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				//Engine::rotateY(this->VIEW_MATRIX, -this->rotation_angle);

				ac_yaw += rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYP(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);

				
				/*
				std::cout << "Old: " << camera_roll << std::endl;

				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);

				std::cout << "New: " << camera_roll << std::endl;
				*/
				
				/*
				this->camera_yaw = this->camera_yaw + rotation_angle;

				if (this->camera_yaw >= (2 * M_PI)) {
					this->camera_yaw = fmod(this->camera_yaw, 2 * M_PI);
				}
				*/


				//this->camera_yaw = correct_yaw;
				//this->camera_pitch = correct_pitch;
			}
				break;
			case SDL_SCANCODE_L:
				/*
				for (Triangle& triangle : this->cube.triangles) {
					draw_triangle(triangle);
					rotateY(triangle, 0.01);
					//rotateZ(triangle);
					//rotate(triangle);
					//rotateX(triangle);
					//rotateY(triangle);
				}
				*/

				// rotateY(camera, rotation_angle);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateY(mesh, -rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateY(instance, -rotation_angle);
				}
				*/

			{
				//Mat rotation = Engine::rotationY_matrix(-rotation_angle);
				//Mat rotation_inv = Engine::rotationY_matrix(rotation_angle);
				
				//this->camera_direction = rotation * this->camera_direction;
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;

				///*
				Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), -rotation_angle);
				Mat rotation = rotationY.get_rotationmatrix();
				this->camera_direction = rotation * this->camera_direction;
				this->q_camera_direction = rotationY * this->q_camera_direction;
				this->q_camera = rotationY * this->q_camera;
				//*/

				//this->camera_direction = Quaternion::RotatePoint(this->camera_direction, this->camera_up, -rotation_angle);

				//Engine::rotateY(this->camera_direction, -rotation_angle);
				
				//Engine::rotateY(this->camera_up, -rotation_angle);
				//Engine::rotateY(this->ROTATION_MATRIX, this->rotation_angle);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				Engine::LookAt();

				ac_yaw -= rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYP(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/

				/*
				double correct_yaw = atan2f(camera_direction.get(1, 1), camera_direction.get(3, 1));
				double correct_pitch = atan2f(-camera_direction.get(2, 1), sqrt(pow(camera_direction.get(1, 1), 2) + pow(camera_direction.get(3, 1), 2)));

				if (correct_yaw < 0) correct_yaw = (2 * M_PI) + correct_yaw;
				if (correct_pitch < 0) correct_pitch = (2 * M_PI) + correct_pitch;

				camera_yaw = correct_yaw;
				camera_pitch = correct_pitch;
				*/

				/*
				this->camera_yaw = this->camera_yaw - rotation_angle;

				if (this->camera_yaw < 0) {
					this->camera_yaw = (2 * M_PI) + fmod(this->camera_yaw, 2 * M_PI);
				}
				*/
			}
				break;
			case SDL_SCANCODE_I:
				// rotateX(camera, -rotation_angle);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateX(mesh, rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateX(instance, rotation_angle);
				}
				*/

				// Rotation should be flipped as Y axis points down and not up???

			{
				//Mat rotation = Engine::rotationX_matrix(rotation_angle);
				//Mat rotation_inv = Engine::rotationX_matrix(-rotation_angle);
				//this->camera_direction = rotation * this->camera_direction;
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;

				Mat camera_right = Engine::CrossProduct3D(camera_up, camera_direction);
				///*
				Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), rotation_angle);
				Mat rotation = rotationX.get_rotationmatrix();

				this->camera_direction = rotation * camera_direction;
				this->camera_up = rotation * camera_up;

				this->q_camera_direction = rotationX * this->q_camera_direction;
				this->q_camera_up = rotationX * this->q_camera_up;
				this->q_camera = rotationX * this->q_camera;
				//*/

				//this->camera_direction = Quaternion::RotatePoint(this->camera_direction, camera_right, rotation_angle);
				//this->camera_up = Quaternion::RotatePoint(this->camera_up, camera_right, rotation_angle);

				//Engine::rotateX(this->camera_direction, rotation_angle);
				//Engine::rotateX(this->camera_up, rotation_angle);

				//Engine::rotateX(this->ROTATION_MATRIX, -this->rotation_angle);

				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				Engine::LookAt();

				ac_pitch += rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYP(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;

				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/

				/*
				double correct_yaw = atan2f(camera_direction.get(1, 1), camera_direction.get(3, 1));
				double correct_pitch = atan2f(-camera_direction.get(2, 1), sqrt(pow(camera_direction.get(1, 1), 2) + pow(camera_direction.get(3, 1), 2)));

				if (correct_yaw < 0) correct_yaw = (2 * M_PI) + correct_yaw;
				if (correct_pitch < 0) correct_pitch = (2 * M_PI) + correct_pitch;

				camera_yaw = correct_yaw;
				camera_pitch = correct_pitch;
				*/

				/*
				this->camera_pitch = this->camera_pitch + rotation_angle;

				if (this->camera_pitch >= (2 * M_PI)) {
					this->camera_pitch = fmod(this->camera_pitch, 2 * M_PI);
				}
				*/
			}
				break;
			case SDL_SCANCODE_K:
				// rotateX(camera, rotation_angle);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateX(mesh, -rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateX(instance, -rotation_angle);
				}
				*/

				// Rotation should be flipped as Y axis points down and not up???

			{
				//Mat rotation = Engine::rotationX_matrix(-rotation_angle);
				//Mat rotation_inv = Engine::rotationX_matrix(rotation_angle);
				
				//this->camera_direction = rotation * this->camera_direction;
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;

				Mat camera_right = Engine::CrossProduct3D(camera_up, camera_direction);

				///*
				Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), -rotation_angle);
				Mat rotation = rotationX.get_rotationmatrix();

				this->camera_direction = rotation * camera_direction;
				this->camera_up = rotation * camera_up;

				this->q_camera_direction = rotationX * this->q_camera_direction;
				this->q_camera_up = rotationX * this->q_camera_up;
				this->q_camera = rotationX * this->q_camera;
				//*/

				/*
				this->camera_direction = Quaternion::RotatePoint(this->camera_direction, camera_right, -rotation_angle);
				this->camera_up = Quaternion::RotatePoint(this->camera_up, camera_right, -rotation_angle);
				*/

				//Engine::rotateX(this->camera_direction, -rotation_angle);
				//Engine::rotateX(this->camera_up, -rotation_angle);
				
				//Engine::rotateX(this->ROTATION_MATRIX, this->rotation_angle);

				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				Engine::LookAt();

				ac_pitch -= rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYP(this->default_camera_direction, this->camera_direction, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/

				/*
				double correct_yaw = atan2f(camera_direction.get(1, 1), camera_direction.get(3, 1));
				double correct_pitch = atan2f(-camera_direction.get(2, 1), sqrt(pow(camera_direction.get(1, 1), 2) + pow(camera_direction.get(3, 1), 2)));

				if (correct_yaw < 0) correct_yaw = (2 * M_PI) + correct_yaw;
				if (correct_pitch < 0) correct_pitch = (2 * M_PI) + correct_pitch;

				camera_yaw = correct_yaw;
				camera_pitch = correct_pitch;
				*/
				
				/*
				this->camera_pitch = this->camera_pitch - rotation_angle;

				if (this->camera_pitch < 0) {
					this->camera_pitch = (2 * M_PI) + fmod(this->camera_pitch, 2 * M_PI);
				}
				*/
			}
				break;
			case SDL_SCANCODE_Q:
				// rotateZ(camera, -rotation_angle);

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateZ(mesh, rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateZ(instance, rotation_angle);
				}
				*/

				// Rotation should be flipped as Y axis points down and not up???

			{
				//Mat rotation_inv = Engine::rotationZ_matrix(-rotation_angle);
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;
				//Engine::rotateZ(this->ROTATION_MATRIX, -this->rotation_angle);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;

				Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), rotation_angle);
				Mat rotation = rotationZ.get_rotationmatrix();
				this->camera_up = rotation * this->camera_up;
				this->q_camera_up = rotationZ * this->q_camera_up;
				this->q_camera = rotationZ * this->q_camera;

				//Engine::rotateZ(this->camera_up, this->rotation_angle);

				Engine::LookAt();
				//Engine::rotateZ(this->ROTATION_MATRIX, -this->rotation_angle);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				//this->camera_direction = rotation_inv * this->camera_direction;

				ac_roll += rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYR(this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;

				*/

				/*
				this->camera_roll += rotation_angle;

				if (this->camera_roll >= (2 * M_PI)) {
					this->camera_roll = fmod(this->camera_roll, 2 * M_PI);
				}
				*/

				// have a camera direction
				// keep track of roll to know how to rotate it around where it points to
			}
				break;
			case SDL_SCANCODE_E:
				// rotateZ(camera, rotation_angle);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					rotateZ(mesh, -rotation_angle);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					rotateZ(instance, -rotation_angle);
				}
				*/

				// Rotation should be flipped as Y axis points down and not up???
			{
				//Mat rotation_inv = Engine::rotationZ_matrix(rotation_angle);
				//Mat rotation_inv = Engine::rotationZ_matrix(-rotation_angle);
				//this->VIEW_MATRIX = rotation_inv * this->VIEW_MATRIX;

				//Engine::rotateZ(this->ROTATION_MATRIX, this->rotation_angle);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				// 
				
				Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), -rotation_angle);
				Mat rotation = rotationZ.get_rotationmatrix();
				this->camera_up = rotation * this->camera_up;
				this->q_camera_up = rotationZ * this->q_camera_up;
				this->q_camera = rotationZ * this->q_camera;
				//Engine::rotateZ(this->camera_up, -this->rotation_angle);


				Engine::LookAt();
				//this->camera_direction = rotation_inv * this->camera_direction;

				ac_roll -= rotation_angle;
				Engine::Quaternion_GetAnglesFromDirectionYR(this->default_camera_up, this->camera_up, this->camera_yaw, this->camera_pitch, this->camera_roll);

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/

				/*
				this->camera_roll -= rotation_angle;

				if (this->camera_roll < 0) {
					this->camera_roll = -fmod(this->camera_roll, 2 * M_PI);
				}
				*/
			}
				break;
			case SDL_SCANCODE_A:
				// translate(camera, -tx, 0, 0);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, tx, 0, 0);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, tx, 0, 0);
				}
				*/

			{
				//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(1, 4) - tx, 1, 4);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;

				//this->camera_position.set(this->camera_position.get(1, 1) + tx, 1, 1);
				Mat camera_right = Engine::CrossProduct3D(this->camera_up, this->camera_direction);
				this->camera_position += tx * camera_right;
				Engine::LookAt();
			}
				//this->VIEW_MATRIX = Engine::translation_matrix(tx, 0, 0) * this->VIEW_MATRIX;

				/*
				for (Mat& vtx : mesh.vertices) {
					
					//vtx.set(vtx.get(1, 1) + 0.2, 1, 1);
				}
				*/
				break;
			case SDL_SCANCODE_D:
				// translate(camera, tx, 0, 0);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, -tx, 0, 0);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, -tx, 0, 0);
				}
				*/

			{
				//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(1, 4) + tx, 1, 4);
				//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
				//this->camera_position.set(this->camera_position.get(1, 1) - tx, 1, 1);
				Mat camera_right = Engine::CrossProduct3D(this->camera_up, this->camera_direction);
				this->camera_position -= tx * camera_right;
				Engine::LookAt();
			}
				//this->VIEW_MATRIX = Engine::translation_matrix(-tx, 0, 0) * this->VIEW_MATRIX;

				/*
				for (Mat& vtx : mesh.vertices) {
					vtx.set(vtx.get(1, 1) - 0.2, 1, 1);
				}
				*/
				break;
			case SDL_SCANCODE_W:
				// translate(camera, 0, 0, tz);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, 0, 0, -tz);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, 0, 0, -tz);
				}
				*/

				{
					//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(3, 4) - tz, 3, 4);
					//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
					//this->camera_position.set(this->camera_position.get(3, 1) + tz, 3, 1);
					this->camera_position += tz * this->camera_direction;
					//this->VIEW_MATRIX.set(-Mat::dot(this->camera_position, Mat({ {this->VIEW_MATRIX.get(1, 3)}, {this->VIEW_MATRIX.get(2, 3)}, {this->VIEW_MATRIX.get(3, 3)}, { 0 } }, 4, 1)), 3, 4);
					Engine::LookAt();
				}
				//this->VIEW_MATRIX = Engine::translation_matrix(0, 0, -tz) * this->VIEW_MATRIX;

				/*
				for (Mat& vtx : mesh.vertices) {
					vtx.set(vtx.get(2, 1) + 0.2, 2, 1);
				}
				*/
				break;
			case SDL_SCANCODE_S:
				// translate(camera, 0, 0, -tz);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, 0, 0, tz);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, 0, 0, tz);
				}
				*/

				{
					//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(3, 4) + tz, 3, 4);
					//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
					//this->camera_position.set(this->camera_position.get(3, 1) - tz, 3, 1);
					this->camera_position -= tz * this->camera_direction;
					Engine::LookAt();
				}
				//this->VIEW_MATRIX = Engine::translation_matrix(0, 0, tz) * this->VIEW_MATRIX;
				/*
				for (Mat& vtx : mesh.vertices) {
					vtx.set(vtx.get(2, 1) - 0.2, 2, 1);
				}
				*/
				break;
			case SDL_SCANCODE_UP:
				// translate(camera, 0, -ty, 0);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, 0, ty, 0);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, 0, ty, 0);
				}
				*/

				{
					//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(2, 4) - ty, 2, 4);
					//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
					//this->camera_position.set(this->camera_position.get(2, 1) + ty, 2, 1);
					this->camera_position += ty * this->camera_up;
					Engine::LookAt();
				}
				//this->VIEW_MATRIX = Engine::translation_matrix(0, ty, 0) * this->VIEW_MATRIX;
				/*
				for (Mat& vtx : mesh.vertices) {
					vtx.set(vtx.get(3, 1) - 0.2, 3, 1);
				}
				*/
				break;
			case SDL_SCANCODE_DOWN:
				// translate(camera, 0, ty, 0);				

				/*
				for (Mesh& mesh : loaded_meshes) {
					translate(mesh, 0, -ty, 0);
				}
				*/

				/*
				for (Instance& instance : this->current_scene.scene_instances) {
					translate(instance, 0, -ty, 0);
				}
				*/

				{
					//this->TRANSLATION_MATRIX.set(this->TRANSLATION_MATRIX.get(2, 4) + ty, 2, 4);
					//this->VIEW_MATRIX = SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
					//this->camera_position.set(this->camera_position.get(2, 1) - ty, 2, 1);
					this->camera_position -= ty * this->camera_up;
					Engine::LookAt();
				}
				//this->VIEW_MATRIX = Engine::translation_matrix(0, -ty, 0) * this->VIEW_MATRIX;
				/*
				for (Mat& vtx : mesh.vertices) {
					vtx.set(vtx.get(3, 1) + 0.2, 3, 1);
				}
				*/
				break;
			}
		default:
			break;
		}
	}

	return 0;
}

void Scene::save_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, const Mat& default_camera_position, const Mat& camera_position, const Mat& default_camera_direction, const Mat& camera_direction, const Mat& default_camera_up, const Mat& camera_up, double yaw, double pitch, double roll) {
	char scene_filepath[255];
	sprintf_s(scene_filepath, 255, "%s%s", scenes_folder, scene_filename);
	nlohmann::json json_object;

	json_object["scene"] = scene_filename;

	//json_object["camera"] = std::vector<std::string>{"default_position", "translation", "default_direction", "direction", "rotation"};
	//json_object["camera"] = { "default_position", "translation", "default_direction", "direction", "rotation" };

	// The default camera position is considered the world coordinate in which the camera will be considered to be (0, 0, 0)
	//json_object["camera"]["default_position"] = std::vector<std::string>{ "x", "y", "z" };
	json_object["camera"]["default_position"]["x"] = default_camera_position.get(1, 1);
	json_object["camera"]["default_position"]["y"] = default_camera_position.get(2, 1);
	json_object["camera"]["default_position"]["z"] = default_camera_position.get(3, 1);

	// Camera position relative to the default camera position
	//json_object["camera"]["translation"] = std::vector<std::string>{ "x", "y", "z" };
	json_object["camera"]["translation"]["x"] = camera_position.get(1, 1);
	json_object["camera"]["translation"]["y"] = camera_position.get(2, 1);
	json_object["camera"]["translation"]["z"] = camera_position.get(3, 1);

	json_object["camera"]["default_direction"]["x"] = default_camera_direction.get(1, 1);
	json_object["camera"]["default_direction"]["y"] = default_camera_direction.get(2, 1);
	json_object["camera"]["default_direction"]["z"] = default_camera_direction.get(3, 1);

	//json_object["camera"]["direction"] = std::vector<std::string>{ "x", "y", "z" };
	json_object["camera"]["direction"]["x"] = camera_direction.get(1, 1);
	json_object["camera"]["direction"]["y"] = camera_direction.get(2, 1);
	json_object["camera"]["direction"]["z"] = camera_direction.get(3, 1);

	json_object["camera"]["default_up"]["x"] = default_camera_up.get(1, 1);
	json_object["camera"]["default_up"]["y"] = default_camera_up.get(2, 1);
	json_object["camera"]["default_up"]["z"] = default_camera_up.get(3, 1);

	json_object["camera"]["up"]["x"] = camera_up.get(1, 1);
	json_object["camera"]["up"]["y"] = camera_up.get(2, 1);
	json_object["camera"]["up"]["z"] = camera_up.get(3, 1);

	//json_object["camera"]["rotation"] = std::vector<std::string>{ "x", "y", "z" };
	//json_object["camera"]["rotation"]["y"] = yaw;// *(180 / M_PI);
	//json_object["camera"]["rotation"]["x"] = pitch;// *(180 / M_PI);
	//json_object["camera"]["rotation"]["z"] = roll;// *(180 / M_PI);

	json_object["camera"]["rotation"]["y"] = yaw * (180 / M_PI);// *(180 / M_PI);
	json_object["camera"]["rotation"]["x"] = pitch * (180 / M_PI);// *(180 / M_PI);
	json_object["camera"]["rotation"]["z"] = roll * (180 / M_PI);// *(180 / M_PI);


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

	//json_object["models"] = models;

	for (auto model = models.begin(); model != models.end(); model++) {
		std::vector<std::string> txt_array;
		txt_array.push_back("instances");
		//json_object["models"][*model] = txt_array;

		std::vector<std::string> instance_ids;
		for (size_t instance_id = 0; instance_id < this->total_instances; instance_id++) {
			Instance* instance = Scene::get_instance_ptr(instance_id);
			std::string mesh_filename = instance->mesh->mesh_filename;

			if (mesh_filename == *model) {

				//json_object["models"][*model]["instances"][*instance_name] = std::vector<std::string>{"show", "translation", "rotation", "scale"};

				json_object["models"][*model]["instances"][instance->instance_name]["show"] = instance->show;

				//json_object["models"][*model]["instances"][*instance_name]["translation"] = std::vector<std::string>{ "x", "y", "z" };
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["x"] = instance->tx;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["y"] = instance->ty;
				json_object["models"][*model]["instances"][instance->instance_name]["translation"]["z"] = instance->tz;

				//json_object["models"][*model]["instances"][*instance_name]["rotation"] = std::vector<std::string>{ "x", "y", "z" };
				//json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["y"] = instance->yaw;//* (180 / M_PI);
				//json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["x"] = instance->pitch;//* (180 / M_PI);
				//json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["z"] = instance->roll;// * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["y"] = instance->yaw * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["x"] = instance->pitch * (180 / M_PI);
				json_object["models"][*model]["instances"][instance->instance_name]["rotation"]["z"] = instance->roll * (180 / M_PI);

				//json_object["models"][*model]["instances"][*instance_name]["scale"] = std::vector<std::string>{ "x", "y", "z" };
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

void Scene::load_scene(const char* scenes_folder, const char* scene_filename, const char* models_folder, bool verbose, Mat& default_camera_position, Mat& camera_position, Mat& default_camera_direction, Mat& camera_direction, Mat& default_camera_up, Mat& camera_up, double& camera_yaw, double& camera_pitch, double& camera_roll, Mat& TRANSLATION_MATRIX, Mat& ROTATION_MATRIX, Mat& SCALING_MATRIX, Mat& VIEW_MATRIX, bool use_scene_camera_settings) {
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

	/*
	default_camera_direction.normalize();
	view_camera_direction.normalize();

	Mat old_direction = camera_direction;
	old_direction.normalize();

	Mat rotation_axis = Engine::CrossProduct3D(old_direction, default_camera_direction);
	double rotation_angle = acos(Mat::dot(old_direction, default_camera_direction));

	Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

	default_camera_yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z), 1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));
	default_camera_pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));

	if (default_camera_yaw < 0) default_camera_yaw = 2 * M_PI + default_camera_yaw;
	if (default_camera_pitch < 0) default_camera_pitch = 2 * M_PI + default_camera_pitch;
	*/


	/*
	default_camera_yaw = atan2(2 * ((rotation.x * rotation.z) + (rotation.y * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.y + rotation.y))));
	default_camera_pitch = asin(2 * ((rotation.x * rotation.w) - (rotation.y * rotation.z)));
	default_camera_roll = atan2(2 * ((rotation.x * rotation.y) + (rotation.z * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.z + rotation.z))));;
	*/

	//rotation_axis = Mat({ {0}, {1}, {0} }, 3, 1);

	//std::cout << "Rotation axis: "; rotation_axis.print();
	//std::cout << "Rotation angle: " << rotation_angle << std::endl;

	
	//std::cout << "Qw: " << rotation.w << " | Qi: " << rotation.x << " | Qj: " << rotation.y << " | Qk: " << rotation.z << std::endl;

	//exit(-1);

	//default_camera_yaw = asin(2 * ((rotation.w * rotation.y) - (rotation.z * rotation.x)));
	//default_camera_pitch = atan2(2 * ((rotation.x * rotation.w) + (rotation.y * rotation.z)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.y + rotation.y))));
	//default_camera_roll = atan2(2 * ((rotation.x * rotation.y) + (rotation.z * rotation.w)), 1 - (2 * ((rotation.y * rotation.y) + (rotation.z + rotation.z))));


	//std::cout << "Y: " << default_camera_yaw << std::endl;
	//std::cout << "P: " << default_camera_pitch << std::endl;
	//std::cout << "R: " << default_camera_roll << std::endl;
	

	//exit(-1);

	// Reset view matrix
	VIEW_MATRIX = Mat::identity_matrix(4);

	// If the camera gets scaled, it happens here
	// GET AND STORE THE CAMERA SCALING PARAMETERS
	// DO THE OPPOSITE TO THE VIEW MATRIX

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

	// Translate camera to user specified position (relative to the default position)
	TRANSLATION_MATRIX = Engine::translation_matrix(-camera_position.get(1, 1), -camera_position.get(2, 1), -camera_position.get(3, 1));

	/*
	if (this->scene_data["camera"].contains("default_direction")) {
		double x = this->scene_data["camera"]["default_direction"]["x"].get<double>();
		double y = this->scene_data["camera"]["default_direction"]["y"].get<double>();
		double z = this->scene_data["camera"]["default_direction"]["z"].get<double>();

		default_camera_direction.set(x, 1, 1);
		default_camera_direction.set(y, 2, 1);
		default_camera_direction.set(z, 3, 1);
	}
	*/



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

			//double correct_yaw = atanf(camera_direction.get(3, 1) / camera_direction.get(1, 1)) + (M_PI / 2);
			//double correct_pitch = atanf((camera_direction.get(2, 1)) / (camera_direction.get(3, 1)));
			// 
			// 
			// 
			//double correct_yaw = atan2f(camera_direction.get(3, 1), camera_direction.get(1, 1)) + (M_PI / 2);
			//double correct_pitch = atan2f(camera_direction.get(2, 1), camera_direction.get(3, 1)) + M_PI;

			// If no rotation parameters were given, derive them from the direction vector
			if (!rotation_given) {
				Engine::Quaternion_GetAnglesFromDirectionYP(default_camera_direction, camera_direction, camera_yaw, camera_pitch, camera_roll);
				Engine::rotateY(tmp_dir, camera_yaw);
				Engine::rotateX(tmp_dir, camera_pitch);
				std::cout << "1 Yaw: " << camera_yaw << std::endl;
				std::cout << "1 Pitch: " << camera_pitch << std::endl;
				std::cout << "1 Roll: " << camera_roll << std::endl;

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/
			}

			/*
			default_camera_direction.normalize();



			new_direction.normalize();

			Mat rotation_axis = Engine::CrossProduct3D(default_camera_direction, new_direction);
			double rotation_angle = acos(Mat::dot(default_camera_direction, new_direction));

			Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

			camera_yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z), 1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));
			camera_pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));
			camera_roll = 0;

			if (camera_yaw < 0) camera_yaw = 2 * M_PI + camera_yaw;
			if (camera_pitch < 0) camera_pitch = 2 * M_PI + camera_pitch;
			*/

			/*
			default_camera_yaw = atan2(2 * ((rotation.x * rotation.z) + (rotation.y * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.y + rotation.y))));
			default_camera_pitch = asin(2 * ((rotation.x * rotation.w) - (rotation.y * rotation.z)));
			default_camera_roll = atan2(2 * ((rotation.x * rotation.y) + (rotation.z * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.z + rotation.z))));;
			*/

			//camera_yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z), 1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));
			//camera_pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));
			//camera_roll = 0;



			/*
			double correct_yaw = atan2f(-camera_direction.get(3, 1), camera_direction.get(1, 1)) - (M_PI / 2);
			double correct_pitch = atan2f(camera_direction.get(2, 1), -camera_direction.get(3, 1));

			if (correct_yaw < 0) correct_yaw += (2 * M_PI);
			if (correct_pitch < 0) correct_pitch += (2 * M_PI);

			if (correct_yaw >= 2 * M_PI) correct_yaw = fmod(correct_yaw, 2 * M_PI);
			if (correct_pitch >= 2 * M_PI) correct_pitch = fmod(correct_pitch, 2 * M_PI);

			camera_yaw = Utils::round_to(correct_yaw, 3);
			camera_pitch = Utils::round_to(correct_pitch, 3);

			double one_degree_in_radians = 1 * (M_PI / 180);

			// If yaw is roughly equal to PI, and yaw is roughly equal to pitch, set pitch equal to 0 in order to avoid gimbal lock
			if (abs(camera_yaw - Utils::round_to(M_PI, 3)) <= one_degree_in_radians && abs(camera_yaw - camera_pitch) == one_degree_in_radians) {
				camera_pitch = 0;
				camera_yaw = Utils::round_to(M_PI, 3);
			}
			*/


			/*
			if (this->camera_pitch == 0) {
				this->camera_pitch = correct_pitch;
			}
			else if (this->camera_pitch != correct_pitch) {
				throw std::runtime_error("Error: Camera direction vector does not correspond to the given pitch.");
			}

			if (this->camera_yaw == 0) {
				this->camera_yaw = correct_yaw;
			}
			else if (this->camera_yaw != correct_yaw) {
				throw std::runtime_error("Error: Camera direction vector does not correspond to the given yaw.");
			}
			*/
		}
		
		// If direction was not given, but rotation was - rotates the default camera direction vectors by the rotation parameters
		else if (rotation_given) {
			/*
			Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), camera_roll);
			Mat rotation = rotationZ.get_rotationmatrix();
			camera_up = rotation * camera_up;
			camera_right = rotation * camera_right;
			*/

			/*
			camera_direction = Quaternion::RotatePoint(camera_direction, camera_up, camera_yaw);
			camera_right = Quaternion::RotatePoint(camera_right, camera_up, camera_yaw);
			camera_direction = Quaternion::RotatePoint(camera_direction, camera_right, camera_pitch);
			camera_up = Quaternion::RotatePoint(camera_up, camera_right, camera_pitch);
			*/

			Quaternion rotationY = Quaternion::AngleAxis(camera_up.get(1, 1), camera_up.get(2, 1), camera_up.get(3, 1), camera_yaw);
			Mat rotation = rotationY.get_rotationmatrix();
			camera_direction = rotation * camera_direction;
			//camera_right = rotation * camera_right;
			camera_right = Engine::CrossProduct3D(camera_up, camera_direction);

			Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), camera_pitch);
			rotation = rotationX.get_rotationmatrix();

			camera_direction = rotation * camera_direction;

			//camera_up = Engine::CrossProduct3D(camera_direction, camera_right);
			camera_up = rotation * camera_up;

			Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), camera_roll);
			rotation = rotationZ.get_rotationmatrix();
			camera_up = rotation * camera_up;


			//camera_right = Engine::CrossProduct3D(camera_up, camera_direction);

			/*

			Quaternion rotationZ = Quaternion::AngleAxis(camera_direction.get(1, 1), camera_direction.get(2, 1), camera_direction.get(3, 1), camera_roll);
			rotation = rotationZ.get_rotationmatrix();

			camera_up = rotation * camera_up;

			//camera_up = Engine::CrossProduct3D(camera_direction, camera_right);
			// 
			camera_right = Engine::CrossProduct3D(camera_up, camera_direction);
			//camera_up = rotation * camera_up;
			*/

			//camera_up = Engine::CrossProduct3D(camera_direction, camera_right);



			//Mat cam_x_axis = Engine::CrossProduct3D(camera_up, camera_direction);





			//Engine::rotateX(camera_direction, camera_pitch);
			//Engine::rotateY(camera_direction, camera_yaw);
			
		}

		if (this->scene_data["camera"].contains("up")) {
			double x = this->scene_data["camera"]["up"]["x"].get<double>();
			double y = this->scene_data["camera"]["up"]["y"].get<double>();
			double z = this->scene_data["camera"]["up"]["z"].get<double>();

			camera_up.set(x, 1, 1);
			camera_up.set(y, 2, 1);
			camera_up.set(z, 3, 1);

			//double correct_yaw = atanf(camera_direction.get(3, 1) / camera_direction.get(1, 1)) + (M_PI / 2);
			//double correct_pitch = atanf((camera_direction.get(2, 1)) / (camera_direction.get(3, 1)));
			// 
			// 
			// 
			//double correct_yaw = atan2f(camera_direction.get(3, 1), camera_direction.get(1, 1)) + (M_PI / 2);
			//double correct_pitch = atan2f(camera_direction.get(2, 1), camera_direction.get(3, 1)) + M_PI;

			if (!rotation_given) {
				//Engine::Quaternion_GetAnglesFromDirectionYR(default_camera_up, camera_up, camera_yaw, camera_pitch, camera_roll);
				Engine::GetRoll(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				std::cout << "2 Yaw: " << camera_yaw << std::endl;
				std::cout << "2 Pitch: " << camera_pitch << std::endl;
				std::cout << "2 Roll: " << camera_roll << std::endl;

				Engine::rotateX(tmp_up, camera_pitch);
				Engine::rotateZ(tmp_up, camera_roll);
				
				

				//Engine::DoIt(camera_direction, camera_up, camera_yaw, camera_pitch, camera_roll);
				/*
				std::cout << "Old: " << camera_roll << std::endl;
				Mat cam_right = Engine::CrossProduct3D(camera_up, camera_direction);
				cam_right.normalize();
				Mat projUP = default_camera_up - camera_direction * Mat::dot(default_camera_up, camera_direction);
				projUP.normalize();

				camera_roll = atan2f(Mat::dot(cam_right, projUP), Mat::dot(camera_up, projUP));
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				camera_roll = fmod((camera_roll + 2 * M_PI), 2 * M_PI);
				std::cout << "New: " << camera_roll << std::endl;
				*/
			}

			/*
			default_camera_direction.normalize();



			new_direction.normalize();

			Mat rotation_axis = Engine::CrossProduct3D(default_camera_direction, new_direction);
			double rotation_angle = acos(Mat::dot(default_camera_direction, new_direction));

			Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

			camera_yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z), 1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));
			camera_pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));
			camera_roll = 0;

			if (camera_yaw < 0) camera_yaw = 2 * M_PI + camera_yaw;
			if (camera_pitch < 0) camera_pitch = 2 * M_PI + camera_pitch;
			*/

			/*
			default_camera_yaw = atan2(2 * ((rotation.x * rotation.z) + (rotation.y * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.y + rotation.y))));
			default_camera_pitch = asin(2 * ((rotation.x * rotation.w) - (rotation.y * rotation.z)));
			default_camera_roll = atan2(2 * ((rotation.x * rotation.y) + (rotation.z * rotation.w)), 1 - (2 * ((rotation.x * rotation.x) + (rotation.z + rotation.z))));;
			*/

			//camera_yaw = atan2(2 * (rotation.w * rotation.y + rotation.x * rotation.z), 1 - 2 * (rotation.y * rotation.y + rotation.x * rotation.x));
			//camera_pitch = asin(2 * (rotation.w * rotation.x - rotation.z * rotation.y));
			//camera_roll = 0;



			/*
			double correct_yaw = atan2f(-camera_direction.get(3, 1), camera_direction.get(1, 1)) - (M_PI / 2);
			double correct_pitch = atan2f(camera_direction.get(2, 1), -camera_direction.get(3, 1));

			if (correct_yaw < 0) correct_yaw += (2 * M_PI);
			if (correct_pitch < 0) correct_pitch += (2 * M_PI);

			if (correct_yaw >= 2 * M_PI) correct_yaw = fmod(correct_yaw, 2 * M_PI);
			if (correct_pitch >= 2 * M_PI) correct_pitch = fmod(correct_pitch, 2 * M_PI);

			camera_yaw = Utils::round_to(correct_yaw, 3);
			camera_pitch = Utils::round_to(correct_pitch, 3);

			double one_degree_in_radians = 1 * (M_PI / 180);

			// If yaw is roughly equal to PI, and yaw is roughly equal to pitch, set pitch equal to 0 in order to avoid gimbal lock
			if (abs(camera_yaw - Utils::round_to(M_PI, 3)) <= one_degree_in_radians && abs(camera_yaw - camera_pitch) == one_degree_in_radians) {
				camera_pitch = 0;
				camera_yaw = Utils::round_to(M_PI, 3);
			}
			*/


			/*
			if (this->camera_pitch == 0) {
				this->camera_pitch = correct_pitch;
			}
			else if (this->camera_pitch != correct_pitch) {
				throw std::runtime_error("Error: Camera direction vector does not correspond to the given pitch.");
			}

			if (this->camera_yaw == 0) {
				this->camera_yaw = correct_yaw;
			}
			else if (this->camera_yaw != correct_yaw) {
				throw std::runtime_error("Error: Camera direction vector does not correspond to the given yaw.");
			}
			*/
		}
		else {
			//Mat cam_x_axis = Engine::CrossProduct3D(camera_up, camera_direction);

			/*
			Quaternion rotationX = Quaternion::AngleAxis(camera_right.get(1, 1), camera_right.get(2, 1), camera_right.get(3, 1), camera_pitch);
			Mat rotation = rotationX.get_rotationmatrix();
			camera_up = rotation * camera_up;
			*/

			

			

			//Engine::rotateX(camera_up, camera_pitch);
			//Engine::rotateZ(camera_up, camera_roll);
		}

		

		/*
		// Loads scene camera parameters (position, direction, and orientation)
		if (this->scene_data["camera"].contains("direction")) {
			double x = this->scene_data["camera"]["direction"]["x"].get<double>();
			double y = this->scene_data["camera"]["direction"]["y"].get<double>();
			double z = this->scene_data["camera"]["direction"]["z"].get<double>();

			camera_direction.set(x, 1, 1);
			camera_direction.set(y, 2, 1);
			camera_direction.set(z, 3, 1);
		}
		*/

		//VIEW_MATRIX = Engine::rotationY_matrix(-default_camera_yaw) * VIEW_MATRIX;
		//VIEW_MATRIX = Engine::rotationX_matrix(-default_camera_pitch) * VIEW_MATRIX;
		//VIEW_MATRIX = Engine::rotationZ_matrix(-default_camera_roll) * VIEW_MATRIX;

		// Rotate camera direction vector as per the given orientation in pitch and yaw (we need to rotate the camera direction (starts as (0, 0, 1)) into the default camera direction (0, 0, -1)
		//camera_direction = default_camera_direction;






		/*
		camera_yaw = fmod(camera_yaw + default_camera_yaw, 2 * M_PI);
		camera_pitch = fmod(camera_pitch + default_camera_pitch, 2 * M_PI);
		camera_roll = fmod(camera_roll + default_camera_roll, 2 * M_PI);

		camera_direction = default_camera_direction;
		*/

		/*
		camera_direction = default_camera_direction;
		camera_up = default_camera_up;

		Engine::rotateY(camera_direction, camera_yaw);
		Engine::rotateX(camera_direction, camera_pitch);

		Engine::rotateX(camera_up, camera_yaw);
		Engine::rotateZ(camera_up, camera_roll);
		*/
	}

	std::cout << "Camera position: " << std::endl;
	camera_position.print();

	std::cout << "Yaw: " << camera_yaw << std::endl;
	std::cout << "Pitch: " << camera_pitch << std::endl;
	std::cout << "Roll: " << camera_roll << std::endl;

	std::cout << "Camera direction: " << std::endl;
	camera_direction.print();

	/*
	if (this->scene_data["camera"].contains("up")) {
		double up_x = this->scene_data["camera"]["up"]["x"].get<double>();
		double up_y = this->scene_data["camera"]["up"]["y"].get<double>();
		double up_z = this->scene_data["camera"]["up"]["z"].get<double>();

		camera_up.set(up_x, 1, 1);
		camera_up.set(up_y, 2, 1);
		camera_up.set(up_z, 3, 1);
	}
	*/

	std::cout << "Camera up: ";
	camera_up.print();
	std::cout << std::endl;


	//Engine::rotateZ(tmp_dir, camera_roll);
	
	//Engine::rotateY(tmp_up, camera_yaw);


	std::cout << "RERO DIR: " << std::endl;
	tmp_dir.print();

	std::cout << "RERO UP: " << std::endl;
	tmp_up.print();

	tmp_dir = default_camera_direction;
	tmp_up = default_camera_up;

	tmp_dir = Quaternion::RotatePoint(tmp_dir, tmp_up, camera_yaw);
	Mat tmp_right = Engine::CrossProduct3D(tmp_up, tmp_dir);
	tmp_dir = Quaternion::RotatePoint(tmp_dir, tmp_right, camera_pitch);
	tmp_up = Quaternion::RotatePoint(tmp_up, tmp_right, camera_pitch);
	tmp_up = Quaternion::RotatePoint(tmp_up, tmp_dir, camera_roll);

	std::cout << "RERO DIR 2 : " << std::endl;
	tmp_dir.print();

	std::cout << "RERO UP 2 : " << std::endl;
	tmp_up.print();

	camera_direction = tmp_dir;
	camera_up = tmp_up;

	//exit(-1);

	Engine::rotateZ(ROTATION_MATRIX, -camera_roll);
	Engine::rotateX(ROTATION_MATRIX, -camera_pitch);
	Engine::rotateY(ROTATION_MATRIX, -camera_yaw);

	//VIEW_MATRIX =  SCALING_MATRIX * ROTATION_MATRIX * TRANSLATION_MATRIX;
	VIEW_MATRIX = Engine::LookAt(camera_position, camera_direction, camera_up);
	std::cout << "View matrix: " << std::endl;
	VIEW_MATRIX.print();
	//std::cout << "TRANSLATION: " << std::endl;
	//TRANSLATION_MATRIX.print();
	//std::cout << "ROTATION: " << std::endl;
	//ROTATION_MATRIX.print();
	//std::cout << "SCALE: " << std::endl;
	//SCALING_MATRIX.print();
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

				// The translation matrix holds the inverse translation from the perspective of the camera
				translation = Engine::translation_matrix(translation_x, translation_y, translation_z);
				//printf("Translation (X, Y, Z): %f, %f, %f\n", translation_x, translation_y, translation_z);
			}

			// Checks if scale has been given for this instance, updates scale matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("scale")) {
				has_scaling = true;
				scale_x = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["x"];
				scale_y = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["y"];
				scale_z = this->scene_data["models"][model_filename]["instances"][instance_name]["scale"]["z"];

				scale = Engine::scale_matrix(scale_x, scale_y, scale_z);
				//printf("Scale (X, Y, Z): %f, %f, %f\n", scale_x, scale_y, scale_z);
			}

			// Checks if rotation has been given for this instance, updates rotation matrix if so
			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("rotation")) {
				has_rotation = true;
				yaw = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["y"] * (M_PI / 180);
				pitch = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["x"] * (M_PI / 180);
				roll = this->scene_data["models"][model_filename]["instances"][instance_name]["rotation"]["z"] * (M_PI / 180);

				//rotation = Engine::rotationZ_matrix(rotation_z) * Engine::rotationX_matrix(rotation_x) * Engine::rotationY_matrix(rotation_y);
				//rotation = Engine::quaternion_rotationZ_matrix(rotation_z) * Engine::quaternion_rotationX_matrix(rotation_x) * Engine::quaternion_rotationY_matrix(rotation_y);


				orientation = Quaternion::FromYawPitchRoll(yaw, pitch, roll);
				rotation = orientation.get_rotationmatrix();
				//Engine::rotateY(rotation, yaw);
				//Engine::rotateX(rotation, pitch);
				//Engine::rotateZ(rotation, roll);
				//printf("Rotation (radians) (X, Y, Z): %f, %f, %f\n\n", rotation_x, rotation_y, rotation_z);
			}

			//std::cout << "Translation matrix " << std::endl; translation.print();
			//std::cout << "Scale matrix" << std::endl;  scale.print();
			//std::cout << "Rotation matrix" << std::endl; rotation.print();

			Mat MODEL_TO_WORLD = Mat::identity_matrix(4);
			bool has_model_to_world = false;

			///*
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

						MODEL_TO_WORLD.print();

						//MODEL_TO_WORLD.print();

						//MODEL_TO_WORLD.print();
						//MODEL_TO_WORLD = MODEL_TO_WORLD.transposed();
						//MODEL_TO_WORLD.print();
						//exit(-1);
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
			//*/

			//MODEL_TO_WORLD = translation * scale * rotation;
			//MODEL_TO_WORLD = translation * rotation * scale;

			bool show = true;

			if (this->scene_data["models"][model_filename]["instances"][instance_name].contains("show")) {
				show = this->scene_data["models"][model_filename]["instances"][instance_name]["show"].get<bool>();
			}

			//std::cout << "Show: " << show << std::endl;

			//std::cout << "Model to world matrix" << std::endl;
			//MODEL_TO_WORLD.print();

			Instance mesh_instance;

			// Loads from model-to-world
			if (!has_translation && !has_scaling && !has_rotation && has_model_to_world) {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], MODEL_TO_WORLD, show, this->total_instances);
			}

			// Loads from parameters
			else if (has_translation || has_scaling || has_rotation) {
				mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], translation, rotation, scale, orientation, yaw, pitch, roll, show, this->total_instances);
			}
			else if (has_model_to_world) {
				
			}
			

			if (this->total_instances == 5) {
				printf("%f %f %f %f %f %f %f %f %f\n", mesh_instance.yaw, mesh_instance.pitch, mesh_instance.roll, mesh_instance.tx, mesh_instance.ty, mesh_instance.tz, mesh_instance.sx, mesh_instance.sy, mesh_instance.sz);

				translation.print();
				rotation.print();
				scale.print();
			}

			//Instance mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], translation_x, translation_y, translation_z, yaw, pitch, roll, scale_x, scale_y, scale_z, show, this->total_instances);

			//Instance mesh_instance = Instance(instance_name, &this->scene_meshes[this->total_meshes - 1], MODEL_TO_WORLD, show, this->total_instances);
			/*
			mesh_instance.tx = translation_x;
			mesh_instance.ty = translation_y;
			mesh_instance.tz = translation_z;

			mesh_instance.yaw = yaw;
			mesh_instance.pitch = pitch;
			mesh_instance.roll = roll;

			mesh_instance.scale_x = scale_x;
			mesh_instance.scale_y = scale_y;
			mesh_instance.scale_z = scale_z;
			*/

			this->scene_instances.push_back(std::move(mesh_instance));

			//scene_meshes.push_back(instance_mesh);

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
	//const uint32_t triangle_count = mesh.total_vertices() / 3;
	//uint32_t total_triangles = 0;
	//std::vector<Triangle> triangles(triangle_count);
	
	for (int face = 0; face < mesh.total_faces(); face++) {

		/*
		std::cout << "NEW" << std::endl;
		std::cout << mesh.faces_indices[face][0] - 1 << ", " << mesh.faces_indices[face][1] - 1 << ", " << mesh.faces_indices[face][2] - 1 << ", " << mesh.faces_indices[face][3] - 1<< std::endl;

		std::cout << mesh.vertices[mesh.faces_indices[face][0] - 1] << std::endl;
		std::cout << mesh.vertices[mesh.faces_indices[face][1] - 1] << std::endl;
		std::cout << mesh.vertices[mesh.faces_indices[face][2] - 1] << std::endl;
		std::cout << mesh.vertices[mesh.faces_indices[face][3] - 1] << std::endl;
		*/
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

		// If face is given as a quad
		if (mesh.faces_indices[face].size() == 4) {
			uint32_t idx_d = mesh.faces_indices[face][3] - 1;

			if (idx_d > total_vertices_count || idx_d < 0) {
				continue;
			}

			vertices[3] = mesh.vertices[idx_d];
		}

		// Draw quad if face is rectangular
		if (mesh.faces_indices[face].size() == 4) {
			//Triangle triangle_a = Triangle(vertices[0], vertices[1], vertices[2]);
			//Triangle triangle_b = Triangle(vertices[0], vertices[2], vertices[3]);
			//triangles.push_back(triangle_a);
			//triangles.push_back(triangle_b);
			//total_triangles++;
			//total_triangles++;
			//triangles[total_triangles++] = triangle_a;
			//triangles[total_triangles++] = triangle_b;
			draw_quad(vertices[0], vertices[1], vertices[2], vertices[3], MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
		}

		// Draw triangle if face is triangular
		else if (mesh.faces_indices[face].size() == 3) {
			//Triangle triangle = Triangle(vertices[0], vertices[1], vertices[2]);
			//triangles.push_back(triangle);
			//total_triangles++;
			//triangles[total_triangles++] = triangle;
			draw_triangle(vertices[0], vertices[1], vertices[2], MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
		}

	}

	/*
	bool clear = false;
	while (!clear) {
		clear = true;
		for (int i = 1; i < triangle_count; i++) {
			Triangle previous_triangle = triangles[i - 1];
			Triangle current_triangle = triangles[i];

			double previous_average_z = (previous_triangle.vertices[0].get(3, 1) + previous_triangle.vertices[1].get(3, 1) + previous_triangle.vertices[2].get(3, 1)) / 3;
			double current_average_z = (current_triangle.vertices[0].get(3, 1) + current_triangle.vertices[1].get(3, 1) + current_triangle.vertices[2].get(3, 1)) / 3;

			if (current_average_z < previous_average_z) {
				triangles[i - 1] = current_triangle;
				triangles[i] = previous_triangle;
				clear = false;
			}
		}
	}



	for (Triangle triangle : triangles) {
		draw_triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color);
	}
	*/

	//iter_swap(v.begin() + position, v.begin() + next_position);
}


void Engine::draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	draw_triangle(v0, v1, v2, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
	draw_triangle(v0, v2, v3, MODEL_TO_WORLD, draw_outline, outline_color, fill, fill_color, shade);
}

void Engine::draw_triangle(Mat v0, Mat v1, Mat v2, const Mat& MODEL_TO_WORLD, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade) {
	//std::vector<Mat> projected_vertices;
	/*

	for (Mat vertex : triangle.vertices) {
		/// 3D to 2D
		// apply fov transformation
		// apply aspect ratio transformation

		//Mat rotated_vertex = ROTATION_MATRIX * vertex;
		Mat rotated_vertex = vertex;

		/// Projected 2D point from normalized coordinates to screen space
		Mat temp_vertex = Mat(
			{
				{rotated_vertex.get(1, 1)},
				{rotated_vertex.get(2, 1)},
				{rotated_vertex.get(3, 1)},
				{1}
			}, 4, 1);

		//Mat projected_vertex = PROJECTION_MATRIX * temp_vertex;

		Mat projected_vertex = temp_vertex;

		Mat projection_matrix = Mat(
			{
				{1/(tan(FOVr / 2)), 0, 0, 0},
				{0, AR * 1/(tan(FOVr / 2)), 0, 0},
				{0, 0, far / (far - near), (far * -near) / (far - near)},
				{0, 0, 1, 0}
			}
		, 4, 4);

		double zb = projected_vertex.get(3, 1);

		projected_vertex = projection_matrix * projected_vertex;

		double za = projected_vertex.get(3, 1);

		//std::cout << "ZB: " << zb << " | ZA: " << za << std::endl;

		// Perspective divide
		// Also maps the max Z limit from far to 1 (range was [0, far], now becomes [0, 1])
		if (projected_vertex.get(4, 1) != 0) {
			//projected_vertex /= this->near * projected_vertex.get(4, 1);

			//std::cout << "Original Z: " << projected_vertex.get(4, 1) << std::endl;
			//std::cout << "New Z: " << projected_vertex.get(3, 1) << std::endl;
			projected_vertex /= projected_vertex.get(4, 1);
			//projected_vertex.set(this->near * projected_vertex.get(1, 1), 1, 1);
			//projected_vertex.set(this->near * projected_vertex.get(2, 1), 2, 1);
			za = projected_vertex.get(3, 1);

			//std::cout << "Normalized Z: " << za << std::endl;
		}

		try {
			projected_vertex.set(
				Utils::normalize(projected_vertex.get(1, 1), x_min, x_max, -1, 1),
				1, 1
			);

			projected_vertex.set(
				Utils::normalize(projected_vertex.get(2, 1), y_min, y_max, -1, 1),
				2, 1
			);

			projected_vertex.set(
				Utils::normalize(projected_vertex.get(3, 1), z_min, z_max, 0, 1),
				3, 1
			);

			//projected_vertex.print();
		}
		catch(...) {}

		//projected_vertex.set(projected_vertex.get(1, 1) / projected_vertex.get(3, 1), 1, 1);
		//projected_vertex.set(projected_vertex.get(2, 1) / projected_vertex.get(3, 1), 2, 1);

		temp_vertex = Mat({
			{projected_vertex.get(1, 1)},
			{projected_vertex.get(2, 1)},
			{1}
			}, 3, 1);

		projected_vertex = SCALE_MATRIX * temp_vertex;
		projected_vertices.push_back(projected_vertex);
		//projected_vertices.push_back(PROJECTION_MATRIX * vertex);
	}

	draw_line(projected_vertices[0].get(1, 1), projected_vertices[0].get(2, 1), projected_vertices[1].get(1, 1), projected_vertices[1].get(2, 1));

	draw_line(projected_vertices[1].get(1, 1), projected_vertices[1].get(2, 1), projected_vertices[2].get(1, 1), projected_vertices[2].get(2, 1));

	draw_line(projected_vertices[2].get(1, 1), projected_vertices[2].get(2, 1), projected_vertices[0].get(1, 1), projected_vertices[0].get(2, 1));

	*/

	/*
	double v0_originalx = v0.get(1, 1);
	double v1_originalx = v1.get(1, 1);
	double v2_originalx = v2.get(1, 1);

	double v0_originaly = v0.get(2, 1);
	double v1_originaly = v1.get(2, 1);
	double v2_originaly = v2.get(2, 1);

	double v0_originalz = v0.get(3, 1);
	double v1_originalz = v1.get(3, 1);
	double v2_originalz = v2.get(3, 1);
	*/

	//Clip after projection, but before perspective divide ?
	// Clip at near and far planes
	// Should be based on lines (two connecting vertices) instead of an entire face, and only clip part of the line if 
	// the line is not entirelly outside the view
		// Check if in view frustum, not only front or behind
	/*
	Plane -> y = a(-z - k)
	a == big constant number theoretically approaching +-infinity
	k == constant translation in the positive z axis (where the plane sits) (should it be -z - k instead, since z points in the opposite direction here?) / Near and far plane's positions
	*/

	/*
	// If none of the vertices are within the near and far planes
	if (!v0_valid && !v1_valid && !v2_valid) return;

	// If at least one of the vertices is not within the near and far planes, clip to near or far plane
	if (!(v0_valid && v1_valid && v2_valid)) {

		// Make triangle (v0 valid)
		if (v0_valid && !v1_valid && !v2_valid) {

			// If behind near plane, clip to near plane
			if (v1_w < 0) {

			}

			// If after far plane, clip to far plane
			else {

			}
		}

		// (v1 valid)
		else if (!v0_valid && v1_valid && !v2_valid) {

		}

		// (v2 valid)
		else if (!v0_valid && !v1_valid && v2_valid) {

		}

		// Make quad (v0, v1 valid)
		else if (v0_valid && v1_valid && !v2_valid) {

		}

		// (v1, v2 valid)
		else if (!v0_valid && v1_valid && v2_valid) {

		}

		// (v0, v2 valid)
		else if (v0_valid && !v1_valid && v2_valid) {

		}
	}
	*/

	const static Mat FlipZ = Mat(
		{ 
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}
	, 4, 4);

	Mat world_v0 = MODEL_TO_WORLD * v0;
	Mat world_v1 = MODEL_TO_WORLD * v1;
	Mat world_v2 = MODEL_TO_WORLD * v2;


	v0 =  this->VIEW_MATRIX * FlipZ * MODEL_TO_WORLD * v0;
	v1 =  this->VIEW_MATRIX * FlipZ * MODEL_TO_WORLD * v1;
	v2 =  this->VIEW_MATRIX * FlipZ * MODEL_TO_WORLD * v2;

	// MODEL SPACE -> WORLD SPACE ->  NOW IN CAMERA SPACE

	if (abs(v0.get(3, 1)) > this->far || abs(v1.get(3, 1)) > this->far || abs(v2.get(3, 1)) > this->far) {
		return;
	}

	// Flip the Z coordinate of each vertex as I use a right handed coordinate system in which the "forward" direction is in the -Z
	// At this point it is a regular right handed coordinate system, with positive x axis to the right, positive y axis up, and positive z axis pointing outwards from the camera
	// Should include this z flip in the view matrix?

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
	//double xright = (abs(new_near) * tan(FOVr / 2)) * 0.99;


	/*
	std::cout << "Right plane point: " << (abs(new_near) * tan(FOVr / 2)) << std::endl;
	std::cout << "Just before the right plane point: " << (abs(new_near) * tan(FOVr / 2)) * 0.9983 << std::endl;
	std::cout << "Difference: " <<  ((abs(new_near) * tan(FOVr / 2))) - ((abs(new_near) * tan(FOVr / 2)) * 0.9983) << std::endl;
	exit(-1);
	*/
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


	/*
	static const Mat left_plane_point = Mat(
		{
			{0},
			{0},
			{0},
			{0}
		}
	, 4, 1);

	static const Mat right_plane_point = Mat(
		{
			{(double)(Engine::WIDTH - 1)},
			{0},
			{0},
			{0}
		}
	, 4, 1);

	static const Mat top_plane_point = Mat(
		{
			{0},
			{0},
			{0},
			{0}
		}
	, 4, 1);

	static const Mat bottom_plane_point = Mat(
		{
			{0},
			{(double)(Engine::HEIGHT - 1)},
			{0},
			{0}
		}
	, 4, 1);
	*/



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

			{
				/*
				if (n_clipped_triangle == 0) {
					Triangle tst = sub_clipped_triangles[0];
					Mat v0 = tst.vertices[0];
					Mat v1 = tst.vertices[1];
					Mat v2 = tst.vertices[2];

					bool cull = false;
					Mat vec_a = v0 - v1;
					vec_a.normalize();
					Mat vec_b = v2 - v1;
					vec_b.normalize();
					Mat triangle_normal = Engine::CrossProduct3D(vec_a, vec_b);
					triangle_normal.normalize();

					Mat middle_point = v1 + (0.5 * (v0 - v1)) + (0.5 * (v2 - v1));

					Mat eye_to_triangle_dist = middle_point - camera_position;

					camera_position.print();
					middle_point.print();
					eye_to_triangle_dist.normalize();
					eye_to_triangle_dist.print();
					triangle_normal.print();
					v0.print();
					v1.print();
					v2.print();
					//double correlation = Mat::dot(triangle_normal, eye_to_triangle_dist);
					double correlation = Mat::dot(triangle_normal, camera_direction / camera_direction.norm());
					if (correlation >= 0) {
						cull = true;
					}
					std::cout << "Cull? " << cull << std::endl;
				}
				*/

			}

			for (Triangle current_triangle : sub_clipped_triangles) {
				Mat v0 = current_triangle.vertices[0];
				Mat v1 = current_triangle.vertices[1];
				Mat v2 = current_triangle.vertices[2];

				bool visible = true;

				// Also flips the y axis, so now the positive x axis still points to the right, the positive z axis points into the screen, and the positive y axis points down (basically a right-handed coordinate system rotated counter-clockwise 180 degrees around the x axis, so a pitch/vertical rotation)

				// This projection matrix's z coordinate gets multiplied by the `far` plane value
				// So the range, after the projection, of the `z` coordinate is (0, far), not (0, 1)
				// This is so that after the perspective divide, which happens in the sequence, it becomes (0, 1), already including it

				const static Mat FlipY = Mat(
					{
						{1, 0, 0, 0},
						{0, 1, 0, 0},
						{0, 0, 1, 0},
						{0, 0, 0, 1}
					}
				, 4, 4);

				v0 = this->PROJECTION_MATRIX * v0;
				v1 = this->PROJECTION_MATRIX * v1;
				v2 = this->PROJECTION_MATRIX * v2;

				// NOW IN CLIP SPACE SOME GRAPHICS LIBRARIES

				double v0_originalz = v0.get(4, 1);
				double v1_originalz = v1.get(4, 1);
				double v2_originalz = v2.get(4, 1);

				// Cull triangle if all vertices are further than the far plane
				//if (v0_originalz > this->far && v1_originalz > this->far && v2_originalz > this->far) continue;



				// Perspective divide
				v0 /= v0_originalz;
				v1 /= v1_originalz;
				v2 /= v2_originalz;


				bool cull = false;
				Mat vec_a = v0 - v1;
				vec_a.normalize();
				Mat vec_b = v2 - v1;
				vec_b.normalize();
				Mat triangle_normal = Engine::CrossProduct3D(vec_a, vec_b);
				triangle_normal.normalize();

				//Mat middle_point = v1 + (0.5 * (v0 - v1)) + (0.5 * (v2 - v1));

				//Mat eye_to_triangle_dist = middle_point - camera_position;
				Mat eye_to_triangle_dist = v1 - (camera_position);
				eye_to_triangle_dist.normalize();
				//double correlation = Mat::dot(triangle_normal, eye_to_triangle_dist);
				double correlation = Mat::dot(triangle_normal, camera_direction);
				if (correlation >= 0) {
					cull = true;
					continue;
				}


				if (shade) {
					Mat vec_a = v0 - v1;
					vec_a.normalize();
					Mat vec_b = v2 - v1;
					vec_b.normalize();
					Mat triangle_normal = Engine::CrossProduct3D(vec_a, vec_b);
					triangle_normal.normalize();
					Mat normalized_light_source = light_source_dir / light_source_dir.norm();
					Mat dist_vertex_to_light = v0 - light_source_pos;
					//double distance = abs(Engine::PointDistanceToPlane(&v0, &light_source_pos, &normalized_light_source));
					double distance = sqrt(pow(world_v0.get(1, 1) - light_source_pos.get(1, 1), 2) + pow(world_v0.get(2, 1) - light_source_pos.get(2, 1), 2) + pow(world_v0.get(3, 1) - light_source_pos.get(3, 1), 2));
					//double distance = sqrt(pow(v0.get(1, 1) - light_source_pos.get(1, 1), 2) + pow(v0.get(2, 1) - light_source_pos.get(2, 1), 2) + pow(v0.get(3, 1) - light_source_pos.get(3, 1), 2));
					double similarity = Mat::dot(normalized_light_source, triangle_normal);

					//std::cout << light_intensity << std::endl;
					if (similarity <= 0 && distance < light_reach) {
						//double light_intensity = (1 / pow(this->light_reach, 2)) * pow(distance, 2);

						//double light_intensity = abs((distance - light_reach)) * -similarity;
						//double light_intensity = -similarity;
						double light_intensity = -similarity;
						double distance_multiplier = -pow(1 / light_reach * distance, 1.5) + 1;

						light_intensity = light_intensity * distance_multiplier;

						uint8_t color = (uint8_t)(light_intensity * 255.0);

						//color /= pow(distance, 1/3);
						fill_color = 0x000000FF | (color << 24) | (color << 16) | (color << 8);
					}
					else {
						visible = false;
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

				// Transformation into screen space coordinates (i.e. 2D x-y)
				// Positive x is to the right
				// Positive y is down (i.e. top is 0 and bottom is window height - 1)
				v0 = this->SCALE_MATRIX * v0;
				v1 = this->SCALE_MATRIX * v1;
				v2 = this->SCALE_MATRIX * v2;

				/*
				v0.set(floor(abs(v0.get(1, 1) - 1)), 1, 1);
				v0.set(floor(abs(v0.get(2, 1) - 1)), 2, 1);

				v1.set(floor(abs(v1.get(1, 1) - 1)), 1, 1);
				v1.set(floor(abs(v1.get(2, 1) - 1)), 2, 1);

				v2.set(floor(abs(v2.get(1, 1) - 1)), 1, 1);
				v2.set(floor(abs(v2.get(2, 1) - 1)), 2, 1);
				*/



				uint8_t alpha = 0xFF;
				uint8_t red = rand() % 0xFF;
				uint8_t blue = rand() % 0xFF;
				uint8_t green = rand() % 0xFF;
				uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;


				outline_color = color;
				//fill_color = color;

				bool is_v0_equal_v1 = v0 == v1;
				bool is_v0_equal_v2 = v0 == v2;
				bool is_v1_equal_v2 = v1 == v2;

				//draw_outline = true;
				//fill = true;

				if (draw_outline) {
					///*

					// Validates pixel being within window boundaries
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
					//*/
					// 
					// Checks whether 2 of the triangle's vertices are the same, and only draws line if that is not the case (avoids drawing the same line more than once)
					if (!is_v0_equal_v1) {
						draw_line(
							v0.get(1, 1), v0.get(2, 1),
							v1.get(1, 1), v1.get(2, 1),
							outline_color
						);
					}
					
					if (!is_v1_equal_v2 && v2 != v0) {
						draw_line(
							v1.get(1, 1), v1.get(2, 1),
							v2.get(1, 1), v2.get(2, 1),
							outline_color
						);
					}

					if (!is_v0_equal_v2 && v0 != v1 && v2 != v1) {
						draw_line(
							v2.get(1, 1), v2.get(2, 1),
							v0.get(1, 1), v0.get(2, 1),
							outline_color
						);
					}
				}

				if ((fill || shade) && visible) {
					// Ignore triangle rasterization if 2 of the vertices are the same (meaning triangle is basically a line, that is already done when the lines are drawn)
					if (is_v0_equal_v1 || is_v0_equal_v2 || is_v1_equal_v2) {
						return; 
					}
					
					fill_triangle(v0, v1, v2, fill_color, shade);
				}
			}
		}
	}
}

void Engine::draw_line(double x1, double y1, double x2, double y2, uint32_t outline_color) {
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

	if (abs(x1 - x2) > 0.5) {
		double dy = (y2 - y1) / (x2 - x1);
		double abs_dx = abs(1 / dy);

		//printf("%f\n", dy);
		double original_x = fmin(x1, x2);
		double y = (original_x == x1) ? y1 : y2;
		//double y = dy > 0 ? fmin(y1, y2) : fmax(y1, y2);

		/*
		if (fabs(x1 - x2) <= 3) {
			printf("x1: %d | y1: %d | x2: %d | y2: %d | dy: %f\n", nx1, ny1, nx2, ny2, dy);
		}
		*/

		/*

		if (dy == 0 && ny1 != ny2) {
			if (original_x == nx1) {
				printf("dy: %f | (%d, %d) -> (%d, %d)\n", (y2 - y1) / (x2 - x1), nx1, ny1, nx2, ny2);
			}
			else {
				printf("dy: %f | (%d, %d) -> (%d, %d)\n", (y2 - y1) / (x2 - x1), nx2, ny2, nx1, ny1);
			}
			
		}

		*/

		// Clip edges of the line if they're out of the screen to optimize (check intersections with screen) x and y must be within screen boundaries
		if (abs(dy) <= abs_dx) {
			uint16_t x = round(fmax(0, original_x));
			if (x != original_x) {
				y += (x - original_x) * dy;
			}

			uint16_t rounded_y = round(y);

			for (x;
				x < (uint16_t) round(fmax(x1, x2)); x++) {

				this->buffer[(WIDTH * rounded_y) + (uint16_t)x] = outline_color;

				/*

				if (x != original_x) {
					if (x > original_x && dy < -1) {
						for (int tmp_y = (y - dy) - 1; tmp_y > y; tmp_y--) {
							if (tmp_y > 0 && tmp_y < HEIGHT && x > 0) this->buffer[((WIDTH * tmp_y) + (uint16_t)x)] = outline_color;
						}
					}
					else if (x > original_x && dy > 1) {
						for (int tmp_y = (y - dy) + 1; tmp_y < y; tmp_y++) {
							if (tmp_y > 0 && tmp_y < HEIGHT && x > 0) this->buffer[((WIDTH * tmp_y) + (uint16_t)x)] = outline_color;
						}
					}
				}
				*/

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

			for (y;
				y != round(target_y); y += dy > 0 ? 1 : -1) {

				this->buffer[(WIDTH * (int16_t)y) + rounded_x] = outline_color;


				/*
				if (x != original_x) {
					if (x > original_x && dy < -1) {
						for (int tmp_y = (y - dy) - 1; tmp_y > y; tmp_y--) {
							if (tmp_y > 0 && tmp_y < HEIGHT && x > 0) this->buffer[((WIDTH * tmp_y) + (uint16_t)x)] = outline_color;
						}
					}
					else if (x > original_x && dy > 1) {
						for (int tmp_y = (y - dy) + 1; tmp_y < y; tmp_y++) {
							if (tmp_y > 0 && tmp_y < HEIGHT && x > 0) this->buffer[((WIDTH * tmp_y) + (uint16_t)x)] = outline_color;
						}
					}
				}
				*/

				x += abs_dx;
				rounded_x = round(x);

			}
		}


	}

	// If a vertical line
	else {
		uint16_t x = round(x1);
		uint16_t y = round(fmax(0, fmin(y1, y2)));
		for (y; y < (uint16_t)round(fmax(y1, y2)); y++) {
			this->buffer[(WIDTH * y) + x] = outline_color;
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

	/*
	// If vectors are (about) unit length and opposite directions
	if (abs(v1.norm() - 1) < 0.01 && abs(v2.norm() - 1) < 0.01) {
		if (v1 == (-1 * v2)) {
			// FIND AND RETURN VECTOR PERPENDICULAR TO THIS LINE
			return Engine::identity_matrix();
		}
		
	}
	*/

	Mat result = Mat(
		{ 
			{(v1_y * v2_z) - (v1_z * v2_y)},
			{(v1_z * v2_x) - (v1_x * v2_z)},
			{(v1_x * v2_y) - (v1_y * v2_x)},
			{0}
		}
	, 4, 1);

	return result;
}

void Engine::fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, uint32_t fill_color, bool shade) {
	/*
	int x1 = v0.get(1, 1);
	int y1 = v0.get(2, 1);

	int x2 = v1.get(1, 1);
	int y2 = v1.get(2, 1);

	int x3 = v2.get(1, 1);
	int y3 = v2.get(2, 1);

	auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) {
		if (i >= 0 && i < WIDTH && ny >= 0 && ny < HEIGHT)
		{
			buffer[ny * WIDTH + i] = fill_color;
		}
		}
	};

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		SWAP(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
		// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}


	return;
	*/
	// V0 has to be swapped with V1 or V2, why does the order matter?
	Mat v0a = v0;
	Mat v1a = v1;
	Mat v2a = v2;

	double v0_x = v0a.get(1, 1);
	double v0_y = v0a.get(2, 1);

	double v1_x = v1a.get(1, 1);
	double v1_y = v1a.get(2, 1);

	double v2_x = v2a.get(1, 1);
	double v2_y = v2a.get(2, 1);

	/*
	double min_x = fmin(fmin(v0_x, v1_x), v2_x);
	double min_y = fmin(fmin(v0_y, v1_y), v2_y);
	if (min_y != v0_y) {
		if (min_y == v1_y) {
			double old_v0_x = v0_x;
			double old_v0_y = v0_y;
			v0_x = v1_x;
			v0_y = v1_y;
			v1_x = old_v0_x;
			v1_y = old_v0_y;
		}
		else if (min_y == v2_y) {
			double old_v0_x = v0_x;
			double old_v0_y = v0_y;
			v0_x = v2_x;
			v0_y = v2_y;
			v2_x = old_v0_x;
			v2_y = old_v0_y;
		}
	}

	if (min_y == v0_y) {
		if (v1_y == v0_y && v1_x > v0_x) {
			double old_v0_x = v0_x;
			v0_x = v1_x;
			v1_x = old_v0_x;
		} else if (v2_y == v0_y && v2_x > v0_x) {
			double old_v0_x = v0_x;
			v0_x = v2_x;
			v2_x = old_v0_x;
		}
	}


	// Swap V1, V2
	if (v2_y < v1_y && v1_x == v2_x) {
		double old_v1_x = v1_x;
		double old_v1_y = v1_y;
		v1_x = v2_x;
		v1_y = v2_y;
		v2_x = old_v1_x;
		v2_y = old_v1_y;
	}
	else if (v2_x < v1_x) {
		double old_v1_x = v1_x;
		double old_v1_y = v1_y;
		v1_x = v2_x;
		v1_y = v2_y;
		v2_x = old_v1_x;
		v2_y = old_v1_y;
	}
	*/


	/*
	if (v0_x < 0) v0_x = 0;
	if (v0_y < 0) v0_y = 0;
	if (v1_x < 0) v1_x = 0;
	if (v1_y < 0) v1_y = 0;
	if (v2_x < 0) v2_x = 0;
	if (v2_y < 0) v2_y = 0;

	if (v0_x >= WIDTH) v0_x = WIDTH - 1;
	if (v1_x >= WIDTH) v1_x = WIDTH - 1;
	if (v2_x >= WIDTH) v2_x = WIDTH - 1;

	if (v0_y >= HEIGHT) v0_y = HEIGHT - 1;
	if (v1_y >= HEIGHT) v1_y = HEIGHT - 1;
	if (v2_y >= HEIGHT) v2_y = HEIGHT - 1;
	*/

	uint16_t bounding_box_left_x = round(fmin(fmin(v0_x, v1_x), v2_x));
	uint16_t bounding_box_right_x = round(fmax(fmax(v0_x, v1_x), v2_x));
	uint16_t bounding_box_top_y = round(fmin(fmin(v0_y, v1_y), v2_y));
	uint16_t bounding_box_bottom_y = round(fmax(fmax(v0_y, v1_y), v2_y));

	const Mat veca = v0 - v1;
	const Mat vecb = v2 - v1;

	double veca_x = veca.get(1, 1);
	double veca_y = veca.get(2, 1);

	double vecb_x = vecb.get(1, 1);
	double vecb_y = vecb.get(2, 1);

	//uint8_t red = a * 0xFF;
	//uint8_t green = b * 0xFF;
	//uint8_t blue = c * 0xFF;

	/*
	Mat vs1 = Mat(
		{
			{(double)v1_x - v0_x},
			{(double)v1_y - v0_y},
			{0},
			{0}
		}
	, 4, 1);

	Mat vs2 = Mat(
		{
			{(double)v2_x - v0_x},
			{(double)v2_y - v0_y},
			{0},
			{0}
		}
	, 4, 1);
	*/


	for (uint16_t pixel_y = bounding_box_top_y; pixel_y < bounding_box_bottom_y; pixel_y++) {
		if (pixel_y >= this->HEIGHT || pixel_y < 0) return;
		for (uint16_t pixel_x = bounding_box_left_x; pixel_x < bounding_box_right_x; pixel_x++) {
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
			//Mat pixel = (a * v1) + (b * v2) + (c * v0) // (1 - alpha - beta )v1 + (beta)v2 + (alpha)v0 // v1 + alpha(v0 - v1) + beta(v2 - v1)

			// How do I deal with colinear vertices? Degenerate triangle, all vertices have the same y. Should lines be represented as a triangle like this?
			// Swap vertices if v2 and v1 have the same Y, since that makes alpha & beta undefined
			/*
			if (v2_y == v1_y) {
				double old_v2_x = v2_x;
				double old_v2_y = v2_y;
				v2_x = v0_x;
				v2_y = v0_y;
				v0_x = old_v2_x;
				v0_y = old_v2_y;
			}
			*/


			/*
			if (v2_y == v0_y) {
				double old_v1_x = v1_x;
				double old_v1_y = v1_y;
				v1_x = v2_x;
				v1_y = v2_y;
				v2_x = old_v1_x;
				v2_y = old_v1_y;
			}
			*/


			// 

			//double alpha = ((v1_x * (v2_y - v1_y)) + ((pixel_y - v1_y) * (v2_x - v1_x)) - (pixel_x * (v2_y - v1_y))) / (((v0_y - v1_y) * (v2_x - v1_x)) - ((v0_x - v1_x) * (v2_y - v1_y)));

			/*
			double alpha = (double)(((pixel_x - v0_x) * (v2_y - v0_y)) - ((pixel_y - v0_y) * (v2_x - v0_x))) / (((v2_y - v0_y) * (v1_x - v0_x)) - ((v1_y - v0_y) * (v2_x - v0_x)));
			double beta = (double)(pixel_y - v0_y - (alpha * (v1_y - v0_y))) / (v2_y - v0_y);

			// NEW
			//double alpha = (((pixel_x - v0_x) * (v2_y - v0_y)) - ((pixel_y - v0_y) * (v2_x - v0_x))) / (((v1_x - v0_x) * (v2_y - v0_y)) - ((v1_y - v0_y) * (v2_x - v0_x)));
			// double beta = (((v1_x - v0_x) * (pixel_y - v0_y)) - ((v1_y - v0_y) * (pixel_x - v0_x))) / (((v1_x - v0_x) * (v2_y - v0_y)) - ((v1_y - v0_y) * (v2_x - v0_x)));

			static Mat point = Mat(
				{
					{1},
					{1},
					{1},
					{1}
				}
			, 4, 1);

			point.set((double)pixel_x, 1, 1);
			point.set((double)pixel_y, 2, 1);

			static Mat q = point;
			q.set(point.get(1, 1) - v0.get(1, 1), 1, 1);
			q.set(point.get(2, 1) - v0.get(2, 1), 2, 1);

			//Mat vs1 = v1 - v0;
			//Mat vs2 = v2 - v0;

			static Mat vs1 = point;
			vs1.set(v1.get(1, 1) - v0.get(1, 1), 1, 1);
			vs1.set(v1.get(2, 1) - v0.get(2, 1), 2, 1);

			static Mat vs2 = point;
			vs2.set(v2.get(1, 1) - v0.get(1, 1), 1, 1);
			vs2.set(v2.get(2, 1) - v0.get(2, 1), 2, 1);
			*/

			//double alpha2 = (double)CrossProduct(q, vs2) / CrossProduct(vs1, vs2);
			//double beta2 = (double)CrossProduct(vs1, q) / CrossProduct(vs1, vs2);


			// NOT WORKING

			//double alpha = (((pixel_x - v0_x) * (v2_y - v0_y)) - ((pixel_y - v0_y) * (v2_x - v0_x))) / (((v2_y - v0_y) * (v1_x - v0_x)) - ((v1_y - v0_y) * (v2_x - v0_x)));
			//double beta = (pixel_y - v0_y - (alpha * (v1_y - v0_y))) / (v2_y - v0_y);

			//double alpha = (((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));
			//double beta = (((v0_x - v1_x) * (pixel_y - v1_y)) - ((v0_y - v1_y) * (pixel_x - v1_x))) / (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));

			double division = (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));

			if (division == 0) {
				throw std::runtime_error("Error: Division by 0 during barycentric coordinates calculation (rasterization).");
			}


			double alpha = (((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / division;
			double beta = (((v0_x - v1_x) * (pixel_y - v1_y)) - ((v0_y - v1_y) * (pixel_x - v1_x))) / division;

			// WORKING
			//double alpha = (((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));
			//double beta = (((v0_x - v1_x) * (pixel_y - v1_y)) - ((v0_y - v1_y) * (pixel_x - v1_x))) / (((v0_x - v1_x) * (v2_y - v1_y)) - ((v0_y - v1_y) * (v2_x - v1_x)));


			/*
			if (abs(alpha - alpha2) > 0.001 || abs(beta - beta2) > 0.001) {
				std::cout << abs(alpha - alpha2) << std::endl;
				std::cout << abs(beta - beta2) << std::endl;
				exit(-1);
			};
			*/

			/*
			double alpha = (double)(((pixel_x - v1_x) * (v2_y - v1_y)) - ((pixel_y - v1_y) * (v2_x - v1_x))) / (((v2_y - v1_y) * (v0_x - v1_x)) - ((v0_y - v1_y) * (v2_x - v1_x)));
			double beta = (double)(pixel_y - v1_y - (alpha * (v0_y - v1_y))) / (v2_y - v1_y);


			Mat point = Mat(
				{
					{(double)pixel_x},
					{(double)pixel_y},
					{1},
					{1}
				}
			, 4, 1);

			Mat q = point - v0;


			Mat vs1 = v1 - v0;
			Mat vs2 = v2 - v0;
			double alpha2 = (double)CrossProduct(q, vs2) / CrossProduct(vs1, vs2);
			double beta2 = (double)CrossProduct(vs1, q) / CrossProduct(vs1, vs2);
			alpha = alpha2;
			beta = beta2;
			*/

			//std::cout << "A: " << alpha << " | " "B: " << beta << std::endl;

			/*
			Mat q = Mat(
				{
					{(double)pixel_x - v0_x},
					{(double) pixel_y - v0_y},
					{0},
					{0}
				}
			, 4, 1);

			double kk = CrossProduct(vs1, vs2);

			alpha = CrossProduct(q, vs2) / kk;
			beta = CrossProduct(vs1, q) / kk;
			*/

			// Checks whether is within triangle/edge
			//if ((pixel_t0 >= 0 && pixel_t1 >= 0) || (pixel_t0 <= 1 && pixel_t1 <= 1)) {

			if (alpha >= 0 && beta >= 0 && alpha + beta <= 1) {
				//std::cout << "Alpha: " << alpha << " | Beta: " << beta << std::endl;
				//return;
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

				if (!shade) {
					uint8_t alpha = 0xFF;
					uint8_t red = c * 0xFF; // v1
					uint8_t green = a * 0xFF; // v0 (when v2 at same height as v1 swaps with v2)
					uint8_t blue = b * 0xFF; // v2 (when at same height as v1 swaps with v0)
					fill_color = (red << 24) | (green << 16) | (blue << 8) | alpha;
				}

				this->buffer[(this->WIDTH * pixel_y) + pixel_x] = fill_color;
				//this->buffer[WIDTH * pixel_y + pixel_x] = 0xFFFFFFFF;
				//this->buffer[WIDTH * pixel_y + pixel_x] = fill_color;
			}
		}
	}

	/*
	static const uint8_t n_partitions = 4;

	uint16_t mini_boxes_width = round((bounding_box_right_x - bounding_box_left_x) / (double) n_partitions);
	uint16_t mini_boxes_height = round((bounding_box_bottom_y - bounding_box_top_y) / (double) n_partitions);

	std::cout << mini_boxes_width << " | " << mini_boxes_height << std::endl;
	*/

	return;
	/*


	Get triangle bounding box (clip it to screen dimensions) 
		Make mini bounding boxes
			For each mini bounding box (clip it to screen dimensions)
				If all edges of the box are outside the triangle, go to the next one
				Otherwise, check all pixels within
		// Go through triangle's bounding box pixels
		for (uint16_t pixel_y = bounding_box_top_y; pixel_row < bounding_box_bottom_y; pixel_y++) {
			for (uint16_t pixel_x = bounding_box_left_x; pixel < bounding_box_right_x; pixel_x++) {
				int8_t pixel_t0 = 0;
				int8_t pixel_t1 = 1;

				// Checks whether is within triangle/edge
				if ((pixel_t0 >= 0 && pixel_t1 >= 0) || (pixel_t0 <= 1 && pixel_t1 <= 1)) {

					// Skips pixel if sits at bottom/right edge
					// Ignore t0 == 0 && t1 == 0 as well as
					if ((t0 == 0 || t1 == 0) && !left_edge && !top_edge)){
						continue;
					}

					Color_pixel(WIDTH * pixel_y + pixel_x);
				}
			}
	}
	

	(Pixel center within the general triangle ? )
		Draw_pixel
		return;

	go from left edge to right(arent barycentric coords redundant here since the pixels in between edges are within the triangle ? )

		interpolate barycentric coordinates instead ? (i.e.go from t_k = 0 to t_k = 1 in 0.01 steps or so ? ) no edge detection here in principle.What about gaps ?
		from t0 = 0 to t0 = 1 at step = 0.01
			from t1 = 0 to t1 = 1 at step = 0.01
				pixel_coord = (1 - t0 - t1)v2 + t0(v2 - v1) + t1(v2 - v0)
				color_pixel(pixel_coord);

	how do i know whether it is a top or left edge? do i need a separate check for that? would be when t0 = 0 or t0 = 1 or t1 = 0 or t1 = 1
	sort vertices by x and y, check if left and top edge and not draw?
					

	return;
	*/

	/*
	(Triangle has singular middle_vertex?)
		Split_general_triangle_into_bottom_and_top_triangles

	(Triangle has top edge ? )
		(Pixel center sits exactly at top edge ? )
			Draw_pixel
			return;
	(Triangle has left edge ? )
		(Pixel center sits exactly at left edge ? )
			Draw_pixel
			return;
	*/

	// Highest vertex will be the vertex closest to y = 0 (also the leftmost vertex if there are 2 at the same height)
	//return;

	/*	

	uint16_t v0_x = v0.get(1, 1);
	uint16_t v0_y = v0.get(2, 1);

	uint16_t v1_x = v1.get(1, 1);
	uint16_t v1_y = v1.get(2, 1);

	uint16_t v2_x = v2.get(1, 1);
	uint16_t v2_y = v2.get(2, 1);

	if (v0_x == v1_x && v0_y == v1_y) {
		return;
	}
	else if (v0_x == v2_x && v0_y == v2_y) {
		return;
	}
	else if (v1_x == v2_x && v1_y == v2_y) {
		return;
	}
	else if (v0_y == v1_y && v0_y == v2_y) {
		return;
	}

	uint16_t highest_leftmost_vertex_x = v0_x;
	uint16_t highest_leftmost_vertex_y = v0_y;

	uint16_t highest_rightmost_vertex_x = v0_x;
	uint16_t highest_rightmost_vertex_y = v0_y;

	uint16_t lowest_leftmost_vertex_x = v0_x;
	uint16_t lowest_leftmost_vertex_y = v0_y;

	uint16_t lowest_rightmost_vertex_x = v0_x;
	uint16_t lowest_rightmost_vertex_y = v0_y;

	uint16_t middle_vertex_x = v0_x;
	uint16_t middle_vertex_y = v0_y;

	if (v1_y > v2_y && v1_y > v0_y && v2_y != v0_y) {
		if (v2_y < v0_y) {
			highest_leftmost_vertex_x = v2_x;
			highest_leftmost_vertex_y = v2_y;

			highest_rightmost_vertex_x = v2_x;
			highest_rightmost_vertex_y = v2_y;
		}

		lowest_leftmost_vertex_x = v1_x;
		lowest_leftmost_vertex_y = v1_y;

		lowest_rightmost_vertex_x = v1_x;
		lowest_rightmost_vertex_y = v1_y;
	} 
	else if (v2_y > v1_y && v2_y > v0_y && v1_y != v0_y) {
		if (v1_y < v0_y) {
			highest_leftmost_vertex_x = v1_x;
			highest_leftmost_vertex_y = v1_y;

			highest_rightmost_vertex_x = v1_x;
			highest_rightmost_vertex_y = v1_y;
		}

		lowest_leftmost_vertex_x = v2_x;
		lowest_leftmost_vertex_y = v2_y;

		lowest_rightmost_vertex_x = v2_x;
		lowest_rightmost_vertex_y = v2_y;
	}
	else if (v1_y == v0_y) {
		if (v1_y > v2_y) {
			if (v1_x > v0_x) {
				lowest_rightmost_vertex_x = v1_x;
				lowest_rightmost_vertex_y = v1_y;
			}
			else if (v1_x < v0_x) {
				lowest_leftmost_vertex_x = v1_x;
				lowest_leftmost_vertex_y = v1_y;
			}

			highest_leftmost_vertex_x = v2_x;
			highest_leftmost_vertex_y = v2_y;
			highest_rightmost_vertex_x = v2_x;
			highest_rightmost_vertex_y = v2_y;
		}
		else {
			if (v1_x > v0_x) {
				highest_rightmost_vertex_x = v1_x;
				highest_rightmost_vertex_y = v1_y;
			}
			else if (v1_x < v0_x) {
				highest_leftmost_vertex_x = v1_x;
				highest_leftmost_vertex_y = v1_y;
			}

			lowest_leftmost_vertex_x = v2_x;
			lowest_leftmost_vertex_y = v2_y;
			lowest_rightmost_vertex_x = v2_x;
			lowest_rightmost_vertex_y = v2_y;
		}

	}
	else if (v2_y == v0_y) {
		if (v2_y > v1_y) {
			if (v2_x > v0_x) {
				lowest_rightmost_vertex_x = v2_x;
				lowest_rightmost_vertex_y = v2_y;
			}
			else if (v2_x < v0_x) {
				lowest_leftmost_vertex_x = v2_x;
				lowest_leftmost_vertex_y = v2_y;
			}

			highest_leftmost_vertex_x = v1_x;
			highest_leftmost_vertex_y = v1_y;
			highest_rightmost_vertex_x = v1_x;
			highest_rightmost_vertex_y = v1_y;
		}
		else {
			if (v2_x > v0_x) {
				highest_rightmost_vertex_x = v2_x;
				highest_rightmost_vertex_y = v2_y;
			}
			else if (v2_x < v0_x) {
				highest_leftmost_vertex_x = v2_x;
				highest_leftmost_vertex_y = v2_y;
			}

			lowest_leftmost_vertex_x = v1_x;
			lowest_leftmost_vertex_y = v1_y;
			lowest_rightmost_vertex_x = v1_x;
			lowest_rightmost_vertex_y = v1_y;
		}
	}
	else if (v2_y == v1_y) {
		if (v2_y > v0_y) {
			if (v2_x > v1_x) {
				lowest_leftmost_vertex_x = v1_x;
				lowest_leftmost_vertex_y = v1_y;
				lowest_rightmost_vertex_x = v2_x;
				lowest_rightmost_vertex_y = v2_y;
			}
			else if (v2_x < v1_x) {
				lowest_leftmost_vertex_x = v2_x;
				lowest_leftmost_vertex_y = v2_y;
				lowest_rightmost_vertex_x = v1_x;
				lowest_rightmost_vertex_y = v1_y;
			}
		}
		else {
			if (v2_x > v1_x) {
				highest_leftmost_vertex_x = v1_x;
				highest_leftmost_vertex_y = v1_y;
				highest_rightmost_vertex_x = v2_x;
				highest_rightmost_vertex_y = v2_y;
			}
			else if (v2_x < v1_x) {
				highest_leftmost_vertex_x = v2_x;
				highest_leftmost_vertex_y = v2_y;
				highest_rightmost_vertex_x = v1_x;
				highest_rightmost_vertex_y = v1_y;
			}
		}

	}

	bool no_vertices_at_same_height = true;
	if (v0_y == v1_y || v0_y == v2_y || v1_y == v2_y) {
		no_vertices_at_same_height = false;
	}
	else {
		middle_vertex_x = v0_x;
		middle_vertex_y = v0_y;

		if (middle_vertex_y < v0_y && middle_vertex_y < v1_y) {
			if (v1_y < v0_y) {
				middle_vertex_x = v1_x;
				middle_vertex_y = v1_y;
			}
		}
		else if (middle_vertex_y < v1_y && middle_vertex_y < v2_y) {
			if (v1_y < v2_y) {
				middle_vertex_x = v1_x;
				middle_vertex_y = v1_y;
			}
			else {
				middle_vertex_x = v2_x;
				middle_vertex_y = v2_y;
			}
		}
		else if (middle_vertex_y < v2_y && middle_vertex_y < v0_y) {
			if (v2_y < v0_y) {
				middle_vertex_x = v2_x;
				middle_vertex_y = v2_y;
			}
		}
	}

	double left_line_dx = 0;
	double right_line_dx = 0;

	if (!no_vertices_at_same_height) {
		left_line_dx = (highest_leftmost_vertex_x - lowest_leftmost_vertex_x) / (highest_leftmost_vertex_y - lowest_leftmost_vertex_y);
		right_line_dx = (highest_rightmost_vertex_x - lowest_rightmost_vertex_x) / (highest_rightmost_vertex_y - (lowest_rightmost_vertex_y));
	}
	else {
		if (highest_rightmost_vertex_y == middle_vertex_y) {
			return;
		}

		double dx = (highest_leftmost_vertex_x - middle_vertex_x) / (highest_leftmost_vertex_y - middle_vertex_y);

		if (dx <= 0) {
			left_line_dx = dx;
			right_line_dx = (highest_rightmost_vertex_x - lowest_rightmost_vertex_x) / (highest_rightmost_vertex_y - (lowest_rightmost_vertex_y));
		}
		else {
			right_line_dx = dx;
			left_line_dx = (highest_rightmost_vertex_x - lowest_rightmost_vertex_x) / (highest_rightmost_vertex_y - (lowest_rightmost_vertex_y));
		}
	}

	for (int y = highest_leftmost_vertex_y; y < lowest_leftmost_vertex_y; y++) {
		if (y >= HEIGHT) return;
		else if (y < 0) continue;

		int left_line_edge = ((y - highest_leftmost_vertex_y) * left_line_dx) + highest_leftmost_vertex_x;
		int right_line_edge = ((y - highest_rightmost_vertex_y) * right_line_dx) + highest_rightmost_vertex_x;

		for (int x = left_line_edge; x < (left_line_edge + right_line_edge) / 2; x++) {
			if (x >= WIDTH) return;
			else if (x < 0) continue;
			// Coloring from left side
			this->buffer[(this->WIDTH * y) + x] = fill_color;

			if (right_line_edge - (x - left_line_edge) >= WIDTH) return;
			else if (right_line_edge - (x - left_line_edge) < 0) continue;
			// Coloring from right side
			this->buffer[(this->WIDTH * y) + (right_line_edge - (x - left_line_edge))] = fill_color;
		}

		if (no_vertices_at_same_height && y == middle_vertex_y) {
			double dx = (middle_vertex_x - lowest_leftmost_vertex_x) / (middle_vertex_y - lowest_leftmost_vertex_y);

			//if (left_line_dx <= 0 && dx > 0) {
			if (left_line_dx != dx) {
				left_line_dx = dx;
				highest_leftmost_vertex_x = middle_vertex_x;
				highest_leftmost_vertex_y = middle_vertex_y;
			}
			//else if (right_line_dx <= 0 && dx > 0) {
			else if (right_line_dx != dx) {
				right_line_dx = dx;
				highest_rightmost_vertex_x = middle_vertex_x;
				highest_rightmost_vertex_y = middle_vertex_y;
			}
			else {
				//std::cout << "Error: Invalid condition path reached" << std::endl;
				//exit(-1);
			}
		}
	}

	*/
}

void Engine::draw() {
	// Clears pixel buffer to the background color
	for (int i = 0; i < this->WIDTH * this->HEIGHT; i++) {
		this->buffer[i] = this->BG_COLOR;
	}

	if (!this->z_sorted) {
		//std::deque<Instance> z_sorted_instances = this->current_scene.scene_instances;
		while (!this->z_sorted) {
			//std::cout << "Doing" << std::endl;
			this->z_sorted = true;
			for (auto instance = this->current_scene.scene_instances.begin(); instance != this->current_scene.scene_instances.end() - 1; instance++) {
				Instance* current_instance = &*instance;
				Instance* next_instance = &*(instance + 1);

				Mat current_nearest_vertex = current_instance->TRANSLATION_MATRIX * current_instance->ROTATION_MATRIX * current_instance->SCALING_MATRIX * current_instance->mesh->vertices[0];
				Mat next_nearest_vertex = next_instance->TRANSLATION_MATRIX * next_instance->ROTATION_MATRIX * next_instance->SCALING_MATRIX * next_instance->mesh->vertices[0];

				for (const Mat& vertex : current_instance->mesh->vertices) {
					Mat current_instance_vertex = current_instance->TRANSLATION_MATRIX * current_instance->ROTATION_MATRIX * current_instance->SCALING_MATRIX * vertex;

					if (current_instance_vertex.get(3, 1) < current_nearest_vertex.get(3, 1)) {
						current_nearest_vertex = current_instance_vertex;
					}
				}

				for (const Mat& vertex : next_instance->mesh->vertices) {
					Mat next_instance_vertex = next_instance->TRANSLATION_MATRIX * next_instance->ROTATION_MATRIX * next_instance->SCALING_MATRIX * vertex;

					if (next_instance_vertex.get(3, 1) < next_nearest_vertex.get(3, 1)) {
						next_nearest_vertex = next_instance_vertex;
					}
				}

				double current_farthest_z = current_nearest_vertex.get(3, 1);
				double next_farthest_z = next_nearest_vertex.get(3, 1);

				// Swap entries
				if (next_farthest_z > current_farthest_z) {
					std::swap(*current_instance, *next_instance);
					this->z_sorted = false;
				}

			}
		}
	}

	/*
	int idx = 0;
	for (auto instance = z_sorted_instances.begin(); instance != z_sorted_instances.end() - 1; instance++) {
		Instance* current_instance = &*instance;
		Instance* next_instance = &*(instance + 1);

		Mat current_nearest_vertex = current_instance->TRANSLATION_MATRIX * current_instance->ROTATION_MATRIX * current_instance->SCALING_MATRIX *  current_instance->mesh->vertices[0];
		Mat next_nearest_vertex = next_instance->TRANSLATION_MATRIX * next_instance->ROTATION_MATRIX * next_instance->SCALING_MATRIX *  next_instance->mesh->vertices[0];

		for (const Mat& vertex : current_instance->mesh->vertices) {
			Mat current_instance_vertex = current_instance->TRANSLATION_MATRIX * current_instance->ROTATION_MATRIX * current_instance->SCALING_MATRIX * vertex;

			if (current_instance_vertex.get(3, 1) < current_nearest_vertex.get(3, 1)) {
				current_nearest_vertex = current_instance_vertex;
			}
		}

		for (const Mat& vertex : next_instance->mesh->vertices) {
			Mat next_instance_vertex = next_instance->TRANSLATION_MATRIX * next_instance->ROTATION_MATRIX * next_instance->SCALING_MATRIX * vertex;

			if (next_instance_vertex.get(3, 1) < next_nearest_vertex.get(3, 1)) {
				next_nearest_vertex = next_instance_vertex;
			}
		}

		double current_farthest_z = current_nearest_vertex.get(3, 1);
		double next_farthest_z = next_nearest_vertex.get(3, 1);

		idx++;
		std::cout << idx << ": " << current_farthest_z << std::endl;

	}

	std::cout << z_sorted_instances.size() << std::endl;
	std::cout << this->current_scene.scene_instances.size() << std::endl;
	exit(-1);
	*/

	//draw_mesh(cube);


	//for (const Instance& instance : this->current_scene.scene_instances) {
	for (const Instance& instance : this->current_scene.scene_instances) {
		if (instance.show != 0) draw_instance(instance, this->wireframe_triangles, 0x00FF00FF, this->rasterize_triangles, 0xFF0000FF, this->shade_triangles);
	}
	

	/*
	for (Triangle& triangle : this->cube.triangles) {
		draw_triangle(triangle);
		//rotateZ(triangle);
		//rotate(triangle);
		//rotateX(triangle);
		//rotateY(triangle);
	}
	*/

	//draw_line(250, 400, 500, 200);
	/*
	uint8_t red = 255 * ((sin(SDL_GetTicks() / 1000.f) + 1) / 2);
	uint8_t green = 100 * ((sin(SDL_GetTicks() / 1000.f) + 1) / 2);
	uint8_t blue = 60 * ((sin(SDL_GetTicks() / 1000.f) + 1) / 2);
	uint32_t new_pixel = 0x000000FF | (red << 24) | (green << 16) | (blue << 8);

	for (int i = 0; i < this->WIDTH * this->HEIGHT; i++) {
		this->buffer[i] = new_pixel;
	}
	*/
}

void Engine::render() {
	SDL_SetRenderDrawColor(this->renderer, this->CLEAR_COLOR >> 24, this->CLEAR_COLOR >> 16, this->CLEAR_COLOR >> 8, this->CLEAR_COLOR);
	SDL_RenderClear(this->renderer);
	SDL_UpdateTexture(this->texture, nullptr, this->buffer, this->WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	SDL_RenderPresent(this->renderer);
}

void Engine::rotateX(Instance& instance, double radians) {
	Mat rotation = Engine::quaternion_rotationX_matrix(radians);

	instance.ROTATION_MATRIX = rotation * instance.ROTATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	/*
	double tx = instance.position.get(1, 1);
	double ty = instance.position.get(2, 1);
	double tz = instance.position.get(3, 1);
	instance.MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = rotation * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * instance.MODEL_TO_WORLD;
	*/
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

	/*
	double tx = instance.position.get(1, 1);
	double ty = instance.position.get(2, 1);
	double tz = instance.position.get(3, 1);
	instance.MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = rotation * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * instance.MODEL_TO_WORLD;
	*/
}

void Engine::rotateY(Mesh& mesh, double radians) {
	/*
	Mat rotation = Mat(
		{
			{cos(radians), 0, sin(radians)},
			{0, 1, 0},
			{-sin(radians), 0, cos(radians)}
		},
		3, 3
	);
	*/

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

	/*
	for (int i = 0; i < triangle.vertices.size(); i++) {
		Mat point = triangle.vertices[i];

		Mat rotation = Mat({
	{cos(radians), 0, sin(radians)},
	{0, 1, 0},
	{-sin(radians), 0, cos(radians)}
			}, 3, 3);

		Mat rotation = Mat(
			{
				{cos(radians), 0, -sin(radians)},
				{0, 1, 0},
				{sin(radians), 0, cos(radians)}
			},
			3, 3
		);

		point = rotation * point;

		triangle.vertices[i] = point;
	}
	*/
}

void Engine::rotateY(Mat& matrix, double radians) {
	Mat rotation = Engine::quaternion_rotationY_matrix(radians);

	matrix = rotation * matrix;
}

void Engine::rotateZ(Instance& instance, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	instance.ROTATION_MATRIX = rotation * instance.ROTATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	/*
	double tx = instance.position.get(1, 1);
	double ty = instance.position.get(2, 1);
	double tz = instance.position.get(3, 1);
	instance.MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = rotation * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * instance.MODEL_TO_WORLD;
	*/
}

void Engine::rotateZ(Mesh& mesh, double radians) {
	/*
	Mat rotation = Mat({
{cos(0.005), -sin(0.005), 0},
{sin(0.005), cos(0.005), 0},
		{0, 0, 1}
		}, 3, 3);
		*/

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

	/*
	for (int i = 0; i < triangle.vertices.size(); i++) {
		Mat point = triangle.vertices[i];

		
		Mat rotation = Mat({
	{cos(0.005), -sin(0.005), 0},
	{sin(0.005), cos(0.005), 0},
			{0, 0, 1}
			}, 3, 3);
			

		Mat rotation = Mat(
			{
				{cos(radians), sin(radians), 0},
				{-sin(radians), cos(radians), 0},
				{0, 0, 1}
			},
			3, 3
		);

		point = rotation * point;

		triangle.vertices[i] = point;
	}
	*/
}

void Engine::rotateZ(Mat& matrix, double radians) {
	Mat rotation = Engine::quaternion_rotationZ_matrix(radians);

	matrix = rotation * matrix;
}

void Engine::translate(Instance& instance, double tx, double ty, double tz) {
	Mat translation_matrix = Engine::translation_matrix(tx, ty, tz);

	instance.TRANSLATION_MATRIX = translation_matrix * instance.TRANSLATION_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;

	//instance.MODEL_TO_WORLD = translation_matrix * instance.MODEL_TO_WORLD;
}

void Engine::translate(Mesh& mesh, double tx, double ty, double tz) {
	Mat translation_matrix = Engine::translation_matrix(tx, ty, tz);

	for (Mat& vertex : mesh.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Quad& quad, double tx, double ty, double tz) {
	Engine::translate(quad.triangles[0], tx, ty, tz);
	Engine::translate(quad.triangles[1], tx, ty, tz);
}

void Engine::translate(Triangle& triangle, double tx, double ty, double tz) {
	Mat translation_matrix = Engine::translation_matrix(tx, ty, tz);

	for (Mat& vertex : triangle.vertices) {
		vertex = translation_matrix * vertex;
	}
}

void Engine::translate(Mat& matrix, double tx, double ty, double tz) {
	Mat translation_matrix = Engine::translation_matrix(tx, ty, tz);

	matrix = translation_matrix * matrix;
}

void Engine::scale(Instance& instance, double sx, double sy, double sz) {
	Mat scaling_matrix = Engine::scale_matrix(sx, sy, sz);

	instance.SCALING_MATRIX = scaling_matrix * instance.SCALING_MATRIX;
	instance.MODEL_TO_WORLD = instance.TRANSLATION_MATRIX * instance.ROTATION_MATRIX * instance.SCALING_MATRIX;
	
	/*
	double tx = instance.position.get(1, 1);
	double ty = instance.position.get(2, 1);
	double tz = instance.position.get(3, 1);
	instance.MODEL_TO_WORLD = Engine::translation_matrix(-tx, -ty, -tz) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::quaternion_rotationZ_matrix(-instance.roll) * Engine::quaternion_rotationX_matrix(-instance.pitch) * Engine::quaternion_rotationY_matrix(-instance.yaw) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = scale_matrix * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::quaternion_rotationZ_matrix(instance.roll) * Engine::quaternion_rotationX_matrix(instance.pitch) * Engine::quaternion_rotationY_matrix(instance.yaw) * instance.MODEL_TO_WORLD;
	instance.MODEL_TO_WORLD = Engine::translation_matrix(tx, ty, tz) * instance.MODEL_TO_WORLD;
	*/
}

void Engine::scale(Mesh& mesh, double sx, double sy, double sz) {
	Mat scale_matrix = Engine::scale_matrix(sx, sy, sz);

	for (Mat& vertex : mesh.vertices) {
		vertex = scale_matrix * vertex;
	}
}

void Engine::scale(Quad& quad, double sx, double sy, double sz) {
	Engine::scale(quad.triangles[0], sx, sy, sz);
	Engine::scale(quad.triangles[1], sx, sy, sz);
}

void Engine::scale(Triangle& triangle, double sx, double sy, double sz) {
	Mat scale_matrix = Engine::scale_matrix(sx, sy, sz);

	for (Mat& vertex : triangle.vertices) {
		vertex = scale_matrix * vertex;
	}

}

void Engine::scale(Mat& matrix, double sx, double sy, double sz) {
	Mat scale_matrix = Engine::scale_matrix(sx, sy, sz);

	matrix = scale_matrix * matrix;
}

Mat Engine::translation_matrix(double tx, double ty, double tz) {
	Mat translation_matrix = Mat(
		{
			{1, 0, 0, tx},
			{0, 1, 0, ty},
			{0, 0, 1, tz},
			{0, 0, 0, 1}
		}
	, 4, 4);

	return translation_matrix;
}

Mat Engine::scale_matrix(double sx, double sy, double sz) {
	Mat scale_matrix = Mat(
		{
		{sx, 0, 0, 0},
		{0, sy, 0, 0},
		{0, 0, sz, 0},
		{0, 0, 0, 1}
		}
	, 4, 4);

	return scale_matrix;
}

Mat Engine::euler_rotationX_matrix(double radians) {
	// Positive y-axis pointing up, positive z-axis pointing out of the screen
	///*
	Mat rotation_matrix = Mat(
		{
			{1, 0, 0, 0},
			{0, cos(radians), -sin(radians), 0},
			{0, sin(radians), cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);
	//*/

	// Positive y-axis pointing down, positive z-axis pointing into the screen
	/*
	Mat rotation_matrix = Mat(
		{
			{1, 0, 0, 0},
			{0, cos(radians), sin(radians), 0},
			{0, -sin(radians), cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);
	*/

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

	/*
	Mat rotation_matrix = Mat(
		{
			{cos(radians), 0, -sin(radians), 0},
			{0, 1, 0, 0},
			{sin(radians), 0, cos(radians), 0},
			{0, 0, 0, 1}
		},
		4, 4
	);
	*/

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
/// Calculates whether there is an intersection between the given line and plane, updating the `intersection_point` variable to the actual intersection point and returns 1 that is the case, otherwise returns 0 when no intersection is found
/// </summary>
/// <param name="plane_point">A point in the plane</param>
/// <param name="plane_normal">A normal vector to the plane</param>
/// <param name="intersection_point">Reference to a variable which will store the intersection point if it exists</param>
/// <returns>1 if line intersects plane, 0 otherwise</returns>
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

double Engine::PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal) {
	return Mat::dot(*point - *plane_point, *plane_normal);
}

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

void Engine::Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch) {
	yaw = atan2f(direction_vector.get(1, 1), direction_vector.get(3, 1));
	pitch = atan2f(-direction_vector.get(2, 1), sqrt(pow(direction_vector.get(1, 1), 2) + pow(direction_vector.get(3, 1), 2)));

	if (yaw < 0) yaw = (2 * M_PI) + yaw;
	if (pitch < 0) pitch = (2 * M_PI) + pitch;
}

double clamp(double value, double min, double max) {
	return fmax(min, fmin(max, value));
}

void Engine::EulerAngles_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll) {
	pitch = asin(-clamp(rotation_matrix.get(2, 3), -1, 1));

	if (abs(rotation_matrix.get(2, 3) < 0.9999999)) {
		yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
		roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
	}
	else {
		yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
		roll = 0;
	}
}

// Gets yaw, pitch, and roll from quaternion
void Engine::Quaternion_GetAnglesFromQuaternionYP(const Quaternion& quaternion, double& yaw, double& pitch, double& roll) {
	Mat rotation_matrix = quaternion.get_rotationmatrix();
	pitch = asin(-clamp(rotation_matrix.get(2, 3), -1, 1));

	if (abs(rotation_matrix.get(2, 3) < 0.9999999)) {
		yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
		roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
	}
	else {
		yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
		roll = 0;
	}

	/*
	pitch = atan2(2.0 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x), quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
	yaw = asin(-2.0 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y));
	roll = atan2(2.0 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z), quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z);
	*/

	/*
	float sinp = 2.0f * (quaternion.w * quaternion.x - quaternion.z * quaternion.y);

	if (std::abs(pitch) >= 1.0f) {
		// Gimbal lock case
		pitch = std::copysign(M_PI / 2.0f, sinp); // Use ±90° depending on direction

		// In gimbal lock, yaw and roll are linked
		// We can choose to set one to zero and calculate the other
		yaw = std::atan2(2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x),
			quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
		roll = 0.0f; // Choose roll = 0 in gimbal lock
	}
	else {
		// Normal case
		pitch = std::asin(sinp);
		yaw = std::atan2(2.0f * (quaternion.w * quaternion.y + quaternion.x * quaternion.z),
			1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y));
		roll = std::atan2(2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y),
			1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z));
	}
	*/

	//yaw = atan2f(2 * ((quaternion.w * quaternion.y) + (quaternion.x * quaternion.z)), 1 - (2 * ((quaternion.y * quaternion.y) + (quaternion.x * quaternion.x))));
	//pitch = asinf(2 * ((quaternion.w * quaternion.x) - (quaternion.z * quaternion.y)));
	//roll = atan2f(2 * (quaternion.w * quaternion.z + quaternion.y * quaternion.x), 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z));


	//roll = atan2f(2 * (quaternion.w * quaternion.z + quaternion.y * quaternion.x), 1 - 2 * (quaternion.x * quaternion.x + quaternion.z * quaternion.z));

	/*
	float sinPitch = 2.0f * (quaternion.w * quaternion.x - quaternion.y * quaternion.z);

	// Edge case handling: if sinPitch is +/-1, we're at pitch = +/-90°
	// This creates a gimbal lock situation
	if (abs(sinPitch) >= 0.999f) {
		// In gimbal lock
		pitch = (M_PI / 2) * (sinPitch < 0 ? -1 : 1);
		// Yaw and roll become linked in gimbal lock
		// We typically assign all rotation to yaw and set roll to zero
		yaw = atan2f(-2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y),
			quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z);
		roll = 0.0f;
	}
	else {
		// Normal case - no gimbal lock
		// Yaw (around Y)
		yaw = atan2f(
			2.0f * (quaternion.w * quaternion.y + quaternion.x * quaternion.z),
			quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z
		);

		// Pitch (around X)
		pitch = asinf(sinPitch);

		// Roll (around Z)
		roll = atan2f(
			2.0f * (quaternion.w * quaternion.z + quaternion.y * quaternion.x),
			quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z
		);
	}
	*/

	/*
	if (yaw < 0) yaw = 2 * M_PI + yaw;
	if (pitch < 0) pitch = 2 * M_PI + pitch;
	if (roll < 0) roll = 2 * M_PI + roll;
	*/

	//roll = 0;
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


// Gets yaw and roll from quaternion
void Engine::Quaternion_GetAnglesFromQuaternionYR(const Quaternion& quaternion, double& yaw, double& pitch, double& roll) {
	Mat rotation_matrix = quaternion.get_rotationmatrix();
	pitch = asin(-clamp(rotation_matrix.get(2, 3), -1, 1));

	if (abs(rotation_matrix.get(2, 3) < 0.9999999)) {
		yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
		roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
	}
	else {
		yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
		roll = 0;
	}

	/*
	pitch = atan2(2.0 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x), quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
	yaw = asin(-2.0 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y));
	roll = atan2(2.0 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z), quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z);
	*/

	//yaw = yaw;

	//pitch = asinf(2 * ((quaternion.w * quaternion.x) - (quaternion.z * quaternion.y)));

	/*
	float sinp = 2.0f * (quaternion.w * quaternion.x - quaternion.z * quaternion.y);

	if (std::abs(pitch) >= 1.0f) {
		// Gimbal lock case
		pitch = std::copysign(M_PI / 2.0f, sinp); // Use ±90° depending on direction

		// In gimbal lock, yaw and roll are linked
		// We can choose to set one to zero and calculate the other
		yaw = std::atan2(2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x),
			quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
		roll = 0.0f; // Choose roll = 0 in gimbal lock
	}
	else {
		// Normal case
		pitch = std::asin(sinp);
		yaw = std::atan2(2.0f * (quaternion.w * quaternion.y + quaternion.x * quaternion.z),
			1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y));
		roll = std::atan2(2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y),
			1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z));
	}
	*/

	//yaw = atan2f(2 * ((quaternion.w * quaternion.y) + (quaternion.x * quaternion.z)), 1 - (2 * ((quaternion.y * quaternion.y) + (quaternion.x * quaternion.x))));
	//roll = atan2f(2 * (quaternion.w * quaternion.z + quaternion.y * quaternion.x), 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z));

	/*
	if (yaw < 0) yaw = 2 * M_PI + yaw;
	if (pitch < 0) pitch = 2 * M_PI + pitch;
	if (roll < 0) roll = 2 * M_PI + roll;
	*/

	//roll = 0;
}

void Engine::Quaternion_GetAnglesFromDirectionYP(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll) {
	const Mat normalized_v1 = default_direction_vector / default_direction_vector.norm();
	const Mat normalized_v2 = direction_vector / direction_vector.norm();

	Mat rotation_axis = Engine::CrossProduct3D(normalized_v1, normalized_v2);
	rotation_axis.normalize();

	double rotation_angle = acos(Mat::dot(normalized_v1, normalized_v2));
	const Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

	Engine::Quaternion_GetAnglesFromQuaternionYP(rotation, yaw, pitch, roll);
}

void Engine::Quaternion_GetAnglesFromDirectionYR(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll) {
	const Mat normalized_v1 = default_direction_vector / default_direction_vector.norm();
	const Mat normalized_v2 = direction_vector / direction_vector.norm();
	
	Mat rotation_axis = Engine::CrossProduct3D(normalized_v1, normalized_v2);
	rotation_axis.normalize();

	double rotation_angle = acos(Mat::dot(normalized_v1, normalized_v2));
	const Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

	Engine::Quaternion_GetAnglesFromQuaternionYR(rotation, yaw, pitch, roll);
}

void Engine::close() {
	free(this->buffer);

	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);

	SDL_Quit();
}

void Engine::LookAt() {

	// Setting 4th dimension to 0 for dot product
	Mat camera_position = this->camera_position;
	camera_position.set(0, 4, 1);

	Mat cam_z_axis = ((camera_direction + camera_position) - camera_position);
	cam_z_axis.set(0, 4, 1);
	//Mat vec_b = Engine::CrossProduct3D(this->camera_up, vec_a);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	//Mat vec_c = Engine::CrossProduct3D(vec_a, vec_b);
	Mat cam_y_axis = Engine::CrossProduct3D(cam_z_axis, cam_x_axis);

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

	//Mat vec_b = Engine::CrossProduct3D(this->camera_up, vec_a);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	//Mat vec_c = Engine::CrossProduct3D(vec_a, vec_b);
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

Mat Engine::LookAt(const Mat& camera_position, const Mat& camera_direction, const Mat& camera_up) {

	// Setting 4th dimension to 0 for dot product
	Mat tmp_camera_position = camera_position;
	tmp_camera_position.set(0, 4, 1);

	Mat cam_z_axis = ((camera_direction + camera_position) - tmp_camera_position);

	cam_z_axis.set(0, 4, 1);
	//Mat vec_b = Engine::CrossProduct3D(camera_up, vec_a);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	//Mat vec_c = Engine::CrossProduct3D(vec_a, vec_b);
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

	//Mat vec_b = Engine::CrossProduct3D(camera_up, vec_a);
	Mat cam_x_axis = Engine::CrossProduct3D(camera_up, cam_z_axis);
	//Mat vec_c = Engine::CrossProduct3D(vec_a, vec_b);
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

void Engine::DoIt(const Mat& camera_direction, const Mat& camera_up, double& yaw, double& pitch, double& roll) {
	// Get current direction vectors
	Mat right = Engine::CrossProduct3D(camera_up, camera_direction);

	// Extract yaw (rotation around Y axis)
	yaw = std::atan2(camera_direction.get(1, 1), -camera_direction.get(3, 1));

	// Extract pitch (rotation around X axis)
	pitch = std::asin(-camera_direction.get(2, 1));

	// Extract roll using up vector
	// Project up vector onto the plane perpendicular to forward
	Mat projectedUp = camera_up - camera_direction * (camera_up.get(1, 1) * camera_direction.get(1, 1) + camera_up.get(2, 1) * camera_direction.get(2, 1) + camera_up.get(3, 1) * camera_direction.get(3, 1));

	projectedUp.normalize();

	// Calculate roll based on projected up vector
	roll = std::atan2(right.get(2, 1), projectedUp.get(2, 1));
}