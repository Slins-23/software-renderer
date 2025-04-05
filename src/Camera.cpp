#include "Camera.h"
void Camera::initialize(double WIDTH, double HEIGHT, double near, double far, double FOV_degrees) {
	this->near = near;
	this->far = far;
	this->FOV = FOV_degrees;
	this->FOVr = FOV_degrees * (M_PI / 180);

	update_window_resized(WIDTH, HEIGHT);
}

void Camera::update_window_resized(double WIDTH, double HEIGHT) {
	this->AR = (double)WIDTH / (double)HEIGHT;

	this->PROJECTION_MATRIX = Mat(
		{
			{(1 / (tan(FOVr / 2))), 0, 0, 0},
			{0, AR * (1 / (tan(FOVr / 2))), 0, 0},
			{0, 0, far / (far - near), (far * -near) / (far - near)},
			{0, 0, 1, 0}
		}
	, 4, 4);

	this->SCALE_MATRIX = Mat(
		{
			{WIDTH / 2., 0, 0, WIDTH / 2.},
			{0, HEIGHT / 2., 0, HEIGHT / 2.},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}, 4, 4);
}

void Camera::update_projection_matrix() {
	this->PROJECTION_MATRIX = Mat(
		{
			{(1 / (tan(this->FOVr / 2))), 0, 0, 0},
			{0, this->AR * (1 / (tan(this->FOVr / 2))), 0, 0},
			{0, 0, this->far / (this->far - this->near), (this->far * -this->near) / (this->far - this->near)},
			{0, 0, 1, 0}
		}
	, 4, 4);
}

/// <summary>
/// Updates the view matrix to look at the current camera direction vector in the current camera position, with the current camera up vector.
/// </summary>
void Camera::LookAt() {

	// Setting 4th dimension to 0 for dot product
	Mat position = this->position;
	position.set(0, 4, 1);

	Mat cam_z_axis = ((this->direction + position) - position);
	cam_z_axis.set(0, 4, 1);
	Mat cam_x_axis = Mat::CrossProduct3D(this->up, cam_z_axis);
	Mat cam_y_axis = Mat::CrossProduct3D(cam_z_axis, cam_x_axis);

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

	this->VIEW_MATRIX.set(-Mat::dot(cam_x_axis, position), 1, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_y_axis, position), 2, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_z_axis, position), 3, 4);

	this->VIEW_MATRIX.set(0, 4, 1);
	this->VIEW_MATRIX.set(0, 4, 2);
	this->VIEW_MATRIX.set(0, 4, 3);
	this->VIEW_MATRIX.set(1, 4, 4);
}

/// <summary>
/// Updates the view matrix to look at the target vector.
/// </summary>
/// <param name="target_vector">Vector to look at</param>
void Camera::LookAt(const Mat& target_vector) {

	// Setting 4th dimension to 0 for dot product
	Mat position = this->position;
	position.set(0, 4, 1);

	Mat tmp_target_vector = target_vector;
	tmp_target_vector.set(0, 4, 1);

	Mat cam_z_axis = target_vector - position;
	cam_z_axis.set(0, 4, 1);
	cam_z_axis.normalize();
	this->direction = cam_z_axis;

	Mat cam_x_axis = Mat::CrossProduct3D(up, cam_z_axis);
	Mat cam_y_axis = Mat::CrossProduct3D(cam_z_axis, cam_x_axis);
	this->up = cam_y_axis;

	this->VIEW_MATRIX.set(cam_x_axis.get(1, 1), 1, 1);
	this->VIEW_MATRIX.set(cam_x_axis.get(2, 1), 1, 2);
	this->VIEW_MATRIX.set(cam_x_axis.get(3, 1), 1, 3);

	this->VIEW_MATRIX.set(cam_y_axis.get(1, 1), 2, 1);
	this->VIEW_MATRIX.set(cam_y_axis.get(2, 1), 2, 2);
	this->VIEW_MATRIX.set(cam_y_axis.get(3, 1), 2, 3);

	this->VIEW_MATRIX.set(cam_z_axis.get(1, 1), 3, 1);
	this->VIEW_MATRIX.set(cam_z_axis.get(2, 1), 3, 2);
	this->VIEW_MATRIX.set(cam_z_axis.get(3, 1), 3, 3);

	this->VIEW_MATRIX.set(-Mat::dot(cam_x_axis, position), 1, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_y_axis, position), 2, 4);
	this->VIEW_MATRIX.set(-Mat::dot(cam_z_axis, position), 3, 4);

	this->VIEW_MATRIX.set(0, 4, 1);
	this->VIEW_MATRIX.set(0, 4, 2);
	this->VIEW_MATRIX.set(0, 4, 3);
	this->VIEW_MATRIX.set(1, 4, 4);
}

/// <summary>
/// Returns a view matrix which looks at the given camera direction vector in the given camera position, with the given camera up vector.
/// </summary>
/// <param name="position">Camera position vector</param>
/// <param name="direction">Camera direction vector</param>
/// <param name="up">Camera up vector</param>
/// <returns>A 4x4 view matrix looking at the camera direction in the camera position, with the camera up vector.</returns>
Mat Camera::LookAt(const Mat& position, const Mat& direction, const Mat& up) {

	// Setting 4th dimension to 0 for dot product
	Mat tmp_camera_position = position;
	tmp_camera_position.set(0, 4, 1);

	Mat cam_z_axis = ((direction + position) - tmp_camera_position);

	cam_z_axis.set(0, 4, 1);
	Mat cam_x_axis = Mat::CrossProduct3D(up, cam_z_axis);
	Mat cam_y_axis = Mat::CrossProduct3D(cam_z_axis, cam_x_axis);

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
/// <param name="position">Camera position vector</param>
/// <param name="direction">Camera direction vector</param>
/// <param name="target_vector">Vector to look at</param>
/// <param name="up">Camera up vector</param>
/// <returns>A 4x4 view matrix looking at the camera direction in the camera position, with the camera up vector.</returns>
Mat Camera::LookAt(const Mat& position, Mat& direction, const Mat& target_vector, Mat& up) {

	// Setting 4th dimension to 0 for dot product
	Mat tmp_camera_position = position;
	tmp_camera_position.set(0, 4, 1);
	Mat tmp_target_vector = target_vector;
	tmp_target_vector.set(0, 4, 1);

	Mat cam_z_axis = target_vector - tmp_camera_position;

	cam_z_axis.set(0, 4, 1);
	cam_z_axis.normalize();
	direction = cam_z_axis;

	Mat cam_x_axis = Mat::CrossProduct3D(up, cam_z_axis);
	Mat cam_y_axis = Mat::CrossProduct3D(cam_z_axis, cam_x_axis);
	up = cam_y_axis;

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

void Camera::update_view_inverse() {
	Mat VIEW_TRANSPOSED = this->VIEW_MATRIX.transposed();
	Mat VIEW_INVERSE_ROT = VIEW_TRANSPOSED;
	VIEW_INVERSE_ROT.set(0, 4, 1);
	VIEW_INVERSE_ROT.set(0, 4, 2);
	VIEW_INVERSE_ROT.set(0, 4, 3);

	Mat VIEW_INVERSE_TRANS = Mat::translation_matrix(-this->VIEW_MATRIX.get(1, 4), -this->VIEW_MATRIX.get(2, 4), -this->VIEW_MATRIX.get(3, 4));

	this->VIEW_INVERSE = VIEW_INVERSE_ROT * VIEW_INVERSE_TRANS;
}