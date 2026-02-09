#include "Editor.h"

#include "../render/SpriteBatch.h"
#include "../components/Camera.h"
#include "../components/CameraSys.h"
#include "../render/Renderer.h"
#include "EditorActivePart.h"

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
