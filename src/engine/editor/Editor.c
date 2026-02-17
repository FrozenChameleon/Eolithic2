#include "Editor.h"

#include "../render/SpriteBatch.h"
#include "../components/Camera.h"
#include "../components/CameraSys.h"
#include "../render/Renderer.h"
#include "EditorActivePart.h"
#include "imgui.h"

static Camera _mCamera;
static bool _mHasInit;

void Editor_Init(void)
{
    if (_mHasInit)
    {
        return;
    }

    Rectangle drawableSize = Renderer_GetDrawableSize();
	CameraSys_InitCamera(&_mCamera);
	Camera_Resize(&_mCamera, drawableSize.Width, drawableSize.Height);

    _mHasInit = true;
}
void Editor_Update(void)
{
	CameraSys_UpdateCamera(&_mCamera);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Test"))
        {
            if (ImGui::MenuItem("AAA", "Ctrl+N"))
            {
                int hello = 0;
            }
            if (ImGui::MenuItem("BBB", "Ctrl+Z"))
            {
                int hello = 0;
            }
			ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
void Editor_Draw(SpriteBatch* spriteBatch)
{
	EditorActivePart_Draw(spriteBatch);
}
void Editor_DrawHud(SpriteBatch* spriteBatch)
{
    EditorActivePart_DrawHud(spriteBatch);
}

Camera* Editor_GetCamera(void)
{
	return &_mCamera;
}
