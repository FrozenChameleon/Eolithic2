#include "EditorPart.h"

#include "Editor.h"
#include "../utils/Utils.h"
#include "../render/DrawRectangle.h"
#include "../leveldata/Tile.h"
#include "../input/Input.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../math/Math.h"
#include "../utils/Logger.h"
#include "../utils/Cvars.h"
#include "EditorGlobals.h"
#include "imgui.h"
#include "../render/DrawTool.h"
#include "../../third_party/stb_ds.h"
#include "../render/SpriteBatch.h"
#include "../render/Renderer.h"
#include "LevelPatch.h"
#include "../core/GameUpdater.h"
#include "../leveldata/Tilemap.h"
#include "../utils/MString.h"

static const Color COLOR_SELECTION_RECTANGLE = { 255, 0, 0, 84 };
static const Color COLOR_GRID_INSIDE = { 84, 84, 153, 255 };
static const Color COLOR_GRID_OUTSIDE = { 118, 118, 172, 255 };

static MString* _mTempString;
static bool _mIsBlocking;
static int _mTabState;
static int _mCurrentLayerAtTimeOfCopy;
static Point _mSelectionRectangleAnchor;
static Vector2 _mMovementSpaceBarAnchor;
static bool _mIsSpaceBarMovement;
static EditorSelectionState _mSelectionState;
static Rectangle _mSelectionRectangle;
static LevelPatch** arr_patches_to_push;
static LevelPatch** arr_patches_undo;
static LevelPatch** arr_patches_redo;
static uint32_t _mUndoDepth;
static Tilemap* _mCopyTiles;
static DrawRectangle* arr_many_rectangles;
static DrawRectangle* arr_many_rectangles_grid;

static void LogBoundsHelper(const char* str, Rectangle bounds)
{
	MString* tempString = NULL;
	MString_AssignString(&tempString, str);
	MString_AddAssignString(&tempString, " Bounds: [");
	MString_AddAssignInt(&tempString, bounds.X);
	MString_AddAssignString(&tempString, ",");
	MString_AddAssignInt(&tempString, bounds.Y);
	MString_AddAssignString(&tempString, ",");
	MString_AddAssignInt(&tempString, bounds.Width);
	MString_AddAssignString(&tempString, ",");
	MString_AddAssignInt(&tempString, bounds.Height);
	MString_AddAssignString(&tempString, "]");
	Logger_Log(LOGGER_INFORMATION, MString_Text(tempString));
	MString_Dispose(&tempString);
}
static void LogBoundsHelperSelectionRectangle(const char* str)
{
	LogBoundsHelper(str, EditorPart_GetSelectionRectangleAsGridBounds());
}

