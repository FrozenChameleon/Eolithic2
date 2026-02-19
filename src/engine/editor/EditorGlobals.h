#pragma once

#include "../utils/Macros.h"

#define EDITOR_GLOBALS_MENU_ITEM_SIZE 24

typedef enum EditorSelectionState
{
	EDITOR_SELECTION_STATE_NOTHING = 0,
    EDITOR_SELECTION_STATE_SELECTED = 1
} EditorSelectionState;

bool EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered();
