#include "Mesh.h"
#include "Quaternion.h"
#pragma once

struct Instance {
private:

	void create_instance_nameid(uint32_t& total_instances);
public:
	std::string instance_name = "";
	uint32_t instance_id = 0;
	Mesh* mesh = nullptr;

	double tx = 0;
	double ty = 0;
	double tz = 0;

	Quaternion orientation = Quaternion(0, 0, 0, 1);

	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	double sx = 1;
	double sy = 1;
	double sz = 1;

	Mat SCALING_MATRIX = Mat::identity_matrix(4);
	Mat ROTATION_MATRIX = Mat::identity_matrix(4);
	Mat TRANSLATION_MATRIX = Mat::identity_matrix(4);

	Mat MODEL_TO_WORLD = Mat::identity_matrix(4);



	bool show = true;
	bool is_light_source = false;

	Instance() {};

	Instance(Mesh* mesh, double tx, double ty, double tz, Orientation rotation_orientation, const Quaternion& orientation, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances);

	Instance(Mesh* mesh, double tx, double ty, double tz, Orientation rotation_orientation, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances);

	Instance(Orientation rotation_orientation, Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances);

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances);

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, bool show, uint32_t& total_instances);

	Instance(Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, double yaw, double pitch, double roll, bool show, uint32_t& total_instances);

	Instance(Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances);

	Instance(std::string instance_name, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, const Quaternion& orientation, double yaw, double pitch, double roll, bool show, uint32_t& total_instances);

	Instance(std::string instance_name, Mesh* mesh, double tx, double ty, double tz, Orientation rotation_orientation, const Quaternion& orientation, double sx, double sy, double sz, bool show, uint32_t& total_instances);

	Instance(std::string instance_name, Orientation rotation_orientation, Mesh* mesh, double tx, double ty, double tz, double yaw, double pitch, double roll, double sx, double sy, double sz, bool show, uint32_t& total_instances);

	Instance(std::string instance_name, Orientation rotation_orientation, Mesh* mesh, Mat TRANSLATION_MATRIX, Mat ROTATION_MATRIX, Mat SCALING_MATRIX, bool show, uint32_t& total_instances);

	Instance(std::string instance_name, Mesh* mesh, Mat MODEL_TO_WORLD, bool show, uint32_t& total_instances);

	// Returns the vertex that represents the center point of the instance in world space
	Mat GetCenterVertex() const;
};