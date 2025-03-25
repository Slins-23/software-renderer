#include "SettingsTab.h"

void SettingsTab::draw() {
	char fps_text[255];
	sprintf_s(fps_text, 255, "FPS: %.2lf", this->framerate);
	ImGui::Text(fps_text);
	ImGui::Spacing();

	ImGui::PushItemWidth(50);
	if (ImGui::DragScalar("##FPS Limit", ImGuiDataType_U8, &this->FPS_LIMIT, 0.1, &zero, nullptr, 0, ImGuiSliderFlags_None)) {
		this->MSPERFRAME = 1000.0 / this->FPS_LIMIT;
	}
	ImGui::SameLine();
	ImGui::Text("FPS Limit");
	
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##FPS calculation interval (in ms)", ImGuiDataType_Double, &this->fps_update_interval, 1.0f, &zero, nullptr, "%.0lf", ImGuiSliderFlags_None);
	ImGui::SameLine();
	ImGui::Text("FPS calculation interval (in ms)");
	


	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##Window opacity", ImGuiDataType_Double, this->general_window_opacity, 0.001f, &zero, &one, "%.2lf", ImGuiSliderFlags_None);
	ImGui::SameLine();
	ImGui::Text("Window opacity");

	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##Z-fighting tolerance", ImGuiDataType_Double, &this->z_fighting_tolerance, 0.0001, &zero, &one, "%.4f", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("Treshold for avoiding z-fighting during depth testing. (higher = more precision but more z-fighting)");
	ImGui::SameLine();
	ImGui::Text("Z-fighting tolerance");


	ImGui::Checkbox("##Wireframe", &this->wireframe_render);
	ImGui::SameLine();
	ImGui::Text("Wireframe");
	
	ImGui::Checkbox("##Rasterize", &this->rasterize);
	ImGui::SameLine();
	ImGui::Text("Rasterize");

	
	
	ImGui::Checkbox("##Shade", &this->shade);
	ImGui::SameLine();
	ImGui::Text("Shade");
	
	ImGui::Checkbox("##Depth test", &this->depth_test);
	ImGui::SameLine();
	ImGui::Text("Depth test");
	
	ImGui::Checkbox("##Backface cull", &this->backface_cull);
	ImGui::SameLine();
	ImGui::Text("Backface cull");
}
