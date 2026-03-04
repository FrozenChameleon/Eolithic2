#include "ThingEditor.h"

#include "imgui.h"
#include "../utils/Macros.h"
#include "../render/Color.h"
#include "../../third_party/stb_ds.h"
#include "../leveldata/ImageData.h"
#include "../leveldata/ImageDataInstance.h"
#include "../leveldata/ThingSettings.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "../globals/Globals.h"
#include "../render/SpriteBatch.h"
#include "../render/DrawTool.h"
#include "EditorGlobals.h"
#include "../math/Math.h"
#include "../resources/ResourceList.h"
#include "../globals/OeState.h"
#include "../globals/OePhase.h"
#include "../render/Renderer.h"

#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 20
#define PREVIEW_WIDTH 500
#define PREVIEW_HEIGHT 500

typedef struct PreviewGraphicsData
{
    char* key;
    ImageDataInstance* value; //THIS IS AN STB ARRAY, SHOULD BE ARR_
} PreviewGraphicsData;

typedef struct PreviewImage
{
	PreviewGraphicsData* sh_images;
	bool mShowCollision;
	Color mColor;
} PreviewImage;

static bool _mShowAdvancedStuff;
static char _mTempFileName[EE_FILENAME_MAX];
static ThingSettings _mSettings;
static PreviewImage _mPreview;
#define POSSIBLE_SCALES_LEN 9
static float _mPossibleScales[POSSIBLE_SCALES_LEN] = { 0.25f, 0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 5.0f };
static float _mScale = 1.0f;

static char _mManualAddStateString[EE_FILENAME_MAX];
static char _mManualAddPhaseString[EE_FILENAME_MAX];
static char _mCurrentState[EE_FILENAME_MAX];
static char _mCurrentPhase[EE_FILENAME_MAX];
static char _mCurrentImage[EE_FILENAME_MAX];
static int _mCurrentImageIndex;
static bool _mHasInit;

static bool IsSingleStateMode()
{
    return Cvars_GetAsBool(CVARS_EDITOR_IS_SINGLE_STATE_MODE);
}
static bool Button(const char* name)
{
    return ImGui::Button(name, ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT));
}
static void PreviewImageData_Update()
{
	for (int i = 0; i < shlen(_mPreview.sh_images); i += 1)
    {
        ImageDataInstance* arr_list = _mPreview.sh_images[i].value;
        for (int j = 0; j < arrlen(arr_list); j += 1)
        {
            if (arr_list[j].mData->mCanAnimate)
            {
                Animation_Update(&arr_list[j].mAnimation);
            }
        }
    }
}
static void PreviewImageData_Clear()
{
	shfree(_mPreview.sh_images);
}
static void PreviewImageData_LoadArray(const char* state, ImageData* imageDatas, int32_t imageDatasLen)
{
    for (int i = 0; i < shlen(_mPreview.sh_images); i += 1)
    {
        ImageDataInstance* arr_list = _mPreview.sh_images[i].value;
        for (int j = 0; j < arrlen(arr_list); j += 1)
        {
            if (arr_list[j].mData->mCanAnimate)
            {
                Animation_ResetAnimation(&arr_list[j].mAnimation);
            }
        }
    }

	int64_t previewStateIndex = shgeti(_mPreview.sh_images, state);
    if (previewStateIndex < 0)
    {
        shput(_mPreview.sh_images, state, NULL);
    }

    previewStateIndex = shgeti(_mPreview.sh_images, state);

    arrsetlen(_mPreview.sh_images[previewStateIndex].value, 0);

    for (int i = 0; i < imageDatasLen; i += 1)
    {
        ImageDataInstance instance;
		ImageDataInstance_Init(&instance, &imageDatas[i]);
		arrput(_mPreview.sh_images[previewStateIndex].value, instance);
    }
}
static void PreviewImageData_DrawHud(SpriteBatch* spriteBatch)
{
	DrawTool_DrawRectangle2(spriteBatch, COLOR_CORNFLOWER_BLUE, 100, Rectangle_Create(0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT), 0, false);

	for (int i = 0; i < shlen(_mPreview.sh_images); i += 1)
    {
        ImageDataInstance* arr_list = _mPreview.sh_images[i].value;
        for (int j = 0; j < arrlen(arr_list); j += 1)
        {
            int tempX = PREVIEW_WIDTH / 2;
            int tempY = PREVIEW_HEIGHT / 2;

			Vector2 position = Vector2_Create((float)tempX, (float)tempY);
			ImageDataInstance_DrawInterpolated(&arr_list[j], spriteBatch, COLOR_WHITE, NULL, position, position, Vector2_Create2(_mScale), 0, false, false,
                arr_list[j].mData->mDepth + 100 + 1);

            if (_mPreview.mShowCollision)
            {
                float tempWidth = _mSettings.mCollisionWidth * _mScale;
                float tempHeight = _mSettings.mCollisionHeight * _mScale;
                int width = (int)tempWidth;
                int height = (int)tempHeight;
				DrawTool_DrawRectangle2(spriteBatch, _mPreview.mColor, 200, Rectangle_Create(tempX, tempY, width, height), 0, true);
            }
        }
    }
}
static void ClearPreview()
{
    PreviewImageData_Clear();
}

