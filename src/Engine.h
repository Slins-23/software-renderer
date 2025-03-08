#pragma once
#include <iostream>
#include <vector>
#include <cassert>
#include <array>
#include <cstring>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "json.hpp"
#include "Quaternion.h"
#include "Quad.h"
#include "Mesh.h"
#include "Instance.h"
#include "Scene.h"
#include "Light.h"
#include "WindowManager.h"

enum ShadingType {
	Flat,
	Gouraud,
	Phong
};

class Engine {
private:
	uint32_t* pixel_buffer = nullptr;
	double* depth_buffer = nullptr;

	SDL_Event event;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;

	void update_projection_matrix() {
		this->PROJECTION_MATRIX = Mat(
			{
				{(1 / (tan(this->FOVr / 2))), 0, 0, 0},
				{0, this->AR * (1 / (tan(this->FOVr / 2))), 0, 0},
				{0, 0, this->far / (this->far - this->near), (this->far * -this->near) / (this->far - this->near)},
				{0, 0, 1, 0}
			}
		, 4, 4);
	}

	void update_view_inverse();

	void LookAt();
	void LookAt(const Mat& target_vector);
public:
	const char* TITLE = "Renderer";
	uint16_t WIDTH = 800;
	uint16_t HEIGHT = 600;

	const uint8_t FPS_LIMIT = 144;
	const uint8_t MSPERFRAME = 1000.0 / FPS_LIMIT;

	// SDL window clear/default color
	const uint32_t CLEAR_COLOR = 0xFFFFFFFF;

	// Background color
	const uint32_t BG_COLOR = 0x000000FF;

	// Line/wireframe color
	const uint32_t LINE_COLOR = 0x00FF00FF;
	
	// Rasterization color
	const uint32_t FILL_COLOR = 0x66285CFF;

	bool playing = true;

	// How often, in milliseconds, should the average FPS over the given time interval be printed to the console
	double fps_update_interval = 500;

	bool depth_test = true; // Toggle 5
	bool backface_cull = true; // Toggle 4
	bool shade = true; // Toggle 3
	bool rasterize = true; // Toggle 2
	bool wireframe_render = false; // Toggle 1
	bool editing_mode = false; // Toggle 6

	ShadingType shading_type = ShadingType::Flat;
	
	bool transform_light = false; // Toggle 7 | Whether transformations will be applied to the light source (true) or to the target instance (false)

	// Which coordinate system is the reference orientation for instance transformations (use only world for now, I still need to implement and accomodate for some edge cases, especially when transitioning from one space to another)
	bool using_instance_world_orientation = false;
	bool using_instance_local_orientation = true;

	double z_fighting_tolerance = 0.994;

	double near = 0.01;
	double far = 1000;
	double FOV = 60;
	double FOVr = FOV * (M_PI / 180);
	double AR = (double) this->WIDTH / (double) this->HEIGHT;
	//double AR = 16/9.f;

	// Translation amounts
	double translation_amount = 0.01;
	double tx = translation_amount;
	double ty = translation_amount;
	double tz = translation_amount;

	// Scale factor
	// Scaling all factors equally does not change the apperance of the object
	double scale_factor = 0.05;
	double sx = 1;
	double sy = 1;
	double sz = 1;

	// Whether to use scene's camera settings (otherwise default to the default settings)
	double use_scene_camera_settings = true;

	double camera_velocity = 0.1;
	double camera_xvelocity = camera_velocity;
	double camera_yvelocity = camera_velocity;
	double camera_zvelocity = camera_velocity;

