#include "AnimMan.h"

#include "imgui.h"
#include "../utils/Utils.h"
#include "../io/File.h"
#include "../utils/MString.h"
#include "../leveldata/AnimTile.h"
#include "EditorGlobals.h"
#include "../utils/Logger.h"
#include "../render/Texture.h"
#include "../math/Math.h"
#include "../resources/ResourceList.h"
#include "../core/Func.h"

static char _mTempFileName[EE_FILENAME_MAX];
static AnimTile _mAnimTile;

static void ReloadAnimation()
{
	AnimTile_LoadAnimation(&_mAnimTile);
}
static void Save()
{
	if (!ImGui::IsWindowFocused() || Utils_StringEqualTo(_mTempFileName, EE_STR_NOT_SET))
	{
		return;
	}

	ResourceMan* rmAnim = ResourceList_AnimTile();

	MString* path = MString_CreateForJustThisFrame();
	File_PathCombine2(&path, rmAnim->_mDirectories[0], _mTempFileName);
	MString_AddAssignString(&path, ".txt");

	MString* strDebugInfo = MString_CreateForJustThisFrame();
	if (ResourceMan_HasResource(rmAnim, _mTempFileName))
	{
		MString_Combine2(&strDebugInfo, "Overwriting anim tile ", MString_Text(path));
	}
	else
	{
		MString_Combine2(&strDebugInfo, "Saving new anim tile ", MString_Text(path));
	}
	Logger_LogInformation(MString_Text(strDebugInfo));

	Resource* resource = ResourceMan_CreateResourceIfMissing(rmAnim, _mTempFileName, MString_Text(path), true);
	AnimTile* resourceData = (AnimTile*)Resource_PeekData(resource);
	*resourceData = _mAnimTile;
	Resource_Save(resource, true);
	Do_PlaySound2("editorSave", 1.0f);
}
static void CreateWindow()
{
	ImGui::SetNextWindowSize(ImVec2(1000, 720));
	if (!ImGui::Begin(KEY_WINDOW_ANIM_MAN))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(2);
	if (ImGui::Button("Save (Ctrl-S)", ImVec2(100, 25)))
	{
		Save();
	}
	if (EditorGlobals_IsKeyCtrlPressedAndKeySTapped())
	{
		Save();
	}

	if (ImGui::Button("Setup New", ImVec2(100, 25)))
	{
		Logger_LogInformation("New anim tile setup");
		Utils_memset(&_mAnimTile, 0, sizeof(AnimTile));
		Utils_strlcpy(_mTempFileName, EE_STR_NOT_SET, EE_FILENAME_MAX);
	}

	ImGui::InputText("Name", _mTempFileName, EE_FILENAME_MAX);
	ResourceMan* rmAnim = ResourceList_AnimTile();

	{
		IStringArray* sa = IStringArray_CreateForJustThisFrame();
		ResourceMan_FillArrayWithAllResourceNames(rmAnim, sa);
		if (ImGui::BeginListBox("Anim Tiles", ImVec2(500, 800)))
		{
			for (int i = 0; i < IStringArray_Length(sa); i += 1)
			{
				const char* filename = IStringArray_Get(sa, i);
				bool isSelected = Utils_StringEqualTo(_mTempFileName, filename) ? true : false;
				if (ImGui::Selectable(filename, isSelected))
				{
					Resource* resAnim = ResourceMan_GetResourceByIndex(rmAnim, i);
					Utils_strlcpy(_mTempFileName, Resource_GetName(resAnim), EE_FILENAME_MAX);
					AnimTile* resourceData = (AnimTile*)Resource_GetData(resAnim);
					_mAnimTile = *resourceData;
				}
			}
			ImGui::EndListBox();
		}
	}

	ImGui::NextColumn();

	if (ImGui::InputInt("Flip Speed", &_mAnimTile.mFlipSpeed))
	{
		AnimTile_LoadAnimation(&_mAnimTile);
	}
	if (ImGui::InputFloat("Rotation", &_mAnimTile.mRotation, 90))
	{
		_mAnimTile.mRotation = Math_ClampFloat(_mAnimTile.mRotation, 0, 360);
		ReloadAnimation();
	}
	if (ImGui::Checkbox("Flip X?", &_mAnimTile.mIsFlipX))
	{
		ReloadAnimation();
	}
	if (ImGui::Checkbox("Flip Y?", &_mAnimTile.mIsFlipY))
	{
		ReloadAnimation();
	}

	IStringArray* listOfSheetNames = IStringArray_CreateForJustThisFrame();
	Sheet_GetListOfSheetNames(listOfSheetNames);
	if (ImGui::BeginCombo("Textures", _mAnimTile.mTextureName))
	{
		for (int i = 0; i < IStringArray_Length(listOfSheetNames); i += 1)
		{
			const char* sheetName = IStringArray_Get(listOfSheetNames, i);
			if (ImGui::Selectable(sheetName))
			{
				Utils_strlcpy(_mAnimTile.mTextureName, sheetName, EE_FILENAME_MAX);
				ReloadAnimation();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("Is Additive?", &_mAnimTile.mIsAdditive))
	{
		ReloadAnimation();
	}
	if (ImGui::InputInt("Scaler", &_mAnimTile.mScaler))
	{
		_mAnimTile.mScaler = Math_MaxInt(_mAnimTile.mScaler, 1);
	}

	Sheet* animSheet = AnimTile_GetAnimationSheet(&_mAnimTile);
	Texture* animTexture = Sheet_GetTexture(animSheet);
	ImGui::Image((ImTextureID)(intptr_t)animTexture->mTextureData, ImVec2((float)animTexture->mBounds.Width, (float)animTexture->mBounds.Height));

	ImGui::End();

	AnimTile_UpdateResource(&_mAnimTile);
}
void AnimMan_Update()
{
	CreateWindow();
}
