#pragma once
#include "Matrix.h"
#include "Quaternion.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

class Camera {
private:

public:
	Mat VIEW_MATRIX = Mat::identity_matrix(4);

	// Used exclusively for getting the clipped triangle's world space vertices, as clipping is performed in view space
	Mat VIEW_INVERSE = Mat::identity_matrix(4);


	// For transforming from NDC space to Screen space (window coordinates)
	Mat SCALE_MATRIX = Mat(
		{
			{800 / 2., 0, 0, 800 / 2.},
			{0, 600 / 2., 0, 600 / 2.},
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

	Quaternion orientation = Quaternion(0, 0, 0, 1);

	double AR = 4/3.0;
	//double AR = 16/9.f;

	double near = 0.01;
	double far = 1000;
	double FOV = 60;
	double FOVr = FOV * (M_PI / 180);

	Mat default_position = Mat({
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	Mat position = Mat(
		{
			{0},
			{0},
			{0},
			{1}
		}, 4, 1
	);

	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	// Initial camera direction (not standard camera direction, only what it starts at, the default position is (0, 0, -1))
	Mat default_direction = Mat({
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	// Camera direction vector relative to world space
	Mat direction = Mat(
		{
		{0},
		{0},
		{1},
		{0}
		}, 4, 1
	);

	Mat default_up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat up = Mat(
		{
			{0},
			{1},
			{0},
			{0}
		}, 4, 1
	);

	Mat default_right = Mat(
		{
			{1},
			{0},
			{0},
			{0}
		}, 4, 1
	);

	Camera(double WIDTH = 800, double HEIGHT = 600, double near = 0.01, double far = 1000, double FOV_degrees = 60) {
		initialize(WIDTH, HEIGHT, near, far, FOV_degrees);
	}

	void initialize(double WIDTH = 800, double HEIGHT = 600, double near = 0.01, double far = 1000, double FOV_degrees = 60);

	void update_window_resized(double WIDTH = 800, double HEIGHT = 600);

	void update_projection_matrix();

	void update_view_inverse();

	void LookAt();
	void LookAt(const Mat& target_vector);

	static Mat LookAt(const Mat& position, const Mat& direction, const Mat& up);
	static Mat LookAt(const Mat& position, Mat& direction, const Mat& target_vector, Mat& up);


};