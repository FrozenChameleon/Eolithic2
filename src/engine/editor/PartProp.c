#include "PartProp.h"

#include "../math/Rectangle.h"
#include "../math/Point.h"
#include "../math/Vector2.h"
#include "../math/Points.h"
#include "../leveldata/Prop.h"
#include "../leveldata/PropInstance.h"
#include "EditorPart.h"
#include "../leveldata/Tile.h"
#include "../../third_party/stb_ds.h"
#include "../input/Input.h"
#include "imgui.h"
#include "../utils/Utils.h"
#include "Editor.h"
#include "../math/Math.h"
#include "../core/Func.h"
#include "../components/Camera.h"
#include "../leveldata/LevelData.h"
#include "../utils/Cvars.h"
#include "../resources/ResourceList.h"
#include "../render/DrawTool.h"
#include "../utils/MString.h"
#include "../render/Texture.h"

typedef struct MousedOverPropData
{
	int mLoc;
	Rectangle mRectangle;
	char mName[EE_FILENAME_MAX];
	bool mWasFound;
	Point mTile;
    PropInstance* arr_props_list;
} MousedOverPropData;

static bool _mIsInSelectingMode;

static int32_t _mCurrentPropData;
static int32_t _mSelectedProp = -1;
static int32_t _mDragFrames;
static int32_t _mDepthForJankDepthTool = 70;
static bool _mIsSnapped = true;
static bool _mIsDraggingDisabled;
static bool _mIsScaleToolsDisabled = true;
static bool _mIsJankDepthToolEnabled;
static bool _mIsShowingPropDepthInfoDisabled;
static bool _mHideSelectionRectangle;
static char _mCurrentMode[EE_FILENAME_MAX];
static Rectangle _mMouseOverRectangle;
static PropInstance _mDummyProp;
static PropInstance _mNextProp;
static PropInstance _mDragProp;
static Point _mSelectedTile = Points_NegativeOne;
static Vector2 _mDragOffset;
static MousedOverPropData _mDragData;

static bool _mHasInit;
static void Init()
{
    if (_mHasInit)
    {
		return;
    }

	PropInstance_Init(&_mDummyProp);
    PropInstance_Init(&_mNextProp);
    PropInstance_Init(&_mDragProp);

    _mHasInit = true;
}

