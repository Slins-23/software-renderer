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


	void LookAt();
	void LookAt(const Mat& target_vector);
public:
	const char* TITLE = "Renderer";
	uint16_t WIDTH = 800;
	uint16_t HEIGHT = 600;

	const uint8_t FPS = 144;
	const uint8_t MSPERFRAME = 1000 / FPS;

	// SDL window clear/default color
	const uint32_t CLEAR_COLOR = 0xFFFFFFFF;

	// Background color
	const uint32_t BG_COLOR = 0x000000FF;

	// Line/wireframe color
	const uint32_t LINE_COLOR = 0x00FF00FF;
	
	// Rasterization color
	const uint32_t FILL_COLOR = 0xFF0000FF;

	bool playing = true;

	// How often, in milliseconds, should the average FPS over the given time interval be printed to the console
	double fps_update_interval = 500;

	bool depth_test = true; // Toggle 5
	bool backface_cull = true; // Toggle 4
	bool shade = true; // Toggle 3
	bool rasterize = false; // Toggle 2
	bool wireframe_render = false; // Toggle 1

	double z_fighting_tolerance = 0.994;

	double near = 0.1;
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

	Mat light_source_pos = Mat(
		{
			{0},
			{0},
			{1},
			{1},
		}, 4, 1
		);

	Mat light_source_dir = Mat(
		{
			{0},
			{0},
			{1},
			{0},
		}, 4, 1
		);

	double default_light_intensity = 0.3;
	double default_minimum_exposure = 0.1;

	double light_intensity = default_light_intensity;
	double minimum_exposure = default_minimum_exposure;

	uint32_t light_color = 0x66285CFF;

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

	Instance light_instance;

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
	
	bool setup();
	bool handle_events();

	void draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_mesh(const Mesh& mesh, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_triangle(Mat v0, Mat v1, Mat v2, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_line(double x1, double y1, double x2, double y2, const Mat& vec_a, const Mat& vec_b, const double& vec_a_originalz, const double& vec_b_originalz, uint32_t outline_color);
	void draw();
	void render();

	void fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const double& v0_originalz, const double& v1_originalz, const double& v2_originalz, uint32_t fill_color, bool shade);
 
	static void rotateX(Instance& mesh, double radians);
	static void rotateX(Mesh& mesh, double radians);
	static void rotateX(Quad& quad, double radians);
	static void rotateX(Triangle& triangle, double radians);
	static void rotateX(Mat& matrix, double radians);
	
	static void rotateY(Instance& mesh, double radians);
	static void rotateY(Mesh& mesh, double radians);
	static void rotateY(Quad& quad, double radians);
	static void rotateY(Triangle& triangle, double radians);
	static void rotateY(Mat& matrix, double radians);

	static void rotateZ(Instance& mesh, double radians);
	static void rotateZ(Mesh& mesh, double radians);
	static void rotateZ(Quad& quad, double radians);
	static void rotateZ(Triangle& triangle, double radians);
	static void rotateZ(Mat& matrix, double radians);

	static void translate(Instance& mesh, double tx, double ty, double tz);
	static void translate(Mesh& mesh, double tx, double ty, double tz);
	static void translate(Quad& quad, double tx, double ty, double tz);
	static void translate(Triangle& triangle, double tx, double ty, double tz);
	static void translate(Mat& matrix, double tx, double ty, double tz);

	static void scale(Instance& instance, double sx, double sy, double sz);
	static void scale(Mesh& mesh, double sx, double sy, double sz);
	static void scale(Quad& quad, double sx, double sy, double sz);
	static void scale(Triangle& triangle, double sx, double sy, double sz);
	static void scale(Mat& matrix, double sx, double sy, double sz);

	static Mat quaternion_rotationX_matrix(double radians);
	static Mat quaternion_rotationY_matrix(double radians);
	static Mat quaternion_rotationZ_matrix(double radians);
	static Mat euler_rotationX_matrix(double radians);
	static Mat euler_rotationY_matrix(double radians);
	static Mat euler_rotationZ_matrix(double radians);

	static bool LinePlaneIntersection(const Mat* plane_point, const Mat* plane_normal, const Mat* line_start, const Mat* line_end, Mat& intersection_point);

	static uint8_t ClipTriangleToPlane(const Mat* plane_point, const Mat* plane_normal, const Triangle* input_triangle, Triangle& clipped_triangle_a, Triangle& clipped_triangle_b);
	static double PointDistanceToPlane(const Mat* point, const Mat* plane_point, const Mat* plane_normal);

	static void Euler_GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch);

	static void Euler_FromMatrix(const Mat& rotation_matrix, double& yaw, double& pitch, double& roll);

	static void GetRoll(const Mat& camera_direction, const Mat& camera_up, const double& yaw, const double& pitch, double& roll);


	static Mat LookAt(const Mat& camera_position, const Mat& camera_direction, const Mat& camera_up);
	static Mat LookAt(const Mat& camera_position, Mat& camera_direction, const Mat& target_vector, Mat& camera_up);

	void close();

	void load_scene(bool verbose);
};