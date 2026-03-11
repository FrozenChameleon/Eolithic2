#include "ActivePart.h"

#include "imgui.h"
#include "../render/SpriteBatch.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../utils/Cvars.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../render/DrawRectangle.h"
#include "../render/Color.h"
#include "../utils/Logger.h"
#include "Editor.h"
#include "PartFuncData.h"
#include "PartCollision.h"
#include "EditorGlobals.h"
#include "EditorPart.h"
#include "../input/Input.h"
#include "PartTile.h"
#include "PartProp.h"
#include "PartThing.h"

static int32_t _mCurrentLayer;
static int32_t _mCurrentPart;

static PartFuncData _mEditorParts[8];

 static bool _mHasInit;

 static void SetPart(int32_t part)
 {
	 //TODO
	 /*
	 if (Get_LevelData().IsMetaMap() && !OeGlobals.DebugIsMetaMapEditTileModeAtMapLoad())
	 {
		 part = EDITORPART_META_MAP;
	 }*/

	 switch (part)
	 {
	 case EDITORPART_COLLISION:
		 Logger_Log(LOGGER_INFORMATION, "Mode: Collision");
		 break;
	case EDITORPART_TILE_PICKER:
		 Logger_Log(LOGGER_INFORMATION, "Mode: Tile");
		 break;
	 case EDITORPART_THING: 
		 Logger_Log(LOGGER_INFORMATION, "Mode: Thing");
		 break;
	 case EDITORPART_PROP:
		 Logger_Log(LOGGER_INFORMATION, "Mode: Prop");
		 break;/*
	 case PART_LINE:
		 OeLogger.LogInformation("Mode: Line");
		 break;
	 case PART_CAMERA:
		 OeLogger.LogInformation("Mode: Camera");
		 break;
	 case PART_META_MAP:
		 OeLogger.LogInformation("Mode: Meta Map");
		 break;
	 case PART_DRAW:
		 OeLogger.LogInformation("Mode: Draw");
		 break;
	 default:
		 OeLogger.LogInformation("Mode: Nothing");
		 break;*/
	 }

	 _mCurrentPart = part;

	 ActivePart_GetCurrentPart().JustChangedToThisPart();
 }

void ActivePart_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	/*
	for (int i = 0; i < _mEditorParts.Length; i += 1)
	{
		_mEditorParts[i] = OeEditorPart.CreateDummyFuncData();
	}
	*/
	_mEditorParts[0] = PartCollision_GetFuncData();
	_mEditorParts[1] = PartTile_GetFuncData();
	_mEditorParts[2] = PartThing_GetFuncData();
	_mEditorParts[3] = PartProp_GetFuncData();/*
	_mEditorParts[4] = OeEditorPartLine.GetFuncData();
	_mEditorParts[5] = OeEditorPartCamera.GetFuncData();
	_mEditorParts[6] = OeEditorPartMetaMap.GetFuncData();
	_mEditorParts[7] = OeEditorPartDraw.GetFuncData();*/

	_mHasInit = true;

	ActivePart_SetPartToCollision();
}

int32_t ActivePart_GetCurrentLayer(void)
{
	return _mCurrentLayer;
}
void ActivePart_Update()
{
	ActivePart_GetCurrentPart().CreateWindows();

	if (EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered())
	{
		return;
	}

	if (EditorPart_GetEditorZoom(0) <= 0)
	{
		EditorPart_SetEditorZoom(0, 100);
	}

	if (EditorPart_IsBlocking())
	{
		if (Input_IsLeftMousePressed() ||
			Input_IsMiddleMousePressed() ||
			Input_IsRightMousePressed())
		{
			return;
		}

		EditorPart_SetBlocking(false);
	}

	if (EditorPart_HandleSelectionRectangle())
	{
		EditorPart_SetBlocking(true);
		return;
	}

	ActivePart_GetCurrentPart().Update();
}
void ActivePart_Draw(SpriteBatch* spriteBatch)
{
    //Init();

    ActivePart_GetCurrentPart().Draw(spriteBatch);

    if (_mCurrentPart == EDITORPART_META_MAP)
    {
		return;
	}
	
	LevelData* ld = Get_LevelData();

	bool useGameCamera = Cvars_GetAsBool(CVARS_EDITOR_USE_GAME_CAMERA);
	LevelData_DrawTiles2(ld, spriteBatch, useGameCamera ? Get_Camera() : Editor_GetCamera(), ActivePart_GetCurrentLayer(), 2);

	LevelData_DrawProps(ld, spriteBatch, Editor_GetCamera());

	LevelData_DrawCollision(ld, spriteBatch, Editor_GetCamera());

	LevelData_DrawThings(ld, spriteBatch, Editor_GetCamera());

	/*
	if (_mCurrentPart != PART_LINE)
	{
		Get_LevelData().DrawLines(spriteBatch);
	}

	Get_LevelData().DrawCamera(spriteBatch);

	if (OeCvars.GetAsBool(OeCvars.EDITOR_SHOW_GAME))
	{
		OeGameStateManager.ActiveGameState.Draw(spriteBatch);
	}*/
}
void ActivePart_DrawHud(SpriteBatch* spriteBatch)
{
	ActivePart_GetCurrentPart().DrawHud(spriteBatch);
}
PartFuncData ActivePart_GetCurrentPart(void)
{
	return _mEditorParts[_mCurrentPart];
}
void ActivePart_SetPartToCollision()
{
	SetPart(EDITORPART_COLLISION);
}
void ActivePart_SetPartToTile()
{
	SetPart(EDITORPART_TILE_PICKER);
}
void ActivePart_SetPartToThing()
{
	SetPart(EDITORPART_THING);
}
void ActivePart_SetPartToProp()
{
	SetPart(EDITORPART_PROP);
}
void ActivePart_SetPartToLine()
{
	/*if (!OeEditorGlobalDefs.EDITOR_GLOBAL_DEF_DISABLE_EDITOR_PART_LINE)
	{
		SetPart(PART_LINE);
	}*/
}
void ActivePart_SetPartToCamera()
{
	/*if (!OeEditorGlobalDefs.EDITOR_GLOBAL_DEF_DISABLE_EDITOR_PART_CAMERA)
	{
		SetPart(PART_CAMERA);
	}*/
}
void ActivePart_SetPartToDraw()
{
	//SetPart(PART_DRAW);
}
void ActivePart_SetPartToMetaMap()
{
	/*if (!Get_LevelData().IsMetaMap())
	{
		return;
	}

	if (OeGlobals.DebugIsMetaMapEditTileModeAtMapLoad())
	{
		return;
	}

	SetPart(PART_META_MAP);*/
}
