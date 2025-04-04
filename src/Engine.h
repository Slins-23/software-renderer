#pragma once
#include <iostream>
#include <vector>
#include <cassert>
#include <array>
#include <cstring>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Windows/WindowManager.h"
#include "Quad.h"

class Engine {
private:
	uint32_t* pixel_buffer = nullptr;
	double* depth_buffer = nullptr;

	SDL_Event event;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;

	// SDL window clear/default color
	const uint32_t CLEAR_COLOR = 0xFFFFFFFF;
public:
	const char* TITLE = "Renderer";
	uint16_t WIDTH = 800;
	uint16_t HEIGHT = 600;

	WindowManager window_manager;
	
	bool setup();
	bool handle_events();

	void draw_instance(const Instance& instance, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade);
	void draw_mesh(const Mesh& mesh, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes);
	void draw_quad(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& v3, Mat v0_normal, Mat v1_normal, Mat v2_normal, Mat v3_normal, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes);
	void draw_triangle(Mat v0, Mat v1, Mat v2, Mat v0_normal, Mat v1_normal, Mat v2_normal, const Mat& model_to_world, bool draw_outline, uint32_t outline_color, bool fill, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes);
	void draw_line(double x1, double y1, double x2, double y2, const Mat& vec_a, const Mat& vec_b, const double& vec_a_originalz, const double& vec_b_originalz, uint32_t outline_color, bool is_axes);
	void draw();
	void render();

	void fill_triangle(const Mat& v0, const Mat& v1, const Mat& v2, const Mat& world_v0_normal, const Mat& world_v1_normal, const Mat& world_v2_normal, const Mat& world_v0, const Mat& world_v1, const Mat& world_v2, const Mat& v0_color, const Mat& v1_color, const Mat& v2_color, uint32_t fill_color, bool shade, bool is_light_source, bool is_axes);
 
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

	void close();
};