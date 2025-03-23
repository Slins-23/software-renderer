#pragma once
#include "../Window.h"
#include "SceneTab.h"

class InstancesTab : Window {
public:
	//Orientation* rotation_orientation = nullptr;

	SceneTab* scene_tab = nullptr;

	Instance* target_instance = nullptr;
	//uint32_t target_instance_idx = 0;

	double display_qx = 0;
	double display_qy = 0;
	double display_qz = 0;
	double display_qw = 1;

	std::string chosen_instance_name = "";
	uint32_t chosen_instance_id = 0;

	std::string chosen_mesh_name = "";
	uint32_t chosen_mesh_id = 0;

	Instance* created_instance = nullptr;
	char create_instance_name[255] = "";

	InstancesTab(SceneTab* scene_tab);

	void UpdateInstanceTranslation();
	void UpdateInstanceScaling();
	void UpdateInstanceRotation(uint8_t rotation_type);
	void update_transform_axes();
	void update_instances_queue_reconstruction(bool selected_deleted);
	void draw();
};
