#include "EditorGlobals.h"

#include "imgui.h"

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
