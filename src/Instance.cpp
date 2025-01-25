#include "Instance.h"

void Instance::create_instance_nameid(uint32_t& total_instances) {
	std::string final_string = std::string(mesh->mesh_filename);
	std::string new_id = "_";

	this->instance_id = total_instances;

	if (this->instance_id < 10) {
		new_id += "00";
	}
	else if (this->instance_id < 100) {
		new_id += "0";
	}

	new_id += std::to_string(instance_id);

	final_string.replace((final_string.length() - 4), (final_string.length() - 1), new_id);

	this->instance_name = final_string;

	total_instances++;
}

Instance::Instance(Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;

	this->orientation = orientation;
	orientation.GetAngles(this->yaw, this->pitch, this->roll);
	this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();

	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;

	this->orientation = orientation;
	orientation.GetAngles(this->yaw, this->pitch, this->roll);
	this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();

	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;

	Quaternion orientation = Quaternion::FromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->orientation = orientation;

	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->ROTATION_MATRIX = this->orientation.get_rotationmatrix();
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;



	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
	this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
	this->ROTATION_MATRIX = ROTATION_MATRIX;
	this->SCALING_MATRIX = SCALING_MATRIX;
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = TRANSLATION_MATRIX.get(1, 4);
	this->ty = TRANSLATION_MATRIX.get(2, 4);
	this->tz = TRANSLATION_MATRIX.get(3, 4);

	this->orientation = orientation;

	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll,

		this->sx = SCALING_MATRIX.get(1, 1);
	this->sy = SCALING_MATRIX.get(2, 2);
	this->sz = SCALING_MATRIX.get(3, 3);

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}


Instance::Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, bool show, uint32_t& total_instances) {
	this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
	this->ROTATION_MATRIX = ROTATION_MATRIX;
	this->SCALING_MATRIX = SCALING_MATRIX;
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = TRANSLATION_MATRIX.get(1, 4);
	this->ty = TRANSLATION_MATRIX.get(2, 4);
	this->tz = TRANSLATION_MATRIX.get(3, 4);

	this->orientation = orientation;

	this->sx = SCALING_MATRIX.get(1, 1);
	this->sy = SCALING_MATRIX.get(2, 2);
	this->sz = SCALING_MATRIX.get(3, 3);

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
	this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
	this->ROTATION_MATRIX = ROTATION_MATRIX;
	this->SCALING_MATRIX = SCALING_MATRIX;
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;

	this->tx = TRANSLATION_MATRIX.get(1, 4);
	this->ty = TRANSLATION_MATRIX.get(2, 4);
	this->tz = TRANSLATION_MATRIX.get(3, 4);

	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;

	this->sx = SCALING_MATRIX.get(1, 1);
	this->sy = SCALING_MATRIX.get(2, 2);
	this->sz = SCALING_MATRIX.get(3, 3);

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances) {
	this->MODEL_TO_WORLD = MODEL_TO_WORLD;
	this->mesh = mesh;
	this->show = show;

	this->mesh = mesh;
	this->show = show;

	this->create_instance_nameid(total_instances);
}

Instance::Instance(std::string instance_name, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances) {
	this->orientation = orientation;
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;

	this->tx = TRANSLATION_MATRIX.get(1, 4);
	this->ty = TRANSLATION_MATRIX.get(2, 4);
	this->tz = TRANSLATION_MATRIX.get(3, 4);

	this->sx = SCALING_MATRIX.get(1, 1);
	this->sy = SCALING_MATRIX.get(2, 2);
	this->sz = SCALING_MATRIX.get(3, 3);

	this->instance_name = instance_name;
	this->mesh = mesh;
	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->ROTATION_MATRIX = orientation.get_rotationmatrix();
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
	this->show = show;
	this->instance_id = total_instances;



	//aQuaternion_GetAnglesFromQuaternionYP(orientation, this->yaw, this->pitch, this->roll);



	total_instances++;
}

Instance::Instance(std::string instance_name, Mesh* mesh, double tx, double ty, double tz, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
	this->orientation = orientation;
	this->instance_name = instance_name;
	this->mesh = mesh;
	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->ROTATION_MATRIX = orientation.get_rotationmatrix();
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
	this->show = show;
	this->instance_id = total_instances;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;

	orientation.GetAngles(this->yaw, this->pitch, this->roll);

	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	total_instances++;
}

Instance::Instance(std::string instance_name, Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances) {
	Quaternion orientation = Quaternion::FromYawPitchRoll(yaw, pitch, roll);
	this->orientation = orientation;

	this->instance_name = instance_name;
	this->mesh = mesh;
	this->TRANSLATION_MATRIX = Mat::translation_matrix(tx, ty, tz);
	//this->ROTATION_MATRIX = aquaternion_rotationZ_matrix(roll) * aquaternion_rotationX_matrix(pitch) * aquaternion_rotationY_matrix(yaw);
	this->ROTATION_MATRIX = orientation.get_rotationmatrix();
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->SCALING_MATRIX = Mat::scale_matrix(sx, sy, sz);
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
	this->show = show;
	this->instance_id = total_instances;

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;

	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;

	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	total_instances++;
}

Instance::Instance(std::string instance_name, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, bool show, uint32_t& total_instances) {
	this->instance_name = instance_name;
	this->mesh = mesh;
	this->TRANSLATION_MATRIX = TRANSLATION_MATRIX;
	this->ROTATION_MATRIX = ROTATION_MATRIX;
	this->SCALING_MATRIX = SCALING_MATRIX;
	this->MODEL_TO_WORLD = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX;
	this->show = show;
	this->instance_id = total_instances;

	double tx = TRANSLATION_MATRIX.get(1, 4);
	double ty = TRANSLATION_MATRIX.get(2, 4);
	double tz = TRANSLATION_MATRIX.get(3, 4);

	this->tx = tx;
	this->ty = ty;
	this->tz = tz;

	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;

	Quaternion orientation = Quaternion::FromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->orientation = orientation;

	double sx = SCALING_MATRIX.get(1, 1);
	double sy = SCALING_MATRIX.get(2, 2);
	double sz = SCALING_MATRIX.get(3, 3);

	this->sx = sx;
	this->sy = sy;
	this->sz = sz;

	total_instances++;
}

Instance::Instance(std::string instance_name, Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances) {
	this->instance_name = instance_name;
	this->mesh = mesh;
	this->MODEL_TO_WORLD = MODEL_TO_WORLD;
	this->show = show;
	this->instance_id = total_instances;

	total_instances++;
}

Mat Instance::GetCenterVertex() const {
	Mat center = Mat({ {0}, {0}, {0}, {1} }, 4, 1);

	double min_x = std::numeric_limits<double>::max();
	double max_x = std::numeric_limits<double>::lowest();
	double min_y = std::numeric_limits<double>::max();
	double max_y = std::numeric_limits<double>::lowest();
	double min_z = std::numeric_limits<double>::max();
	double max_z = std::numeric_limits<double>::lowest();

	for (const Mat& vertex : this->mesh->vertices) {
		Mat current_instance_vertex = this->TRANSLATION_MATRIX * this->ROTATION_MATRIX * this->SCALING_MATRIX * vertex;
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