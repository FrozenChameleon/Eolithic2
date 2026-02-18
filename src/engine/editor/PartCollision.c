#include "PartCollision.h"

#include "EditorPart.h"
#include "../render/SpriteBatch.h"
#include "../utils/MString.h"
#include "../utils/Cvars.h"
#include "../utils/Utils.h"
#include "imgui.h"

static int32_t _mType = 1;
static MString* _mTempString;

void PartCollision_CreateWindows()
{
	bool test = false;
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (!ImGui::Begin("Collision Mode", &test, ImGuiWindowFlags_NoResize))
	{
		return;
	}

	//TODO
	/*
	for (int i = 1; i < 10; i += 1)
	{
	if (ImGui::RadioButton(Editor_GetEditorCollisionName(i), i == _mType))
	{
	_mType = i;
	}
	}
	*/

	ImGui::NewLine();

	ImGui::Text("--- Keys ---");
	ImGui::Text("Numbers 1-0 - Change Collision");
	ImGui::Text("LClick - Set Collision");
	ImGui::Text("RClick - Remove Collision");

	ImGui::End();
}
void PartCollision_UpdateHelper()
{
	/*
	int collisionType = -1;
	if (OeInput.IsKeyTapped(Keys.D1))
	{
	collisionType = 1;
	}
	else if (OeInput.IsKeyTapped(Keys.D2))
	{
	collisionType = 2;
	}
	else if (OeInput.IsKeyTapped(Keys.D3))
	{
	collisionType = 3;
	}
	else if (OeInput.IsKeyTapped(Keys.D4))
	{
	collisionType = 4;
	}
	else if (OeInput.IsKeyTapped(Keys.D5))
	{
	collisionType = 5;
	}
	else if (OeInput.IsKeyTapped(Keys.D6))
	{
	collisionType = 6;
	}
	else if (OeInput.IsKeyTapped(Keys.D7))
	{
	collisionType = 7;
	}
	else if (OeInput.IsKeyTapped(Keys.D8))
	{
	collisionType = 8;
	}
	else if (OeInput.IsKeyTapped(Keys.D9))
	{
	collisionType = 9;
	}
	else if (OeInput.IsKeyTapped(Keys.D0))
	{
	collisionType = 10;
	}

	if (collisionType != -1)
	{
	_mType = collisionType;
	}

	if (OeInput.IsLeftMousePressed())
	{
	OperationHelper(_mType);
	}

	if (OeInput.IsRightMousePressed())
	{
	OperationHelper(0);
	}

	OeCvars.Set(OeCvars.EDITOR_SHOW_COLLISION, true);*/
}
void PartCollision_OperationHelper(int type)
{
	/*if (!IsSelecting())
	{
	SetPatch1(GetPatchSingleSelection());
	Operation(GetRestrainedMouseX(), GetRestrainedMouseY(), type);
	SetPatch2(GetPatchSingleSelection());
	}
	else
	{
	SetPatch1(GetPatchSelectionRectangle());
	Rectangle selectionRectangle = GetSelectionRectangle();
	for (int i = selectionRectangle.Left; i < selectionRectangle.Right; i += OeUtils.GetHalfTileSize())
	{
	for (int j = selectionRectangle.Top; j < selectionRectangle.Bottom; j += OeUtils.GetHalfTileSize())
	{
	Operation(i, j, type);
	}
	}
	SetPatch2(GetPatchSelectionRectangle());
	}

	CheckPatch();*/
}
const char* PartCollision_GetStatus()
{
	MString_AssignString(&_mTempString, "|Collision=");
	return MString_Text(_mTempString);
	//TODO return  + OeEditor.GetEditorCollisionName(_mType) + "|Bit=" + _mType;
}
void PartCollision_DrawHud(SpriteBatch* spriteBatch)
{
	EditorPart_DoDefaultEditorPartDrawHudRoutine(spriteBatch, PartCollision_GetStatus());
}
void PartCollision_Draw(SpriteBatch* spriteBatch)
{
	//TODO  EditorPart_DoDefaultEditorPartDrawRoutine(spriteBatch, DefaultDrawSingleSelectionSelectedTiles);
}
void PartCollision_Operation(float x, float y, int type)
{
	/*
	OeTile tile = GetGridTileFromPixel(new Vector2(x, y));
	if (tile != null)
	{
	tile.mCollisionType = type;
	Point point = OeCollisionEngineSys.GetCollisionGridPosition(x, y);
	if (OeFunc.Is_ComponentPackPresent<OeCollisionEngine>())
	{
	OeFunc.Get_CollisionEngine().mCollisionGridPristine[point.X, point.Y] = type;
	}
	}
	*/
}
void PartCollision_JustChangedToThisPart()
{
	Cvars_SetAsBool(CVARS_EDITOR_SHOW_COLLISION, true);
}
void PartCollision_Update(double deltaTime)
{
	EditorPart_DoDefaultEditorPartUpdateRoutine(deltaTime);

	PartCollision_UpdateHelper();
}

PartFuncData PartCollision_GetFuncData()
{
    PartFuncData data = EditorPart_CreateDummyFuncData();

    Utils_strlcpy(data.mDebugName, "COLLISION", EE_FILENAME_MAX);

    data.CreateWindows = PartCollision_CreateWindows;
    data.Update = PartCollision_Update;
    data.Draw = PartCollision_Draw;
    data.DrawHud = PartCollision_DrawHud;
    data.JustChangedToThisPart = PartCollision_JustChangedToThisPart;

    return data;
}