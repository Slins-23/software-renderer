#pragma once
#include "Quaternion.h"
#include "Mesh.h"
#include "Instance.h"


enum LightType {
	directional,
	point,
	spotlight
};

enum ShadingType {
	Flat,
	Gouraud,
	Phong
};

struct Light {
	Light() {};

	LightType lighting_type = LightType::directional;
	ShadingType shading_type = ShadingType::Flat;

	Mat default_direction = Mat({ {0}, {0}, {1}, {0} }, 4, 1);
	Mat	default_up = Mat({ {0}, {1}, {0}, {0} }, 4, 1);

	// Assumes that both direction and up vectors are perpendicular, otherwise the resulting vector will not be of unit length and need to be normalized
	Mat default_right = Mat::CrossProduct3D(default_up, default_direction);

	// Should be the default world axes. I should eventually implement a constructor for this
	Mat position = Mat({ {0}, {0}, {0}, {1} }, 4, 1);
	Mat direction = default_direction;
	Mat up = default_up;

	double tx = 0;
	double ty = 0;
	double tz = 0;

	double yaw = 0;
	double pitch = 0;
	double roll = 0;

	Quaternion orientation;

	double intensity = 1;
	double minimum_exposure = 0.1;

	uint32_t color = 0x66285CFF;

	bool enabled = true;
	bool has_model = false;

	Mesh* mesh = nullptr;
	Instance* instance = nullptr;
};