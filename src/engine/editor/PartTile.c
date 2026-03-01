#include "PartTile.h"

#include "../render/SpriteBatch.h"
#include "../leveldata/DrawTile.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "EditorPart.h"
#include "imgui.h"
#include "WindowTilePicker.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../../third_party/stb_ds.h"
#include "../input/Input.h"
#include "../math/Math.h"
#include "../render/DrawTool.h"
#include "EditorGlobals.h"
#include "WindowAnimTilePicker.h"

static int _mRotation;
static bool _mIsAnimation;
static bool _mFlipX;
static bool _mFlipY;
static Color _mColorFade = { 255, 255, 255, 127 };
static Point* _mBucketPoints;

PartFuncData PartTile_GetFuncData()
{
    PartFuncData data = EditorPart_CreateDummyFuncData();

	Utils_strlcpy(data.mDebugName, "TILE", EE_FILENAME_MAX);

    data.CreateWindows = PartTile_CreateWindows;
    data.Update = PartTile_Update;
    data.Draw = PartTile_Draw;
    data.DrawHud = PartTile_DrawHud;
    data.JustChangedToThisPart = PartTile_JustChangedToThisPart;

    return data;
}
void PartTile_CreateWindows()
{
    LevelData* levelData = Get_LevelData();
    if (WindowTilePicker_CreateWindow(LevelData_GetTilesetTexture(levelData)))
    {
        _mFlipX = false;
        _mFlipY = false;
        _mRotation = 0;
        _mIsAnimation = false;
    }

    bool whatever = false;
    ImGui::SetNextWindowSize(ImVec2(300, 500));
    if (!ImGui::Begin("Tile Mode", &whatever, ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    if (WindowAnimTilePicker_CreateWindow())
    {
        _mFlipX = false;
        _mFlipY = false;
        _mRotation = 0;
        _mIsAnimation = true;
    }

	ImGui::InputInt("Rotation: ", &_mRotation);
    ImGui::Checkbox("Flip X", & _mFlipX);
	ImGui::Checkbox("Flip Y", &_mFlipY);
    ImGui::Text("Auto Tile (Ctrl + Num9)");
    if (ImGui::RadioButton("Off", Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) == -1))
    {
		Cvars_SetAsInt(CVARS_EDITOR_AUTO_TILER, -1);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("9", Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) == 9))
    {
        Cvars_SetAsInt(CVARS_EDITOR_AUTO_TILER, 9);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("13", Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) == 13))
    {
        Cvars_SetAsInt(CVARS_EDITOR_AUTO_TILER, 13);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("16", Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) == 16))
    {
        Cvars_SetAsInt(CVARS_EDITOR_AUTO_TILER, 16);
    }
    ImGui::SameLine();
    ImGui::NewLine();

    ImGui::Text("--- Keys ---");
    ImGui::Text("LClick - Set Tile");
    ImGui::Text("RClick - Remove Tile");
    ImGui::Text("MClick - Grab Tile");
    ImGui::Text("Numbers 1-0 - Change Layer");
    ImGui::Text("Z - Flip X");
    ImGui::Text("X - Flip Y");
    ImGui::Text("R - Rotate");
    ImGui::Text("(Minus) - Reset Flip/Rotate");
    ImGui::Text("B - Bucket Fill");
    const char* failure = "Failure!";
    if (ImGui::Button("Collision Auto Tile Pass", ImVec2(200, 20)))
    {
        if (Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) == -1)
        {
            //ImGui::OpenPopup(failure);
            return;
        }

        //AutoTilerPass();
    }

    //ImGui::MessageModal(failure, "Auto Tile Not Set!");

	ImGui::End();
}
void PartTile_AutoTilerPass()
{
    /*OeLevelData levelData = OeFunc.Get_LevelData();
    OeTile[, ] actual = levelData.mTileData;
    for (int i = 0; i < actual.GetLength(0); i += 1)
    {
        for (int j = 0; j < actual.GetLength(1); j += 1)
        {
            OeTile tile = actual[i, j];
            if (tile.mCollisionType == 1)
            {
                tile.DeleteDrawTiles();
            }
        }
    }

    int tileSize = OeUtils.GetTileSize();
    int selectedX = OeWindowTilePicker.GetTexturePositionX();
    int selectedY = OeWindowTilePicker.GetTexturePositionY();
    for (int i = 0; i < actual.GetLength(0); i += 1)
    {
        for (int j = 0; j < actual.GetLength(1); j += 1)
        {
            OeTile tile = actual[i, j];
            if (tile.mCollisionType == 1)
            {
                OeEditorAutoTiler.AutoTile(Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER), GetLevelData().GetTileData(),
                    GetLevelData().mTilesetName, tileSize, i, j, GetCurrentLayer(),
                    new Rectangle(selectedX, selectedY, tileSize, tileSize), false, true);
            }
        }
    }*/
}
void PartTile_DrawSingleSelectionSelectedTiles(SpriteBatch* spriteBatch)
{
    Point currentGrid = EditorPart_GetCurrentGrid();
    int x = currentGrid.X * TILE_SIZE;
    int y = currentGrid.Y * TILE_SIZE;
	Rectangle pickerBoundaryInPixels = WindowTilePicker_GetSelectedBoundaryInPixels();
    int width = pickerBoundaryInPixels.Width;
	int height = pickerBoundaryInPixels.Height;

	Rectangle levelBoundaryInPixels = LevelData_GetBoundaryInPixels(Get_LevelData());
    width = Math_MinInt(width, levelBoundaryInPixels.Width - x);
    height = Math_MinInt(height, levelBoundaryInPixels.Height - y);

    DrawTool_DrawRectangleHollow2(spriteBatch, COLOR_WHITE, 200, Rectangle_Create(x, y, width, height), 0, false, 2);
}
void PartTile_BucketFill(DrawTile* givenDraw, int x, int y)
{
    /*OeTile currentTile = GetGridTile(x, y);
    if (currentTile == null)
    {
        return;
    }

    if (!OeDrawTile.IsEqualTo(ref currentTile.mDrawTiles[GetCurrentLayer()], ref givenDraw))
    {
        return;
    }

    int x1 = x;
    int y1 = y;
    int x2 = x + 1;
    int y2 = y + 1;

    SetPatch1(GetPatch(x1, x2, y1, y2, x1, y1));
    Operation(x1, x2, y1, y2, false, false, false);
    SetPatch2(GetPatch(x1, x2, y1, y2, x1, y1));
    CheckPatch();

    Point point = new Point(x, y);
    for (int i = 0; i < _mBucketPoints.Count; i += 1)
    {
        if (_mBucketPoints[i] == point)
        {
            return;
        }
    }
    _mBucketPoints.Add(point);

    BucketFill(givenDraw, x + 1, y);
    BucketFill(givenDraw, x - 1, y);
    BucketFill(givenDraw, x, y + 1);
    BucketFill(givenDraw, x, y - 1);*/
}
void PartTile_Update()
{
    EditorPart_DoDefaultEditorPartUpdateRoutine();

    PartTile_UpdateHelper();
}
void PartTile_UpdateHelper()
{
    EditorPart_HandleCurrentLayerSelection();

	arrsetlen(_mBucketPoints, 0);

    Cvars_SetAsBool(CVARS_EDITOR_SHOW_TILES, true);

    if (!Input_IsModifierPressed())
    {
        if (Input_IsKeyTapped(KEYS_OEMMINUS))
        {
            _mFlipX = false;
            _mFlipY = false;
            _mRotation = 0;
        }
        if (Input_IsKeyTapped(KEYS_Z))
        {
            _mFlipX = !_mFlipX;
        }
        if (Input_IsKeyTapped(KEYS_X))
        {
            _mFlipY = !_mFlipY;
        }
        if (Input_IsKeyTapped(KEYS_R))
        {
            _mRotation += 90;
            if (_mRotation > 270)
            {
                _mRotation = 0;
            }
        }
    }

    if (Input_IsLeftMousePressed())
    {
        PartTile_OperationHelper(false, false, false);
    }

    if (Input_IsMiddleMouseTapped())
    {
        PartTile_OperationHelper(false, false, true);
    }

    if (Input_IsRightMousePressed())
    {
        PartTile_OperationHelper(true, false, false);
    }

    if (Input_IsKeyTapped(KEYS_B))
    {
        Point currentGrid = EditorPart_GetCurrentGrid();
        int gridX = currentGrid.X;
        int gridY = currentGrid.Y;
        Tile* currentTile = EditorPart_GetGridTile(gridX, gridY);
        PartTile_BucketFill(&currentTile->mDrawTiles[EditorPart_GetCurrentLayer()], gridX, gridY);
    }
}
void PartTile_Draw(SpriteBatch* spriteBatch)
{
    EditorPart_DoDefaultEditorPartDrawRoutine(spriteBatch, PartTile_DrawSingleSelectionSelectedTiles);
}
void PartTile_DrawHud(SpriteBatch* spriteBatch)
{
    EditorPart_DoDefaultEditorPartDrawHudRoutine(spriteBatch, PartTile_GetStatus());

    PartTile_DrawHudHelper(spriteBatch);
}
void PartTile_DrawHudHelper(SpriteBatch* spriteBatch)
{
    /*int locX = Input_GetMouseX();
    int locY = Input_GetMouseY();
    if (_mIsAnimation)
    {
        if (OeWindowAnimTilePicker.GetAnimTileResource() != null)
        {
            OeWindowAnimTilePicker.GetAnimTileResource().GetData().Draw(spriteBatch, OeColors.WHITE, 100, locX, locY, _mRotation, _mFlipX, _mFlipY);
        }
    }
    else
    {
        OeTexture texture = GetLevelData().GetTilesetTexture();
        if (texture != null)
        {
            if (texture.FileExists())
            {
                int width = OeWindowTilePicker.GetSelectedRealWidth();
                int height = OeWindowTilePicker.GetSelectedRealHeight();
                spriteBatch.Draw(texture, _mColorFade, 100, null, new Vector2(locX, locY),
                    new Rectangle(OeWindowTilePicker.GetTexturePositionX(), OeWindowTilePicker.GetTexturePositionY(), width, height),
                    Vector2.One, _mRotation, _mFlipX, _mFlipY, new Vector2(width, height) / 2f);
            }
        }
    }*/
}
void PartTile_OperationHelper(bool reset, bool resetAll, bool grabTile)
{
    EditorGlobals_StallOperationCounter();

	Rectangle selection = EditorPart_GetSelectionRectangleAsGridBounds();
    if (!_mIsAnimation)
    {
		Rectangle tilePickerBoundary = WindowTilePicker_GetSelectedBoundary();
        selection.Width = tilePickerBoundary.Width;
        selection.Height = tilePickerBoundary.Height;
    }

	Rectangle levelBoundary = LevelData_GetBoundary(Get_LevelData());
    selection.Width = Math_MinInt(selection.Width, levelBoundary.Width);
    selection.Height = Math_MinInt(selection.Height, levelBoundary.Height);

    PartTile_Operation(selection, reset, resetAll, grabTile);
}
void PartTile_Operation(Rectangle selection, bool resetTile, bool resetAll, bool grabTile)
{
    int incX = 1;
    int incY = 1;
    int counterX = 0;
    int counterY = 0;
    int counterLimitX = selection.Width - 1;
    int counterLimitY = selection.Width - 1;

    if (_mFlipX)
    {
        counterX = counterLimitX;
        incX = -1;
    }

    if (_mFlipY)
    {
        counterY = counterLimitY;
        incY = -1;
    }

	EditorPart_PushPatches(selection);

	for (int i = selection.X; i < (selection.X + selection.Width); i += 1)
    {
		for (int j = selection.Y; j < (selection.Y + selection.Height); j += 1)
        {
            bool autoTile = Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER) != -1;
            if (_mIsAnimation)
            {
                autoTile = false;
            }

			Point texturePos = WindowTilePicker_GetTexturePosition();
            Point selectedTexture = Point_Create(texturePos.X + (counterX * TILE_SIZE), texturePos.Y + (counterY * TILE_SIZE));

			DrawTile* draws = LevelData_GetTile(Get_LevelData(), i, j)->mDrawTiles;
            DrawTile* currentLayerDt = &draws[EditorPart_GetCurrentLayer()];
            if (resetAll)
            {
                for (int k = 0; k < TILE_DRAW_LAYER_LENGTH; k += 1)
                {
					DrawTile_Init(&draws[k]);
                }
            }
            else if (resetTile)
            {
                if (autoTile)
                {
                    /*OeEditorAutoTiler.AutoTile(Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER), GetLevelData().GetTileData(),
                        GetLevelData().mTilesetName, tileSize, i, j, GetCurrentLayer(),
                        new Rectangle(selectedTextureX, selectedTextureY, tileSize, tileSize), false, false);*/
                }
                DrawTile_Init(currentLayerDt);
            }
            else if (grabTile)
            {
				if (!DrawTile_IsZero(currentLayerDt))
                {
                    _mRotation = (int)currentLayerDt->mRotation;
                    _mFlipX = currentLayerDt->mFlipX;
                    _mFlipY = currentLayerDt->mFlipY;
					if (!DrawTile_IsAnimation(currentLayerDt))
                    {
                        Point newTexturePosition = currentLayerDt->mPoint;
                        WindowTilePicker_SetTexturePosition(newTexturePosition);
                        _mIsAnimation = false;
                    }
                    else
                    {
                        WindowAnimTilePicker_SetAnimTile(currentLayerDt->mAnimation);
                        _mIsAnimation = true;
                    }
                }
            }
            else
            {
                if (autoTile)
                {
                    /*OeEditorAutoTiler.AutoTile(Cvars_GetAsInt(CVARS_EDITOR_AUTO_TILER), GetLevelData().GetTileData(),
                        GetLevelData().mTilesetName, tileSize, i, j, GetCurrentLayer(),
                        new Rectangle(selectedTextureX, selectedTextureY, tileSize, tileSize), false, true);*/
                }
                else
                {
                    if (!_mIsAnimation)
                    {
                        currentLayerDt->mRotation = (float)_mRotation;
                        currentLayerDt->mFlipX = _mFlipX;
                        currentLayerDt->mFlipY = _mFlipY;
						DrawTile_LoadSheet(currentLayerDt, selectedTexture.X, selectedTexture.Y);
					}
					else
					{
                        Resource* animTileRes = WindowAnimTilePicker_GetAnimTileResource();
						if (animTileRes != NULL)
						{
                            currentLayerDt->mRotation = _mRotation;
                            currentLayerDt->mFlipX = _mFlipX;
                            currentLayerDt->mFlipY = _mFlipY;
							DrawTile_LoadAnimation(currentLayerDt, Resource_GetName(animTileRes));
						}
					}
				}
			}

            counterY += incY;
            if (!_mFlipY)
			{
                if (counterY > counterLimitY)
                {
                    counterY = 0;
                }
            }
            else
            {
                if (counterY < 0)
                {
                    counterY = counterLimitY;
                }
            }
        }

        if (!_mFlipY)
        {
            counterY = 0;
        }
        else
        {
            counterY = counterLimitY;
        }
        counterX += incX;
        if (!_mFlipX)
        {
            if (counterX > counterLimitX)
            {
                counterX = 0;
            }
        }
        else
        {
            if (counterX < 0)
            {
                counterX = counterLimitX;
            }
        }
    }

	EditorPart_FinishPatches();
}
const char* PartTile_GetStatus()
{
    return NULL;
    /*string name = "";
    if (OeWindowAnimTilePicker.GetAnimTileResource() != null)
    {
        name = OeWindowAnimTilePicker.GetAnimTileResource().GetName();
    }
    return "|Name: " + name + "|Layer=" + GetCurrentLayer() + "|Depth=" + GetLevelData().mLayerData[GetCurrentLayer()].mDepth + "|Rotation=" + _mRotation + "|FlipX=" + _mFlipX + "|FlipY=" + _mFlipY;*/
}
void PartTile_JustChangedToThisPart()
{
    /*Cvars_Set(Cvars_EDITOR_SHOW_COLLISION, false);
    Cvars_Set(Cvars_EDITOR_SHOW_TILES, true);
    _mFlipX = false;
    _mFlipY = false;
    _mRotation = 0;*/
}
