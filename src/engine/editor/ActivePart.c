#include "ActivePart.h"

#include "imgui.h"
#include "../render/SpriteBatch.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../utils/Cvars.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../render/DrawRectangle.h"
#include "../utils/Logger.h"
#include "Editor.h"
#include "PartFuncData.h"
#include "PartCollision.h"
#include "EditorGlobals.h"
#include "EditorPart.h"
#include "../input/Input.h"

static const Color COLOR_SELECTION_RECTANGLE = { 255, 0, 0, 84 };
static const Color COLOR_GRID_INSIDE = { 84, 84, 153, 255 };
static const Color COLOR_GRID_OUTSIDE = { 118, 118, 172, 255 };

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

static int32_t _mCurrentLayer;
static int32_t _mCurrentPart;
static DrawRectangle* arr_many_rectangles_grid;

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
		 Logger_LogInformation("Mode: Collision");
		 break;
	 /*case PART_TILE_PICKER:
		 OeLogger.LogInformation("Mode: Tile");
		 break;
	 case PART_THING:
		 OeLogger.LogInformation("Mode: Thing");
		 break;
	 case PART_PROP:
		 OeLogger.LogInformation("Mode: Prop");
		 break;
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

 static void SetPartToCollision()
 {
	 SetPart(EDITORPART_COLLISION);
 }

static void DrawGrid(SpriteBatch* spriteBatch)
{
	arrsetlen(arr_many_rectangles_grid, 0);

	int margin = 2;

	Camera* camera = Editor_GetCamera();
	bool isCameraVeryZoomedOut = camera->mWorldZoom > 3;
	int x1 = Camera_GetX1(camera);
	int y1 = Camera_GetY1(camera);

	LevelData* levelData = Get_LevelData();
	int width = LevelData_GetGridSizeX(levelData);
    int height = LevelData_GetGridSizeY(levelData);
	if (levelData->_mIsMetaMap)
	{
		//width = levelData.GetMetaMap().GetTileMapWidth(); //TODO META MAP
	   // height = levelData.GetMetaMap().GetTileMapHeight();
	}

	width = Camera_GetX2(camera, width);
	height = Camera_GetY2(camera, height);

	if (isCameraVeryZoomedOut)
	{
        int bigX = x1 * TILE_SIZE;
        int bigY = y1 * TILE_SIZE;
        int bigWidth = (width * TILE_SIZE) - bigX;
        int bigHeight = (height * TILE_SIZE) - bigY;
		arrput(arr_many_rectangles_grid, DrawRectangle_Create(COLOR_GRID_INSIDE, Rectangle_Create(bigX, bigY, bigWidth, bigHeight)));
    }
    else
	{
		for (int i = x1; i < width; i += 1)
		{
			for (int j = y1; j < height; j += 1)
			{
				arrput(arr_many_rectangles_grid, DrawRectangle_Create(COLOR_GRID_OUTSIDE, 
					Rectangle_Create(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE)));
				arrput(arr_many_rectangles_grid, DrawRectangle_Create(COLOR_GRID_INSIDE, 
					Rectangle_Create(i * TILE_SIZE + (margin / 2), j * TILE_SIZE + (margin / 2), TILE_SIZE - margin, TILE_SIZE - margin)));
			}
		}
	}

	SpriteBatch_DrawManyRectangle(spriteBatch, 0, arr_many_rectangles_grid);
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
	_mEditorParts[0] = PartCollision_GetFuncData();/*
	_mEditorParts[1] = OeEditorPartTile.GetFuncData();
	_mEditorParts[2] = OeEditorPartThing.GetFuncData();
	_mEditorParts[3] = OeEditorPartProp.GetFuncData();
	_mEditorParts[4] = OeEditorPartLine.GetFuncData();
	_mEditorParts[5] = OeEditorPartCamera.GetFuncData();
	_mEditorParts[6] = OeEditorPartMetaMap.GetFuncData();
	_mEditorParts[7] = OeEditorPartDraw.GetFuncData();*/

	_mHasInit = true;

	SetPartToCollision();
}

int32_t ActivePart_GetCurrentLayer(void)
{
	return _mCurrentLayer;
}
void ActivePart_Update(double deltaTime)
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

	ActivePart_GetCurrentPart().Update(deltaTime);
}
void ActivePart_Draw(SpriteBatch* spriteBatch)
{
    //Init();

    //GetCurrentFuncData().Draw(spriteBatch);

    if (_mCurrentPart == EDITORPART_META_MAP)
    {
		return;
	}
	
	LevelData* ld = Get_LevelData();

	DrawGrid(spriteBatch);

	bool useGameCamera = Cvars_GetAsBool(CVARS_EDITOR_USE_GAME_CAMERA);
	LevelData_DrawTiles2(ld, spriteBatch, useGameCamera ? Get_Camera() : Editor_GetCamera(), ActivePart_GetCurrentLayer(), 2);
	LevelData_DrawTiles2(ld, spriteBatch, useGameCamera ? Get_Camera() : Editor_GetCamera(), ActivePart_GetCurrentLayer(), 2);

	LevelData_DrawProps(ld, spriteBatch, Editor_GetCamera());

	/*
	Get_LevelData().DrawCollision(spriteBatch, ref OeEditor.GetCamera());

	if (_mCurrentPart != PART_LINE)
	{
		Get_LevelData().DrawLines(spriteBatch);
	}

	Get_LevelData().DrawThings(spriteBatch, ref OeEditor.GetCamera());

	Get_LevelData().DrawCamera(spriteBatch);

	if (OeCvars.GetAsBool(OeCvars.EDITOR_SHOW_GAME))
	{
		OeGameStateManager.ActiveGameState.Draw(spriteBatch);
	}*/
}
void ActivePart_DrawHud(SpriteBatch* spriteBatch)
{

}
PartFuncData ActivePart_GetCurrentPart(void)
{
	return _mEditorParts[_mCurrentPart];
}