static void VerifyCurrentState()
{
    if (Utils_IsStringEmpty(_mCurrentState))
    {
        Utils_strlcpy(_mCurrentState, _mSettings.mDefaultState, EE_FILENAME_MAX);
    }
    if (Utils_IsStringEmpty(_mCurrentState) || !ThingSettings_HasState(&_mSettings, _mCurrentState))
    {
		if (ThingSettings_GetStateLength(&_mSettings) != 0)
        {
            Utils_strlcpy(_mCurrentState, _mSettings.sh_graphics_data[0].key, EE_FILENAME_MAX);
        }
        else
        {
            Utils_strlcpy(_mCurrentState, EE_STR_EMPTY, EE_FILENAME_MAX);
            Utils_strlcpy(_mCurrentPhase, EE_STR_EMPTY, EE_FILENAME_MAX);
        }
    }
}
static void VerifyCurrentPhase()
{
    if (Utils_IsStringEmpty(_mCurrentPhase))
    {
        Utils_strlcpy(_mCurrentPhase, _mSettings.mDefaultPhase, EE_FILENAME_MAX);
    }
    if (Utils_IsStringEmpty(_mCurrentPhase) || !ThingSettings_HasStateAndPhase(&_mSettings, _mCurrentState, _mCurrentPhase))
    {
        if (ThingSettings_GetPhaseLength(&_mSettings, _mCurrentState) != 0)
        {
            int64_t stateIndex = ThingSettings_GetStateIndex(&_mSettings, _mCurrentState);
            Utils_strlcpy(_mCurrentPhase, _mSettings.sh_graphics_data[stateIndex].value[0].key, EE_FILENAME_MAX);
        }
        else
        {
            Utils_strlcpy(_mCurrentPhase, EE_STR_EMPTY, EE_FILENAME_MAX);
        }
    }
}
static void ResetAnimation()
{
    VerifyCurrentState();
    if (!Utils_IsStringEmpty(_mCurrentState))
    {
        VerifyCurrentPhase();
    }
    if (!Utils_IsStringEmpty(_mCurrentState) && !Utils_IsStringEmpty(_mCurrentPhase))
    {
		int32_t imageDataLen = 0;
		ImageData* imageData = ThingSettings_GetImageDatas(&_mSettings, _mCurrentState, _mCurrentPhase, &imageDataLen);
        PreviewImageData_LoadArray(_mCurrentState, imageData, imageDataLen);
    }
}
static ImageData* GetCurrentImageData()
{
    if (Utils_IsStringEmpty(_mCurrentState) || Utils_IsStringEmpty(_mCurrentPhase))
    {
        _mCurrentImageIndex = 0;
        return NULL;
    }

    ThingGraphicsEntry* entry = shget(_mSettings.sh_graphics_data, _mCurrentState);
    ImageData* arr_list = shget(entry, _mCurrentPhase);
    if (_mCurrentImageIndex < arrlen(arr_list))
    {
        return &arr_list[_mCurrentImageIndex];
    }

    _mCurrentImageIndex = 0;
    return NULL;
}

