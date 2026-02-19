#include "Editor.h"

#include "../render/SpriteBatch.h"
#include "../components/Camera.h"
#include "../components/CameraSys.h"
#include "../render/Renderer.h"
#include "../input/Input.h"
#include "../utils/Cvars.h"
#include "../utils/MString.h"
#include "EditorPart.h"
#include "ActivePart.h"
#include "imgui.h"

static Camera _mCamera;

static bool _mHasInit;
void Editor_Init(void)
{
    if (_mHasInit)
    {
        return;
    }

	CameraSys_InitCamera(&_mCamera);
    Rectangle drawableSize = Renderer_GetDrawableSize();
	Camera_Resize(&_mCamera, drawableSize.Width, drawableSize.Height); //TODO CHECK THIS?

    ActivePart_Init();

    _mHasInit = true;
}

static void MenuWindows()
{
    if (ImGui::BeginMenu("Windows"))
    {
        if (ImGui::MenuItem("Anim Maanger", "Ctrl+P"))
        {
        }
        if (ImGui::MenuItem("Input Manager", "Ctrl+I"))
        {
        }
        if (ImGui::MenuItem("Mini Toggles", "Ctrl+M"))
        {
        }
        if (ImGui::MenuItem("Particle Manager", "Ctrl+Y"))
        {
        }
        if (ImGui::MenuItem("Prop Manager", "Ctrl+U"))
        {
        }
        if (ImGui::MenuItem("Settings", "Ctrl+L"))
        {
        }
        if (ImGui::MenuItem("Sound Manager", "Ctrl+K"))
        {
        }
        if (ImGui::MenuItem("Thing Editor", "Ctrl+N"))
        {
        }
        if (ImGui::MenuItem("Toggles", "Ctrl+T"))
        {
        }
        if (ImGui::MenuItem("Tool Manager", "Ctrl+H"))
        {
        }
        ImGui::EndMenu();
    }
}
static void MenuMode()
{
    if (ImGui::BeginMenu("Mode"))
    {
        if (ImGui::MenuItem("Collision", "F1"))
        {
        }
        if (ImGui::MenuItem("Tile Picker", "F2"))
        {
        }
        if (ImGui::MenuItem("Thing", "F3"))
        {
        }
        if (ImGui::MenuItem("Prop", "F4"))
        {
        }
        if (ImGui::MenuItem("Line", "F5"))
        {
        }
        if (ImGui::MenuItem("Camera", "F6"))
        {
        }
        ImGui::EndMenu();
    }
}
static void MenuOptions()
{
    if (ImGui::BeginMenu("Options"))
    {
        if (ImGui::MenuItem("Game Speed", "XXX"))
        {
        }
        if (ImGui::MenuItem("Vsync", "XXX"))
        {
        }
        if (ImGui::MenuItem("Fullscreen", "XXX"))
        {
        }
        if (ImGui::MenuItem("Reload Map", "XXX"))
        {
        }
        if (ImGui::MenuItem("Save Map", "XXX"))
        {
        }
        if (ImGui::MenuItem("Fast Reset", "XXX"))
        {
        }
        if (ImGui::MenuItem("Fast Reset + Move", "XXX"))
        {
        }
        if (ImGui::MenuItem("Toggle Editor", "XXX"))
        {
        }
        if (ImGui::MenuItem("God Mode", "XXX"))
        {
        }
        if (ImGui::MenuItem("Lower Game Speed", "XXX"))
        {
        }
        if (ImGui::MenuItem("Increase Game Speed", "XXX"))
        {
        }
        if (ImGui::MenuItem("Pause", "XXX"))
        {
        }
        if (ImGui::MenuItem("Step", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Props", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Things", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Tiles", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Collision", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Lines", "XXX"))
        {
        }
        if (ImGui::MenuItem("Disable Hud", "XXX"))
        {
        }
        if (ImGui::MenuItem("Cycle Debug Info", "XXX"))
        {
        }
        if (ImGui::MenuItem("Set Game Speed To 1", "XXX"))
        {
        }
        if (ImGui::MenuItem("Toggle Auto Speed", "XXX"))
        {
        }
        if (ImGui::MenuItem("Reload Audio", "XXX"))
        {
        }
        if (ImGui::MenuItem("Reload Assets", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Game", "XXX"))
        {
        }
        if (ImGui::MenuItem("Suspend Game", "XXX"))
        {
        }
        if (ImGui::MenuItem("Show Grid", "XXX"))
        {
        }
        if (ImGui::MenuItem("Use Game Camera", "XXX"))
        {
        }
		if (ImGui::MenuItem("Toggle Auto Tile (False)", "XXX"))
		{
		}
		if (ImGui::MenuItem("Reset Camera To Default", "ArrowUp"))
		{
		}
		if (ImGui::MenuItem("Use Camera Snapshot (0)", "XXX"))
		{
		}
        if (ImGui::MenuItem("Use Camera Snapshot (1)", "XXX"))
        {
        }
        if (ImGui::MenuItem("Use Camera Snapshot (2)", "XXX"))
        {
        }
        if (ImGui::MenuItem("Set Camera Snapshot (0)", "XXX"))
        {
        }
        if (ImGui::MenuItem("Set Camera Snapshot (1)", "XXX"))
        {
        }
        if (ImGui::MenuItem("Set Camera Snapshot (2)", "XXX"))
        {
        }
		ImGui::EndMenu();
	}
}
static void MenuDifficulty()
{
	if (ImGui::BeginMenu("Difficulty"))
	{
		if (ImGui::MenuItem("0"))
		{
		}
        if (ImGui::MenuItem("1"))
        {
        }
        if (ImGui::MenuItem("2"))
        {
        }
        if (ImGui::MenuItem("3"))
        {
        }
        if (ImGui::MenuItem("4"))
        {
        }
        if (ImGui::MenuItem("5"))
        {
        }
        if (ImGui::MenuItem("6"))
        {
        }
        if (ImGui::MenuItem("7"))
        {
        }
        if (ImGui::MenuItem("8"))
        {
        }
		if (ImGui::MenuItem("9"))
        {
        }
		ImGui::EndMenu();
	}
}
static void MenuMetaMap()
{
    if (ImGui::BeginMenu("Meta Map"))
    {
        if (ImGui::MenuItem("Edit Tile Mode? (THIS WILL SAVE THE MAP AND DO A MAP RELOAD IMMEDIATELY!!!!)"))
        {
        }
        ImGui::EndMenu();
    }
}
static void MenuHotkeys()
{
    if (ImGui::BeginMenu("Hotkeys"))
    {
		ImGui::MenuItem("This is just information!");
		ImGui::MenuItem("Insert Column", "Alt+C");
        ImGui::MenuItem("Insert Row", "Alt+R");
        ImGui::MenuItem("Remove Column", "Tab+C");
        ImGui::MenuItem("Remove Row", "Tab+R");
        ImGui::MenuItem("Undo (usually...)", "Ctrl+Z");
        ImGui::MenuItem("Flip Held Copy X", "Q");
        ImGui::MenuItem("Flip Held Copy Y", "E");
        ImGui::MenuItem("Selection", "Shift");
        ImGui::MenuItem("Cut Selection", "Ctrl+X");
        ImGui::MenuItem("Copy Selection", "Ctrl+C");
        ImGui::MenuItem("Paste Selection", "Ctrl+V");
        ImGui::MenuItem("Set Active Tile Layer", "1-0");
        ImGui::MenuItem("Zoom", "Ctrl+Scrollwheel");
        ImGui::MenuItem("Zoom", "+/-");
        ImGui::MenuItem("Navigate", "WASD");
        ImGui::MenuItem("Navigate", "Hold Space + Move Mouse");
        ImGui::MenuItem("Navigate Faster", "Hold Alt");
        ImGui::EndMenu();
    }
}
static void MainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        MenuWindows();
        MenuMode();
        MenuOptions();
        MenuDifficulty();
        MenuMetaMap();
        MenuHotkeys();
        ImGui::EndMainMenuBar();
    }
}
void Editor_Update(double deltaTime)
{
	CameraSys_UpdateCamera(&_mCamera);

	MainMenuBar();

    ActivePart_Update(deltaTime);
}
void Editor_Draw(SpriteBatch* spriteBatch)
{
	ActivePart_Draw(spriteBatch);
}
void Editor_DrawHud(SpriteBatch* spriteBatch)
{
    ActivePart_DrawHud(spriteBatch);
}
Camera* Editor_GetCamera(void)
{
	return &_mCamera;
}
const char* Editor_GetEditorCollisionName(int32_t index)
{
    MString* tempString = NULL;
    MString_AssignString(&tempString, CVARS_ENGINE_COLLISION_NAME);
	MString_AddAssignInt(&tempString, index);
    const char* editorCollisionName = Cvars_GetAsString(MString_Text(tempString));
	MString_Dispose(&tempString);
	return editorCollisionName;
}
