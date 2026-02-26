#include "EditorDebugToggles.h"

#include "../input/Input.h"
#include "ActivePart.h"

void EditorDebugToggles_Update()
{
    /*if (OeConsole.IsPresent())
    {
        return;
    }*/

    if (Input_IsCtrlPressed())
    {
        if (Input_IsKeyTapped(KEYS_H))
        {
            //ImGui::GetWindowData(OeEditor.KEY_WINDOW_TOOL_MAN).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_L))
        {
            //ImGui::GetWindowData(OeEditor.KEY_WINDOW_SETTINGS).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_T))
        {
           // ImGui::GetWindowData(OeEditor.KEY_WINDOW_TOGGLES).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_M))
        {
           // ImGui::GetWindowData(OeEditor.KEY_WINDOW_TOGGLES_MINI).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_K))
        {
           // ImGui::GetWindowData(OeWindowSoundGuy.KEY_WINDOW_SOUND_GUY).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_I))
        {
          //  ImGui::GetWindowData(OeWindowInputBuddy.KEY_WINDOW_INPUT_BUDDY).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_P))
        {
          //  ImGui::GetWindowData(OeWindowAnimDude.KEY_WINDOW_ANIM_DUDE).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_U))
        {
           // ImGui::GetWindowData(OeWindowPropMan.KEY_WINDOW_PROP_DUDE).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_Y))
        {
           // ImGui::GetWindowData(OeWindowParticleKing.KEY_WINDOW_PARTICLE_KING).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_N))
        {
           // ImGui::GetWindowData(OeWindowThingEditor.KEY_WINDOW_THING_EDITOR).ToggleClose();
        }
        if (Input_IsKeyTapped(KEYS_NUMPAD9))
        {
          //  OeEditor.ToggleAutoTile();
        }
        if (Input_IsKeyTapped(KEYS_NUMPAD8))
        {
          //  OeCvars.FlipAsBool(OeCvars.EDITOR_USE_GAME_CAMERA);
        }
        if (Input_IsKeyTapped(KEYS_NUMPAD7))
        {
           // OeCvars.FlipAsBool(OeCvars.EDITOR_SHOW_GRID);
        }
        if (Input_IsKeyTapped(KEYS_NUMPAD6))
        {
          //  OeCvars.FlipAsBool(OeCvars.EDITOR_SUSPEND_GAME);
        }
        if (Input_IsKeyTapped(KEYS_NUMPAD5))
        {
           // OeCvars.FlipAsBool(OeCvars.EDITOR_SHOW_GAME);
        }
        if (Input_IsKeyTapped(KEYS_RIGHT))
        {
          //  OeEditor.SetCameraSnapShot(0);
        }
        if (Input_IsKeyTapped(KEYS_DOWN))
        {
          //  OeEditor.SetCameraSnapShot(1);
        }
        if (Input_IsKeyTapped(KEYS_LEFT))
        {
          //  OeEditor.SetCameraSnapShot(2);
        }
    }
    else if (Input_IsShiftPressed())
    {

    }
    else if (Input_IsAltPressed())
    {

    }
    else
    {
        if (Input_IsKeyTapped(KEYS_F1))
        {
            ActivePart_SetPartToCollision();
        }
        if (Input_IsKeyTapped(KEYS_F2))
        {
            ActivePart_SetPartToTile();
        }
        if (Input_IsKeyTapped(KEYS_F3))
        {
			ActivePart_SetPartToThing();
        }
        if (Input_IsKeyTapped(KEYS_F4))
        {
			ActivePart_SetPartToProp();
        }
        if (Input_IsKeyTapped(KEYS_F5))
        {
           // OeEditorActivePart.SetPartToLine();
        }
        if (Input_IsKeyTapped(KEYS_F6))
        {
           // OeEditorActivePart.SetPartToCamera();
        }
        if (Input_IsKeyTapped(KEYS_F7))
        {
          //  OeEditorActivePart.SetPartToDraw();
        }
        if (Input_IsKeyTapped(KEYS_UP))
        {
          //  OeEditor.UseCameraSnapshot(-1);
        }
        if (Input_IsKeyTapped(KEYS_RIGHT))
        {
           // OeEditor.UseCameraSnapshot(0);
        }
        if (Input_IsKeyTapped(KEYS_DOWN))
        {
          //  OeEditor.UseCameraSnapshot(1);
        }
        if (Input_IsKeyTapped(KEYS_LEFT))
        {
          //  OeEditor.UseCameraSnapshot(2);
        }
    }
}
