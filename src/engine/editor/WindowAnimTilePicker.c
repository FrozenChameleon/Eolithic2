#include "WindowAnimTilePicker.h"

#include "../render/Renderer.h"
#include "../render/SpriteBatch.h"
#include "imgui.h"
#include "ImGuiHelper.h"
#include "../utils/Cvars.h"
#include "../leveldata/AnimTile.h"
#include "../render/DrawTool.h"

static Point _mPoint;
static Rectangle _mBounds;

bool WindowAnimTilePicker_CreateWindow()
{
	bool returnValue = false;

	_mBounds.Width = TILE_SIZE * WindowAnimTilePicker_GetRowAmount();
	ResourceMan* rmAnim = ResourceList_AnimTile();
	
	_mBounds.Height = (int32_t)(TILE_SIZE * ((ResourceMan_Length(rmAnim) / WindowAnimTilePicker_GetRowAmount()) + 1));

	//ImGui::SetNextWindowPickerWindow();
	//ImGui::SetNextWindowAlwaysOpen();
	//ImGui::SetNextWindowNoMove(OeCvars.GetAsBool(OeCvars.EDITOR_IS_ANIM_TILE_PICKER_LOCKED));
	ImGui::SetNextWindowSize(ImVec2((float)_mBounds.Width, (float)_mBounds.Height));
	bool whatever = false;
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
	if (!ImGui::Begin("Anim Tile Picker", &whatever, flags))
	{
		ImGui::End();
		return false;
	}

	if (ImGui::IsWindowCollapsed())
	{
		ImGui::End();
		return false;
	}

	Texture* tempTex = Renderer_RenderToTexture(_mBounds.Width, _mBounds.Height, COLOR_CORNFLOWER_BLUE, WindowAnimTilePicker_DrawHud);
	ImGui::Image((ImTextureID)(intptr_t)tempTex->mTextureData, ImVec2((float)_mBounds.Width, (float)_mBounds.Height));

	ImVec2 itemRectMin = ImGui::GetItemRectMin();
	Vector2 windowOffset = { itemRectMin.x, itemRectMin.y };
	int tileWidth = _mBounds.Width / TILE_SIZE;
	int tileHeight = _mBounds.Height / TILE_SIZE;
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		for (int i = 0; i < tileWidth; i += 1)
		{
			for (int j = 0; j < tileHeight; j += 1)
			{
				Rectangle tempRectangle = Rectangle_Create((int)windowOffset.X + _mBounds.X + (i * TILE_SIZE),
					(int)windowOffset.Y + _mBounds.Y + (j * TILE_SIZE), TILE_SIZE, TILE_SIZE);
				ImVec2 imMouse = ImGui::GetMousePos();
				if (Rectangle_Contains(&tempRectangle, (int32_t)imMouse.x, (int32_t)imMouse.y))
				{
					//if (!_mIsDragging)
					//{
					_mPoint.X = i;
					_mPoint.Y = j;
					returnValue = true;
					//}
				}
			}
		}
	}
	else
	{
		//_mIsDragging = false;
	}

	ImGuiHelper_CvarCheckbox("Locked?", CVARS_EDITOR_IS_ANIM_TILE_PICKER_LOCKED);

	ImGui::End();
	return returnValue;
}
int32_t WindowAnimTilePicker_GetRowAmount()
{
	return 480 / TILE_SIZE;
}
void WindowAnimTilePicker_DrawHud(SpriteBatch* spriteBatch)
{
	int myDepth = 100;

	int counterX = 0;
	int counterY = 0;

	ResourceMan* rmAnim = ResourceList_AnimTile();

	for (int i = 0; i < ResourceMan_Length(rmAnim); i += 1)
	{
		AnimTile* at = (AnimTile*)ResourceMan_GetResourceDataByIndex(rmAnim, i);
		AnimTile_Draw(at, spriteBatch, COLOR_WHITE, myDepth, 
			_mBounds.X + counterX * TILE_SIZE, _mBounds.Y + counterY * TILE_SIZE, 0, false, false);
		counterX += 1;
		if (counterX >= WindowAnimTilePicker_GetRowAmount())
		{
			counterX = 0;
			counterY += 1;
		}
	}

	DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, myDepth + 1, 
		Rectangle_Create(_mBounds.X + _mPoint.X * TILE_SIZE, _mBounds.Y + _mPoint.Y * TILE_SIZE, TILE_SIZE, TILE_SIZE),
		0, false, 1);
}
void WindowAnimTilePicker_SetAnimTile(const char* animTile)
{
	int counterX = 0;
	int counterY = 0;
	ResourceMan* rmAnim = ResourceList_AnimTile();
	for (int i = 0; i < ResourceMan_Length(rmAnim); i += 1)
	{
		Resource* res = ResourceMan_GetResourceByIndex(rmAnim, i);
		const char* texName = Resource_GetName(res);
		if (Utils_StringEqualTo(texName, animTile))
		{
			_mPoint.X = counterX;
			_mPoint.Y = counterY;
			return;
		}
		counterX += 1;
		if (counterX >= WindowAnimTilePicker_GetRowAmount())
		{
			counterX = 0;
			counterY += 1;
		}
	}
}
Resource* WindowAnimTilePicker_GetAnimTileResource()
{
	int num = (_mPoint.Y * WindowAnimTilePicker_GetRowAmount()) + _mPoint.X;
	ResourceMan* rmAnim = ResourceList_AnimTile();
	if (num < ResourceMan_Length(rmAnim))
	{
		return ResourceMan_GetResourceByIndex(rmAnim, num);
	}
	return NULL;
}