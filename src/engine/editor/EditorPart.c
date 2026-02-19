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

static const Color COLOR_SELECTION_RECTANGLE = { 255, 0, 0, 84 };
static const Color COLOR_GRID_INSIDE = { 84, 84, 153, 255 };
static const Color COLOR_GRID_OUTSIDE = { 118, 118, 172, 255 };

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

static MString* _mTempString;
static bool _mIsBlocking;
static int _mTabState;
static int _mCurrentLayerAtTimeOfCopy;
static Point _mCopyNodeAnchor;
static Point _mSelectionRectangleAnchor;
static Vector2 _mMovementSpaceBarAnchor;
static bool _mIsSpaceBarMovement;
static EditorSelectionState _mSelectionState;
static Rectangle _mSelectionRectangle;
static LevelPatch* arr_patches;
static LevelPatch* _mPatch1;
static LevelPatch* _mPatch2;
static Tile** _mCopyTiles;
static DrawRectangle* arr_many_rectangles;
static DrawRectangle* arr_many_rectangles_grid;

void EditorPart_SetPatch1(LevelPatch* patch)
{
	_mPatch1 = patch;
}
void EditorPart_SetPatch2(LevelPatch* patch)
{
	_mPatch2 = patch;
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
void EditorPart_DummyUpdate(double deltaTime)
{
	Logger_LogWarning("This should not run!");
}
void EditorPart_DummyDraw(SpriteBatch* spriteBatch)
{
	Logger_LogWarning("This should not run!");
}
void EditorPart_DummyDrawHud(SpriteBatch* spriteBatch)
{
	Logger_LogWarning("This should not run!");
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
	/*if (_mArrayPatches.Count == 0)
	{
		return;
	}

	if (Input_IsCtrlPressed())
	{
		if (Input_IsKeyTapped(Keys_Z))
		{
			_mArrayPatches[0].ApplyPatch(GetLevelData());
			_mArrayPatches.RemoveAt(0);
			OeFunc.Do_ResetCollisionGrid();
		}
	}

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

	Point currentGrid = EditorPart_GetCurrentGrid();
	DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, 150,
		Rectangle_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE, TILE_SIZE, TILE_SIZE), 0, false, 2);
}
void EditorPart_DefaultHandleColumnsAndRows(void)
{
	/*int timeHeld = 30;

	Point unrestrainedGrid = GetUnrestrainedCurrentGrid();

	int addGridX = unrestrainedGrid.X;
	addGridX = OeMath.Max(addGridX, 0);
	addGridX = OeMath.Min(addGridX, GetLevelData().GetTileData().GetLength(0));

	int addGridY = unrestrainedGrid.Y;
	addGridY = OeMath.Max(addGridY, 0);
	addGridY = OeMath.Min(addGridY, GetLevelData().GetTileData().GetLength(1));

	if (Input_IsAltPressed())
	{
		if (Input_IsKeyTapped(Keys_C) || (Input_GetKeyTimeHeld(Keys_C) > timeHeld))
		{
			GetLevelData().AddColumn(addGridX);
			OeFunc.Do_ResetCollisionGrid();
			_mArrayPatches.Clear();
		}
		if (Input_IsKeyTapped(Keys_R) || (Input_GetKeyTimeHeld(Keys_R) > timeHeld))
		{
			GetLevelData().AddRow(addGridY);
			OeFunc.Do_ResetCollisionGrid();
			_mArrayPatches.Clear();
		}
	}
	if (Input_IsTabPressed())
	{
		if (Input_IsKeyTapped(Keys_C) || (Input_GetKeyTimeHeld(Keys_C) > timeHeld))
		{
			GetLevelData().DeleteColumn(GetCurrentGrid().X);
			_mArrayPatches.Clear();
		}
		if (Input_IsKeyTapped(Keys_R) || (Input_GetKeyTimeHeld(Keys_R) > timeHeld))
		{
			GetLevelData().DeleteRow(GetCurrentGrid().Y);
			_mArrayPatches.Clear();
		}
	}*/
}
void EditorPart_DefaultHandleCopy(void)
{
	/*int counterX = 0;
	int counterY = 0;

	if (_mCopyTiles != null)
	{
		if (Input_IsKeyTapped(Keys_Q))
		{
			ReverseCopy(false);
			return;
		}
		else if (Input_IsKeyTapped(Keys_E))
		{
			ReverseCopy(true);
			return;
		}
	}

	bool vTapped = Input_IsKeyTapped(Keys_V);
	if (vTapped && Input_IsCtrlPressed(true) && _mCopyTiles != null)
	{
		Point currentGrid = GetCurrentGrid();

		int x1 = currentGrid.X;
		int y1 = currentGrid.Y;

		int x2 = OeMath.Min(x1 + _mCopyTiles.GetLength(0), GetLevelData().GetGridSizeX());
		int y2 = OeMath.Min(y1 + _mCopyTiles.GetLength(1), GetLevelData().GetGridSizeY());

		_mPatch1 = GetPatch(x1, x2, y1, y2, x1, y1);

		for (int i = x1; i < x2; i += 1)
		{
			for (int j = y1; j < y2; j += 1)
			{
				OeTile clone = _mCopyTiles[counterX, counterY].GetClone();

				if (OeEditorUtils.IsCopyingEverything())
				{
					GetLevelData().SetTile(i, j, clone);
				}
				else
				{
					OeTile oldTile = GetLevelData().GetTile(i, j);
					if (oldTile != null)
					{
						if (Cvars_GetAsBool(Cvars_EDITOR_COPY_COLLISION))
						{
							oldTile.mCollisionType = clone.mCollisionType;
						}
						if (Cvars_GetAsBool(Cvars_EDITOR_COPY_TILES))
						{
							if (!Cvars_GetAsBool(Cvars_EDITOR_COPY_LAYER_ONLY))
							{
								oldTile.mDrawTiles = clone.mDrawTiles;
							}
							else
							{
								oldTile.mDrawTiles[GetCurrentLayer()] = clone.mDrawTiles[_mCurrentLayerAtTimeOfCopy];
							}
						}
						if (Cvars_GetAsBool(Cvars_EDITOR_COPY_THINGS))
						{
							oldTile.mInstances = clone.mInstances;
						}
						if (Cvars_GetAsBool(Cvars_EDITOR_COPY_PROPS))
						{
							oldTile.mProps = clone.mProps;
						}
					}
				}

				counterY += 1;
			}
			counterY = 0;
			counterX += 1;
		}

		_mPatch2 = GetPatch(x1, x2, y1, y2, x1, y1);

		CheckPatch();

		OeFunc.Do_ResetCollisionGrid();

		OeLogger.LogInformation("Paste: " + x1 + "," + y1 + " to " + x2 + "," + y2);
	}

	bool isCut = Input_IsKeyTapped(Keys_X);
	bool isCopy = Input_IsKeyTapped(Keys_C);
	if (isCut || isCopy)
	{
		if (Input_IsCtrlPressed(true))
		{
			int width = 1;
			int height = 1;
			int x1 = 0;
			int x2 = 0;
			int y1 = 0;
			int y2 = 0;

			if (IsSelecting())
			{
				width = GetSelectionRectangleGridX2() - GetSelectionRectangleGridX1();
				height = GetSelectionRectangleGridY2() - GetSelectionRectangleGridY1();
				x1 = GetSelectionRectangleGridX1();
				x2 = GetSelectionRectangleGridX2();
				y1 = GetSelectionRectangleGridY1();
				y2 = GetSelectionRectangleGridY2();
			}
			else
			{
				x1 = AlignToGrid(GetRestrainedMouseX());
				x2 = x1 + 1;
				y1 = AlignToGrid(GetRestrainedMouseY());
				y2 = y1 + 1;
			}

			_mCopyTiles = new OeTile[width, height];

			_mCopyNodeAnchor.X = x1;
			_mCopyNodeAnchor.Y = y1;

			for (int i = x1; i < x2; i += 1)
			{
				for (int j = y1; j < y2; j += 1)
				{
					_mCopyTiles[counterX, counterY] = GetLevelData().GetTile(i, j).GetClone();
					counterY += 1;
				}
				counterY = 0;
				counterX += 1;
			}

			if (isCut)
			{
				CutTiles(x1, x2, y1, y2);
			}

			_mCurrentLayerAtTimeOfCopy = GetCurrentLayer();

			if (isCopy)
			{
				OeLogger.LogInformation("Copy: " + x1 + "," + y1 + " to " + x2 + "," + y2);
			}
			else if (isCut)
			{
				OeLogger.LogInformation("Cut: " + x1 + "," + y1 + " to " + x2 + "," + y2);
			}

			ResetSelectionRectangle();
		}
	}*/
}
int EditorPart_GetCurrentLayer(void)
{
	return Cvars_GetAsInt(CVARS_EDITOR_CURRENT_LAYER);
}
void EditorPart_ClearPatches(void)
{
	//_mArrayPatches.Clear();
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
void EditorPart_DefaultHandleKeyMovement(double deltaTime)
{
	double speedX = 0;
	double speedY = 0;
	double speed = Cvars_GetAsInt(CVARS_EDITOR_DRAW_MOVE_SPEED) * 16;

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
		speedX *= deltaTime;
		speedY *= deltaTime;

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
	/*
	if (!Input_IsKeyTapped(Keys_Delete))
	{
		return;
	}

	Point currentGrid = GetCurrentGrid();
	int x1 = currentGrid.X;
	int x2 = x1 + 1;
	int y1 = currentGrid.Y;
	int y2 = y1 + 1;

	if (IsSelecting())
	{
		x1 = GetSelectionRectangleGridX1();
		x2 = GetSelectionRectangleGridX2();
		y1 = GetSelectionRectangleGridY1();
		y2 = GetSelectionRectangleGridY2();
	}

	_mPatch1 = GetPatch(x1, x2, y1, y2, x1, y1);
	for (int i = x1; i < x2; i += 1)
	{
		for (int j = y1; j < y2; j += 1)
		{
			Tile* tile = EditorPart_GetGridTile(i, j);

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_COLLISION))
			{
				tile.DeleteCollision();
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_TILES))
			{
				tile.DeleteDrawTiles();
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_THINGS))
			{
				tile.mInstances.Clear();
			}

			if (Cvars_GetAsBool(CVARS_EDITOR_DELETE_PROPS))
			{
				tile.mProps.Clear();
			}
		}
	}
	_mPatch2 = GetPatch(x1, x2, y1, y2, x1, y1);
	CheckPatch();*/
}
void EditorPart_DoDefaultEditorPartUpdateRoutine(double deltaTime)
{
	EditorPart_DefaultHandleKeyMovement(deltaTime);

	EditorPart_DefaultHandleSpacebarHingeMovement(deltaTime);

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
	//_mManyRectangles.Clear();

	if (Cvars_GetAsBool(CVARS_EDITOR_SHOW_GRID))
	{
		EditorPart_DrawGrid(spriteBatch);
	}

	/*
	if (EditorPart_IsSelecting())
	{
		int x1 = AlignToGrid(_mSelectionRectangle.X) * tileSize;
		int y1 = AlignToGrid(_mSelectionRectangle.Y) * tileSize;

		int tileWidth = ((AlignToGrid(_mSelectionRectangle.Right) * tileSize)) - x1;
		int tileHeight = ((AlignToGrid(_mSelectionRectangle.Bottom) * tileSize)) - y1;

		//OeDrawTool.DrawRectangle(spriteBatch, _mColorSelectionRectangle, 110, x1, y1, tileWidth, tileHeight, 0, false);
		if (!OeGui.IsAnythingHoveredOrAnyItemFocusedOrActive())
		{
			OeDrawTool.DrawRectangleHollow(spriteBatch, OeColors.WHITE, 111, new Rectangle(x1, y1, tileWidth, tileHeight), 0, false, 2);
		}
	}*/

	if (Input_IsCtrlPressed2(true) && (_mCopyTiles != NULL))
	{
		//DrawTilesBorder(spriteBatch, _mCopyTiles);
		//DrawCopyTiles(spriteBatch);
	}
	else
	{
		drawSingleSelectionSelectedTiles(spriteBatch);
	}

	/*
	* 
	* 	


	if (Cvars_GetAsBool(Cvars_EDITOR_DRAW_SHOW_GRID))
	{
		ref OeComCamera camera = ref OeEditor.GetCamera();
		int width = OeComCamera.GetScreenWidthInTiles(ref camera) + 2;
		int height = OeComCamera.GetScreenHeightInTiles(ref camera) + 2;
		int leftOfCamera = OeComCamera.GetLeft(ref camera);
		int topOfCamera = OeComCamera.GetTop(ref camera);

		for (int i = 0; i < width; i += 1)
		{
			for (int j = 0; j < height; j += 1)
			{
				Point position = new Point(((leftOfCamera / tileSize) * tileSize) + (i * tileSize), ((topOfCamera / tileSize) * tileSize) + (j * tileSize));
				if (position.X >= 0 && position.Y >= 0 && position.X < OeFunc.Get_LevelData().GetRealSizeX() && position.Y < OeFunc.Get_LevelData().GetRealSizeY())
				{
					Color tempColor = Cvars_GetAsBool(Cvars_EDITOR_DRAW_GRID_BLACK) ? OeColors.WHITE : OeColors.BLACK;
					_mManyRectangles.Add(new OeDrawRectangle(tempColor, new Rectangle(position.X, position.Y, tileSize / 5, 1)));
					_mManyRectangles.Add(new OeDrawRectangle(tempColor, new Rectangle(position.X, position.Y, 1, tileSize / 5)));
				}
			}
		}
	}

	spriteBatch.DrawManyRectangle(100, _mManyRectangles);*/
}
void EditorPart_DoDefaultEditorPartDrawHudRoutine(SpriteBatch* spriteBatch, const char* status)
{
	Vector2 mouse = Input_GetMouse();
	Vector2 adjustedMouse = Input_GetCameraAdjustedMouse(Editor_GetCamera());

	MString* mouseDisplay = NULL;
	MString* adjustedMouseDisplay = NULL;
	MString* xTileDisplay = NULL;
	MString* yTileDisplay = NULL;
	MString* xDisplay = NULL;
	MString* yDisplay = NULL;

	MString_AssignString(&mouseDisplay, "|Mouse:");
	MString_AddAssignInt(&mouseDisplay, (int32_t)mouse.X);
	MString_AddAssignString(&mouseDisplay, ",");
	MString_AddAssignInt(&mouseDisplay, (int32_t)mouse.Y);

	MString_AssignString(&adjustedMouseDisplay, "|AdjustedMouse:");
	MString_AddAssignInt(&adjustedMouseDisplay, (int32_t)adjustedMouse.X);
	MString_AddAssignString(&adjustedMouseDisplay, ",");
	MString_AddAssignInt(&adjustedMouseDisplay, (int32_t)adjustedMouse.Y);

	MString_AssignString(&xTileDisplay, "|X-Tile:");
	MString_AddAssignInt(&xTileDisplay, (int32_t)(adjustedMouse.X / TILE_SIZE));

	MString_AssignString(&yTileDisplay, "|Y-Tile:");
	MString_AddAssignInt(&yTileDisplay, (int32_t)(adjustedMouse.Y / TILE_SIZE));

	MString_AssignString(&xDisplay, "|X:");
	MString_AddAssignInt(&xDisplay, (int32_t)(adjustedMouse.X));

	MString_AssignString(&yDisplay, "|Y:");
	MString_AddAssignInt(&yDisplay, (int32_t)(adjustedMouse.Y));

	/*string xLockedDisplay = "|X (Lock):" + ((mousePosX / TILE_SIZE) * TILE_SIZE);
	string yLockedDisplay = "|Y (Lock):" + ((mousePosY / TILE_SIZE) * TILE_SIZE);
	string scaleDisplay = "|Scale:" + (OeEditor.GetCamera().mWorldZoom);
	string parallaxDisplay = "";// "|Parallax:" + returnBoolean(EditorGlobals.SHOW_PARALLAX);
	string collisionDisplay = "|Collision:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_COLLISION));
	string thingDisplay = "|Thing:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_THINGS));
	string propDisplay = "|Prop:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_PROPS));
	string tileParallaxDisplay = "";// " |T-Parallax:" + returnBoolean(!EditorGlobals.DISABLE_TILE_PARALLAX);
	string gridDisplay = "|Grid:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_GRID));
	string tileDisplay = "|Tile:" + (Cvars_GetAsBool(Cvars_EDITOR_SHOW_TILES));
	string selectedX = (IsSelecting() ? "|Select X: " + (GetSelectionRectangleGridX2() - GetSelectionRectangleGridX1()) : "|Select X: 0");
	string selectedY = (IsSelecting() ? "|Select Y: " + (GetSelectionRectangleGridY2() - GetSelectionRectangleGridY1()) : "|Select Y: 0");
	string autoTilerDisplay = "|Auto:" + (Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER) == -1 ? "Off" : Cvars_Get(Cvars_EDITOR_AUTO_TILER));
	string fpsDisplay = "|FPS:" + OeGameDrawer.GetFPS();*/

	Rectangle drawableSize = Renderer_GetDrawableSize();
	DrawTool_DrawRectangle2(spriteBatch, COLOR_BLACK, 100,
		Rectangle_Create(0, drawableSize.Height - EDITOR_GLOBALS_MENU_ITEM_SIZE, drawableSize.Width, EDITOR_GLOBALS_MENU_ITEM_SIZE), 0, false);

	MString* finalString = NULL;
	MString_AddAssignMString(&finalString, mouseDisplay);
	MString_AddAssignMString(&finalString, adjustedMouseDisplay);
	MString_AddAssignMString(&finalString, xTileDisplay);
	MString_AddAssignMString(&finalString, yTileDisplay);
	MString_AddAssignMString(&finalString, xDisplay);
	MString_AddAssignMString(&finalString, yDisplay);

	SpriteBatch_DrawString(spriteBatch, "editor", MString_Text(finalString), Color_White, 150, 
		Vector2_Create(140, 32));
		//spriteBatch.DrawString(, status + scaleDisplay + selectedX + selectedY + xTileDisplay + yTileDisplay + xDisplay + yDisplay + xLockedDisplay + yLockedDisplay + parallaxDisplay + collisionDisplay + thingDisplay
		//+ propDisplay + tileParallaxDisplay + gridDisplay + tileDisplay + autoTilerDisplay + fpsDisplay, OeColors.WHITE, 100, new Vector2(0, OeGame.GetCurrentBackBufferHeight() - OeGui.MENU_ITEM_SIZE));*/
}
void EditorPart_DrawCopyTiles(SpriteBatch* spriteBatch)
{
	/*int tileSize = OeUtils.GetTileSize();

	Point currentGrid = GetCurrentGrid();
	OeDrawTool.DrawRectangle(spriteBatch, new Color(0, 0, 0, 120), 190,
		new Rectangle(currentGrid.X * tileSize, currentGrid.Y * tileSize, _mCopyTiles.GetLength(0) * tileSize, _mCopyTiles.GetLength(1) * tileSize), 0, false);

	OeTexture texture = GetLevelData().GetTilesetTexture();
	for (int i = 0; i < _mCopyTiles.GetLength(0); i += 1)
	{
		for (int j = 0; j < _mCopyTiles.GetLength(1); j += 1)
		{
			OeTile tile = _mCopyTiles[i, j];

			int adjX = currentGrid.X + i;
			int adjY = currentGrid.Y + j;

			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_COLLISION))
			{
				tile.DrawCollision(spriteBatch, tileSize, ref OeEditor.GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_THINGS))
			{
				tile.DrawThings(spriteBatch, tileSize, ref OeEditor.GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_PROPS))
			{
				tile.DrawProps(spriteBatch, tileSize, ref OeEditor.GetCamera(), adjX, adjY, true);
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_TILES))
			{
				DrawTilesAsEditor(tile, spriteBatch, tileSize, adjX, adjY, GetLevelData(), true, texture);
			}
		}
	}*/
}
void EditorPart_DrawTilesAsEditor(Tile* tile, SpriteBatch* spriteBatch, int tileSize, int gridX, int gridY,
	LevelData* level, bool overrideDepth, Texture* wrapper)

{
	/*for (int k = 0; k < OeTile.LAYER_AMOUNT; k += 1)
	{
		if (!Cvars_GetAsBool(Cvars_EDITOR_COPY_LAYER_ONLY) || k == _mCurrentLayerAtTimeOfCopy)
		{
			if (Cvars_GetAsBool(Cvars_EDITOR_SHOW_LAYER + k))
			{
				int currentDepth = level.mLayerData[k].mDepth;
				if (overrideDepth)
				{
					currentDepth = OeTile.OVERRIDE_DEPTH;
				}
				OeDrawTile.Draw(ref tile.mDrawTiles[k], spriteBatch, wrapper, OeColors.WHITE, currentDepth, gridX * tileSize, gridY * tileSize);
			}
		}
	}*/
}
void EditorPart_DrawTilesBorder(SpriteBatch* spriteBatch, Tile* tiles, int tilesWidth, int tilesHeight)
{
	/*int tileSize = OeUtils.GetTileSize();

	Point currentGrid = GetCurrentGrid();
	int x = currentGrid.X * tileSize;
	int y = currentGrid.Y * tileSize;

	int width = tiles.GetLength(0) * tileSize;
	int height = tiles.GetLength(1) * tileSize;

	int widthAdjustment = (x + width) - GetLevelData().GetRealSizeX();
	int heightAdjustment = (y + height) - GetLevelData().GetRealSizeY();

	if (widthAdjustment > 0)
	{
		width -= widthAdjustment;
	}
	if (heightAdjustment > 0)
	{
		height -= heightAdjustment;
	}

	OeDrawTool.DrawRectangleHollow(spriteBatch, OeColors.WHITE, 100, new Rectangle(x, y, width, height), 0, false, 2);*/
}
void EditorPart_CutTiles(int x1, int x2, int y1, int y2)
{
	/*int counterX = 0;
	int counterY = 0;

	int width = GetSelectionRectangleGridX2() - GetSelectionRectangleGridX1();
	int height = GetSelectionRectangleGridY2() - GetSelectionRectangleGridY1();

	//_mMoveTiles = new OeTile[width, height];

	//_mMoveNodeAnchor.X = x1;
	//_mMoveNodeAnchor.Y = y1;

	_mPatch1 = GetPatch(x1, x2, y1, y2, x1, y1);

	for (int i = x1; i < x2; i += 1)
	{
		for (int j = y1; j < y2; j += 1)
		{
			//_mMoveTiles[counterX, counterY] = GetLevelData().GetTile(i, j).GetClone(false, null, true);
			OeTile originalTile = GetLevelData().GetTile(i, j);
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_COLLISION))
			{
				originalTile.DeleteCollision();
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_TILES))
			{
				if (!Cvars_GetAsBool(Cvars_EDITOR_COPY_LAYER_ONLY))
				{
					originalTile.DeleteDrawTiles();
				}
				else
				{
					originalTile.DeleteDrawTiles(GetCurrentLayer());
				}
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_THINGS))
			{
				originalTile.DeleteThings();
			}
			if (Cvars_GetAsBool(Cvars_EDITOR_COPY_PROPS))
			{
				originalTile.DeleteProps();
			}
			counterY += 1;
		}
		counterY = 0;
		counterX += 1;
	}

	_mPatch2 = GetPatch(x1, x2, y1, y2, x1, y1);

	CheckPatch();

	OeFunc.Do_ResetCollisionGrid();*/
}
void EditorPart_Move(void)
{
	//UNUSED?
}
void EditorPart_CheckPatch(void)
{
	/*if (_mPatch1 != null)
	{
		if (!_mPatch1.IsEqualTo(_mPatch2))
		{
			AddPatch(_mPatch1);
		}
	}
	_mPatch1 = null;
	_mPatch2 = null;*/
}
void EditorPart_AddPatch(LevelPatch* patch)
{
	//_mArrayPatches.Insert(0, patch);
}
bool EditorPart_IsSelecting(void)
{
	return !Rectangle_IsEmpty(&_mSelectionRectangle);
}
void EditorPart_ReverseCopy(bool isFlipY)
{
	/*OeTile[, ] newTiles = new OeTile[_mCopyTiles.GetLength(0), _mCopyTiles.GetLength(1)];

	int xLength = _mCopyTiles.GetLength(0);
	int yLength = _mCopyTiles.GetLength(1);
	for (int i = 0; i < xLength; i += 1)
	{
		for (int j = 0; j < yLength; j += 1)
		{
			int newLocX = (xLength - 1) - i;
			int newLocY = (yLength - 1) - j;

			if (!isFlipY)
			{
				newTiles[i, j] = _mCopyTiles[newLocX, j].GetClone();
			}
			else
			{
				newTiles[i, j] = _mCopyTiles[i, newLocY].GetClone();
			}

			OeDrawTile[] draws = newTiles[i, j].mDrawTiles;
			for (int k = 0; k < draws.Length; k += 1)
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

	_mCopyTiles = newTiles;*/
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
	Rectangle realSize = LevelData_GetRealSize(Get_LevelData());

	float x = vec.X;
	x = Math_MaxFloat(x, 0);
	x = Math_MinFloat(x, (float)(realSize.Width - 1));

	float y = vec.Y;
	y = Math_MaxFloat(y, 0);
	y = Math_MinFloat(y, (float)(realSize.Height - 1));

	return Vector2_Create(x, y);
}
float EditorPart_GetRestrainedMouseX(void)
{
	return EditorPart_GetRestrainedMouse().X;
}
float EditorPart_GetRestrainedMouseY(void)
{
	return EditorPart_GetRestrainedMouse().Y;
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
	return LevelData_GetTilePoint(Get_LevelData(), EditorPart_GetCurrentGrid());
}
Point EditorPart_GetGridAlignedMousePixel(void)
{
	Point currentGrid = EditorPart_GetCurrentGrid();
	return Point_Create(currentGrid.X * TILE_SIZE, currentGrid.Y * TILE_SIZE);
}
Tile* EditorPart_GetGridTileFromPixel(Vector2 pos)
{
	return LevelData_GetTilePoint(Get_LevelData(), EditorPart_AlignToGridVector2(pos));
}
Tile* EditorPart_GetGridTile(int x, int y)
{
	return LevelData_GetTile(Get_LevelData(), x, y);
}
Tile* EditorPart_GetGridTilePoint(Point pos)
{
	return LevelData_GetTilePoint(Get_LevelData(), pos);
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
	return _mSelectionRectangle;
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
LevelPatch* EditorPart_GetPatchSingleSelection(void)
{
	Point currentGrid = EditorPart_GetCurrentGrid();
	return EditorPart_GetPatch(currentGrid.X, currentGrid.X + 1, currentGrid.Y, currentGrid.Y + 1, currentGrid.X, currentGrid.Y);
}
LevelPatch* EditorPart_GetPatchSelectionRectangle(void)
{
	return NULL;//TODO
	/*return GetPatch(GetSelectionRectangleGridX1(), GetSelectionRectangleGridX2(), GetSelectionRectangleGridY1(), GetSelectionRectangleGridY2(), GetSelectionRectangleGridX1(),
		GetSelectionRectangleGridY1());*/
}
LevelPatch* EditorPart_GetPatch(int x1, int x2, int y1, int y2, int i, int j)
{
	return NULL; //TODO
	//return new LevelPatch(GetCloneOfTiles(x1, x2, y1, y2), i, j);
}
Tile* EditorPart_GetCloneOfTiles(int x1, int x2, int y1, int y2)
{
	return NULL;
	//TODO
	/*OeTile[, ] tiles = new OeTile[x2 - x1, y2 - y1];

	int counterX = 0;
	int counterY = 0;
	for (int i = x1; i < x2; i += 1)
	{
		for (int j = y1; j < y2; j += 1)
		{
			tiles[counterX, counterY] = GetLevelData().GetTile(i, j).GetClone();
			counterY += 1;
		}
		counterY = 0;
		counterX += 1;
	}

	return tiles;*/
}
void EditorPart_DefaultHandleSpacebarHingeMovement(double deltaTime)
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
		halfMovement = Vector2_MulDouble(halfMovement, deltaTime * 16);
		Vector2 newPosition = Vector2_Add(EditorPart_GetEditorPosition(0), halfMovement);
		EditorPart_SetEditorPosition(0, newPosition);
	}
}
bool EditorPart_IsMouseInsideSelectionRectangle(void)
{
	int32_t currentMouseX = (int32_t)EditorPart_GetRestrainedMouseX();
	int32_t currentMouseY = (int32_t)EditorPart_GetRestrainedMouseY();
	return Rectangle_Contains(&_mSelectionRectangle, currentMouseX, currentMouseY);
}
void EditorPart_ResetSelectionRectangle(void)
{
	_mSelectionState = EDITOR_SELECTION_STATE_NOTHING;
	_mSelectionRectangle = Rectangle_Empty;
	_mSelectionRectangleAnchor = Point_Zero;
}
bool EditorPart_HandleSelectionRectangle(void)
{
	if (_mSelectionState == EDITOR_SELECTION_STATE_NOTHING)
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
	}

	switch (_mSelectionState)
	{
	case EDITOR_SELECTION_STATE_NOTHING:
		if (Input_IsShiftTapped())
		{
			EditorPart_ResetSelectionRectangle();
			_mSelectionRectangleAnchor = EditorPart_GetCurrentGrid();
			_mSelectionState = EDITOR_SELECTION_STATE_SELECTED;
		}
		break;
	case EDITOR_SELECTION_STATE_SELECTED:
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
				_mSelectionState = EDITOR_SELECTION_STATE_NOTHING;
			}
		}
		break;
	}

	return false;
}