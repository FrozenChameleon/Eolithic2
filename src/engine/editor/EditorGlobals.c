#include "EditorGlobals.h"

#include "imgui.h"

bool EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered()
{
	if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
	{
		return true;
	}
	return false;
}
