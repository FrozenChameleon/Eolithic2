#include "EditorActivePart.h"

#include "../render/SpriteBatch.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../utils/Cvars.h"
#include "Editor.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../render/DrawRectangle.h"

static const Color COLOR_SELECTION_RECTANGLE = { 255, 0, 0, 84 };
static const Color COLOR_GRID_INSIDE = { 84, 84, 153, 255 };
static const Color COLOR_GRID_OUTSIDE = { 118, 118, 172, 255 };

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

static int32_t _mCurrentLayer;
static EditorPartType _mCurrentPart;
static DrawRectangle* arr_many_rectangles_grid;

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

int32_t EditorActivePart_GetCurrentLayer(void)
{
	return _mCurrentLayer;
}
void EditorActivePart_Draw(SpriteBatch* spriteBatch)
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
	LevelData_DrawTiles2(ld, spriteBatch, useGameCamera ? Get_Camera() : Editor_GetCamera(), EditorActivePart_GetCurrentLayer(), 2);

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
void EditorActivePart_DrawHud(SpriteBatch* spriteBatch)
{

}