static void RefreshPropInstanceName()
{
    ResourceMan* rmProp = ResourceList_Prop();
    Resource* prop = ResourceMan_GetResourceByIndex(rmProp, _mCurrentPropData);
    Utils_strlcpy(_mNextProp.mName, Resource_GetName(prop), EE_FILENAME_MAX);
}
static PropInstance* GetSelectedArrPropList()
{
    if (Point_EqualTo(_mSelectedTile, Points_NegativeOne))
    {
        return NULL;
    }

    Tile* tile = EditorPart_GetGridTile(_mSelectedTile.X, _mSelectedTile.Y);
    if (tile == NULL)
    {
        return NULL;
    }

    return tile->arr_props;
}
static bool IsDragging()
{
    return _mDragFrames >= PART_PROP_FRAMES_DRAG_TIME;
}
static void UpdatePropOffset(PropInstance* propInstance)
{
    Vector2 propPos = Input_GetCameraAdjustedMouse(Editor_GetCamera());
    if (IsDragging())
    {
        Vector2_AddAssign(&propPos, _mDragOffset);
	}

    if (_mIsSnapped)
    {
        propInstance->mOffset = Vector2_Zero;
    }
    else
    {
        propInstance->mOffset = Vector2_Create(Utils_fmodf(propPos.X, TILE_SIZE), Utils_fmodf(propPos.Y, TILE_SIZE));
    }
}
static bool IsPropSelected()
{
    PropInstance* arr_selected_prop_list = GetSelectedArrPropList();
    if (arr_selected_prop_list == NULL)
    {
        return false;
    }

    if (_mSelectedProp > (arrlen(arr_selected_prop_list) - 1))
    {
        return false;
    }

    return true;
}
static PropInstance* GetSelectedProp()
{
    if (!IsPropSelected())
    {
        return &_mDummyProp;
    }
    else
    {
        return &GetSelectedArrPropList()[_mSelectedProp];
    }
}
static PropInstance* GetCurrentPropInstance()
{
    if (IsDragging())
    {
        return &_mDragProp;
    }
    else if (_mIsInSelectingMode)
    {
        return GetSelectedProp();
    }
    else
    {
        return &_mNextProp;
    }
}
static void NextProp(int direction)
{
    ResourceMan* rmProp = ResourceList_Prop();
    int64_t count = ResourceMan_Length(rmProp);
    int32_t loc = _mCurrentPropData + direction;
    if (((loc < count) && (direction == 1)) || ((loc >= 0) && (direction == -1)))
    {
        _mCurrentPropData = loc;
        RefreshPropInstanceName();
    }
    else
    {
        return;
    }
}
static void MouseWheelChanged(bool up)
{
    if (Input_IsCtrlPressed())
    {
        return;
    }

    int direction = 1;
    if (!up)
    {
        direction = -1;
    }

    PropInstance* propInstance = GetCurrentPropInstance();
    if (Input_IsKeyPressed(KEYS_D2))
    {
        propInstance->mDepth += direction * 5;

        if (propInstance->mDepth < 0)
        {
            propInstance->mDepth = 0;
        }
        if (propInstance->mDepth > 100)
        {
            propInstance->mDepth = 100;
        }
    }
    else if (Input_IsKeyPressed(KEYS_D3))
    {
        if (!_mIsScaleToolsDisabled)
        {
            propInstance->mScale += direction * 0.1f;
            //TODO propInstance->mScale = Math_RoundToTenths(propInstance->mScale);
        }
    }
    else
    {
        if (!_mIsInSelectingMode)
        {
            if (direction == 1)
            {
                NextProp(1);
            }
            else
            {
                NextProp(-1);
            }
        }
    }
}
static void HandleShortcuts()
{
    PropInstance* propInstance = GetCurrentPropInstance();

    Utils_strlcpy(_mCurrentMode, EE_STR_EMPTY, EE_FILENAME_MAX);
    if (Input_IsKeyTapped(KEYS_D1))
    {
        if (!IsDragging())
        {
            _mIsInSelectingMode = !_mIsInSelectingMode;
        }
    }
    else if (Input_IsKeyPressed(KEYS_D2))
    {
        Utils_strlcpy(_mCurrentMode, "|DEPTH MODE|", EE_FILENAME_MAX);
    }
    else if (Input_IsKeyPressed(KEYS_D3))
    {
        Utils_strlcpy(_mCurrentMode, "|SCALE MODE|", EE_FILENAME_MAX);
    }
    else if (Input_IsKeyTapped(KEYS_D4))
    {
        _mIsSnapped = !_mIsSnapped;
    }
    else if (Input_IsKeyTapped(KEYS_D5))
    {
        propInstance->mDepth = 70;
        propInstance->mRotation = 0;
        propInstance->mFlipX = false;
        propInstance->mFlipY = false;
        if (!_mIsScaleToolsDisabled)
        {
            propInstance->mScale = 1;
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
    if (!Input_IsModifierPressed())
    {
        if (Input_IsKeyTapped(KEYS_Z))
        {
            propInstance->mFlipX = !propInstance->mFlipX;
        }
        if (Input_IsKeyTapped(KEYS_X))
        {
            propInstance->mFlipY = !propInstance->mFlipY;
        }
        if (Input_IsKeyTapped(KEYS_R))
        {
            int rotation = (int)propInstance->mRotation;
            rotation += 90;
            if (rotation > 270)
            {
                rotation = 0;
            }
            propInstance->mRotation = rotation;
        }
    }
}
static void HandleDragging()
{
    UpdatePropOffset(&_mDragProp);
    _mIsInSelectingMode = true;
    if (!Input_IsLeftMousePressed())
    {
        Vector2 actualPos = Vector2_Add(EditorPart_GetUnrestrainedMouse(), _mDragOffset);
        Vector2 restrainedValue = EditorPart_GetRestrainedValue(actualPos);
        Point posTile = EditorPart_AlignToGridVector2(restrainedValue);
        _mSelectedTile.X = posTile.X;
        _mSelectedTile.Y = posTile.Y;
        int x1 = _mSelectedTile.X;
        int x2 = _mSelectedTile.X + 1;
        int y1 = _mSelectedTile.Y;
        int y2 = _mSelectedTile.Y + 1;
        Tile* tile = EditorPart_GetGridTilePoint(_mSelectedTile);
		EditorPart_PushPatch(tile);
        arrput(tile->arr_props, _mDragProp);
        EditorPart_FinishPatches();
        _mSelectedProp = arrlen(tile->arr_props) - 1;
        _mDragFrames = 0;
    }
    _mMouseOverRectangle = Rectangle_Empty;
}
static void HandleOperations(MousedOverPropData* mousedOverProp)
{
    if (Input_IsRightMouseTapped())
    {
        if (!EditorPart_IsSelecting())
        {
            if (mousedOverProp->mWasFound)
            {
                Point tilePos = mousedOverProp->mTile;
                int x1 = tilePos.X;
                int x2 = tilePos.X + 1;
                int y1 = tilePos.Y;
                int y2 = tilePos.Y + 1;
                Tile* tile = EditorPart_GetGridTilePoint(tilePos);
				EditorPart_PushPatch(tile);
                arrdel(tile->arr_props, mousedOverProp->mLoc);
				EditorPart_FinishPatches();
                return;
            }
        }
        else
        {
            int x1 = EditorPart_GetSelectionRectangleGridX1();
            int x2 = EditorPart_GetSelectionRectangleGridX2();
            int y1 = EditorPart_GetSelectionRectangleGridY1();
            int y2 = EditorPart_GetSelectionRectangleGridY2();

            for (int i = x1; i < x2; i += 1)
            {
                for (int j = y1; j < y2; j += 1)
                {
                    Tile* tile = EditorPart_GetGridTile(i, j);
                    EditorPart_PushPatch(tile);
                    arrsetlen(tile->arr_props, 0);
                    EditorPart_FinishPatches();
                }
            }
        }
    }

    if (Input_IsLeftMouseTapped())
    {
        if (_mIsInSelectingMode)
        {
            if (!mousedOverProp->mWasFound)
            {
                _mSelectedTile = Points_NegativeOne;
                _mSelectedProp = -1;
            }
        }
        else
        {
            if (!_mIsInSelectingMode)
            {
                if (PropInstance_IsPropValid(&_mNextProp))
                {
					Tile* tile = EditorPart_GetCurrentGridTile();
                    EditorPart_PushPatch(tile);
                    arrput(tile->arr_props, _mNextProp);
                    EditorPart_FinishPatches();
                }
            }
        }
    }

	if (_mIsInSelectingMode && Input_IsLeftMousePressed() && (_mDragFrames == 0) && mousedOverProp->mWasFound)
	{
		_mSelectedTile = mousedOverProp->mTile;
		_mSelectedProp = mousedOverProp->mLoc;
		PropInstance* tempProp = GetSelectedProp();
		Vector2 tilePos = Vector2_Create((_mSelectedTile.X * TILE_SIZE) + tempProp->mOffset.X,
			(_mSelectedTile.Y * TILE_SIZE) + tempProp->mOffset.Y);
		_mDragOffset = Vector2_Sub(tilePos, EditorPart_GetUnrestrainedMouse());
		if (!_mIsDraggingDisabled)
		{
			_mDragData = *mousedOverProp;
			_mDragFrames += 1;
		}
	}
}
static void GetMousedOverProp(MousedOverPropData* mousedOverProp)
{
	Utils_memset(mousedOverProp, 0, sizeof(MousedOverPropData));

    LevelData* levelData = Get_LevelData();

    Camera* camera = Editor_GetCamera();

    Rectangle levelBoundary = LevelData_GetBoundary(levelData);
    int x1 = Camera_GetX1(camera);
    int x2 = Camera_GetX2(camera, levelBoundary.Width);
    int y1 = Camera_GetY1(camera);
    int y2 = Camera_GetY2(camera, levelBoundary.Height);

    for (int i = x1; i < x2; i += 1)
    {
        for (int j = y1; j < y2; j += 1)
        {
            Tile* t = EditorPart_GetGridTile(i, j);
            PropInstance* arr_props_list = t->arr_props;
            for (int k = 0; k < arrlen(arr_props_list); k += 1)
            {
                PropInstance* propInstance = &arr_props_list[k];
                Vector2 position = Vector2_Create(i * TILE_SIZE, j * TILE_SIZE);
                Rectangle tempRectangle = PropInstance_GetRectangle(propInstance, position);
                Vector2 restrainedMouse = EditorPart_GetRestrainedMouse();
                Point currentMouse = Points_ToPointFromVector2(restrainedMouse);
                if (Rectangle_ContainsPoint(&tempRectangle, currentMouse))
                {
                    Point texRelativeMouse = Point_Create(currentMouse.X - tempRectangle.X, currentMouse.Y - tempRectangle.Y);
                    if (PropInstance_IsPropActuallyTouched(propInstance, texRelativeMouse))
                    {
                        mousedOverProp->mRectangle = tempRectangle;
                        Utils_strlcpy(mousedOverProp->mName, propInstance->mName, EE_FILENAME_MAX);
                        mousedOverProp->arr_props_list = arr_props_list;
                        mousedOverProp->mTile.X = i;
                        mousedOverProp->mTile.Y = j;
                        mousedOverProp->mLoc = k;
                        mousedOverProp->mWasFound = true;
                        return;
                    }
                }

            }
        }
    }
}
static void DrawHelper(SpriteBatch* spriteBatch)
{
    Vector2 propPos = Input_GetCameraAdjustedMouse(Editor_GetCamera());
    if (IsDragging())
    {
        Vector2_AddAssign(&propPos, _mDragOffset);
    }

    float tempX = propPos.X / TILE_SIZE;
    float tempY = propPos.Y / TILE_SIZE;
    propPos.X = (int)tempX * TILE_SIZE;
    propPos.Y = (int)tempY * TILE_SIZE;

    if (!_mIsShowingPropDepthInfoDisabled)
    {
        LevelData_DrawProps2(Get_LevelData(), spriteBatch, Editor_GetCamera(), true);
    }

    if (_mIsInSelectingMode && !_mHideSelectionRectangle)
    {
        //mMouseOverRectangle = OePropInstance.GetRectangle(ref mDragProp, propPos);
        if (!Rectangle_IsEmpty(&_mMouseOverRectangle))
        {
            DrawTool_DrawRectangleHollow2(spriteBatch, PROPINSTANCE_SELECTBOX_COLOR_OFF, 100, _mMouseOverRectangle, 0, false, 3);
        }
    }

    if (!_mIsInSelectingMode)
    {
        PropInstance_Draw(GetCurrentPropInstance(), spriteBatch, propPos);
    }

    if (IsDragging())
    {
        PropInstance_Draw(&_mDragProp, spriteBatch, propPos);
    }
}
static const char* GetStatus()
{
    return "";
    /*string selectMode = _mIsInSelectingMode ? "|MODE=SELECT" : "|MODE=CREATE";
    ref OePropInstance prop = ref GetCurrentPropInstance();
    string name = OeUtils.NOT_SET;
    if (prop.mName != null)
    {
        name = OePropInstance.GetPropData(ref prop).ToString();
    }
    return selectMode + _mCurrentMode + "|Prop=" + name + "|Rotation=" + prop.mRotation + "|Depth=" +
        prop.mDepth + "|Scale: " + prop.mScale + "|Flip X=" + prop.mFlipX + "|Flip Y="
        + prop.mFlipY + "|Snap=" + _mIsSnapped;*/
}
static bool IsSelectMode()
{
    return _mIsInSelectingMode;
}
static void SetCurrentProp(int index)
{
    _mCurrentPropData = index;
    RefreshPropInstanceName();
}
static void UpdateHelper()
{
    Init();

    if (_mIsJankDepthToolEnabled)
    {
        if (EditorPart_IsSelecting() && _mIsInSelectingMode)
        {
            int x1 = EditorPart_GetSelectionRectangleGridX1();
            int x2 = EditorPart_GetSelectionRectangleGridX2();
            int y1 = EditorPart_GetSelectionRectangleGridY1();
            int y2 = EditorPart_GetSelectionRectangleGridY2();
            for (int i = x1; i < x2; i += 1)
            {
                for (int j = y1; j < y2; j += 1)
                {
                    PropInstance* arr_current_props = EditorPart_GetGridTile(i, j)->arr_props;
					for (int k = 0; k < arrlen(arr_current_props); k += 1)
                    {
                        arr_current_props[k].mDepth = _mDepthForJankDepthTool;
                    }
                }
            }
        }
    }

    if (!IsDragging())
    {
        if (_mDragFrames > 0)
        {
            if (Input_IsLeftMousePressed())
            {
                _mDragFrames += 1;
                if (IsDragging())
                {
                    int tileX = _mDragData.mTile.X;
                    int tileY = _mDragData.mTile.Y;
                    Tile* tile = EditorPart_GetGridTile(tileX, tileY);
					EditorPart_PushPatch(tile);
                    _mDragProp = _mDragData.arr_props_list[_mDragData.mLoc];
					arrdel(_mDragData.arr_props_list, _mDragData.mLoc);
                    EditorPart_FinishPatches();
                }
            }
            else
            {
                _mDragFrames = 0;
            }
        }
    }

    _mMouseOverRectangle = Rectangle_Empty;

    HandleShortcuts();

    if (_mIsInSelectingMode)
    {
        if (IsDragging())
        {
            HandleDragging();
            return;
        }
    }
    else
    {
		if (!PropInstance_IsPropValid(&_mNextProp))
        {
            NextProp(1);
        }
		UpdatePropOffset(&_mNextProp);
    }

	MousedOverPropData mousedOverProp = { 0 };
	GetMousedOverProp(&mousedOverProp);
    if (mousedOverProp.mWasFound)
    {
        ImGui::SetTooltip(mousedOverProp.mName);
    }
    _mMouseOverRectangle = mousedOverProp.mRectangle;

	HandleOperations(&mousedOverProp);

    if (_mIsInSelectingMode)
    {
        if (IsPropSelected())
        {
			_mMouseOverRectangle = PropInstance_GetRectangle(GetSelectedProp(), Vector2_Create(_mSelectedTile.X * TILE_SIZE, _mSelectedTile.Y * TILE_SIZE));
        }
    }
}

PartFuncData PartProp_GetFuncData()
{
    PartFuncData data = EditorPart_CreateDummyFuncData();

	Utils_strlcpy(data.mDebugName, "PROP", EE_FILENAME_MAX);

    data.CreateWindows = PartProp_CreateWindows;
    data.Update = PartProp_Update;
    data.Draw = PartProp_Draw;
    data.DrawHud = PartProp_DrawHud;
    data.JustChangedToThisPart = PartProp_JustChangedToThisPart;
    data.JustLoadedMap = PartProp_JustLoadedMap;

    return data;
}
void PartProp_CreateWindows()
{
    ResourceMan* rmProp = ResourceList_Prop();
    if (ResourceMan_Length(rmProp) == 0)
    {
        return;
    }

    //ImGui::SetNextWindowAlwaysOpen();
	ImGui::SetNextWindowSize(ImVec2(500, 600));
    if (!ImGui::Begin(KEY_WINDOW_PROP_SELECTOR))
    {
		ImGui::End();
        return;
    }

    Prop* hoveredProp = NULL;

    {
        IStringArray* sa = IStringArray_CreateForJustThisFrame();
        ResourceMan_FillArrayWithAllResourceNames(rmProp, sa);
        if (ImGui::BeginListBox("##Props", ImVec2(500, 400)))
        {
            for (int i = 0; i < IStringArray_Length(sa); i += 1)
            {
				bool isSelected = (i == _mCurrentPropData);
				const char* filename = IStringArray_Get(sa, i);
                if (ImGui::Selectable(filename, &isSelected))
                {
                    SetCurrentProp(i);
                }
                if (ImGui::IsItemHovered())
                {
					hoveredProp = (Prop*)ResourceMan_GetResourceDataByIndex(rmProp, i);
                }
            }
            ImGui::EndListBox();
        }
    }
  
    ImGui::NewLine();

    ImGui::Text("Mode");
    if (ImGui::RadioButton("Create", !_mIsInSelectingMode))
    {
        _mIsInSelectingMode = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Select", _mIsInSelectingMode))
    {
        _mIsInSelectingMode = true;
    }

	ImGui::Checkbox("Snap?", &_mIsSnapped);

	ImGui::SliderInt("Depth", &GetCurrentPropInstance()->mDepth, 0, 100);
	if (ImGui::InputInt("Depth(Textbox)", &GetCurrentPropInstance()->mDepth))
    {
		GetCurrentPropInstance()->mDepth = Math_ClampInt(GetCurrentPropInstance()->mDepth, 0, 100);
    }

	ImGui::SliderFloat("Rotation", &GetCurrentPropInstance()->mRotation, 0, 360);
	if (ImGui::InputFloat("Rotation(Textbox)", &GetCurrentPropInstance()->mRotation, 90))
    {
        GetCurrentPropInstance()->mRotation = Math_ClampFloat(GetCurrentPropInstance()->mRotation, 0, 360);
    }

	if (ImGui::InputFloat("Offset X", &GetCurrentPropInstance()->mDrawOffset.X))
    {
        GetCurrentPropInstance()->mDrawOffset.X = (int32_t)GetCurrentPropInstance()->mDrawOffset.X;
    }
	if (ImGui::InputFloat("Offset Y", &GetCurrentPropInstance()->mDrawOffset.Y))
    {
        GetCurrentPropInstance()->mDrawOffset.Y = (int32_t)GetCurrentPropInstance()->mDrawOffset.Y;
    }

    if (!_mIsScaleToolsDisabled)
    {
		ImGui::InputFloat("Scale", &GetCurrentPropInstance()->mScale);
	}

	ImGui::Checkbox("Flip X", &GetCurrentPropInstance()->mFlipX);
	ImGui::Checkbox("Flip Y", &GetCurrentPropInstance()->mFlipY);
	ImGui::Checkbox("Disable Prop Dragging", &_mIsDraggingDisabled);
	ImGui::Checkbox("Do Not Show Depth?", &_mIsShowingPropDepthInfoDisabled);
	ImGui::Checkbox("Hide Selection Rectangle?", &_mHideSelectionRectangle);

	ImGui::NewLine();

	ImGui::Text("--- Keys ---");
	ImGui::Text("LClick - Create / Select");
	ImGui::Text("RClick - Remove");
    ImGui::Text("1 - Toggle Mode");
    ImGui::Text("Hold 2 - Depth Mode (Use MWheel)");
    {
        MString* tempString = NULL;
        MString_AssignString(&tempString, "Hold 3 - Scale Mode (Use MWheel)");
		MString_AddAssignString(&tempString, _mIsScaleToolsDisabled ? " (Disabled)" : "");
        ImGui::Text(MString_Text(tempString));
		MString_Dispose(&tempString);
    }
    ImGui::Text("4 - Toggle Snap");
    ImGui::Text("5 - Reset Current Prop");
    ImGui::Text("R - Rotate");
    ImGui::Text("Z - Flip X");
    ImGui::Text("X - Flip Y");
    ImGui::Text("MWheel - Select Prop (Create Mode)");

    ImGui::NewLine();
	ImGui::Checkbox("Enable Jank Depth Tool", &_mIsJankDepthToolEnabled);
	if (ImGui::InputInt("Jank Depth", &_mDepthForJankDepthTool))
    {
        _mDepthForJankDepthTool = Math_ClampInt(_mDepthForJankDepthTool, 0, 100);
    }
    ImGui::NewLine();

    ImGui::End();

    //ImGui::SetNextWindowAlwaysOpen();
   // ImGui::SetNextWindowAlwaysAutoResize();
    if (!ImGui::Begin(KEY_WINDOW_PROP_SELECTOR_PREVIEW))
    {
        ImGui::End();
        return;
    }

    Prop* previewProp;
    if (hoveredProp != NULL)
    {
        previewProp = hoveredProp;
    }
    else
    {
        previewProp = (Prop*)ResourceMan_GetResourceDataByIndex(rmProp, _mCurrentPropData);
    }
    if (previewProp != NULL)
    {
        Sheet* propSheet = Prop_GetSheet(previewProp);
		Resource* texRes = propSheet->mTextureResource;
		Texture* tex = (Texture*)Resource_GetData(texRes);
		ImGui::Image((ImTextureID)(intptr_t)tex->mTextureData, ImVec2(tex->mBounds.Width, tex->mBounds.Height));
    }

    ImGui::End();
}
void PartProp_Update()
{
    EditorPart_DoDefaultEditorPartUpdateRoutine();

    UpdateHelper();
}
void PartProp_DrawHud(SpriteBatch* spriteBatch)
{
    EditorPart_DoDefaultEditorPartDrawHudRoutine(spriteBatch, GetStatus());
}
void PartProp_Draw(SpriteBatch* spriteBatch)
{
    EditorPart_DoDefaultEditorPartDrawRoutine(spriteBatch, EditorPart_DefaultDrawSingleSelectionSelectedTiles);

    DrawHelper(spriteBatch);
}
void PartProp_JustChangedToThisPart()
{
    Cvars_SetAsBool(CVARS_EDITOR_SHOW_COLLISION, false);
    Cvars_SetAsBool(CVARS_EDITOR_SHOW_PROPS, true);
}
void PartProp_JustLoadedMap()
{
    _mSelectedTile = Points_NegativeOne;
}