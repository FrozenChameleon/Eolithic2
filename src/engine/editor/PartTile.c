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

static int _mRotation;
static bool _mIsAnimation;
static bool _mFlipX;
static bool _mFlipY;
//Color _mColorFade = OeColors.ToColor(OeColors.WHITE, 127);
//List<Point> _mBucketPoints = new List<Point>();

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
        return;
    }

    /*if (OeWindowAnimTilePicker.CreateWindow())
    {
        _mFlipX = false;
        _mFlipY = false;
        _mRotation = 0;
        _mIsAnimation = true;
    }*/


    ImGui::Text("Rotation: " + _mRotation);
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
    /*int tileSize = Utils_GetTileSize();

    Point currentGrid = GetCurrentGrid();
    int x = currentGrid.X * tileSize;
    int y = currentGrid.Y * tileSize;
    int width = OeWindowTilePicker.GetSelectedRealWidth();
    int height = OeWindowTilePicker.GetSelectedRealHeight();

    width = OeMath.Min(width, GetLevelData().GetRealSizeX() - x);
    height = OeMath.Min(height, GetLevelData().GetRealSizeY() - y);

    OeDrawTool.DrawRectangleHollow(spriteBatch, OeColors.WHITE, 200, new Rectangle(x, y, width, height), 0, false, 2);*/
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
    /*HandleCurrentLayerSelection();

    _mBucketPoints.Clear();

    Cvars_Set(Cvars_EDITOR_SHOW_TILES, true);

    if (!OeInput.IsModifierPressed())
    {
        if (OeInput.IsKeyTapped(Keys.OemMinus))
        {
            _mFlipX = false;
            _mFlipY = false;
            _mRotation = 0;
        }
        if (OeInput.IsKeyTapped(Keys.Z))
        {
            _mFlipX = !_mFlipX;
        }
        if (OeInput.IsKeyTapped(Keys.X))
        {
            _mFlipY = !_mFlipY;
        }
        if (OeInput.IsKeyTapped(Keys.R))
        {
            _mRotation += 90;
            if (_mRotation > 270)
            {
                _mRotation = 0;
            }
        }
    }

    if (OeInput.IsLeftMousePressed())
    {
        OperationHelper(false, false, false);
    }

    if (OeInput.IsMiddleMouseTapped())
    {
        OperationHelper(false, false, true);
    }

    if (OeInput.IsRightMousePressed())
    {
        OperationHelper(true, false, false);
    }

    if (OeInput.IsKeyTapped(Keys.B))
    {
        Point currentGrid = GetCurrentGrid();
        int gridX = currentGrid.X;
        int gridY = currentGrid.Y;
        OeTile currentTile = GetGridTile(gridX, gridY);
        BucketFill(currentTile.mDrawTiles[GetCurrentLayer()], gridX, gridY);
    }*/
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
    /*int locX = OeInput.GetMouseX();
    int locY = OeInput.GetMouseY();
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
    /*int x1;
    int y1;
    int x2;
    int y2;
    int width = 1;
    int height = 1;
    if (!_mIsAnimation)
    {
        width = OeWindowTilePicker.GetSelectedGridWidth();
        height = OeWindowTilePicker.GetSelectedGridHeight();
    }

    if (!IsSelecting())
    {
        Point currentGrid = GetCurrentGrid();
        x1 = currentGrid.X;
        y1 = currentGrid.Y;
        x2 = x1 + width;
        y2 = y1 + height;
    }
    else
    {
        x1 = GetSelectionRectangleGridX1();
        y1 = GetSelectionRectangleGridY1();
        x2 = GetSelectionRectangleGridX2();
        y2 = GetSelectionRectangleGridY2();
    }

    x2 = OeMath.Min(x2, GetLevelData().GetGridSizeX());
    y2 = OeMath.Min(y2, GetLevelData().GetGridSizeY());

    SetPatch1(GetPatch(x1, x2, y1, y2, x1, y1));
    Operation(x1, x2, y1, y2, reset, resetAll, grabTile);
    SetPatch2(GetPatch(x1, x2, y1, y2, x1, y1));

    CheckPatch();*/
}
void PartTile_Operation(int x1, int x2, int y1, int y2, bool resetTile, bool resetAll, bool grabTile)
{
    /*int tileSize = OeUtils.GetTileSize();

    int counterX = 0;
    int counterY = 0;
    int width = 1;
    int height = 1;
    if (!_mIsAnimation)
    {
        width = OeWindowTilePicker.GetSelectedGridWidth();
        height = OeWindowTilePicker.GetSelectedGridHeight();
    }
    int counterLimitX = width - 1;
    int counterLimitY = height - 1;
    int incX = 1;
    int incY = 1;

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

    for (int i = x1; i < x2; i += 1)
    {
        for (int j = y1; j < y2; j += 1)
        {
            bool autoTile = Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER) != -1;
            if (_mIsAnimation)
            {
                autoTile = false;
            }

            int selectedTextureX = OeWindowTilePicker.GetTexturePositionX() + (counterX * tileSize);
            int selectedTextureY = OeWindowTilePicker.GetTexturePositionY() + (counterY * tileSize);

            OeDrawTile[] draws = GetLevelData().GetTile(i, j).mDrawTiles;
            if (resetAll)
            {
                for (int k = 0; k < draws.Length; k += 1)
                {
                    OeDrawTile.Init(out draws[k]);
                }
            }
            else if (resetTile)
            {
                if (autoTile)
                {
                    OeEditorAutoTiler.AutoTile(Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER), GetLevelData().GetTileData(),
                        GetLevelData().mTilesetName, tileSize, i, j, GetCurrentLayer(),
                        new Rectangle(selectedTextureX, selectedTextureY, tileSize, tileSize), false, false);
                }
                OeDrawTile.Init(out draws[GetCurrentLayer()]);
            }
            else if (grabTile)
            {
                if (!OeDrawTile.IsZero(ref draws[GetCurrentLayer()]))
                {
                    _mRotation = (int)draws[GetCurrentLayer()].mRotation;
                    _mFlipX = draws[GetCurrentLayer()].mFlipX;
                    _mFlipY = draws[GetCurrentLayer()].mFlipY;
                    if (!OeDrawTile.IsAnimation(ref draws[GetCurrentLayer()]))
                    {
                        OeWindowTilePicker.SetTexturePositionX(draws[GetCurrentLayer()].mPoint.X);
                        OeWindowTilePicker.SetTexturePositionY(draws[GetCurrentLayer()].mPoint.Y);
                        _mIsAnimation = false;
                    }
                    else
                    {
                        OeWindowAnimTilePicker.SetAnimTile(draws[GetCurrentLayer()].mAnimation);
                        _mIsAnimation = true;
                    }
                }
            }
            else
            {
                if (autoTile)
                {
                    OeEditorAutoTiler.AutoTile(Cvars_GetAsInt(Cvars_EDITOR_AUTO_TILER), GetLevelData().GetTileData(),
                        GetLevelData().mTilesetName, tileSize, i, j, GetCurrentLayer(),
                        new Rectangle(selectedTextureX, selectedTextureY, tileSize, tileSize), false, true);
                }
                else
                {
                    if (!_mIsAnimation)
                    {
                        draws[GetCurrentLayer()].mRotation = _mRotation;
                        draws[GetCurrentLayer()].mFlipX = _mFlipX;
                        draws[GetCurrentLayer()].mFlipY = _mFlipY;
                        OeDrawTile.LoadSheet(ref draws[GetCurrentLayer()], selectedTextureX, selectedTextureY);
                    }
                    else
                    {
                        if (OeWindowAnimTilePicker.GetAnimTileResource() != null)
                        {
                            draws[GetCurrentLayer()].mRotation = _mRotation;
                            draws[GetCurrentLayer()].mFlipX = _mFlipX;
                            draws[GetCurrentLayer()].mFlipY = _mFlipY;
                            OeDrawTile.LoadAnimation(ref draws[GetCurrentLayer()], OeWindowAnimTilePicker.GetAnimTileResource().GetName());
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
    }*/
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
