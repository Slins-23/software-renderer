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
	ImGui::SetItemTooltip("The FPS cap.");
	ImGui::SameLine();
	ImGui::Text("FPS Limit");
	
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##FPS calculation interval (in ms)", ImGuiDataType_Double, &this->fps_update_interval, 1.0f, &zero, nullptr, "%.0lf", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("How often, in milliseconds, does the framerate get averaged and updated.");
	ImGui::SameLine();
	ImGui::Text("FPS calculation interval (in ms)");

	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##Window opacity", ImGuiDataType_Double, this->general_window_opacity, 0.001f, &zero, &one, "%.2lf", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("How transparent the window should be.");
	ImGui::SameLine();
	ImGui::Text("Window opacity");

	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##Z-fighting tolerance", ImGuiDataType_Double, &this->z_fighting_tolerance, 0.0001, &zero, &one, "%.4f", ImGuiSliderFlags_None);
	ImGui::SetItemTooltip("Treshold for avoiding z-fighting during depth testing. (higher = more precision but more z-fighting)");
	ImGui::SameLine();
	ImGui::Text("Z-fighting tolerance");


	ImGui::Checkbox("##Wireframe", &this->wireframe_render);
	ImGui::SetItemTooltip("Enables/disables line drawing between the vertices in a triangle.");
	ImGui::SameLine();
	ImGui::Text("Wireframe");
	
	ImGui::Checkbox("##Rasterize", &this->rasterize);
	ImGui::SetItemTooltip("Whether to rasterize/fill the triangles/pixels with color.");
	ImGui::SameLine();
	ImGui::Text("Rasterize");

	
	
	ImGui::Checkbox("##Shade", &this->shade);
	ImGui::SetItemTooltip("When enabled alongside 'Rasterize', rasterization is done by taking into account scene colors and lighting.");
	ImGui::SameLine();
	ImGui::Text("Shade");
	
	ImGui::Checkbox("##Depth test", &this->depth_test);
	ImGui::SetItemTooltip("Enables/disables depth testing.");
	ImGui::SameLine();
	ImGui::Text("Depth test");
	
	ImGui::Checkbox("##Backface cull", &this->backface_cull);
	ImGui::SetItemTooltip("Enables/disables backface culling.");
	ImGui::SameLine();
	ImGui::Text("Backface cull");
}
