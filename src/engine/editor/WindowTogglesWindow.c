#include "WindowTogglesWindow.h"

#include "imgui.h"
#include "EditorGlobals.h"
#include "Editor.h"
#include "../utils/MString.h"
#include "../utils/Cvars.h"

static bool _mIsActive;

static void AddLayerToggle(int i)
{
	MString* tempName = MString_CreateForJustThisFrame();
	MString_AssignString(&tempName, "Layer ");
	MString_AddAssignInt(&tempName, i);

	MString* tempCvar = MString_CreateForJustThisFrame();
	MString_AssignString(&tempCvar, CVARS_EDITOR_SHOW_LAYER);
	MString_AddAssignInt(&tempCvar, i);

	EditorGlobals_CvarCheckbox(MString_Text(tempName), MString_Text(tempCvar));
}
static void CreateTogglesWindow()
{
	//560, 700
	if (!ImGui::Begin(EDITOR_KEY_WINDOW_TOGGLES, &_mIsActive))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(2);

	EditorGlobals_CvarCheckbox("Copy Collision?", CVARS_EDITOR_COPY_COLLISION);
	EditorGlobals_CvarCheckbox("Copy Tiles?", CVARS_EDITOR_COPY_TILES);
	EditorGlobals_CvarCheckbox("Copy Layer Only?", CVARS_EDITOR_COPY_LAYER_ONLY);
	EditorGlobals_CvarCheckbox("Copy Things?", CVARS_EDITOR_COPY_THINGS);
	EditorGlobals_CvarCheckbox("Copy Props?", CVARS_EDITOR_COPY_PROPS);
#ifndef EDITOR_DEF_DISABLE_COPY_CAMERA_BUTTON
	EditorGlobals_CvarCheckbox("Copy Cameras?", CVARS_EDITOR_COPY_CAMERAS);
#endif
	EditorGlobals_CvarCheckbox("Show Lines?", CVARS_EDITOR_SHOW_LINES);
	EditorGlobals_CvarCheckbox("Show Collision? (Num8)", CVARS_EDITOR_SHOW_COLLISION);
	EditorGlobals_CvarCheckbox("Show Tiles? (Num7)", CVARS_EDITOR_SHOW_TILES);
	EditorGlobals_CvarCheckbox("Show Things? (Num6)", CVARS_EDITOR_SHOW_THINGS);
	EditorGlobals_CvarCheckbox("Show Props? (Num5)", CVARS_EDITOR_SHOW_PROPS);

	EditorGlobals_CvarCheckbox("Show Game? (Ctrl+Num5)", CVARS_EDITOR_SHOW_GAME);
	EditorGlobals_CvarCheckbox("Suspend Game? (Ctrl+Num6)", CVARS_EDITOR_SUSPEND_GAME);
	EditorGlobals_CvarCheckbox("Show Grid? (Ctrl+Num7)", CVARS_EDITOR_SHOW_GRID);
	EditorGlobals_CvarCheckbox("Show Single Selected Square?", CVARS_EDITOR_SHOW_SINGLE_SELECTED_SQUARE);
	EditorGlobals_CvarCheckbox("Use Game Cam? (Ctrl+Num8)", CVARS_EDITOR_USE_GAME_CAMERA);
	EditorGlobals_CvarCheckbox("Editor Autosave?", CVARS_EDITOR_AUTO_SAVE);
	EditorGlobals_CvarCheckbox("Delete Collision?", CVARS_EDITOR_DELETE_COLLISION);
	EditorGlobals_CvarCheckbox("Delete Tiles?", CVARS_EDITOR_DELETE_TILES);
	EditorGlobals_CvarCheckbox("Delete Things?", CVARS_EDITOR_DELETE_THINGS);
	EditorGlobals_CvarCheckbox("Delete Props?", CVARS_EDITOR_DELETE_PROPS);

	EditorGlobals_CvarCheckbox("Magenta?", CVARS_EDITOR_USE_MAGENTA);
	EditorGlobals_CvarCheckbox("Strict Camera?", CVARS_EDITOR_STRICT_CAMERA);
	EditorGlobals_CvarCheckbox("Show Tile Info?", CVARS_EDITOR_SHOW_TILE_INFO);
	EditorGlobals_CvarCheckbox("Show Camera?", CVARS_EDITOR_SHOW_CAMERA);

	ImGui::NextColumn();

	for (int i = 0; i < 10; i += 1)
	{
		AddLayerToggle(i);
	}
	if (ImGui::Button("Show All", ImVec2(100, 25)))
	{
		for (int i = 0; i < 10; i += 1)
		{
			MString* tempString = MString_CreateForJustThisFrame();
			MString_AssignString(&tempString, CVARS_EDITOR_SHOW_LAYER);
			MString_AddAssignInt(&tempString, i);
			Cvars_SetAsBool(MString_Text(tempString), true);
		}
	}

	ImGui::End();
}
void WindowTogglesWindow_Update()
{
	if (!_mIsActive)
	{
		return;
	}

	CreateTogglesWindow();
}
void WindowTogglesWindow_Activate()
{
	_mIsActive = true;
}
