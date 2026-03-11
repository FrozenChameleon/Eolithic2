#include "PropMan.h"

#include "imgui.h"
#include "EditorGlobals.h"
#include "../utils/Macros.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "../leveldata/Prop.h"
#include "../math/Math.h"
#include "../resources/ResourceList.h"
#include "../render/Texture.h"
#include "../io/File.h"
#include "../core/Func.h"

static bool _mIsActive;
static char _mTempFileName[EE_FILENAME_MAX];
static Prop _mProp;

static void PropDudeSave()
{
	if (!ImGui::IsWindowFocused())
	{
		return;
	}

	ResourceMan_CopyToResourceDataAndThenSaveAsText(ResourceList_Prop(), _mTempFileName, NULL, &_mProp);
}
static void CreatePropDudeWindow()
{
	//ImGui::SetNextWindowSize(ImVec2(1000, 720));
	if (!ImGui::Begin(KEY_WINDOW_PROP_DUDE, &_mIsActive))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(2);

	if (ImGui::Button("Save (Ctrl-S)", ImVec2(100, 25)))
	{
		PropDudeSave();
	}
	if (EditorGlobals_IsKeyCtrlPressedAndKeySTapped())
	{
		PropDudeSave();
	}

	if (ImGui::Button("Setup New", ImVec2(100, 25)))
	{
		Logger_Log(LOGGER_INFORMATION, "New prop setup");
		Utils_strlcpy(_mTempFileName, EE_STR_NOT_SET, EE_FILENAME_MAX);
		Prop_Init(&_mProp);
	}

	ImGui::InputText("Name", _mTempFileName, EE_FILENAME_MAX);

	ResourceMan* rmProp = ResourceList_Prop();
	IStringArray* filenames = IStringArray_CreateForJustThisFrame();
	ResourceMan_FillArrayWithAllResourceNames(rmProp, filenames);
	if(ImGui::BeginListBox("Props", ImVec2(500, 500)))
	{
		for (int i = 0; i < IStringArray_Length(filenames); i += 1)
		{
			const char* filename = IStringArray_Get(filenames, i);
			bool isSelected = Utils_StringEqualTo(filename, _mTempFileName);
			if (ImGui::Selectable(filename, isSelected))
			{
				Resource* resource = ResourceMan_GetResourceByIndex(rmProp, i);
				Utils_strlcpy(_mTempFileName, Resource_GetName(resource), EE_FILENAME_MAX);
				Prop* resourceData = (Prop*)Resource_GetData(resource);
				_mProp = *resourceData;
				Prop_LoadDrawing(&_mProp);
			}
		}
		ImGui::EndListBox();
	}

	ImGui::NextColumn();

	if (ImGui::Checkbox("Is Animation?", &_mProp.mIsAnimation))
	{
		Prop_LoadDrawing(&_mProp);
	}
	if (ImGui::InputInt("Flip Speed", &_mProp.mFlipSpeed))
	{
		Prop_LoadDrawing(&_mProp);
	}

	IStringArray* listOfSheetNames = IStringArray_CreateForJustThisFrame();
	Sheet_GetListOfSheetNames(listOfSheetNames);
	if (ImGui::BeginCombo("Textures", _mProp.mTextureName))
	{
		for (int i = 0; i < IStringArray_Length(listOfSheetNames); i += 1)
		{
			const char* sheetName = IStringArray_Get(listOfSheetNames, i);
			bool isSelected = Utils_StringEqualTo(_mProp.mTextureName, sheetName);
			if (ImGui::Selectable(sheetName, isSelected))
			{
				Sheet* selectedSheet = Sheet_GetSheet(sheetName);
				Utils_strlcpy(_mProp.mTextureName, selectedSheet->mSheetName, EE_FILENAME_MAX);
				Prop_LoadDrawing(&_mProp);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("Is Additive?", & _mProp.mIsAdditive))
	{
		Prop_LoadDrawing(&_mProp);
	}

	if (ImGui::InputInt("Scaler", &_mProp.mScaler))
	{
		_mProp.mScaler = Math_MaxInt(_mProp.mScaler, 1);
	}

	Sheet* propSheet = Prop_GetSheet(&_mProp);
	Texture* propTexture = Sheet_GetTexture(propSheet);
	ImGui::Image((ImTextureID)(intptr_t)propTexture->mTextureData, ImVec2((float)propTexture->mBounds.Width, (float)propTexture->mBounds.Height));

	ImGui::End();

	Prop_UpdateResource(&_mProp);
}

void PropMan_Update()
{
	if (!_mIsActive)
	{
		return;
	}

	CreatePropDudeWindow();
}
void PropMan_Activate()
{
	_mIsActive = true;
}
