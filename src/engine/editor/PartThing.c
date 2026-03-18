#include "PartThing.h"

#include "../math/Point.h"
#include "../math/Points.h"
#include "../leveldata/ThingInstance.h"
#include "imgui.h"
#include "../resources/ResourceList.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "EditorPart.h"
#include "../input/Input.h"
#include "../leveldata/Tile.h"
#include "../utils/Cvars.h"
#include "Editor.h"
#include "PartProp.h"
#include "../core/Func.h"
#include "../leveldata/LevelData.h"
#include "../core/GameUpdater.h"
#include "../leveldata/ThingSettings.h"
#include "../globals/Align.h"
#include "../render/DrawTool.h"
#include "../render/SpriteBatch.h"

#define TEXTBOX_WIDTH 400
#define SECTION_WIDTH 450

static int _mSelectedLoc = -1;
static int _mCurrentThingSetting;
static int _mDragFrames;
static Point _mCurrentPoint;
static Point _mSelectedGrid = Points_NegativeOne;
static Point _mSelectedReal;
static Point _mWindowSize;
static ThingInstance* _mDraggedInstance;

static bool IsDragging()
{
    return _mDragFrames >= PART_PROP_FRAMES_DRAG_TIME;
}
static Resource* GetCurrentThingSetting()
{
    ResourceMan* rmThings = ResourceList_ThingSettings();
    if (ResourceMan_Length(rmThings) != 0)
    {
        return ResourceMan_GetResourceByIndex(rmThings, _mCurrentThingSetting);
    }
    return NULL;
}
static bool NodeInMouse(Point point)
{
    Point current = Point_Add(_mSelectedGrid, point);
    current = Point_Mul(current, Points_TileSize());

    Vector2 restrainedMouse = EditorPart_GetRestrainedMouse();
    Rectangle tempRectangle = Rectangle_Create(current.X, current.Y, TILE_SIZE, TILE_SIZE);
    if (Rectangle_Contains(tempRectangle, (int)restrainedMouse.X, (int)restrainedMouse.Y))
    {
        return true;
    }

    return false;
}
static ThingInstance CreateInstance(Resource* resource)
{
    ThingInstance instance = { 0 };
    Utils_strlcpy(instance.mName, Resource_GetName(resource), EE_FILENAME_MAX);
    //TODO OeGame.GetHelper().SetupThingInstanceSettings(instance, false);
    return instance;
}
static ThingInstance* GetSelectedInstance()
{
    if (_mDraggedInstance != NULL)
    {
        return _mDraggedInstance;
    }

    if (Point_EqualTo(_mSelectedGrid, Points_NegativeOne))
    {
        return NULL;
    }

    if (_mSelectedLoc == -1)
    {
        return NULL;
    }

    Tile* tile = LevelData_GetTile(Get_LevelData(), _mSelectedGrid.X, _mSelectedGrid.Y);
    if (tile == NULL)
    {
        return NULL;
    }

    ThingInstance* arr_list = tile->arr_instances;
    if (_mSelectedLoc > (arrlen(arr_list) - 1))
    {
        return NULL;
    }

    return &arr_list[_mSelectedLoc];
}
static void Deselect()
{
    if (GetSelectedInstance() != NULL)
    {
        _mSelectedLoc = -1;
        _mSelectedGrid = Points_NegativeOne;
    }
}
static void DrawHelper(SpriteBatch* spriteBatch)
{
    if (GetSelectedInstance() != NULL)
    {
        uint64_t flickerSpeed = 30;
        bool flipped = (GameUpdater_GetGlobalTicks() % flickerSpeed) >= (flickerSpeed / 2);
        Color color = flipped ? COLOR_WHITE : COLOR_MAGENTA;
        if (IsDragging())
        {
            color = flipped ? COLOR_MAGENTA : COLOR_ORANGE;
        }
        ThingInstance_Draw2(GetSelectedInstance(), spriteBatch, color, 130, _mSelectedReal, true);
    }
    else
    {
        Resource* resource = GetCurrentThingSetting();
        ThingSettings* settings = (ThingSettings*)Resource_GetData(resource);
        if (settings == NULL)
        {
            return;
        }

        int offset = 40;
        Point gridAlignedMousePixel = EditorPart_GetGridAlignedMousePixel();
        float x = HALF_TILE_SIZE + gridAlignedMousePixel.X + offset;
        float y = HALF_TILE_SIZE + gridAlignedMousePixel.Y - offset;

        bool drawDefault = false;
        const char* tex = settings->mPreviewSheet;
        if (!Utils_StringEqualTo(tex, EE_STR_NOT_SET))
        {
            Sheet_Draw3(Sheet_GetSheet(tex), spriteBatch, COLOR_WHITE, 100, true, true, NULL, Vector2_Create(x, y), Vector2_One, 0);
        }
        else
        {
            drawDefault = true;
        }

        if (drawDefault)
        {
            DrawTool_DrawRectangle(spriteBatch, COLOR_RED, 100, Rectangle_Create((int)x, (int)y, TILE_SIZE, TILE_SIZE), 0, true);
            SpriteBatch_DrawString2(spriteBatch, "editor", Resource_GetName(resource),
                COLOR_WHITE, 100, Vector2_Create(x, y), ALIGN_CENTER, ALIGN_CENTER);
        }
    }
}
static bool InstanceInMouse(Point pos, ThingInstance* instance)
{
    Vector2 resMouse = EditorPart_GetRestrainedMouse();
    Rectangle tempRectangle = ThingInstance_GetRectangle(instance, pos.X, pos.Y);
    if (Rectangle_Contains(tempRectangle, (int32_t)resMouse.X, (int32_t)resMouse.Y))
    {
        return true;
    }
    return false;
}
static bool InstanceInSelectionRectangle(Point pos, ThingInstance* instance)
{
    if (!EditorPart_IsSelecting())
    {
        return false;
    }

    Rectangle rect1 = EditorPart_GetSelectionRectangle();
    Rectangle rect2 = ThingInstance_GetRectangle(instance, pos.X, pos.Y);
    if (Rectangle_Intersects(rect1, rect2))
    {
        return true;
    }

    return false;
}
static void HandleThingsUnderMouse(bool isSelecting, bool isDeleting)
{
    Camera* camera = Editor_GetCamera();

    Rectangle levelBoundary = LevelData_GetBoundary(Get_LevelData());
    int x1 = Camera_GetX1(camera);
    int x2 = Camera_GetX2(camera, levelBoundary.Width);
    int y1 = Camera_GetY1(camera);
    int y2 = Camera_GetY2(camera, levelBoundary.Height);

    //SetPatch1(GetPatch(x1, x2, y1, y2, x1, y1));
    for (int i = x1; i < x2; i += 1)
    {
        for (int j = y1; j < y2; j += 1)
        {
            Tile* tile = LevelData_GetTile(Get_LevelData(), i, j);
            ThingInstance* arr_array = tile->arr_instances;
            if (arrlen(arr_array) == 0)
            {
                continue;
            }

            for (int k = 0; k < arrlen(arr_array); k += 1)
            {
                ThingInstance* instance = &arr_array[k];
                Point temp = Point_Create(i * TILE_SIZE, j * TILE_SIZE);
                if (InstanceInMouse(temp, instance) || InstanceInSelectionRectangle(temp, instance))
                {
                    if (isDeleting)
                    {
                        if (Input_IsMiddleMousePressed())
                        {
                            arrdel(arr_array, k);
                        }
                    }
                    if (isSelecting)
                    {
                        _mSelectedLoc = k;
                        _mSelectedGrid = Point_Create(i, j);
                        //TODO OeGame.GetHelper().SetupThingInstanceSettings(GetSelectedInstance(), false);
                        _mSelectedReal = Point_Create(_mSelectedGrid.X * TILE_SIZE, _mSelectedGrid.Y * TILE_SIZE);
                        _mDragFrames += 1;
                        return;
                    }
                }
            }
        }
    }
    //SetPatch2(GetPatch(x1, x2, y1, y2, x1, y1));
    //CheckPatch();
}
static void MouseWheelChanged(bool up)
{
    if (Input_IsCtrlPressed())
    {
        return;
    }
    if (up)
    {
        ResourceMan* rmThings = ResourceList_ThingSettings();
        int count = ResourceMan_Length(rmThings);
        _mCurrentThingSetting += 1;
        if (_mCurrentThingSetting >= count - 1)
        {
            _mCurrentThingSetting = count - 1;
        }
    }
    else
    {
        _mCurrentThingSetting -= 1;
        if (_mCurrentThingSetting < 0)
        {
            _mCurrentThingSetting = 0;
        }
    }
}
static void SetThingSettings(int index)
{
    _mCurrentThingSetting = index;
}
static const char* GetStatus()
{
    return "";
    //string name = "NOTHING";
    //if (GetSelectedInstance() != null)
   // {
    //    name = GetSelectedInstance().GetName();
   // }
   // else if (GetCurrentThingSetting() != null)
    //{
     //   name = GetCurrentThingSetting().GetName();
   // }
   // return "|NAME=" + name;
}
static void CreateThingListWindow()
{
	ResourceMan* rmThings = ResourceList_ThingSettings();
    if (ResourceMan_Length(rmThings) == 0)
    {
        return;
    }

    //ImGui::SetNextWindowAlwaysOpen();
	if (!ImGui::Begin("Thing Mode"))
    {
		ImGui::End();
        return;
    }

    ThingSettings* hoveredSettings = NULL;
    {
        IStringArray* sa = IStringArray_CreateForJustThisFrame();
        ResourceMan_FillArrayWithAllResourceNames(rmThings, sa);
        if (ImGui::BeginListBox("##Resources", ImVec2(500, 400)))
        {
            for (int i = 0; i < IStringArray_Length(sa); i += 1)
            {
                bool isSelected = (i == _mCurrentThingSetting);
                const char* filename = IStringArray_Get(sa, i);
                if (ImGui::Selectable(filename, &isSelected))
                {
                    SetThingSettings(i);
                }
                if (ImGui::IsItemHovered())
                {
                    hoveredSettings = (ThingSettings*)ResourceMan_GetResourceDataByIndex(rmThings, i);
                }
            }
            ImGui::EndListBox();
        }
    }

    ImGui::NewLine();

    ImGui::Text("--- Keys ---");
    ImGui::Text("LClick - Select");
    ImGui::Text("Double LClick - Create");
    ImGui::Text("RClick - Create Node");
    ImGui::Text("MClick - Remove Node");
    ImGui::Text("1 - Remove All Nodes");
    ImGui::Text("MWheel - Select Prop (Create Mode)");

    ImGui::End();

    //ImGui::SetNextWindowAlwaysOpen();
    //ImGui::SetNextWindowAlwaysAutoResize();
    if (!ImGui::Begin("Thing Preview"))
    {
        ImGui::End();
        return;
    }

    ThingSettings* previewThing;
    if (hoveredSettings != NULL)
    {
        previewThing = hoveredSettings;
    }
    else
    {
		previewThing = (ThingSettings*)Resource_GetData(GetCurrentThingSetting());
    }
    if (previewThing != NULL)
    {
		ResourceMan* texMan = ResourceList_Texture();
        Texture* tex = (Texture*)ResourceMan_GetResourceData(texMan, previewThing->mPreviewSheet);
        ImGui::Image((ImTextureID)(intptr_t)tex->mTextureData, ImVec2(tex->mBounds.Width, tex->mBounds.Height));
    }

    ImGui::End();
}
static void AddDirectionCb(const char* direction, StringPair* pair, int x, int y)
{
    bool value = false;
    const char* pairValue = pair->mValue;
    if (pairValue != NULL)
    {
        if (Utils_StringEqualTo(pairValue, direction))
        {
            value = true;
        }
    }
    MString* tempString = NULL;
    MString_Combine3(&tempString, pair->mKey, " ", direction);
    if (ImGui::Checkbox(MString_Text(tempString), &value))
    {
        Utils_strlcpy(pair->mValue, direction, EE_FILENAME_MAX);
    }
    MString_Dispose(&tempString);
}
static void AddPair(int number, StringPair* pair, int width, int x, int y)
{
    if (Utils_StringStartsWith(pair->mKey, "INT_"))
    {
        int temp = Utils_ParseInt(pair->mValue); //TODO USED TO BE TRY
        if (ImGui::InputInt(pair->mKey, &temp))
        {
            Utils_strlcpy(pair->mValue, Utils_IntToStringGlobalBuffer(temp), EE_FILENAME_MAX);
        }
    }
    else if (Utils_StringStartsWith(pair->mKey, "FLT_"))
    {
        float temp = Utils_ParseFloat(pair->mValue);
        if (ImGui::InputFloat(pair->mKey, &temp))
        {
            Utils_strlcpy(pair->mValue, Utils_FloatToStringGlobalBuffer(temp), EE_FILENAME_MAX);
        }
    }
    else if (Utils_StringStartsWith(pair->mKey, "DTN_"))
    {
        ImGui::Text(pair->mKey);
        int offsetX = 60;
        int offsetY = 10;
        AddDirectionCb("-1", pair, x, y + offsetY);
        AddDirectionCb("0", pair, x + offsetX, y + offsetY);
        AddDirectionCb("1", pair, x + (offsetX * 2), y + offsetY);
    }
    else if (Utils_StringStartsWith(pair->mKey, "BLN_"))
    {
        bool value = false;
        const char* pairValue = pair->mValue;
        if (pairValue != NULL)
        {
            MString* temp = NULL;
            MString_AssignString(&temp, pairValue);
            MString_ToUpper(&temp);
            if (MString_EqualToString(temp, "T") || MString_EqualToString(temp, "TRUE"))
            {
                value = true;
            }
            else
            {
                value = false;
            }
            MString_Dispose(&temp);
        }
        if (ImGui::Checkbox(pair->mKey, &value))
        {
            if (value)
            {
                Utils_strlcpy(pair->mValue, "T", EE_FILENAME_MAX);
            }
            else
            {
                Utils_strlcpy(pair->mValue, "F", EE_FILENAME_MAX);
            }
        }
    }
    else
    {
        ImGui::InputText(pair->mKey, pair->mValue, EE_FILENAME_MAX);
    }
}
static void CreateThingDataWindow()
{
    //ImGui::SetNextWindowAlwaysOpen();
	//ImGui::SetNextWindowSize(ImVec2(_mWindowSize.X, _mWindowSize.Y));
	if (!ImGui::Begin("Thing Data") || (GetSelectedInstance() == NULL))
    {
		ImGui::End();
        return;
	}

    ThingInstance* instance = GetSelectedInstance();

    StringPair* arr_pairs = instance->arr_settings;

    ImGui::Text(instance->mName);

    int x;
    int y;
    int counterX = 0;
    int counterY = 0;
    int maxWidth = 0;
    int maxHeight = 0;
    int amountLimit = 13;
    for (int i = 0; i < arrlen(arr_pairs); i += 1)
    {
        y = 40 + (60 * counterY);
        if (y > maxHeight)
        {
            maxHeight = y;
        }

        x = 10 + (counterX * SECTION_WIDTH);
        if (x > maxWidth)
        {
            maxWidth = x;
        }

        AddPair(i, &arr_pairs[i], TEXTBOX_WIDTH, x, y);

        counterY += 1;
        if (counterY > amountLimit)
        {
            counterY = 0;
            counterX += 1;
        }
    }

    _mWindowSize.X = maxWidth + SECTION_WIDTH;
    _mWindowSize.Y = maxHeight + 60;

    ImGui::End();
}
static void UpdateHelper()
{
    if (IsDragging())
    {
        _mSelectedReal = EditorPart_GetGridAlignedMousePixel();
        _mSelectedGrid = EditorPart_AlignToGridPoint(_mSelectedReal);
        if (!Input_IsLeftMousePressed())
        {
            if (_mDraggedInstance != NULL)
            {
                int tileX = _mSelectedGrid.X;
                int tileY = _mSelectedGrid.Y;
                //SetPatch1(GetPatch(tileX, tileX + 1, tileY, tileY + 1, tileX, tileY));
                Tile* tile = EditorPart_GetGridTile(tileX, tileY);
                arrput(tile->arr_instances, *_mDraggedInstance);
				_mSelectedLoc = arrlen(tile->arr_instances) - 1;
                _mDraggedInstance = NULL;
                //SetPatch2(GetPatch(tileX, tileX + 1, tileY, tileY + 1, tileX, tileY));
                //CheckPatch();
            }
            _mDragFrames = 0;
        }
        else
        {
            return;
        }
    }

    if (Input_IsKeyTapped(KEYS_D1))
    {
        if (GetSelectedInstance() != NULL)
        {
			arrsetlen(GetSelectedInstance()->arr_nodes, 0);
        }
    }
    if (Input_IsKeyTapped(KEYS_D2))
    {
        if (GetSelectedInstance() != NULL)
        {
            if (arrlen(GetSelectedInstance()->arr_nodes) != 0)
            {
                arrdel(GetSelectedInstance()->arr_nodes, arrlen(GetSelectedInstance()->arr_nodes) - 1);
            }
        }
    }

    if (Input_JustScrolledDown())
    {
        MouseWheelChanged(false);
    }

    if (Input_JustScrolledUp())
    {
        MouseWheelChanged(true);
    }

    Cvars_SetAsBool(CVARS_EDITOR_SHOW_THINGS, true);

    //SELECT
    if (GetSelectedInstance() == NULL) //delete
    {
        if (Input_IsMiddleMouseTapped())
        {
            HandleThingsUnderMouse(false, true);
        }
    }

    if (_mDragFrames > 0)
    {
        if (Input_IsLeftMousePressed())
        {
			_mDragFrames += 1;
			if (IsDragging())
			{
				int tileX = _mSelectedGrid.X;
				int tileY = _mSelectedGrid.Y;
				//SetPatch1(GetPatch(tileX, tileX + 1, tileY, tileY + 1, tileX, tileY));
				_mDraggedInstance = GetSelectedInstance();
				Tile* tile = EditorPart_GetGridTile(tileX, tileY);
				arrdel(tile->arr_instances, _mSelectedLoc);
				// SetPatch2(GetPatch(tileX, tileX + 1, tileY, tileY + 1, tileX, tileY));
				// CheckPatch();
				return;
			}
		}
		else
        {
            _mDragFrames = 0;
        }
    }
    else
    {
        bool doNotScan = false;
        if (GetSelectedInstance() != NULL)
        {
            if (InstanceInMouse(_mSelectedReal, GetSelectedInstance()))
            {
                doNotScan = true;
                if (Input_IsLeftMouseTapped())
                {
                    _mDragFrames += 1;
                }
            }
        }
        if (!doNotScan)
        {
            if (Input_IsLeftMouseTapped())
            {
                Deselect();
                HandleThingsUnderMouse(true, false);
            }
        }
    }
    //

    //NODES
    if (GetSelectedInstance() == NULL)
    {
        _mCurrentPoint = Points_NegativeOne;
    }
    else
    {
        Point currentGrid = EditorPart_GetCurrentGrid();
		Point nodePos = Point_Sub(currentGrid, _mSelectedGrid);

		if (!Point_EqualTo(_mCurrentPoint, Points_NegativeOne))
        {
            if (Input_IsRightMousePressed())
            {
                _mCurrentPoint = nodePos;
            }
            else
            {
                _mCurrentPoint = Points_NegativeOne;
            }
        }

		if ((GetSelectedInstance() != NULL) && (Point_EqualTo(_mCurrentPoint, Points_NegativeOne)))
        {
            Point* cur_arr_nodes = GetSelectedInstance()->arr_nodes;
            bool stop = false;
            for (int i = 0; i < arrlen(cur_arr_nodes); i += 1)
            {
                Point point = cur_arr_nodes[i];
                if (NodeInMouse(point))
                {
                    if (Input_IsMiddleMousePressed())
                    {
						arrdel(cur_arr_nodes, i);
                        stop = true;
                        break;
                    }
                    else if (Input_IsRightMousePressed())
                    {
                        _mCurrentPoint = point;
                        stop = true;
                        break;
                    }
                }
            }
            if (!stop)
            {
                if (Input_IsRightMouseTapped())
                {
                    arrput(cur_arr_nodes, nodePos);
                }
            }
        }
    }
    //

    //CREATE
    if (Input_IsLeftMouseDoubleClicked() && GetSelectedInstance() == NULL)
    {
        Resource* resource = GetCurrentThingSetting();
		ThingSettings* settings = (ThingSettings*)Resource_GetData(resource);
        if (settings != NULL)
        {
            Point currentGrid = EditorPart_GetCurrentGrid();
            int x1 = currentGrid.X;
            int x2 = x1 + 1;
            int y1 = currentGrid.Y;
            int y2 = y1 + 1;

            if (!EditorPart_IsSelecting())
            {
                //SetPatch1(GetPatch(x1, x2, y1, y2, x1, y1));

                ThingInstance instance = CreateInstance(resource);
                Tile* tile = EditorPart_GetCurrentGridTile();
                arrput(tile->arr_instances, instance);

               // SetPatch2(GetPatch(x1, x2, y1, y2, x1, y1));
            }
            else
            {
                x1 = EditorPart_GetSelectionRectangleGridX1();
                x2 = EditorPart_GetSelectionRectangleGridX2();
                y1 = EditorPart_GetSelectionRectangleGridY1();
                y2 = EditorPart_GetSelectionRectangleGridY2();

                //SetPatch1(GetPatch(x1, x2, y1, y2, x1, y1));

                for (int i = x1; i < x2; i += 1)
                {
                    for (int j = y1; j < y2; j += 1)
                    {
                        ThingInstance instance = CreateInstance(resource);
                        Tile* tile = EditorPart_GetGridTile(i, j);
						arrput(tile->arr_instances, instance);
                    }
                }

                //SetPatch2(GetPatch(x1, x2, y1, y2, x1, y1));
            }

            //CheckPatch();
        }
    }
    //
}

