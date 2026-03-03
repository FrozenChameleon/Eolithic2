#include "PartCollision.h"

#include "Editor.h"
#include "EditorPart.h"
#include "../render/SpriteBatch.h"
#include "../utils/MString.h"
#include "../utils/Cvars.h"
#include "../utils/Utils.h"
#include "../leveldata/Tile.h"
#include "../collision/CollisionEngineSys.h"
#include "../core/Func.h"
#include "imgui.h"
#include "../input/Input.h"
#include "EditorGlobals.h"

static int32_t _mType = 1;
static MString* _mTempString;

void PartCollision_CreateWindows()
{
	bool test = false;
	//ImGui::SetNextWindowSize(ImVec2(300, 500));
	if (!ImGui::Begin("Collision Mode", &test, ImGuiWindowFlags_NoResize))
	{
		ImGui::End();
		return;
	}

	for (int i = 1; i < 10; i += 1)
	{
		if (ImGui::RadioButton(Editor_GetEditorCollisionName(i), i == _mType))
		{
			_mType = i;
		}
	}

	ImGui::NewLine();

	ImGui::Text("--- Keys ---");
	ImGui::Text("Numbers 1-0 - Change Collision");
	ImGui::Text("LClick - Set Collision");
	ImGui::Text("RClick - Remove Collision");

	ImGui::End();
}

static void Operation(int type)
{
	EditorGlobals_StallOperationCounter();

	Rectangle gridBounds = EditorPart_GetSelectionRectangleAsGridBounds();
	for (int j = gridBounds.Y; j < (gridBounds.Y + gridBounds.Height); j += 1)
	{
		for (int i = gridBounds.X; i < (gridBounds.X + gridBounds.Width); i += 1)
		{
			Tile* tile = EditorPart_GetGridTile(i, j);
			if (tile == NULL)
			{
				continue;
			}

			EditorPart_PushPatch(tile);

			tile->mCollisionType = type;
			Point point = CollisionEngineSys_GetCollisionGridPosition((float)i, (float)j);
			if (Is_ComponentPackPresent(C_CollisionEngine))
			{
				CollisionEngineSys_SetCollisionBitSafeGrid(Get_CollisionEngine(), point.X, point.Y, type);
			}
		}
	}

	EditorPart_FinishPatches();
}
void PartCollision_UpdateHelper()
{
	int collisionType = -1;
	if (Input_IsKeyTapped(KEYS_D1))
	{
		collisionType = 1;
	}
	else if (Input_IsKeyTapped(KEYS_D2))
	{
		collisionType = 2;
	}
	else if (Input_IsKeyTapped(KEYS_D3))
	{
		collisionType = 3;
	}
	else if (Input_IsKeyTapped(KEYS_D4))
	{
		collisionType = 4;
	}
	else if (Input_IsKeyTapped(KEYS_D5))
	{
		collisionType = 5;
	}
	else if (Input_IsKeyTapped(KEYS_D6))
	{
		collisionType = 6;
	}
	else if (Input_IsKeyTapped(KEYS_D7))
	{
		collisionType = 7;
	}
	else if (Input_IsKeyTapped(KEYS_D8))
	{
		collisionType = 8;
	}
	else if (Input_IsKeyTapped(KEYS_D9))
	{
		collisionType = 9;
	}
	else if (Input_IsKeyTapped(KEYS_D0))
	{
		collisionType = 10;
	}

	if (collisionType != -1)
	{
		_mType = collisionType;
	}

	if (Input_IsLeftMousePressed())
	{
		Operation(_mType);
	}
	else if (Input_IsRightMousePressed())
	{
		Operation(0);
	}

	Cvars_SetAsBool(CVARS_EDITOR_SHOW_COLLISION, true);
}
const char* PartCollision_GetStatus()
{
	MString_AssignString(&_mTempString, "|Collision=");
	MString_AddAssignString(&_mTempString, Editor_GetEditorCollisionName(_mType));
	MString_AddAssignString(&_mTempString, "|Bit=");
	MString_AddAssignInt(&_mTempString, _mType);
	return MString_Text(_mTempString);
}
void PartCollision_DrawHud(SpriteBatch* spriteBatch)
{
	EditorPart_DoDefaultEditorPartDrawHudRoutine(spriteBatch, PartCollision_GetStatus());
}
void PartCollision_Draw(SpriteBatch* spriteBatch)
{
	EditorPart_DoDefaultEditorPartDrawRoutine(spriteBatch, EditorPart_DefaultDrawSingleSelectionSelectedTiles);
}
void PartCollision_JustChangedToThisPart()
{
	Cvars_SetAsBool(CVARS_EDITOR_SHOW_COLLISION, true);
}
void PartCollision_Update()
{
	EditorPart_DoDefaultEditorPartUpdateRoutine();

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