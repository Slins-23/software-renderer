#include "Quaternion.h"

Quaternion::Quaternion() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
}

Quaternion::Quaternion(double x, double y, double z, double w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


void Quaternion::GetAngles(Orientation orientation, double& yaw, double& pitch, double& roll) const {
	Mat rotation_matrix = this->get_rotationmatrix();

	// Intrinsic rotation
	if (orientation == Orientation::local) {
		
		//pitch = asin(-Utils::clamp(rotation_matrix.get(2, 3), -1, 1));
		pitch = asin(-rotation_matrix.get(2, 3));

		if (abs(pitch) < 0.9999999) {
			yaw = atan2(rotation_matrix.get(1, 3), rotation_matrix.get(3, 3));
			roll = atan2(rotation_matrix.get(2, 1), rotation_matrix.get(2, 2));
		}
		else {
			yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
			roll = 0;
		}
	}

	// For extrinsic/world orientation, we need to adjust the angles
	if (orientation == Orientation::world) {
		//pitch = asin(-Utils::clamp(rotation_matrix.get(2, 3), -1, 1));
		pitch = asin(rotation_matrix.get(3, 2));

		if (abs(pitch) < 0.9999999) {
			yaw = atan2(rotation_matrix.get(3, 1), rotation_matrix.get(3, 3));
		}
		else {
			yaw = atan2(-rotation_matrix.get(3, 1), rotation_matrix.get(1, 1));
			roll = 0;
		}
	}
}

// Sets yaw, pitch, and roll from a direction vector (the displacement in each axis from the default direction vector)
void Quaternion::GetAnglesFromDirection(Orientation orientation, const Mat& default_direction_vector, const Mat& direction_vector, double& yaw, double& pitch, double& roll) {
	const Mat normalized_v1 = default_direction_vector / default_direction_vector.norm();
	const Mat normalized_v2 = direction_vector / direction_vector.norm();

	Mat rotation_axis = Mat::CrossProduct3D(normalized_v1, normalized_v2);
	rotation_axis.normalize();

	double rotation_angle = acos(Mat::dot(normalized_v1, normalized_v2));
	const Quaternion rotation = Quaternion::AngleAxis(rotation_axis.get(1, 1), rotation_axis.get(2, 1), rotation_axis.get(3, 1), rotation_angle);

	rotation.GetAngles(orientation, yaw, pitch, roll);
}

Quaternion Quaternion::AngleAxis(double x, double y, double z, double angle) {
	double q_x = sin(angle / 2) * x;
	double q_y = sin(angle / 2) * y;
	double q_z = sin(angle / 2) * z;
	double q_w = cos(angle / 2);

	return Quaternion(q_x, q_y, q_z, q_w);
}

// Defined in default coordinate system (i.e. positive x-axis to the right, positive y-axis up, positive z-axis outward/out of the screen)
Quaternion Quaternion::FromYawPitchRoll(Orientation orientation, double yaw, double pitch, double roll, const Mat& default_x_axis, const Mat& default_y_axis, const Mat& default_z_axis) {
	Quaternion rotation;

	if (orientation == Orientation::world) {
		Quaternion rotationY = Quaternion::AngleAxis(default_y_axis.get(1, 1), default_y_axis.get(2, 1), default_y_axis.get(3, 1), yaw);
		Quaternion rotationX = Quaternion::AngleAxis(default_x_axis.get(1, 1), default_x_axis.get(2, 1), default_x_axis.get(3, 1), pitch);
		Quaternion rotationZ = Quaternion::AngleAxis(default_z_axis.get(1, 1), default_z_axis.get(2, 1), default_z_axis.get(3, 1), roll);

		rotation = rotationZ * rotationX * rotationY;
	}
	else if (orientation == Orientation::local) {
		Mat y_axis = default_y_axis;
		Mat x_axis = default_x_axis;
		Mat z_axis = default_z_axis;

		Quaternion rotationY = Quaternion::AngleAxis(y_axis.get(1, 1), y_axis.get(2, 1), y_axis.get(3, 1), yaw);

		// Rotated x and z axes
		x_axis = Quaternion::RotatePoint(x_axis, y_axis, yaw, false);
		z_axis = Quaternion::RotatePoint(z_axis, y_axis, yaw, false);

		Quaternion rotationX = Quaternion::AngleAxis(x_axis.get(1, 1), x_axis.get(2, 1), x_axis.get(3, 1), pitch);

		// Rotated y and z axes
		y_axis = Quaternion::RotatePoint(y_axis, x_axis, pitch, false);
		z_axis = Quaternion::RotatePoint(z_axis, x_axis, pitch, false);

		Quaternion rotationZ = Quaternion::AngleAxis(z_axis.get(1, 1), z_axis.get(2, 1), z_axis.get(3, 1), roll);

		rotation = rotationZ * rotationX * rotationY;
	}
	return rotation;
}

Mat Quaternion::RotatePoint(const Mat& point, const Mat& axis, double angle, bool is_position) {
	Quaternion q = Quaternion::AngleAxis(axis.get(1, 1), axis.get(2, 1), axis.get(3, 1), angle);
	Quaternion q_point = Quaternion(point.get(1, 1), point.get(2, 1), point.get(3, 1), 0);
	Quaternion q_conjugate = q.get_complexconjugate();

	Quaternion rotated_point = q * q_point * q_conjugate;
	Mat final_point = rotated_point.get_4dvector();

	double fourth_dimension = is_position ? 1 : 0;
	final_point.set(fourth_dimension, 4, 1);

	return final_point;
}

Mat Quaternion::RotatePoint(const Mat& point, double axis_x, double axis_y, double axis_z, double angle, bool is_position) {
	const Mat axis = Mat({ {axis_x}, {axis_y}, {axis_z}, {0} }, 4, 1);

	return Quaternion::RotatePoint(point, axis, angle, is_position);
}

Mat Quaternion::RotatePoint(double point_x, double point_y, double point_z, double axis_x, double axis_y, double axis_z, double angle, bool is_position) {
	const Mat point = Mat({ {point_x}, {point_y}, {point_z}, {0} }, 4, 1);
	const Mat axis = Mat({ {axis_x}, {axis_y}, {axis_z}, {0} }, 4, 1);

	return Quaternion::RotatePoint(point, axis, angle, is_position);
}

Mat Quaternion::RotatePoint(double point_x, double point_y, double point_z, const Mat& axis, double angle, bool is_position) {
	const Mat point = Mat({ {point_x}, {point_y}, {point_z}, {0} }, 4, 1);

	return Quaternion::RotatePoint(point, axis, angle, is_position);
}

void Quaternion::RotatePoint(Quaternion& rotation, Mat& point, bool is_position) {
	double fourth_dimension = is_position ? 1 : 0;
	Quaternion q_point = Quaternion(point.get(1, 1), point.get(2, 1), point.get(3, 1), 0);
	Quaternion q_conjugate = rotation.get_complexconjugate();

	Quaternion rotated_point = rotation * q_point * q_conjugate;
	point = rotated_point.get_4dvector();

	point.set(fourth_dimension, 4, 1);
}

Mat Quaternion::get_3dvector(bool is_position) const {
	double w_component = is_position ? 1 : 0;

	Mat vector_form = Mat(
		{
			{x},
			{y},
			{z},
			{w_component}
		}
	, 4, 1);

	return vector_form;
}

Mat Quaternion::get_4dvector() const {
	Mat vector_form = Mat(
		{
			{x},
			{y},
			{z},
			{w}
		}
	, 4, 1);

	return vector_form;
}

Mat Quaternion::get_rotationmatrix() const {
	Mat rotation_matrix = Mat(
		{
		   {1 - 2 * (this->y * this->y + this->z * this->z), 2 * (this->x * this->y - this->w * this->z), 2 * (this->x * this->z + this->w * this->y), 0},
			{2 * (this->x * this->y + this->w * this->z), 1 - 2 * (this->x * this->x + this->z * this->z), 2 * (this->y * this->z - this->w * this->x), 0},
			{2 * (this->x * this->z - this->w * this->y), 2 * (this->y * this->z + this->w * this->x), 1 - 2 * (this->x * this->x + this->y * this->y), 0},
			{0, 0, 0, 1}
		}
	, 4, 4);

	return rotation_matrix;
}

Quaternion Quaternion::get_complexconjugate() const {
	Quaternion result = Quaternion(-this->x, -this->y, -this->z, this->w);

	return result;
}

double Quaternion::get_angle() const {
	double angle = 2 * acos(this->w);
	return angle;
}

double Quaternion::get_magnitude() const {
	return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Make quaternion a unit quaternion
void Quaternion::normalize() {
	double length = this->get_magnitude();

	this->x /= length;
	this->y /= length;
	this->z /= length;
	this->w /= length;
}

Quaternion Quaternion::operator+(const Quaternion& other) {
	return Quaternion(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
}

Quaternion Quaternion::operator-(const Quaternion& other) {
	return Quaternion(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
}

Quaternion Quaternion::operator*(const Quaternion& other) {
	Quaternion result = Quaternion();
	result.x = (this->x * other.w) + (this->y * other.z) - (this->z * other.y) + (this->w * other.x);
	result.y = (this->y * other.w) + (this->z * other.x) + (this->w * other.y) - (this->x * other.z);
	result.z = (this->z * other.w) + (this->w * other.z) + (this->x * other.y) - (this->y * other.x);
	result.w = (this->w * other.w) - (this->x * other.x) - (this->y * other.y) - (this->z * other.z);

	return result;
}

void Quaternion::GetRoll(Orientation orientation, const Mat& direction, const Mat& up, double& yaw, const double& pitch, double& roll) {
	if (orientation == Orientation::local) {
		if (abs(direction.get(2, 1)) > 0.9999999) {
			// In gimbal lock - collapse roll into yaw
			roll = 0.0;
		}
		else {
			// Normal case - extract roll from up vector
			roll = -atan2(up.get(1, 1) * cos(yaw) - up.get(3, 1) * sin(yaw), up.get(2, 1) / cos(pitch));
		}


		//roll = -atan2(new_vector.get(1, 1) * cos(yaw) - new_vector.get(3, 1) * sin(yaw), new_vector.get(2, 1) / cos(pitch));
	}

	if (orientation == Orientation::world) {
		if (abs(direction.get(2, 1)) > 0.9999999) {
			// In gimbal lock - collapse roll into yaw
			roll = 0.0;
		}
		else {
			// Normal case - extract roll from up vector
			roll = -atan2(up.get(1, 1) * cos(yaw) - up.get(3, 1) * sin(yaw), up.get(2, 1) / cos(pitch));
		}
	}
}