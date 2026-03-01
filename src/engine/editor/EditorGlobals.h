#pragma once

#include "../utils/Macros.h"

#define EDITOR_GLOBALS_MENU_ITEM_SIZE 24

typedef enum EditorSelectionState
{
	EDITOR_SELECTION_STATE_NOTHING = 0,
    EDITOR_SELECTION_STATE_SELECTING = 1,
	EDITOR_SELECTION_STATE_SELECTED = 2
} EditorSelectionState;

bool EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered();
void EditorGlobals_Update();
void EditorGlobals_StallOperationCounter();
uint64_t EditorGlobals_GetOperationCounter();
bool EditorGlobals_IsKeyCtrlPressedAndKeySTapped();