PartFuncData PartThing_GetFuncData()
{
    PartFuncData data = EditorPart_CreateDummyFuncData();

	Utils_strlcpy(data.mDebugName, "THING", EE_FILENAME_MAX);

    data.CreateWindows = PartThing_CreateWindows;
    data.Update = PartThing_Update;
    data.Draw = PartThing_Draw;
    data.DrawHud = PartThing_DrawHud;
    data.JustChangedToThisPart = PartThing_JustChangedToThisPart;

    return data;
}
void PartThing_CreateWindows()
{
    CreateThingListWindow();

    CreateThingDataWindow();
}
void PartThing_Update()
{
    EditorPart_DoDefaultEditorPartUpdateRoutine();

	UpdateHelper();
}
void PartThing_DrawHud(SpriteBatch* spriteBatch)
{
	EditorPart_DoDefaultEditorPartDrawHudRoutine(spriteBatch, GetStatus());
}
void PartThing_Draw(SpriteBatch* spriteBatch)
{
	EditorPart_DoDefaultEditorPartDrawRoutine(spriteBatch, EditorPart_DefaultDrawSingleSelectionSelectedTiles);

	DrawHelper(spriteBatch);
}
void PartThing_JustChangedToThisPart()
{
	Cvars_SetAsBool(CVARS_EDITOR_SHOW_COLLISION, false);
    Cvars_SetAsBool(CVARS_EDITOR_SHOW_THINGS, true);
}
