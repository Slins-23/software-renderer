#pragma once
#include "../Window.h"
#include "../../Scene.h"
#include "SceneTab.h"

class CameraTab : Window {
public:	
	SceneTab* scene_tab = nullptr;

	double camera_display_qx = 0;
	double camera_display_qy = 0;
	double camera_display_qz = 0;
	double camera_display_qw = 0;

	double camera_display_dir_x = 0;
	double camera_display_dir_y = 0;
	double camera_display_dir_z = 0;

	double camera_display_up_x = 0;
	double camera_display_up_y = 0;
	double camera_display_up_z = 0;

	double camera_display_tx = 0;
	double camera_display_ty = 0;
	double camera_display_tz = 0;

	CameraTab(SceneTab* scene_tab);

	void UpdateCameraTranslation(bool display_only);
	void UpdateCameraRotation(uint8_t rotation_type, bool display_only);
	void draw();
};