static void SetupDrawManual()
{
    int buttonWidthForRemove = 50;
    int buttonHeightForRemove = 50;

    const char* defaultStateKey = OeState_DEFAULT;
    const char* defaultPhaseKey = OePhase_DEFAULT;

    Utils_strlcpy(_mSettings.mDefaultState, defaultStateKey, EE_FILENAME_MAX);
	Utils_strlcpy(_mSettings.mDefaultPhase, defaultPhaseKey, EE_FILENAME_MAX);

    if (!ThingSettings_HasState(&_mSettings, defaultStateKey))
    {
		ThingSettings_AddState(&_mSettings, defaultStateKey);
		Utils_strlcpy(_mCurrentState, defaultStateKey, EE_FILENAME_MAX);
        ResetAnimation();
    }

    if (!IsSingleStateMode())
    {
        ImGui::NewLine();
        ImGui::Text("States");
		ImGui::InputText("##Add State Input Text", _mManualAddStateString, EE_FILENAME_MAX);
		Utils_StringToUpper(_mManualAddStateString, EE_FILENAME_MAX);
        ImGui::SameLine();
        if (Button("Add##StateBUTTON"))
        {
			if (!Utils_IsStringEmpty(_mManualAddStateString))
            {
                if (!ThingSettings_HasState(&_mSettings, _mManualAddStateString))
                {
                    ThingSettings_AddState(&_mSettings, _mManualAddStateString);
					Utils_strlcpy(_mCurrentState, _mManualAddStateString, EE_FILENAME_MAX);
                    ResetAnimation();
                }
            }
        }
    }

    ImGui::NewLine();

    IStringArray* thingStates = IStringArray_CreateForJustThisFrame();
    ThingSettings_GetStates(&_mSettings, thingStates);

    if (!IsSingleStateMode())
    {
		if (ImGui::BeginListBox("##Current State", ImVec2(400, 200)))
		{
			for (int i = 0; i < IStringArray_Length(thingStates); i += 1)
			{
				const char* thingState = IStringArray_Get(thingStates, i);
				bool isSelected = Utils_StringEqualTo(thingState, _mCurrentState);
				if (ImGui::Selectable(thingState, isSelected))
				{
					Utils_strlcpy(_mCurrentState, thingState, EE_FILENAME_MAX);
                    ResetAnimation();
				}
			}
			ImGui::EndListBox();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove##STATEBUTTON", ImVec2(buttonWidthForRemove, buttonHeightForRemove)))
		{
			if (ThingSettings_HasState(&_mSettings, _mCurrentState))
			{
				ThingSettings_RemoveState(&_mSettings, _mCurrentState);
				Utils_strlcpy(_mCurrentState, EE_STR_EMPTY, EE_FILENAME_MAX);
				ClearPreview();
			}
		}
		ImGui::NewLine();
	}
	else
    {
		Utils_strlcpy(_mCurrentState, defaultStateKey, EE_FILENAME_MAX);
    }

    VerifyCurrentState();
	if (Utils_IsStringEmpty(_mCurrentState))
    {
        return;
    }

	if (!ThingSettings_HasStateAndPhase(&_mSettings, _mCurrentState, defaultPhaseKey))
    {
		ThingSettings_AddPhase(&_mSettings, _mCurrentState, defaultPhaseKey);
        ClearPreview();
        ResetAnimation();
    }

	if (Utils_IsStringEmpty(_mCurrentPhase))
    {
		Utils_strlcpy(_mCurrentPhase, IStringArray_Get(thingStates, 0), EE_FILENAME_MAX);
    }

    ImGui::Text("Phases");
    ImGui::InputText("##Add Phase Input Text", _mManualAddPhaseString, EE_FILENAME_MAX);
	Utils_StringToUpper(_mManualAddPhaseString, EE_FILENAME_MAX);
    ImGui::SameLine();
	if (Button("Add##PhaseBUTTON"))
	{
		if (!Utils_IsStringEmpty(_mManualAddPhaseString))
		{
			const char* keyOne = _mCurrentState;
			const char* keyTwo = _mManualAddPhaseString;
			if (!Utils_IsStringEmpty(keyOne) && !Utils_IsStringEmpty(keyTwo))
			{
                if (!ThingSettings_HasStateAndPhase(&_mSettings, keyOne, keyTwo))
                {
					ThingSettings_AddPhase(&_mSettings, keyOne, keyTwo);
					Utils_strlcpy(_mCurrentPhase, keyTwo, EE_FILENAME_MAX);
                    ClearPreview();
                    ResetAnimation();
                }
            }
        }
    }
    ImGui::NewLine();

	IStringArray* thingPhases = IStringArray_CreateForJustThisFrame();
	ThingSettings_GetPhases(&_mSettings, thingPhases, _mCurrentState);
    if (ImGui::BeginListBox("##Current Phase", ImVec2(400, 200)))
    {
        for (int i = 0; i < IStringArray_Length(thingPhases); i += 1)
        {
			const char* thingPhase = IStringArray_Get(thingPhases, i);
			bool isSelected = Utils_StringEqualTo(thingPhase, _mCurrentPhase);
            if (ImGui::Selectable(thingPhase, isSelected))
            {
				Utils_strlcpy(_mCurrentPhase, thingPhase, EE_FILENAME_MAX);
                ResetAnimation();
            }
        }
		ImGui::EndListBox();
    }
    ImGui::SameLine();
	if (ImGui::Button("Remove##PHASEBUTTON", ImVec2(buttonWidthForRemove, buttonHeightForRemove)))
    {
        const char* keyOne = _mCurrentState;
        const char* keyTwo = _mCurrentPhase;
		if (!Utils_IsStringEmpty(keyOne) && !Utils_IsStringEmpty(keyTwo))
        {
            if (ThingSettings_HasStateAndPhase(&_mSettings, keyOne, keyTwo))
            {
				ThingSettings_RemovePhase(&_mSettings, keyOne, keyTwo);
				Utils_strlcpy(_mCurrentPhase, EE_STR_EMPTY, EE_FILENAME_MAX);
                ClearPreview();
            }
        }
    }
    ImGui::NewLine();

    VerifyCurrentPhase();
	if (Utils_IsStringEmpty(_mCurrentPhase))
    {
        return;
    }

    ImGui::Text("Images");
	int32_t imageDatasLen = 0;
	ImageData* imageDatas = ThingSettings_GetImageDatas(&_mSettings, _mCurrentState, _mCurrentPhase, &imageDatasLen);
	IStringArray* listOfSheetNames = IStringArray_CreateForJustThisFrame();
	Sheet_GetListOfSheetNames(listOfSheetNames);
    if (ImGui::BeginCombo("##Textures", _mCurrentImage))
    {
		for (int i = 0; i < IStringArray_Length(listOfSheetNames); i += 1)
        {
			const char* sheetName = IStringArray_Get(listOfSheetNames, i);
			bool isSelected = Utils_StringEqualTo(sheetName, _mCurrentImage);
            if (ImGui::Selectable(sheetName, isSelected))
            {
				Utils_strlcpy(_mCurrentImage, sheetName, EE_FILENAME_MAX);
            }
        }
		ImGui::EndCombo();
    }
    ImGui::SameLine();

    if (Button("Add##ImageBUTTON"))
    {
		if (!Utils_IsStringEmpty(_mCurrentImage))
        {
            ImageData data;
			ImageData_Init(&data);
			Sheet* sheet = Sheet_GetSheet(_mCurrentImage);
			Utils_strlcpy(data.mImage, sheet->mSheetName, EE_FILENAME_MAX);
			ThingSettings_AddImageData(&_mSettings, _mCurrentState, _mCurrentPhase, data);
            ResetAnimation();
			_mCurrentImageIndex = imageDatasLen;
        }
    }

    IStringArray* imageNames = IStringArray_CreateForJustThisFrame();
	ThingSettings_GetImages(&_mSettings, imageNames, _mCurrentState, _mCurrentPhase);

    bool wasImageSelected = false;
    const char* firstImageName = NULL;
	if (ImGui::BeginListBox("##ImageTextureBox", ImVec2(400, 200)))
    {
        for (int i = 0; i < IStringArray_Length(imageNames); i += 1)
        {
			const char* imageName = IStringArray_Get(imageNames, i);
            if (i == 0)
            {
				firstImageName = imageName;
            }
			bool isSelected = Utils_StringEqualTo(imageName, _mCurrentImage);
            if (isSelected)
            {
                wasImageSelected = true;
            }
            if (ImGui::Selectable(imageName, isSelected))
            {
                wasImageSelected = true;
                Utils_strlcpy(_mCurrentImage, imageName, EE_FILENAME_MAX);
				_mCurrentImageIndex = i;
            }
        }
		ImGui::EndListBox();
    }
    if (firstImageName != NULL)
    {
		if (Utils_IsStringEmpty(_mCurrentImage) || !wasImageSelected)
        {
            Utils_strlcpy(_mCurrentImage, firstImageName, EE_FILENAME_MAX);
            _mCurrentImageIndex = 0;
        }
    }

    ImageData* imageData = GetCurrentImageData();
	if (imageData == NULL)
    {
        return;
    }

    ImGui::SameLine();
	if (ImGui::Button("Remove##ImageBOX", ImVec2(buttonWidthForRemove, buttonHeightForRemove)))
    {
		ThingSettings_RemoveImageData(&_mSettings, _mCurrentState, _mCurrentPhase, _mCurrentImageIndex);
        ResetAnimation();
        return;
    }

    if (Button("Show In Folder"))
    {
        /*string image = imageList[listDrawStr.mRealIndex].mImage;
        OeSheet sheet = OeSheet.GetSheet(image);
        ShowImage(sheet);*/
    }
}
static void SetupDraw()
{
    if (ImGui::Checkbox("Has Draw?", &_mSettings.mHasDrawing))
    {
        ClearPreview();
    }

    if (!_mSettings.mHasDrawing)
    {
        return;
    }


    EditorGlobals_CvarCheckbox("Single State Mode?", CVARS_EDITOR_IS_SINGLE_STATE_MODE);

    if (_mShowAdvancedStuff)
    {
        ImGui::Checkbox("Do Not Dispose?", &_mSettings.mDoNotDispose);
    }
    if (Button("Clear Preview"))
    {
        ClearPreview();
    }

    SetupDrawManual();
}

static void ShowImage(Sheet* sheet)
{
    /*string platform = SDL2.SDL.SDL_GetPlatform();
    if (platform.Equals("Windows"))
    {
        Process.Start("explorer.exe", "/select, " + OeSheet.GetTextureResource(ref sheet).GetPath());
    }*/
}
static void SetupAi()
{
    ImGui::Checkbox("Has Ai?", &_mSettings.mHasAI);

    if (!_mSettings.mHasAI)
    {
        return;
    }

    /*
    List<int> controllerComponentMap = (List<int>)OeGame.GetHelper().GetControllerComponentTypeMap().Keys;
    string controllerStr = null;
    int indexOfController = controllerComponentMap.IndexOf(_mSettings.mRoutineId);
    List<string> controllerStringArray = OeGame.GetHelper().GetControllerComponentStringArray();
    if (indexOfController >= 0 && indexOfController < controllerStringArray.Count)
    {
        controllerStr = controllerStringArray[indexOfController];
    }
    OeGuiListData listControllers;
    if (ImGui::ComboBox("Controllers", controllerStringArray, controllerStr, out listControllers))
    {
        int selectedIndex = listControllers.mRealIndex;
        _mSettings.mRoutineId = controllerComponentMap[selectedIndex];
    }*/
}
static void SetupCollision()
{
    ImGui::Checkbox("Has Collision?", &_mSettings.mHasCollision);

    if (!_mSettings.mHasCollision)
    {
        return;
    }

    ImGui::InputInt("Width", &_mSettings.mCollisionWidth);
    ImGui::InputInt("Height", &_mSettings.mCollisionHeight);
}
static void ThingEditorSave()
{
    if (ImGui::IsWindowFocused())
    {
        /*if (!_mTempFileName.Equals(OeUtils.NOT_SET))
        {
            string path = OeFile.Combine(OeThingSettings.THINGS_DIRECTORY[0], _mTempFileName + OeUtils.EXTENSION_INI);
            OeResource<OeThingSettings> resource = OeResourceManagers.ThingSettingsManager.CreateResource(path, _mSettings);
            resource.SaveAsIni();
            Do_PlaySound("editorSave", 1f);
        }*/
    }
}
static void SetupImageData()
{
    ImageData* imageData = GetCurrentImageData();
    if (imageData == NULL)
    {
        return;
    }

    ImGui::InputInt("Offset X", &imageData->mOffset.X);
    ImGui::InputInt("Offset Y", &imageData->mOffset.Y);
    if (_mShowAdvancedStuff)
    {
        ImGui::InputFloat("Origin X", &imageData->mOrigin.X);
        ImGui::InputFloat("Origin Y", &imageData->mOrigin.Y);
    }
    ImGui::InputInt("Depth", &imageData->mDepth);
    if (_mShowAdvancedStuff)
    {
        if (ImGui::InputFloat("Rotation", &imageData->mInitialRotation, 90))
        {
            imageData->mInitialRotation = Math_ClampFloat(imageData->mInitialRotation, 0, 360);
        }
        if (ImGui::InputInt("Scaler", &imageData->mScaler))
        {
            imageData->mScaler = Math_MaxInt(imageData->mScaler, 1);
        }
    }
    if (ImGui::Checkbox("Is Additive", &imageData->mIsAdditive))
    {
        ResetAnimation();
    }
    if (ImGui::Checkbox("Is Animation?", &imageData->mCanAnimate))
    {
        ResetAnimation();
    }

    if (!imageData->mCanAnimate)
    {
        return;
    }

    if (ImGui::InputInt("Flip Speed (Frames)", &imageData->mFlipSpeed))
    {
        ResetAnimation();
    }
    if (ImGui::InputInt("Loop Point", &imageData->mAnimationLoopPoint))
    {
        ResetAnimation();
    }
    if (ImGui::Checkbox("Animation Blanks?", &imageData->mAnimationBlanks))
    {
        ResetAnimation();
    }
    if (ImGui::Checkbox("Animation Stop?", &imageData->mAnimationStops))
    {
        ResetAnimation();
    }
}
static void SetupThingPreview()
{
    ImGui::Text("Preview Options");
    if (ImGui::BeginCombo("Preview Scale", Utils_FloatToStringGlobalBuffer(_mScale)))
    {
        for (int i = 0; i < POSSIBLE_SCALES_LEN; i += 1)
        {
            float tempScale = _mPossibleScales[i];
            bool isSelected = (tempScale == _mScale);
            if (ImGui::Selectable(Utils_FloatToStringGlobalBuffer(tempScale), &isSelected))
            {
                _mScale = tempScale;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Checkbox("Show Hitbox?", &_mPreview.mShowCollision);
    if (Button("Reset Animation"))
    {
        ResetAnimation();
    }
}
static void CreateThingEditor()
{
    // 1600, 900
    if (!ImGui::Begin(KEY_WINDOW_THING_EDITOR))
    {
        ImGui::End();
        return;
    }

    if (Globals_DebugEditorJustReloadedGraphics())
    {
        ResetAnimation();
    }

    ImGui::Columns(3);

	ImGui::Checkbox("Show Advanced Stuff?", &_mShowAdvancedStuff);

    if (Button("Save (Ctrl-S)"))
    {
        ThingEditorSave();
    }
    if (EditorGlobals_IsKeyCtrlPressedAndKeySTapped())
    {
        ThingEditorSave();
    }
    if (Button("Create New"))
    {
		Utils_memset(&_mSettings, 0, sizeof(ThingSettings));
		ThingSettings_Init(&_mSettings);
        ClearPreview();
    }
	if (ImGui::InputText("Name", _mTempFileName, EE_FILENAME_MAX))
    {
		Utils_StringToUpper(_mTempFileName, EE_FILENAME_MAX);
    }

	IStringArray* listOfSheetNames = IStringArray_CreateForJustThisFrame();
	Sheet_GetListOfSheetNames(listOfSheetNames);
    if (ImGui::BeginCombo("Editor Texture", _mSettings.mPreviewSheet))
    {
        for (int i = 0; i < IStringArray_Length(listOfSheetNames); i += 1)
        {
			const char* sheetName = IStringArray_Get(listOfSheetNames, i);
			bool isSelected = Utils_StringEqualTo(sheetName, _mSettings.mPreviewSheet);
            if (ImGui::Selectable(sheetName, isSelected))
            {
				Utils_strlcpy(_mSettings.mPreviewSheet, sheetName, EE_FILENAME_MAX);
            }
        }
		ImGui::EndCombo();
    }
    if (Button("Remove"))
    {
		Utils_strlcpy(_mSettings.mPreviewSheet, EE_STR_NOT_SET, EE_FILENAME_MAX);
    }

	ResourceMan* rmThing = ResourceList_ThingSettings();
	IStringArray* thingsFilenames = IStringArray_CreateForJustThisFrame();
	ResourceMan_FillArrayWithAllResourceNames(rmThing, thingsFilenames);
    bool reset = false;
    ImGui::Text("Thing Settings");
    if (ImGui::BeginListBox("##Thing Settings", ImVec2(400, 600)))
    {
        for (int i = 0; i < IStringArray_Length(thingsFilenames); i += 1)
        {
			const char* thingFilename = IStringArray_Get(thingsFilenames, i);
			bool isSelected = Utils_StringEqualTo(thingFilename, _mTempFileName);
            if (ImGui::Selectable(thingFilename, isSelected))
            {
                Resource* res = ResourceMan_GetResourceByIndex(rmThing, i);
				Utils_strlcpy(_mTempFileName, Resource_GetName(res), EE_FILENAME_MAX);
				_mSettings = *(ThingSettings*)Resource_GetData(res);
				Utils_strlcpy(_mCurrentState, _mSettings.mDefaultState, EE_FILENAME_MAX);
                Utils_strlcpy(_mCurrentPhase, _mSettings.mDefaultPhase, EE_FILENAME_MAX);
                _mCurrentImageIndex = 0;
                reset = true;
                //ImGui::ClearUnusedDataForWindow();
                ClearPreview();
            }
        }
		ImGui::EndListBox();
    }

    SetupAi();

    SetupCollision();

    ImGui::NextColumn();

    SetupDraw();

    ImGui::NextColumn();

    if (_mSettings.mHasDrawing)
    {
        SetupThingPreview();

        if (reset)
        {
            ResetAnimation();
        }

        PreviewImageData_Update();

		Texture* thingTexture = Renderer_RenderToTexture(512, 512, COLOR_CORNFLOWER_BLUE, PreviewImageData_DrawHud);
		ImGui::Image((ImTextureID)(intptr_t)thingTexture->mTextureData, ImVec2(512, 512));

        SetupImageData();
    }

    ImGui::End();
}

void ThingEditor_Update()
{
    _mPreview.mColor = Color_Create4(255, 255, 255, 127);

    CreateThingEditor();
}