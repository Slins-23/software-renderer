#include "Window.h"
#include "Tabs/SettingsTab.h"
#include "Tabs/SceneTab.h"

class GeneralWindow : public Window {
protected:
public:
	SettingsTab settings_tab = SettingsTab(&this->window_alpha);
	SceneTab scene_tab;

	GeneralWindow() {};

	void draw() {
		ImGui::SetNextWindowBgAlpha(this->window_alpha);
		ImGui::Begin("General window");

		if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_None)) settings_tab.draw();
		if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_None)) scene_tab.draw();

		ImGui::End();
	}
};