#include "WindowTilePicker.h"

#include "../math/Rectangle.h"
#include "../render/Texture.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "imgui.h"
#include "../render/Renderer.h"
#include "../render/DrawTool.h"
#include "ImGuiHelper.h"

static int _mState = 1;
static bool _mIsDragHold;
static Point _mPointAnchor;
static Point _mPoint1;
static Point _mPoint2;
static Rectangle _mBounds;
static Texture* _mTexture;

bool WindowTilePicker_CreateWindow(Texture* texture)
{
    if ((texture == NULL) || (texture->mTextureData == NULL))
    {
        return false;
    }

    _mTexture = texture;

    int width = _mTexture->mBounds.Width;
	int height = _mTexture->mBounds.Height;

    float scale = Cvars_GetAsFloat(CVARS_EDITOR_TILE_PICKER_SCALE);

    _mBounds.Width = (int)(width * scale);
    _mBounds.Height = (int)(height * scale);

	ImGui::SetNextWindowSize(ImVec2((float)_mBounds.Width, (float)_mBounds.Height));
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (Cvars_GetAsBool(CVARS_EDITOR_IS_TILE_PICKER_LOCKED))
    {
		flags |= ImGuiWindowFlags_NoMove;
    }
    bool whatever = false;
	if (!ImGui::Begin("Tile Picker", &whatever, flags))
    {
        ImGui::End();
        return false;
    }

    if (ImGui::IsWindowCollapsed())
    {
        ImGui::End();
        return false;
    }

	if ((_mBounds.Width != 0) && (_mBounds.Height != 0))
    {
		Texture* tempTex = Renderer_RenderToTexture(_mBounds.Width, _mBounds.Height, COLOR_CORNFLOWER_BLUE, WindowTilePicker_DrawHud);
        ImGui::Image((ImTextureID)(intptr_t)tempTex->mTextureData, ImVec2((float)_mBounds.Width, (float)_mBounds.Height));
		//ImGui::Image((ImTextureID)(intptr_t)texture->mTextureData, ImVec2((float)_mBounds.Width, (float)_mBounds.Height));
        //ImGui::Image(OeGameDrawer.DebugRenderToTexture(_mBounds.Width, _mBounds.Height, Color.CornflowerBlue, DrawHud), true);
    }

    if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered())
    {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            _mState += 1;
            if (_mState >= 3)
            {
                _mState = 0;
            }

            if (_mState == 0)
            {
                 Cvars_SetAsString(CVARS_EDITOR_TILE_PICKER_SCALE, ".5");
            }
            else if (_mState == 1)
            {
                Cvars_SetAsString(CVARS_EDITOR_TILE_PICKER_SCALE, "1");
            }
            else if (_mState == 2)
            {
                Cvars_SetAsString(CVARS_EDITOR_TILE_PICKER_SCALE, "2");
            }
        }
    }

    bool returnValue = WindowTilePicker_FindPoint(width, height, scale);

	ImGuiHelper_CvarCheckbox("Locked?", CVARS_EDITOR_IS_TILE_PICKER_LOCKED);

	ImGui::End();

    return returnValue;
}
bool WindowTilePicker_FindPoint(int width, int height, float scale)
{
	ImVec2 windowOffsetImVec2 = ImGui::GetItemRectMin();
	Vector2 windowOffset = { windowOffsetImVec2.x, windowOffsetImVec2.y };
    int tileWidth = width / TILE_SIZE;
    int tileHeight = height / TILE_SIZE;
    float size = TILE_SIZE * scale;
    int tempSize = (int)size;

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        for (int i = 0; i < tileWidth; i += 1)
        {
            for (int j = 0; j < tileHeight; j += 1)
            {
                Rectangle tempRectangle = Rectangle_Create((int)windowOffset.X + _mBounds.X + (i * tempSize),
                    (int)windowOffset.Y + _mBounds.Y + (j * tempSize), tempSize, tempSize);
                ImVec2 imguiMousePos = ImGui::GetMousePos();
				if (Rectangle_Contains(&tempRectangle, (int32_t)imguiMousePos.x, (int32_t)imguiMousePos.y))
                {
                    if (!_mIsDragHold)
                    {
                        _mPointAnchor.X = i;
                        _mPointAnchor.Y = j;
                        _mIsDragHold = true;
                    }

                    if (_mPointAnchor.X > i)
                    {
                        _mPoint1.X = i;
                        _mPoint2.X = _mPointAnchor.X;
                    }
                    else
                    {
                        _mPoint2.X = i;
                        _mPoint1.X = _mPointAnchor.X;
                    }

                    if (_mPointAnchor.Y > j)
                    {
                        _mPoint1.Y = j;
                        _mPoint2.Y = _mPointAnchor.Y;
                    }
                    else
                    {
                        _mPoint2.Y = j;
                        _mPoint1.Y = _mPointAnchor.Y;
                    }

                    return true;
                }
            }
        }
    }
    else
    {
        _mIsDragHold = false;
    }

    return false;
}
Point WindowTilePicker_GetTexturePosition()
{
	return Point_Create(_mPoint1.X * TILE_SIZE, _mPoint1.Y * TILE_SIZE);
}
void WindowTilePicker_SetTexturePosition(Point point)
{
    _mPoint1.X = point.X / TILE_SIZE;
    _mPoint2.X = _mPoint1.X;
    _mPoint1.Y = point.Y / TILE_SIZE;
    _mPoint2.Y = _mPoint1.Y;
}
Rectangle WindowTilePicker_GetSelectedBoundaryInPixels()
{
	Rectangle boundary = WindowTilePicker_GetSelectedBoundary();
	return Rectangle_Create(boundary.X * TILE_SIZE, boundary.Y * TILE_SIZE, boundary.Width * TILE_SIZE, boundary.Height * TILE_SIZE);
}
Rectangle WindowTilePicker_GetSelectedBoundary()
{
	return Rectangle_Create(0, 0, _mPoint2.X - _mPoint1.X + 1, _mPoint2.Y - _mPoint1.Y + 1);
}
void WindowTilePicker_DrawHud(SpriteBatch* spriteBatch)
{
    if ((_mTexture == NULL) || (_mTexture->mTextureData == NULL))
    {
		return;
    }

    int myDepth = 100;

    float offsetX = 0;
    float offsetY = 0;
    float scale = Cvars_GetAsFloat(CVARS_EDITOR_TILE_PICKER_SCALE);
    if (scale < 1.0f)
    {
        offsetX = (_mTexture->mBounds.Width / 4.0f);
        offsetY = (_mTexture->mBounds.Height / 4.0f);
    }
    else if (scale > 1.0f)
    {
        offsetX = -(_mTexture->mBounds.Width / 2.0f);
        offsetY = -(_mTexture->mBounds.Height / 2.0f);
    }

    int wrapperWidth = _mTexture->mBounds.Width;
    int wrapperHeight = _mTexture->mBounds.Height;
    SpriteBatch_Draw(spriteBatch, _mTexture, COLOR_WHITE, myDepth, NULL, Vector2_Create(_mBounds.X - offsetX, _mBounds.Y - offsetY),
		Rectangle_Create(0, 0, wrapperWidth, wrapperHeight), 
        Vector2_Create2(scale), 0, false, false, Vector2_DivFloat(Vector2_Create((float)wrapperWidth, (float)wrapperHeight), 2.0f));
	Rectangle boundaryInPixels = WindowTilePicker_GetSelectedBoundaryInPixels();
    int width = boundaryInPixels.Width;
    int height = boundaryInPixels.Height;

    float tempX = _mBounds.X + _mPoint1.X * (TILE_SIZE * scale);
    float tempY = _mBounds.Y + _mPoint1.Y * (TILE_SIZE * scale);
    float tempWidth = width * scale;
    float tempHeight = height * scale;
	DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_MAGENTA, myDepth + 1,
		Rectangle_Create((int)tempX, (int)tempY, (int)tempWidth, (int)tempHeight), 0, false, 1);
}