	Mat default_camera_position = Mat({
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	Mat camera_position = Mat(
		{ 
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	// Rotation angle
	double rotation_angle_degrees = 10;
	double rotation_angle = rotation_angle_degrees * (M_PI / 180);

	double default_camera_pitch = 0;
	double default_camera_yaw = 0;
	double default_camera_roll = 0;

	double camera_pitch = 0;
	double camera_yaw = 0;
	double camera_roll = 0; // Should not change camera direction

	// Initial camera direction (not standard camera direction, only what it starts at, the default position is (0, 0, -1))
	
	Quaternion camera_orientation = Quaternion(0, 0, 0, 1);

	Light light_source;

	Mat default_camera_direction = Mat({
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	// Camera direction vector relative to world space
	Mat camera_direction = Mat(
		{ 
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	Mat default_camera_up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat camera_up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat default_camera_right = Mat(
		{
			{1},
			{0},
			{0},
			{0}
		}, 4, 1
	);

	Mat VIEW_MATRIX = Mat::identity_matrix(4);
	Mat VIEW_INVERSE = Mat::identity_matrix(4);


	// For transforming from NDC space to Screen space (window coordinates)
	Mat SCALE_MATRIX = Mat(
		{
			{this->WIDTH / 2., 0, 0, this->WIDTH / 2.},
			{0, this->HEIGHT / 2., 0, this->HEIGHT / 2.},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}, 4, 4);

	// Flips Y and Z
	Mat PROJECTION_MATRIX = Mat(
		{
			{(1 / (tan(FOVr / 2))), 0, 0, 0},
			{0, AR * (1 / (tan(FOVr / 2))), 0, 0},
			{0, 0, far / (far - near), (far * -near) / (far - near)},
			{0, 0, 1, 0}
		}
	, 4, 4);

	const char* models_folder = "D:/Programming/Graphics/Prototyping/models/";
	const char* scenes_folder = "D:/Programming/Graphics/Prototyping/scenes/";
	const char* scene_filename = "hallway.json";
	const char* scene_save_name = "tst.json";
	//const char* light_mesh = "light.obj";
	Scene current_scene;

	Instance* target_instance = nullptr;

	WindowManager window_manager;

	bool show_window = false;
	
	bool setup();
	bool imgui_setup();
	bool handle_events();

	void draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source);
	void draw_mesh(const Mesh& mesh, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source);
	void draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, Mat v0_normal, Mat v1_normal, Mat v2_normal, Mat v3_normal, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source);
	void draw_triangle(Mat v0, Mat v1, Mat v2, Mat v0_normal, Mat v1_normal, Mat v2_normal, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source);
	void draw_line(double x1, double y1, double x2, double y2, const Mat& vec_a, const Mat& vec_b, const double& vec_a_originalz, const double& vec_b_originalz, uint32_t outline_color);
	void draw();
	void render();

	void fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& world_v0_normal, const Mat& world_v1_normal, const Mat& world_v2_normal, const Mat& world_v0, const Mat& world_v1, const Mat& world_v2, const Mat& v0_color, const Mat& v1_color, const Mat& v2_color, uint32_t fill_color, bool shade, bool is_light_source);
 
	static void rotateX(Instance& instance, double radians, Orientation orientation, const Mat& default_camera_direction, const Mat& default_camera_up, const Mat& default_camera_right);
	static void rotateX(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right);
	static void rotateX(Mesh& mesh, double radians, const Mat& default_camera_right);
	static void rotateX(Quad& quad, double radians, const Mat& default_camera_right);
	static void rotateX(Triangle& triangle, double radians, const Mat& default_camera_right);
	static void rotateX(Mat& matrix, double radians, const Mat& default_camera_right);
	
	static void rotateY(Instance& instance, double radians, Orientation orientation, const Mat& default_camera_direction, const Mat& default_camera_up, const Mat& default_camera_right);
	static void rotateY(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right);
	static void rotateY(Mesh& mesh, double radians, const Mat& default_camera_up);
	static void rotateY(Quad& quad, double radians, const Mat& default_camera_up);
	static void rotateY(Triangle& triangle, double radians, const Mat& default_camera_up);
	static void rotateY(Mat& matrix, double radians, const Mat& default_camera_up);

	static void rotateZ(Instance& instance, double radians, Orientation orientation, const Mat& default_camera_direction, const Mat& default_camera_up, const Mat& default_camera_right);
	static void rotateZ(Light& light_source, double radians, Orientation orientation, const Mat& default_world_direction, const Mat& default_world_up, const Mat& default_world_right);
	static void rotateZ(Mesh& mesh, double radians, const Mat& default_camera_direction);
	static void rotateZ(Quad& quad, double radians, const Mat& default_camera_direction);
	static void rotateZ(Triangle& triangle, double radians, const Mat& default_camera_direction);
	static void rotateZ(Mat& matrix, double radians, const Mat& default_camera_direction);

	static void translate(Instance& instance, double tx, double ty, double tz);
	static void translate(Mesh& mesh, double tx, double ty, double tz);
	static void translate(Quad& quad, double tx, double ty, double tz);
	static void translate(Triangle& triangle, double tx, double ty, double tz);
	static void translate(Mat& matrix, double tx, double ty, double tz);

	static void scale(Instance& instance, double sx, double sy, double sz, bool set);
	static void scale(Mesh& mesh, double sx, double sy, double sz);
	static void scale(Quad& quad, double sx, double sy, double sz);
	static void scale(Triangle& triangle, double sx, double sy, double sz);
	static void scale(Mat& matrix, double sx, double sy, double sz);

	static Mat euler_rotationX_matrix(double radians);
	static Mat euler_rotationY_matrix(double radians);
	static Mat euler_rotationZ_matrix(double radians);

	static bool LinePlaneIntersection(const Mat* plane_point, const Mat* plane_normal, const Mat* line_start, const Mat* line_end, Mat& intersection_point);

	static uint8_t ClipTriangleToPlane(const Mat* plane_point, const Mat* plane_normal, const Triangle* input_triangle, Triangle& clipped_triangle_a, Triangle& clipped_triangle_b);
	static double PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal);

	static void Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch);

	static void Euler_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll);

	static void GetRoll(Orientation orientation, const Mat& direction, const Mat& up, double& yaw, const double& pitch, double& roll);


	static Mat LookAt(const Mat& camera_position, const Mat& camera_direction, const Mat& camera_up);
	static Mat LookAt(const Mat& camera_position, Mat& camera_direction, const Mat& target_vector, Mat& camera_up);

	void close();

	void load_scene(bool verbose);
};