#pragma once

#include "../utils/Macros.h"
#include "../math/Vector2.h"
#include "../math/Point.h"
#include "../math/Rectangle.h"
#include "PartFuncData.h"

typedef struct Tile Tile;
typedef struct SpriteBatch SpriteBatch;
typedef struct LevelPatch LevelPatch;
typedef struct EditorPartData EditorPartData;
typedef struct Texture Texture;
typedef struct LevelData LevelData;

void EditorPart_SetPatch1(LevelPatch* patch);
void EditorPart_SetPatch2(LevelPatch* patch);
bool EditorPart_IsBlocking(void);
void EditorPart_SetBlocking(bool value);
void EditorPart_DummyJustChangedToThisPart(void);
void EditorPart_DummySave(void);
void EditorPart_DummyJustLoadedMap(void);
void EditorPart_DummyCreateWindows(void);
void EditorPart_DummyUpdate(double deltaTime);
void EditorPart_DummyDraw(SpriteBatch* spriteBatch);
void EditorPart_DummyDrawHud(SpriteBatch* spriteBatch);
PartFuncData EditorPart_CreateDummyFuncData(void);
void EditorPart_DefaultHandlePatches(void);
void EditorPart_DefaultDrawSingleSelectionSelectedTiles(SpriteBatch* spriteBatch);
void EditorPart_DefaultHandleColumnsAndRows(void);
void EditorPart_DefaultHandleCopy(void);
int EditorPart_GetCurrentLayer(void);
void EditorPart_ClearPatches(void);
void EditorPart_HandleCurrentLayerSelection(void);
void EditorPart_DefaultHandleKeyMovement(double deltaTime);
void EditorPart_HandleTab(void);
int EditorPart_GetEditorZoom(int32_t index);
void EditorPart_SetEditorZoom(int32_t index, int32_t zoom);
Vector2 EditorPart_GetEditorPosition(int32_t index);
void EditorPart_SetEditorPosition(int32_t index, Vector2 position);
void EditorPart_DefaultUpdateCameraZoom(void);
void EditorPart_DefaultUpdateCameraPosition(void);
void EditorPart_DefaultHandleDeleteKey(void);
void EditorPart_DoDefaultEditorPartUpdateRoutine(double deltaTime);
//void EditorPart_DoDefaultEditorPartDrawRoutine(SpriteBatch* spriteBatch, Action<OeSpriteBatch> drawSingleSelectionSelectedTiles);
void EditorPart_DoDefaultEditorPartDrawHudRoutine(SpriteBatch* spriteBatch, const char* status);
void EditorPart_DrawCopyTiles(SpriteBatch* spriteBatch);
void EditorPart_DrawTilesAsEditor(Tile* tile, SpriteBatch* spriteBatch, int tileSize, int gridX, int gridY, 
	LevelData* level, bool overrideDepth, Texture* wrapper);
void EditorPart_DrawTilesBorder(SpriteBatch* spriteBatch, Tile* tiles, int tilesWidth, int tilesHeight);
void EditorPart_CutTiles(int x1, int x2, int y1, int y2);
void EditorPart_Move(void);
void EditorPart_CheckPatch(void);
void EditorPart_AddPatch(LevelPatch* patch);
bool EditorPart_IsSelecting(void);
void EditorPart_ReverseCopy(bool isFlipY);
void EditorPart_HandleReverseCopy(void);
void EditorPart_DrawGrid(SpriteBatch* spriteBatch);
int32_t EditorPart_AlignToGridInt(int32_t pixelValue);
int32_t EditorPart_AlignToGridFloat(float pixelValue);
Point EditorPart_AlignToGridVector2(Vector2 pixelPos);
Point EditorPart_AlignToGridPoint(Point pixelPos);
Vector2 EditorPart_GetUnrestrainedMouse(void);
Vector2 EditorPart_GetRestrainedMouse(void);
Vector2 EditorPart_GetRestrainedValue(Vector2 vec);
float EditorPart_GetRestrainedMouseX(void);
float EditorPart_GetRestrainedMouseY(void);
Point EditorPart_GetUnrestrainedCurrentGrid(void);
Point EditorPart_GetCurrentGrid(void);
Tile* EditorPart_GetCurrentGridTile(void);
Point EditorPart_GetGridAlignedMousePixel(void);
Tile* EditorPart_GetGridTileFromPixel(Vector2 pos);
Tile* EditorPart_GetGridTile(int x, int y);
Tile* EditorPart_GetGridTilePoint(Point pos);
void EditorPart_DefaultHandleZoom(void);
Rectangle EditorPart_GetSelectionRectangle(void);
int EditorPart_GetSelectionRectangleGridX1(void);
int EditorPart_GetSelectionRectangleGridX2(void);
int EditorPart_GetSelectionRectangleGridY1(void);
int EditorPart_GetSelectionRectangleGridY2(void);
LevelPatch* EditorPart_GetPatchSingleSelection(void);
LevelPatch* EditorPart_GetPatchSelectionRectangle(void);
LevelPatch* EditorPart_GetPatch(int x1, int x2, int y1, int y2, int i, int j);
Tile* EditorPart_GetCloneOfTiles(int x1, int x2, int y1, int y2);
void EditorPart_DefaultHandleSpacebarHingeMovement(double deltaTime);
bool EditorPart_IsMouseInsideSelectionRectangle(void);
void EditorPart_ResetSelectionRectangle(void);
bool EditorPart_HandleSelectionRectangle(void);