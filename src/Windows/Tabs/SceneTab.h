#pragma once
#include "../Window.h"
#include "../../Scene.h"
#include <nfd.h>

class CameraTab;
class InstancesTab;
class LightTab;

class SceneTab : public Window {
public:
	char models_folder[255] = "D:\\Programming\\Graphics\\Prototyping\\models\\";
	char scene_folder[255] = "D:\\Programming\\Graphics\\Prototyping\\scenes\\";
	char scene_load_name[255] = "hallway.json";
	char scene_save_name[255] = "tst.json";

	// Affects WASD translation
	double real_translation_speed = 0.01;

	// Affects mouse rotation
	double real_rotation_speed = 0.25;

	// Affects menu translation
	double menu_translation_speed = 0.001;

	// Affects menu rotation
	double menu_rotation_speed = 0.004;

	// Affects menu scaling
	double menu_scaling_speed = 0.003;

	Scene current_scene = Scene(scene_folder, scene_load_name, models_folder, rotation_orientation, update_camera_settings, true);

	Orientation rotation_orientation = Orientation::local;

	// Whether to use scene's camera settings (otherwise default to the default settings)
	bool update_camera_settings = true;

	bool is_instances_open = false;
	bool is_light_open = false;

	char display_rotation_orientation[255] = "Local";


	bool show_transform_axes = true;

	CameraTab* camera_tab = nullptr;
	InstancesTab* instances_tab = nullptr;
	LightTab* light_tab = nullptr;

	float display_BG_COLOR[4] = { static_cast<float>(((this->current_scene.BG_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.BG_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.BG_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((this->current_scene.BG_COLOR & 0x000000FF) / 255.0) };

	float display_LINE_COLOR[4] = { static_cast<float>(((this->current_scene.LINE_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.LINE_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.LINE_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((this->current_scene.LINE_COLOR & 0x000000FF) / 255.0) };

	float display_FILL_COLOR[4] = { static_cast<float>(((this->current_scene.FILL_COLOR >> 24) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.FILL_COLOR >> 16) & 0x000000FF) / 255.0), static_cast<float>(((this->current_scene.FILL_COLOR >> 8) & 0x000000FF) / 255.0), static_cast<float>((this->current_scene.FILL_COLOR & 0x000000FF) / 255.0) };

	bool verbose = true;

	SceneTab();

	void initialize();
	void load_scene(const char* scene_folder, const char* scene_filename, const char* models_folder, bool update_camera_settings, bool verbose);
	void load_scene();
	virtual void draw();

	~SceneTab();
};