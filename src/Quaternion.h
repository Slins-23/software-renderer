#include "Matrix.h"
#include "Utils.h"
#pragma once

struct Quaternion {
	double x = 0;
	double y = 0;
	double z = 0;
	double w = 1;

	Quaternion();

	Quaternion(double x, double y, double z, double w);

	// Sets yaw, pitch, and roll from quaternion
	void GetAngles(double& yaw, double& pitch, double& roll) const;

	static void GetAnglesFromDirection(const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll);

	static Quaternion AngleAxis(double x, double y, double z, double angle);

	static Quaternion FromYawPitchRoll(double yaw, double pitch, double roll);

	static Mat RotatePoint(const Mat& point, const Mat& axis, double angle, bool is_position);

	static Mat RotatePoint(const Mat& point, double axis_x, double axis_y, double axis_z, double angle, bool is_position);

	static Mat RotatePoint(double point_x, double point_y, double point_z, double axis_x, double axis_y, double axis_z, double angle, bool is_position);

	static Mat RotatePoint(double point_x, double point_y, double point_z, const Mat& axis, double angle, bool is_position);

	Mat get_3dvector(bool is_position) const;

	Mat get_4dvector() const;

	Mat get_rotationmatrix() const;

	Quaternion get_complexconjugate() const;

	double get_angle() const;

	double get_magnitude() const;

	void normalize();

	Quaternion operator+(const Quaternion& other);

	Quaternion operator-(const Quaternion& other);

	Quaternion operator*(const Quaternion& other);
};