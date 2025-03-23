#include "SettingsTab.h"

void SettingsTab::draw() {
	char fps_text[255];
	sprintf_s(fps_text, 255, "FPS: %.2lf", this->framerate);
	ImGui::Text(fps_text);
	ImGui::Spacing();

	ImGui::Text("FPS calculation interval (in ms): ");
	ImGui::SameLine();
	ImGui::PushItemWidth(40);
	ImGui::DragScalar("##FPS calculation interval (in ms): ", ImGuiDataType_Double, &this->fps_update_interval, 1.0f, &zero, nullptr, "%.0lf", ImGuiSliderFlags_None);

	ImGui::Text("FPS Limit:");
	ImGui::SameLine();
	if (ImGui::DragScalar("##FPS Limit:", ImGuiDataType_U8, &this->FPS_LIMIT, 1, &zero, nullptr, 0, ImGuiSliderFlags_None)) {
		this->MSPERFRAME = 1000.0 / this->FPS_LIMIT;
	}

	ImGui::Text("Window opacity:");
	ImGui::SameLine();
	ImGui::PushItemWidth(40);
	ImGui::DragScalar("##Window opacity:", ImGuiDataType_Double, this->general_window_opacity, 0.001f, &zero, &one, "%.2lf", ImGuiSliderFlags_None);

	ImGui::Text("Wireframe:");
	ImGui::SameLine();
	ImGui::Checkbox("##Wireframe", &this->wireframe_render);

	ImGui::Text("Rasterize:");
	ImGui::SameLine();
	ImGui::Checkbox("##Rasterize", &this->rasterize);

	ImGui::Text("Shade:");
	ImGui::SameLine();
	ImGui::Checkbox("##Shade", &this->shade);

	ImGui::Text("Depth test:");
	ImGui::SameLine();
	ImGui::Checkbox("##Depth test", &this->depth_test);

	ImGui::Text("Backface cull:");
	ImGui::SameLine();
	ImGui::Checkbox("##Backface cull", &this->backface_cull);

	ImGui::Text("Background color:");
	ImGui::SameLine();
	if (ImGui::ColorEdit4("##BGColor", this->display_BG_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_BG_COLOR[0] * 255.0;
		uint8_t green = display_BG_COLOR[1] * 255.0;
		uint8_t blue = display_BG_COLOR[2] * 255.0;
		uint8_t alpha = display_BG_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->BG_COLOR = color;
	};

	ImGui::Text("Line/wireframe color:");
	ImGui::SameLine();
	if (ImGui::ColorEdit4("##LColor", this->display_LINE_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_LINE_COLOR[0] * 255.0;
		uint8_t green = display_LINE_COLOR[1] * 255.0;
		uint8_t blue = display_LINE_COLOR[2] * 255.0;
		uint8_t alpha = display_LINE_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->LINE_COLOR = color;
	};

	ImGui::Text("Fill/ambient color:");
	ImGui::SameLine();
	if (ImGui::ColorEdit4("##FColor", this->display_FILL_COLOR, ImGuiColorEditFlags_NoInputs)) {
		uint8_t red = display_FILL_COLOR[0] * 255.0;
		uint8_t green = display_FILL_COLOR[1] * 255.0;
		uint8_t blue = display_FILL_COLOR[2] * 255.0;
		uint8_t alpha = display_FILL_COLOR[3] * 255.0;

		uint32_t color = (red << 24) | (green << 16) | (blue << 8) | alpha;

		this->FILL_COLOR = color;
	};

	ImGui::Text("Z fighting tolerance:");
	ImGui::SetItemTooltip("This tolerance value sets a treshold for avoiding z-fighting during depth testing. (higher = more precision but more z-fighting)");
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("##Z-fighting tolerance", ImGuiDataType_Double, &this->z_fighting_tolerance, 0.001, &zero, &one, "%.4f", ImGuiSliderFlags_None);
}