void EditorPart_PushPatchesWithSelectionRectangle()
{
	EditorPart_PushPatches(EditorPart_GetSelectionRectangleAsGridBounds());
}
void EditorPart_PushPatches(Rectangle gridBoundary)
{
	LevelData* ld = Get_LevelData();
	for (int j = gridBoundary.Y; j < (gridBoundary.Y + gridBoundary.Height); j += 1)
	{
		for (int i = gridBoundary.X; i < (gridBoundary.X + gridBoundary.Width); i += 1)
		{
			EditorPart_PushPatch(LevelData_GetTile(ld, i, j));
		}
	}
}
void EditorPart_PushPatch(Tile* src)
{
	arrput(arr_patches_to_push, LevelPatch_Create(src));
}
void EditorPart_FinishPatches()
{
	if (arrlen(arr_patches_to_push) <= 0)
	{
		return;
	}

	for (int i = 0; i < arrlen(arr_patches_to_push); i += 1)
	{
		LevelPatch* lp = arr_patches_to_push[i];
		LevelPatchStatus status = LevelPatch_Finish(lp);
		if (status == LEVEL_PATCH_STATUS_VALID)
		{
			arrput(arr_patches_undo, lp);
			arrsetlen(arr_patches_redo, 0);
		}
	}

	arrsetlen(arr_patches_to_push, 0);
}
void EditorPart_ClearPatches()
{
	arrsetlen(arr_patches_to_push, 0);
	arrsetlen(arr_patches_undo, 0);
	arrsetlen(arr_patches_redo, 0);
}
bool EditorPart_Undo(void)
{
	if (arrlen(arr_patches_undo) <= 0)
	{
		return false;
	}

	uint64_t old_operation_counter = LevelPatch_GetOperationCounter(arrlast(arr_patches_undo));
	while ((arrlen(arr_patches_undo) > 0) && (old_operation_counter == LevelPatch_GetOperationCounter(arrlast(arr_patches_undo))))
	{
		LevelPatch* patch = arrpop(arr_patches_undo);
		LevelPatch_Undo(patch);
		arrput(arr_patches_redo, patch);
	}
	return true;
}
bool EditorPart_Redo(void)
{
	if (arrlen(arr_patches_redo) <= 0)
	{
		return false;
	}

	uint64_t old_operation_counter = LevelPatch_GetOperationCounter(arrlast(arr_patches_redo));
	while ((arrlen(arr_patches_redo) > 0) && (old_operation_counter == LevelPatch_GetOperationCounter(arrlast(arr_patches_redo))))
	{
		LevelPatch* patch = arrpop(arr_patches_redo);
		LevelPatch_Redo(patch);
		arrput(arr_patches_undo, patch);
	}
	return true;
}
bool EditorPart_IsBlocking(void)
{
	return _mIsBlocking;
}
void EditorPart_SetBlocking(bool value)
{
	_mIsBlocking = value;
}
void EditorPart_DummyJustChangedToThisPart(void)
{

}
void EditorPart_DummySave(void)
{

}
void EditorPart_DummyJustLoadedMap(void)
{

}
void EditorPart_DummyCreateWindows(void)
{

}
void EditorPart_DummyUpdate()
{
	Logger_Log(LOGGER_WARNING, "This should not run!");
}
void EditorPart_DummyDraw(SpriteBatch* spriteBatch)
{
	Logger_Log(LOGGER_WARNING, "This should not run!");
}
void EditorPart_DummyDrawHud(SpriteBatch* spriteBatch)
{
	Logger_Log(LOGGER_WARNING, "This should not run!");
}
PartFuncData EditorPart_CreateDummyFuncData(void)
{
	PartFuncData data = { 0 };

	Utils_strlcpy(data.mDebugName, EE_STR_NOT_SET, EE_FILENAME_MAX);

	data.CreateWindows = EditorPart_DummyCreateWindows;
	data.Update = EditorPart_DummyUpdate;
	data.Draw = EditorPart_DummyDraw;
	data.DrawHud = EditorPart_DummyDrawHud;
	data.JustLoadedMap = EditorPart_DummyJustLoadedMap;
	data.JustChangedToThisPart = EditorPart_DummyJustChangedToThisPart;
	data.Save = EditorPart_DummySave;

	return data;
}
void EditorPart_DefaultHandlePatches(void)
{
	if (Input_IsCtrlPressed())
	{
		if (Input_IsKeyTapped(KEYS_Z))
		{
			EditorPart_Undo();
			Do_ResetCollisionGrid();
		}
		if (Input_IsKeyTapped(KEYS_Y))
		{
			EditorPart_Redo();
			Do_ResetCollisionGrid();
		}
	}

	/*
	if (_mArrayPatches.Count > 100)
	{
		_mArrayPatches.RemoveAt(_mArrayPatches.Count - 1);
	}*/
}
void EditorPart_DefaultDrawSingleSelectionSelectedTiles(SpriteBatch* spriteBatch)
{
	if (EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered())
	{
		return;
	}

	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_SINGLE_SELECTED_SQUARE))
	{
		return;
	}

	Rectangle selection = EditorPart_GetSelectionRectangle();
	DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, 150, selection, 0, false, 2);
}
void EditorPart_DefaultHandleColumnsAndRows(void)
{
	int timeHeld = 30;

	Point unrestrainedGrid = EditorPart_GetUnrestrainedCurrentGrid();

	Rectangle levelBoundaryAsGrid = LevelData_GetBoundary(Get_LevelData());

	int addGridX = unrestrainedGrid.X;
	addGridX = Math_MaxInt(addGridX, 0);
	addGridX = Math_MinInt(addGridX, levelBoundaryAsGrid.Width);

	int addGridY = unrestrainedGrid.Y;
	addGridY = Math_MaxInt(addGridY, 0);
	addGridY = Math_MinInt(addGridY, levelBoundaryAsGrid.Height);

	if (Input_IsAltPressed())
	{
		if (Input_IsKeyTapped(KEYS_C) || (Input_GetKeyTimeHeld(KEYS_C) > timeHeld))
		{
			LevelData_AddColumn(Get_LevelData(), addGridX);
			Do_ResetCollisionGrid();
			EditorPart_ClearPatches();
		}
		if (Input_IsKeyTapped(KEYS_R) || (Input_GetKeyTimeHeld(KEYS_R) > timeHeld))
		{
			LevelData_AddRow(Get_LevelData(), addGridY);
			Do_ResetCollisionGrid();
			EditorPart_ClearPatches();
		}
	}
	if (Input_IsTabPressed())
	{
		if (Input_IsKeyTapped(KEYS_C) || (Input_GetKeyTimeHeld(KEYS_C) > timeHeld))
		{
			LevelData_DeleteColumn(Get_LevelData(), EditorPart_GetCurrentGrid().X);
			Do_ResetCollisionGrid();
			EditorPart_ClearPatches();
		}
		if (Input_IsKeyTapped(KEYS_R) || (Input_GetKeyTimeHeld(KEYS_R) > timeHeld))
		{
			LevelData_DeleteRow(Get_LevelData(), EditorPart_GetCurrentGrid().Y);
			Do_ResetCollisionGrid();
			EditorPart_ClearPatches();
		}
	}
}
void EditorPart_DefaultHandleCopy(void)
{
	int counterX = 0;
	int counterY = 0;

	if (_mCopyTiles != NULL)
	{
		if (Input_IsKeyTapped(KEYS_Q))
		{
			EditorPart_ReverseCopy(false);
			return;
		}
		else if (Input_IsKeyTapped(KEYS_E))
		{
			EditorPart_ReverseCopy(true);
			return;
		}
	}

	bool vTapped = Input_IsKeyTapped(KEYS_V);
	if (vTapped && Input_IsCtrlPressed2(true) && (_mCopyTiles != NULL))
	{
		EditorPart_PushPatchesWithSelectionRectangle();
		Tilemap* src = _mCopyTiles;
		Rectangle srcLocation = _mCopyTiles->boundary;
		Tilemap* dst = LevelData_GetTilemap(Get_LevelData());
		Rectangle selection = EditorPart_GetSelectionRectangleAsGridBounds();
		Rectangle dstLocation = Rectangle_Create(selection.X, selection.Y, srcLocation.Width, srcLocation.Height);
		Tilemap_CopyTo(dst, dstLocation, src, srcLocation, true); //TODO PATCH SAFETY
		EditorPart_FinishPatches();

		Do_ResetCollisionGrid();

		LogBoundsHelper("Paste", dstLocation);
	}

	bool isCut = Input_IsKeyTapped(KEYS_X);
	bool isCopy = Input_IsKeyTapped(KEYS_C);
	if (isCut || isCopy)
	{
		if (Input_IsCtrlPressed2(true))
		{
			_mCopyTiles = Tilemap_CreateCloneOfSubsection(LevelData_GetTilemap(Get_LevelData()), EditorPart_GetSelectionRectangleAsGridBounds());

			if (isCut)
			{
				EditorPart_CutTiles(EditorPart_GetSelectionRectangleAsGridBounds());
			}

			_mCurrentLayerAtTimeOfCopy = EditorPart_GetCurrentLayer();

			if (isCopy)
			{
				LogBoundsHelperSelectionRectangle("Copy");
			}
			else if (isCut)
			{
				LogBoundsHelperSelectionRectangle("Cut");
			}

			EditorPart_ResetSelectionRectangle();
		}
	}
}
int EditorPart_GetCurrentLayer(void)
{
	return Cvars_GetAsInt(CVARS_EDITOR_CURRENT_LAYER);
}
void EditorPart_HandleCurrentLayerSelection(void)
{
	if (Input_IsKeyTapped(KEYS_D1))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 0);
	}
	else if (Input_IsKeyTapped(KEYS_D2))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 1);
	}
	else if (Input_IsKeyTapped(KEYS_D3))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 2);
	}
	else if (Input_IsKeyTapped(KEYS_D4))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 3);
	}
	else if (Input_IsKeyTapped(KEYS_D5))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 4);
	}
	else if (Input_IsKeyTapped(KEYS_D6))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 5);
	}
	else if (Input_IsKeyTapped(KEYS_D7))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 6);
	}
	else if (Input_IsKeyTapped(KEYS_D8))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 7);
	}
	else if (Input_IsKeyTapped(KEYS_D9))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 8);
	}
	else if (Input_IsKeyTapped(KEYS_D0))
	{
		Cvars_SetAsInt(CVARS_EDITOR_CURRENT_LAYER, 9);
	}
}
void EditorPart_DefaultHandleKeyMovement()
{
	double speedX = 0;
	double speedY = 0;
	double speed = Cvars_GetAsInt(CVARS_EDITOR_DRAW_MOVE_SPEED);

	if (Input_IsKeyPressed(KEYS_W))
	{
		speedY = -speed;
	}
	if (Input_IsKeyPressed(KEYS_D))
	{
		speedX = speed;
	}
	if (Input_IsKeyPressed(KEYS_S))
	{
		speedY = speed;
	}
	if (Input_IsKeyPressed(KEYS_A))
	{
		speedX = -speed;
	}

	if (Input_IsAltPressed())
	{
		speedX = speedX * 2;
		speedY = speedY * 2;
	}

	if ((speedX != 0) || (speedY != 0))
	{
		Vector2 currentPos = EditorPart_GetEditorPosition(0);
		Vector2 speedVec = Vector2_Create((float)speedX, (float)speedY);
		EditorPart_SetEditorPosition(0, Vector2_Add(currentPos, speedVec));
	}
}
void EditorPart_HandleTab(void)
{
	if (Input_IsKeyTapped(KEYS_TAB))
	{
		if (_mTabState == 0)
		{
			Cvars_SetAsBool(CVARS_EDITOR_SHOW_TILE_INFO, true);
			Cvars_SetAsBool(CVARS_EDITOR_DRAW_SHOW_GRID, true);
			Cvars_SetAsBool(CVARS_EDITOR_DRAW_GRID_BLACK, true);
		}
		else if (_mTabState == 1)
		{
			Cvars_SetAsBool(CVARS_EDITOR_SHOW_TILE_INFO, true);
			Cvars_SetAsBool(CVARS_EDITOR_DRAW_SHOW_GRID, true);
			Cvars_SetAsBool(CVARS_EDITOR_DRAW_GRID_BLACK, false);
		}
		else if (_mTabState == 2)
		{
			Cvars_SetAsBool(CVARS_EDITOR_SHOW_TILE_INFO, false);
			Cvars_SetAsBool(CVARS_EDITOR_DRAW_SHOW_GRID, false);
		}

		_mTabState += 1;
		if (_mTabState > 2)
		{
			_mTabState = 0;
		}
	}
}
int EditorPart_GetEditorZoom(int32_t index)
{
	MString_AssignString(&_mTempString, CVARS_EDITOR_ZOOM);
	MString_AddAssignInt(&_mTempString, index);
	return Cvars_GetAsInt(MString_Text(_mTempString));
}
void EditorPart_SetEditorZoom(int32_t index, int32_t zoom)
{
	MString_AssignString(&_mTempString, CVARS_EDITOR_ZOOM);
	MString_AddAssignInt(&_mTempString, index);
	Cvars_SetAsInt(MString_Text(_mTempString), zoom);
}
Vector2 EditorPart_GetEditorPosition(int32_t index)
{
	MString_AssignString(&_mTempString, CVARS_EDITOR_POSITION_X);
	MString_AddAssignInt(&_mTempString, index);
	float positionX = Cvars_GetAsFloat(MString_Text(_mTempString));

	MString_AssignString(&_mTempString, CVARS_EDITOR_POSITION_Y);
	MString_AddAssignInt(&_mTempString, index);
	float positionY = Cvars_GetAsFloat(MString_Text(_mTempString));

	return Vector2_Create(positionX, positionY);
}
void EditorPart_SetEditorPosition(int32_t index, Vector2 position)
{
	MString_AssignString(&_mTempString, CVARS_EDITOR_POSITION_X);
	MString_AddAssignInt(&_mTempString, index);
	Cvars_SetAsFloat(MString_Text(_mTempString), position.X);

	MString_AssignString(&_mTempString, CVARS_EDITOR_POSITION_Y);
	MString_AddAssignInt(&_mTempString, index);
	Cvars_SetAsFloat(MString_Text(_mTempString), position.Y);
}
void EditorPart_DefaultUpdateCameraZoom(void)
{
	Camera* camera = Editor_GetCamera();
	int32_t editorZoom = EditorPart_GetEditorZoom(0);
	camera->mWorldZoom = editorZoom / 100.0f;
}
void EditorPart_DefaultUpdateCameraPosition(void)
{
	Camera_SetPositionVector2(Editor_GetCamera(), EditorPart_GetEditorPosition(0));
}
void EditorPart_DefaultHandleDeleteKey(void)
{
	if (!Input_IsKeyTapped(KEYS_DELETE))
	{
		return;
	}

	Point currentGrid = EditorPart_GetCurrentGrid();
	int x1 = currentGrid.X;
	int x2 = x1 + 1;
	int y1 = currentGrid.Y;
	int y2 = y1 + 1;

	if (EditorPart_IsSelecting())
	{
		x1 = EditorPart_GetSelectionRectangleGridX1();
		x2 = EditorPart_GetSelectionRectangleGridX2();
		y1 = EditorPart_GetSelectionRectangleGridY1();
		y2 = EditorPart_GetSelectionRectangleGridY2();
	}

	Rectangle gridBoundary = Rectangle_Create(x1, y1, x2 - x1, y2 - y1);
	EditorPart_PushPatches(gridBoundary);

	for (int i = x1; i < x2; i += 1)
	{
		for (int j = y1; j < y2; j += 1)
		{
			Tile* tile = EditorPart_GetGridTile(i, j);

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_COLLISION))
			{
				Tile_DeleteCollision(tile);
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_TILES))
			{
				Tile_DeleteDrawTiles(tile);
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_THINGS))
			{
				Tile_DeleteThings(tile);
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_PROPS))
			{
				Tile_DeleteProps(tile);
			}
		}
	}

	EditorPart_FinishPatches();
}
void EditorPart_DoDefaultEditorPartUpdateRoutine()
{
	EditorPart_DefaultHandleKeyMovement();

	EditorPart_DefaultHandleSpacebarHingeMovement();

	EditorPart_DefaultHandleZoom();

	EditorPart_DefaultHandleCopy();

	EditorPart_DefaultHandlePatches();

	EditorPart_DefaultHandleColumnsAndRows();

	EditorPart_DefaultHandleDeleteKey();

	EditorPart_DefaultUpdateCameraZoom();

	EditorPart_DefaultUpdateCameraPosition();
}
void EditorPart_DoDefaultEditorPartDrawRoutine(SpriteBatch* spriteBatch, EditorDrawFunc drawSingleSelectionSelectedTiles)
{
	arrsetlen(arr_many_rectangles, 0);

	if (Cvars_GetAsBool(CVARS_EDITOR_SHOW_GRID))
	{
		EditorPart_DrawGrid(spriteBatch);
	}

	if (EditorPart_IsSelecting())
	{
		int x1 = EditorPart_AlignToGridInt(_mSelectionRectangle.X) * TILE_SIZE;
		int y1 = EditorPart_AlignToGridInt(_mSelectionRectangle.Y) * TILE_SIZE;

		int tileWidth = ((EditorPart_AlignToGridInt(Rectangle_Right(&_mSelectionRectangle)) * TILE_SIZE)) - x1;
		int tileHeight = ((EditorPart_AlignToGridInt(Rectangle_Bottom(&_mSelectionRectangle)) * TILE_SIZE)) - y1;

		if (!EditorGlobals_ImGuiIsAnyItemActiveOrFocusedOrHovered())
		{
			DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, 111, Rectangle_Create(x1, y1, tileWidth, tileHeight), 0, false, 2);
		}
	}

	if (Input_IsCtrlPressed2(true) && (_mCopyTiles != NULL))
	{
		EditorPart_DrawTilesBorder(spriteBatch, _mCopyTiles);
		EditorPart_DrawCopyTiles(spriteBatch);
	}
	else
	{
		drawSingleSelectionSelectedTiles(spriteBatch);
	}

	if (Cvars_GetAsBool(CVARS_EDITOR_DRAW_SHOW_GRID))
	{
		Camera* camera = Editor_GetCamera();
		int width = Camera_GetScreenWidthInTiles(camera) + 2;
		int height = Camera_GetScreenHeightInTiles(camera) + 2;
		int leftOfCamera = Camera_GetLeft(camera);
		int topOfCamera = Camera_GetTop(camera);
		Rectangle levelBoundaryGrid = LevelData_GetBoundary(Get_LevelData());
		for (int i = 0; i < width; i += 1)
		{
			for (int j = 0; j < height; j += 1)
			{
				Point position = Point_Create(((leftOfCamera / TILE_SIZE) * TILE_SIZE) + (i * TILE_SIZE),
					((topOfCamera / TILE_SIZE) * TILE_SIZE) + (j * TILE_SIZE));

				if ((position.X >= 0) && 
					(position.Y >= 0) && 
					(position.X < levelBoundaryGrid.Width) &&
					position.Y < levelBoundaryGrid.Height)
				{
					Color tempColor = Cvars_GetAsBool(CVARS_EDITOR_DRAW_GRID_BLACK) ? COLOR_WHITE : COLOR_BLACK;
					arrput(arr_many_rectangles, DrawRectangle_Create(tempColor, Rectangle_Create(position.X, position.Y, TILE_SIZE / 5, 1)));
					arrput(arr_many_rectangles, DrawRectangle_Create(tempColor, Rectangle_Create(position.X, position.Y, 1, TILE_SIZE / 5)));
				}
			}
		}
	}

	SpriteBatch_DrawManyRectangle(spriteBatch, 100, arr_many_rectangles);
}
void EditorPart_DoDefaultEditorPartDrawHudRoutine(SpriteBatch* spriteBatch, const char* status)
{
	Vector2 mouse = Input_GetMouse();
	Vector2 adjustedMouse = Input_GetCameraAdjustedMouse(Editor_GetCamera());

	MString* display = MString_CreateForJustThisFrame();

	MString_AddAssignString(&display, "|Mouse:");
	MString_AddAssignInt(&display, (int32_t)mouse.X);
	MString_AddAssignString(&display, ",");
	MString_AddAssignInt(&display, (int32_t)mouse.Y);

	MString_AddAssignString(&display, "|AdjustedMouse:");
	MString_AddAssignInt(&display, (int32_t)adjustedMouse.X);
	MString_AddAssignString(&display, ",");
	MString_AddAssignInt(&display, (int32_t)adjustedMouse.Y);

	MString_AddAssignString(&display, "|X-Tile:");
	MString_AddAssignInt(&display, (int32_t)(adjustedMouse.X / TILE_SIZE));

	MString_AddAssignString(&display, "|Y-Tile:");
	MString_AddAssignInt(&display, (int32_t)(adjustedMouse.Y / TILE_SIZE));

	MString_AddAssignString(&display, "|X:");
	MString_AddAssignInt(&display, (int32_t)(adjustedMouse.X));

	MString_AddAssignString(&display, "|Y:");
	MString_AddAssignInt(&display, (int32_t)(adjustedMouse.Y));

	MString_AddAssignString(&display, "|X (Lock):");
	MString_AddAssignInt(&display, (int32_t)((mouse.X / TILE_SIZE) * TILE_SIZE));

	MString_AddAssignString(&display, "|Y (Lock):");
	MString_AddAssignInt(&display, (int32_t)((mouse.Y / TILE_SIZE) * TILE_SIZE));

	MString_AddAssignString(&display, "|Scale:");
	MString_AddAssignFloat(&display, Editor_GetCamera()->mWorldZoom);

	MString_AddAssignString(&display, "|Select X");
	MString_AddAssignInt(&display, EditorPart_GetSelectionRectangleGridX2() - EditorPart_GetSelectionRectangleGridX1());

	MString_AddAssignString(&display, "|Select Y");
	MString_AddAssignInt(&display, EditorPart_GetSelectionRectangleGridY2() - EditorPart_GetSelectionRectangleGridY1());

	/*
	string parallaxDisplay = "";// "|:" + returnBoolean(EditorGlobals.SHOW_PARALLAX);
	string collisionDisplay = "|Collision:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_COLLISION));
	string thingDisplay = "|Thing:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_THINGS));
	string propDisplay = "|Prop:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_PROPS));
	string tileParallaxDisplay = "";// " |T-Parallax:" + returnBoolean(!EditorGlobals.DISABLE_TILE_PARALLAX);
	string gridDisplay = "|Grid:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_GRID));
	string tileDisplay = "|Tile:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_TILES));
	string autoTilerDisplay = "|Auto:" + (Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER) == -1 ? "Off" : Cvars_Get(Cvars_EDITOR_AUTO_TILER));
	*/

	MString_AddAssignString(&display, "|FPS:");
	MString_AddAssignInt(&display, GameUpdater_GetFPS());

	Rectangle drawableSize = Renderer_GetDrawableSize();
	DrawTool_DrawRectangle2(spriteBatch, COLOR_BLACK, 100,
		Rectangle_Create(0, drawableSize.Height - EDITOR_GLOBALS_MENU_ITEM_SIZE, drawableSize.Width, EDITOR_GLOBALS_MENU_ITEM_SIZE), 0, false);

	SpriteBatch_DrawString(spriteBatch, "editor", MString_Text(display), Color_White, 150,
		Vector2_Create(0, (float)(drawableSize.Height - EDITOR_GLOBALS_MENU_ITEM_SIZE)));
	//spriteBatch.DrawString(, status + scaleDisplay + selectedX + selectedY + xTileDisplay + yTileDisplay + xDisplay + yDisplay + xLockedDisplay + yLockedDisplay + parallaxDisplay + collisionDisplay + thingDisplay
	//+ propDisplay + tileParallaxDisplay + gridDisplay + tileDisplay + autoTilerDisplay + fpsDisplay, OeColors.WHITE, 100, new Vector2(0, OeGame.GetCurrentBackBufferHeight() - OeGui.MENU_ITEM_SIZE));*/
}
void EditorPart_DrawCopyTiles(SpriteBatch* spriteBatch)
{
	Point currentGrid = EditorPart_GetCurrentGrid();
	DrawTool_DrawRectangle2(spriteBatch, Color_Create4(0, 0, 0, 120), 190,
		Rectangle_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE, 
			_mCopyTiles->boundary.Width * TILE_SIZE, _mCopyTiles->boundary.Height * TILE_SIZE), 0, false);

	Texture* texture = LevelData_GetTilesetTexture(Get_LevelData());
	for (int i = 0; i < _mCopyTiles->boundary.Width; i += 1)
	{
		for (int j = 0; j < _mCopyTiles->boundary.Height; j += 1)
		{
			Tile* tile = Tilemap_GetTile(_mCopyTiles, i, j);

			int adjX = currentGrid.X + i;
			int adjY = currentGrid.Y + j;

			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_COLLISION))
			{
				Tile_DrawCollision(tile, spriteBatch, Editor_GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_THINGS))
			{
				Tile_DrawThings(tile, spriteBatch, Editor_GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_PROPS))
			{
				Tile_DrawProps(tile, spriteBatch, Editor_GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_TILES))
			{
				EditorPart_DrawTilesAsEditor(tile, spriteBatch, adjX, adjY, Get_LevelData(), true, texture);
			}
		}
	}
}
void EditorPart_DrawTilesAsEditor(Tile* tile, SpriteBatch* spriteBatch, int gridX, int gridY,
	LevelData* level, bool overrideDepth, Texture* wrapper)
{
	for (int i = 0; i < TILE_DRAW_LAYER_LENGTH; i += 1)
	{
		if (!Cvars_GetAsBool(CVARS_EDITOR_COPY_LAYER_ONLY) || (i == _mCurrentLayerAtTimeOfCopy))
		{
			MString* tempString = MString_CreateForJustThisFrame();
			MString_AssignString(&tempString, CVARS_EDITOR_SHOW_LAYER);
			MString_AddAssignInt(&tempString, i);
			if (Cvars_GetAsBool(MString_Text(tempString)))
			{
				int currentDepth = level->mLayerData[i].mDepth;
				if (overrideDepth)
				{
					currentDepth = TILE_OVERRIDE_DEPTH;
				}
				DrawTile_Draw(&tile->mDrawTiles[i], spriteBatch, wrapper, COLOR_WHITE, currentDepth, gridX * TILE_SIZE, gridY * TILE_SIZE);
			}
		}
	}
}
void EditorPart_DrawTilesBorder(SpriteBatch* spriteBatch, Tilemap* tilemap)
{
	Point currentGrid = EditorPart_GetCurrentGrid();
	int x = currentGrid.X * TILE_SIZE;
	int y = currentGrid.Y * TILE_SIZE;

	int width = tilemap->boundary.Width * TILE_SIZE;
	int height = tilemap->boundary.Height * TILE_SIZE;

	Rectangle boundaryInPixels = LevelData_GetBoundaryInPixels(Get_LevelData());
	int widthAdjustment = (x + width) - boundaryInPixels.Width;
	int heightAdjustment = (y + height) - boundaryInPixels.Height;

	if (widthAdjustment > 0)
	{
		width -= widthAdjustment;
	}
	if (heightAdjustment > 0)
	{
		height -= heightAdjustment;
	}

	DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, 100, Rectangle_Create(x, y, width, height), 0, false, 2);
}
void EditorPart_CutTiles(Rectangle srcLocation)
{
	EditorPart_PushPatches(srcLocation);

	for (int i = srcLocation.X; i < (srcLocation.X + srcLocation.Width); i += 1)
	{
		for (int j = srcLocation.Y; j < (srcLocation.Y + srcLocation.Height); j += 1)
		{
			Tile* originalTile = LevelData_GetTile(Get_LevelData(), i, j);
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_COLLISION))
			{
				Tile_DeleteCollision(originalTile);
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_TILES))
			{
				if (!Cvars_GetAsBool(CVARS_EDITOR_COPY_LAYER_ONLY))
				{
					Tile_DeleteDrawTiles(originalTile);
				}
				else
				{
					Tile_DeleteDrawTilesByLayer(originalTile, EditorPart_GetCurrentLayer());
				}
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_THINGS))
			{
				Tile_DeleteThings(originalTile);
			}
			if (Cvars_GetAsBool(CVARS_EDITOR_COPY_PROPS))
			{
				Tile_DeleteProps(originalTile);
			}
		}
	}

	EditorPart_FinishPatches();

	Do_ResetCollisionGrid();
}
void EditorPart_Move(void)
{
	//UNUSED?
}
bool EditorPart_IsSelecting(void)
{
	return (_mSelectionState != EDITOR_SELECTION_STATE_NOTHING);
}
void EditorPart_ReverseCopy(bool isFlipY)
{
	if (_mCopyTiles == NULL)
	{
		return;
	}

	Tilemap* newTiles = Tilemap_Create(_mCopyTiles->boundary);

	int xLength = newTiles->boundary.Width;
	int yLength = newTiles->boundary.Height;
	for (int i = 0; i < xLength; i += 1)
	{
		for (int j = 0; j < yLength; j += 1)
		{
			int newLocX = (xLength - 1) - i;
			int newLocY = (yLength - 1) - j;

			if (!isFlipY)
			{
				Tilemap_SetTile(newTiles, i, j, Tile_Clone(Tilemap_GetTile(_mCopyTiles, newLocX, j)));
			}
			else
			{
				Tilemap_SetTile(newTiles, i, j, Tile_Clone(Tilemap_GetTile(_mCopyTiles, i, newLocY)));
			}

			DrawTile* draws = Tilemap_GetTile(newTiles, i, j)->mDrawTiles;
			for (int k = 0; k < TILE_DRAW_LAYER_LENGTH; k += 1)
			{
				if (!isFlipY)
				{
					draws[k].mFlipX = !draws[k].mFlipX;
				}
				else
				{
					draws[k].mFlipY = !draws[k].mFlipY;
				}
				if (draws[k].mRotation == 90)
				{
					draws[k].mRotation = 270;
				}
				else if (draws[k].mRotation == 270)
				{
					draws[k].mRotation = 90;
				}
			}
		}
	}

	_mCopyTiles = newTiles;
}
void EditorPart_HandleReverseCopy(void)
{
	//UNUSED?
}
void EditorPart_DrawGrid(SpriteBatch* spriteBatch)
{
	arrsetlen(arr_many_rectangles_grid, 0);

	int margin = 2;

	Camera* camera = Editor_GetCamera();
	bool isCameraVeryZoomedOut = camera->mWorldZoom > 3;
	int x1 = Camera_GetX1(camera);
	int y1 = Camera_GetY1(camera);

	LevelData* levelData = Get_LevelData();
	int width = LevelData_GetBoundary(levelData).Width;
	int height = LevelData_GetBoundary(levelData).Height;
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
int32_t EditorPart_AlignToGridInt(int32_t pixelValue)
{
	return Utils_AlignToTileGridInt(pixelValue, UTILS_ROUNDING_MODE_FLOOR, false);
}
int32_t EditorPart_AlignToGridFloat(float pixelValue)
{
	return Utils_AlignToTileGridFloat(pixelValue, UTILS_ROUNDING_MODE_FLOOR, false);
}
Point EditorPart_AlignToGridVector2(Vector2 pixelPos)
{
	return Utils_AlignToTileGridVector2(pixelPos, UTILS_ROUNDING_MODE_FLOOR, false);
}
Point EditorPart_AlignToGridPoint(Point pixelPos)
{
	return Utils_AlignToTileGridPoint(pixelPos, UTILS_ROUNDING_MODE_FLOOR, false);
}
Vector2 EditorPart_GetUnrestrainedMouse(void)
{
	return Input_GetCameraAdjustedMouse(Editor_GetCamera());
}
Vector2 EditorPart_GetRestrainedMouse(void)
{
	return EditorPart_GetRestrainedValue(EditorPart_GetUnrestrainedMouse());
}
Vector2 EditorPart_GetRestrainedValue(Vector2 vec)
{
	Rectangle boundaryInPixels = LevelData_GetBoundaryInPixels(Get_LevelData());

	float x = vec.X;
	x = Math_MaxFloat(x, 0);
	x = Math_MinFloat(x, (float)(boundaryInPixels.Width - 1));

	float y = vec.Y;
	y = Math_MaxFloat(y, 0);
	y = Math_MinFloat(y, (float)(boundaryInPixels.Height - 1));

	return Vector2_Create(x, y);
}
Point EditorPart_GetUnrestrainedCurrentGrid(void)
{
	return EditorPart_AlignToGridVector2(EditorPart_GetUnrestrainedMouse());
}
Point EditorPart_GetCurrentGrid(void)
{
	return EditorPart_AlignToGridVector2(EditorPart_GetRestrainedMouse());
}
Tile* EditorPart_GetCurrentGridTile(void)
{
	Point temp = EditorPart_GetCurrentGrid();
	return LevelData_GetTile(Get_LevelData(), temp.X, temp.Y);
}
Point EditorPart_GetGridAlignedMousePixel(void)
{
	Point currentGrid = EditorPart_GetCurrentGrid();
	return Point_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE);
}
Tile* EditorPart_GetGridTileFromPixel(Vector2 pos)
{
	Point temp = EditorPart_AlignToGridVector2(pos);
	return LevelData_GetTile(Get_LevelData(), temp.X, temp.Y);
}
Tile* EditorPart_GetGridTile(int x, int y)
{
	return LevelData_GetTile(Get_LevelData(), x, y);
}
Tile* EditorPart_GetGridTilePoint(Point p)
{
	return LevelData_GetTile(Get_LevelData(), p.X, p.Y);
}
void EditorPart_DefaultHandleZoom(void)
{
	bool ctrl = Input_IsCtrlPressed();

	int amount = 10;

	int currentZoom = EditorPart_GetEditorZoom(0);
	int backup = currentZoom;

	if ((Input_JustScrolledDown() && ctrl) || Input_IsKeyTapped(KEYS_SUBTRACT))
	{
		currentZoom += amount;
	}
	else if ((Input_JustScrolledUp() && ctrl) || Input_IsKeyTapped(KEYS_ADD))
	{
		currentZoom -= amount;
		int min = 5;
		if (currentZoom < min)
		{
			currentZoom = min;
		}
	}

	if (currentZoom != backup)
	{
		EditorPart_SetEditorZoom(0, currentZoom);
	}
}
Rectangle EditorPart_GetSelectionRectangle(void)
{
	if (_mSelectionState == EDITOR_SELECTION_STATE_NOTHING)
	{
		Point currentGrid = EditorPart_GetCurrentGrid();
		Rectangle singleSelection = Rectangle_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
		return singleSelection;
	}
	else
	{
		return _mSelectionRectangle;
	}
}
Rectangle EditorPart_GetSelectionRectangleAsGridBounds(void)
{
	Rectangle selectionRectangle = EditorPart_GetSelectionRectangle();
	Rectangle temp;
	temp.X = EditorPart_AlignToGridInt(selectionRectangle.X);
	temp.Y = EditorPart_AlignToGridInt(selectionRectangle.Y);
	temp.Width = EditorPart_AlignToGridInt(selectionRectangle.Width);
	temp.Height = EditorPart_AlignToGridInt(selectionRectangle.Height);
	return temp;
}
int EditorPart_GetSelectionRectangleGridX1(void)
{
	return EditorPart_AlignToGridInt(_mSelectionRectangle.X);
}
int EditorPart_GetSelectionRectangleGridX2(void)
{
	return EditorPart_AlignToGridInt(Rectangle_Right(&_mSelectionRectangle));
}
int EditorPart_GetSelectionRectangleGridY1(void)
{
	return EditorPart_AlignToGridInt(_mSelectionRectangle.Y);
}
int EditorPart_GetSelectionRectangleGridY2(void)
{
	return EditorPart_AlignToGridInt(Rectangle_Bottom(&_mSelectionRectangle));
}
void EditorPart_DefaultHandleSpacebarHingeMovement()
{
	if (Input_IsKeyPressed(KEYS_SPACE))
	{
		if (!_mIsSpaceBarMovement)
		{
			_mMovementSpaceBarAnchor.X = Input_GetMouseX();
			_mMovementSpaceBarAnchor.Y = Input_GetMouseY();
		}
		_mIsSpaceBarMovement = true;
	}
	else
	{
		_mIsSpaceBarMovement = false;
	}

	if (_mIsSpaceBarMovement)
	{
		float movementX = Input_GetMouseX() - _mMovementSpaceBarAnchor.X;
		float movementY = Input_GetMouseY() - _mMovementSpaceBarAnchor.Y;

		int deadzone = 15;

		if (Math_fabsf(movementX) < deadzone)
		{
			movementX = 0;
		}
		else
		{
			if (movementX > deadzone)
			{
				movementX -= deadzone;
			}
			else
			{
				movementX += deadzone;
			}
		}

		if (Math_fabsf(movementY) < deadzone)
		{
			movementY = 0;
		}
		else
		{
			if (movementY > deadzone)
			{
				movementY -= deadzone;
			}
			else
			{
				movementY += deadzone;
			}
		}

		Vector2 halfMovement = Vector2_Create(-movementX / 2.0f, -movementY / 2.0f);
		Vector2 newPosition = Vector2_Add(EditorPart_GetEditorPosition(0), halfMovement);
		EditorPart_SetEditorPosition(0, newPosition);
	}
}
bool EditorPart_IsMouseInsideSelectionRectangle(void)
{
	Vector2 restrainedMouse = EditorPart_GetRestrainedMouse();
	return Rectangle_Contains(&_mSelectionRectangle, (int32_t)restrainedMouse.X, (int32_t)restrainedMouse.Y);
}
void EditorPart_ResetSelectionRectangle(void)
{
	_mSelectionState = EDITOR_SELECTION_STATE_NOTHING;
	_mSelectionRectangle = Rectangle_Empty;
	_mSelectionRectangleAnchor = Point_Zero;
}
bool EditorPart_HandleSelectionRectangle(void)
{
	if (_mSelectionState != EDITOR_SELECTION_STATE_SELECTING)
	{
		if (!EditorPart_IsMouseInsideSelectionRectangle() && !Rectangle_IsEmpty(&_mSelectionRectangle))
		{
			if (Input_IsLeftMouseTapped() ||
				Input_IsMiddleMouseTapped() ||
				Input_IsRightMouseTapped())
			{
				EditorPart_ResetSelectionRectangle();
				return true;
			}
		}
		if (Input_IsShiftTapped())
		{
			EditorPart_ResetSelectionRectangle();
			_mSelectionRectangleAnchor = EditorPart_GetCurrentGrid();
			_mSelectionState = EDITOR_SELECTION_STATE_SELECTING;
		}
	}

	switch (_mSelectionState)
	{
	case EDITOR_SELECTION_STATE_NOTHING:
	{
		Point currentGrid = EditorPart_GetCurrentGrid();
		_mSelectionRectangle = Rectangle_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE, TILE_SIZE, TILE_SIZE);

	}
	break;
	case EDITOR_SELECTION_STATE_SELECTING:
	{
		PointRectangle prect = { 0 };
		prect.mPointOne = _mSelectionRectangleAnchor;
		prect.mPointTwo = EditorPart_GetCurrentGrid();
		_mSelectionRectangle = PointRectangle_GetRectangle(&prect);
		_mSelectionRectangle.Width += 1;
		_mSelectionRectangle.Height += 1;
		{
			MString* tempString = NULL;
			MString_AssignEmptyString(&tempString);
			MString_AddAssignInt(&tempString, _mSelectionRectangle.Width);
			MString_AddAssignString(&tempString, ",");
			MString_AddAssignInt(&tempString, _mSelectionRectangle.Height);
			ImGui::SetTooltip(MString_Text(tempString));
			MString_Dispose(&tempString);
		}
		_mSelectionRectangle.X *= TILE_SIZE;
		_mSelectionRectangle.Y *= TILE_SIZE;
		_mSelectionRectangle.Width *= TILE_SIZE;
		_mSelectionRectangle.Height *= TILE_SIZE;
		if (!Input_IsShiftPressed())
		{
			if (_mSelectionRectangle.Width <= TILE_SIZE && _mSelectionRectangle.Height <= TILE_SIZE)
			{
				EditorPart_ResetSelectionRectangle();
			}
			else
			{
				_mSelectionState = EDITOR_SELECTION_STATE_SELECTED;
			}
		}
	}
	break;
	case EDITOR_SELECTION_STATE_SELECTED:
	{
	}
	break;
	}

	return false;
}