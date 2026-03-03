#include "EditorGlobals.h"

#include "imgui.h"
#include "../input/Input.h"
#include "../utils/Cvars.h"

static uint64_t _mOperationCounter;
static bool _mStallOperationCounter;

bool EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered()
{
	if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
	{
		return true;
	}
	return false;
}
void EditorGlobals_Update()
{
	if (!_mStallOperationCounter)
	{
		_mOperationCounter += 1;
	}

	_mStallOperationCounter = false;
}
void EditorGlobals_StallOperationCounter()
{
	_mStallOperationCounter = true;
}
uint64_t EditorGlobals_GetOperationCounter()
{
	return _mOperationCounter;
}
bool EditorGlobals_IsKeyCtrlPressedAndKeySTapped()
{
	if (Input_IsCtrlPressed() && Input_IsKeyTapped(KEYS_S))
	{
		return true;
	}
	return false;
}
bool EditorGlobals_CvarCheckbox(const char* label, const char* cvar)
{
	bool value = Cvars_GetAsBool(cvar);
	if (ImGui::Checkbox(label, &value))
	{
		Cvars_SetAsBool(cvar, value);
		return true;
	}
	return false;
}
