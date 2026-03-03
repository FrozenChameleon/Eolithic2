#include "WindowTogglesWindowMini.h"

#include "../utils/Macros.h"
#include "../utils/Cvars.h"
#include "imgui.h"
#include "EditorGlobals.h"
#include "Editor.h"
#include "../core/GameHelper.h"
#include "../gamestate/GameStateManager.h"
#include "../input/Input.h"

static bool _mIsActive;
static bool _mHasLoadedWindowFontScale;
static bool _mHasWindowFontScaleChanged;
static bool _mForceInvertCtrl;
static float _mWindowFontScale;

static void CreateTogglesMiniWindow()
{
    //, 200, 220
	if (!ImGui::Begin(EDITOR_KEY_WINDOW_TOGGLES_MINI, &_mIsActive))
	{
        return;
    }
	EditorGlobals_CvarCheckbox("Copy Collision?", CVARS_EDITOR_COPY_COLLISION);
	EditorGlobals_CvarCheckbox("Copy Tiles?", CVARS_EDITOR_COPY_TILES);
	EditorGlobals_CvarCheckbox("Copy Layer Only?", CVARS_EDITOR_COPY_LAYER_ONLY);
	EditorGlobals_CvarCheckbox("Copy Things?", CVARS_EDITOR_COPY_THINGS);
	EditorGlobals_CvarCheckbox("Copy Props?", CVARS_EDITOR_COPY_PROPS);
#ifndef EDITOR_DEF_DISABLE_COPY_CAMERA_BUTTON
	EditorGlobals_CvarCheckbox("Copy Cameras?", CVARS_EDITOR_COPY_CAMERAS);
#endif
	EditorGlobals_CvarCheckbox("Tile Info?", CVARS_EDITOR_SHOW_TILE_INFO);
	EditorGlobals_CvarCheckbox("Show Game?", CVARS_EDITOR_SHOW_GAME);
	EditorGlobals_CvarCheckbox("Suspend Game?", CVARS_EDITOR_SUSPEND_GAME);
	if (ImGui::Button("Next Level", ImVec2(125, 20)))
    {
        const char* nextLevel = GameHelper_DebugGiveNextLevel();
        if (nextLevel != NULL)
        {
            GameStateManager_DebugForceLoadMapNow(nextLevel);
        }
    }
	if (ImGui::Button("Previous Level", ImVec2(125, 20)))
    {
        Editor_LoadPreviousLevel();
    }
	ImGui::Checkbox("Invert Ctrl Mode?", &_mForceInvertCtrl);
    float currentWindowFontScale = _mWindowFontScale;
    ImGui::InputFloat("Window Font Scale", &_mWindowFontScale, 0.1f);
    if (currentWindowFontScale != _mWindowFontScale)
    {
        _mHasWindowFontScaleChanged = true;
    }
    //ImGui::SliderFloat("Window Font Scale", ref _mWindowFontScale, 1, 3);
    ImGui::End();
}

void WindowTogglesWindowMini_Update()
{
    if (!_mIsActive)
    {
        return;
    }

    if (!_mHasLoadedWindowFontScale)
    {
        _mWindowFontScale = Cvars_GetAsFloat(CVARS_EDITOR_WINDOW_FONT_SCALE);
        if (_mWindowFontScale <= 0)
        {
            _mWindowFontScale = 1.0f;
        }
        _mHasLoadedWindowFontScale = true;
        _mHasWindowFontScaleChanged = true;
    }

	Input_SetHackForceInvertCtrl(_mForceInvertCtrl);

    CreateTogglesMiniWindow();

    if (_mHasWindowFontScaleChanged)
    {
        ImGui::GetIO().FontGlobalScale = _mWindowFontScale; // Scale all fonts by 1.5x
        Cvars_SetAsFloat(CVARS_EDITOR_WINDOW_FONT_SCALE, _mWindowFontScale);
        _mHasWindowFontScaleChanged = false;
    }
}
void WindowTogglesWindowMini_Activate()
{
	_mIsActive = true;
}
