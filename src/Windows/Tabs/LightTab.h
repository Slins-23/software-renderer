#pragma once
#include "../../Utils.h"
#include "../Window.h"
#include "SceneTab.h"
#include <nfd.h>

class LightTab : Window {
public:
	SceneTab* scene_tab = nullptr;

	double light_display_qx = 0;
	double light_display_qy = 0;
	double light_display_qz = 0;
	double light_display_qw = 0;

	double light_display_dir_x = 0;
	double light_display_dir_y = 0;
	double light_display_dir_z = 0;

	double light_display_up_x = 0;
	double light_display_up_y = 0;
	double light_display_up_z = 0;

	float display_LIGHT_COLOR[4] = { 0, 0, 0, 0 };

	const char* lighting_types[3] = { "Directional", "Point", "Spotlight" };
	const char* current_lighting_type = "Directional";
	uint8_t lighting_selected_idx = 0;

	const char* shading_types[3] = { "Flat", "Gouraud", "Phong" };
	const char* current_shading_type = "Flat";
	uint8_t shading_selected_idx = 0;

	std::string chosen_light_mesh_name = "";
	uint32_t chosen_light_mesh_id = 0;

	LightTab(SceneTab* scene_tab);

	void UpdateLightTranslation();
	void UpdateLightScaling();
	void UpdateLightRotation(uint8_t rotation_type);
	void update_transform_axes();
	void draw();
};
