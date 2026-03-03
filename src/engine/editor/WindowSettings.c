#include "WindowSettings.h"

#include "imgui.h"
#include "Editor.h"
#include "../utils/Macros.h"
#include "../math/Point.h"
#include "../leveldata/LayerData.h"
#include "../leveldata/LevelData.h"
#include "../core/Func.h"
#include "../resources/ResourceList.h"
#include "../gamestate/GameStateManager.h"
#include "../utils/Utils.h"
#include "../io/File.h"
#include "../math/Math.h"

#define KEY_SETTINGS_CB_LAYER_SELECT "Layer Select"
#define KEY_SETTINGS_CB_FILE_NAMES "Level File Names"
#define KEY_SETTINGS_CB_RECORDING_NAMES "Recording Names"

static LayerData _mTempLayerData[LEVELDATA_LAYER_DATA_LENGTH];
static char _mTempFileName[EE_FILENAME_MAX];
static bool _mTempIsMetaMap;
static Point _mAseImportOffset;
static int _mCurrentLayerSelected;
static bool _mIsActive;

static bool DoesCurrentFilenameExistAsResource()
 {
	 return ResourceMan_HasResource(ResourceList_LevelData(), _mTempFileName);
 }
 static void CreateSettingsWindow()
{
    if (!ImGui::Begin(EDITOR_KEY_WINDOW_SETTINGS, &_mIsActive))
    {
		ImGui::End();
        return;
    }

    LevelData* levelData = Get_LevelData();

    {
        MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine2(&tempString, "Currently Loaded Level File Name: ", Resource_GetName(Get_LevelDataResource()));
		ImGui::Text(MString_Text(tempString));
    }

    {
		IStringArray* levelFilenames = IStringArray_CreateForJustThisFrame();
		ResourceMan_FillArrayWithAllResourceNames(ResourceList_LevelData(), levelFilenames);
        if (ImGui::BeginCombo(KEY_SETTINGS_CB_FILE_NAMES, _mTempFileName))
        {
            for (int i = 0; i < IStringArray_Length(levelFilenames); i += 1)
            {
                const char* levelFilename = IStringArray_Get(levelFilenames, i);
                bool isSelected = Utils_StringEqualTo(levelFilename, _mTempFileName);
                if (ImGui::Selectable(levelFilename, isSelected))
                {
					Utils_strlcpy(_mTempFileName, levelFilename, EE_FILENAME_MAX);
                }
            }
			ImGui::EndCombo();
        }
    }

	if (ImGui::Button("Save (F9)", ImVec2(100, 25)))
    {
        Do_PlaySound2("editorSave", 1.0f);
		Resource_Save(Get_LevelDataResource(), true);
	}

	if (ImGui::Button("Load", ImVec2(100, 25)) && !Utils_IsStringEmptyOrNotSet(_mTempFileName))
	{
		GameStateManager_DebugForceLoadMapNow(_mTempFileName);
	}

	ImGui::InputText("Level File Name", _mTempFileName, EE_FILENAME_MAX);

	ImGui::Checkbox("Is Meta Map?", &_mTempIsMetaMap);

	if (ImGui::Button("Create New Level And Load", ImVec2(200, 25)) && !Utils_IsStringEmptyOrNotSet(_mTempFileName))
	{
        ResourceMan* rmLevel = ResourceList_LevelData();
        if (!ResourceMan_HasResource(rmLevel, _mTempFileName))
        {
            MString* path = MString_CreateForJustThisFrame();
			File_PathCombine2(&path, rmLevel->_mDirectories[0], _mTempFileName);
            MString_AddAssignString(&path, ".txt");
            Resource* resource = ResourceMan_CreateResource(rmLevel, _mTempFileName, MString_Text(path), true);
            if (_mTempIsMetaMap)
            {
                //TODO resource.GetData().SetupAsMetaMap();
            }
            Resource_Save(resource, true);
            GameStateManager_DebugForceLoadMapNow(_mTempFileName);
            _mTempIsMetaMap = false;
        }
	}

    //OeGuiListData listRecordings; //NOT A THING ANYMORE
    //ImGui::ComboBox(KEY_SETTINGS_CB_RECORDING_NAMES, NAMES_RECORDINGS, null, out listRecordings);
    //
    //if (ImGui::Button("Load Recording", 100, 25))
    //{
    //   string recording = NAMES_RECORDINGS[listRecordings.mRealIndex];
    //    if (recording != null)
    //    {
    //        OeGameStateManager.SetupLoadMapWithRecording(recording);
    //        OeGameStateManager.SetGameState(OeGameStateManager.GAME_STATE_NORMAL);
    //    }
    //}*/

	ImGui::InputText("Level Name", levelData->mLevelName, EE_FILENAME_MAX);

    if (ImGui::BeginCombo("Tileset", levelData->mTilesetName))
    {
        IStringArray* tilesetFilenames = IStringArray_CreateForJustThisFrame();
		MString* pathToTilesets = MString_CreateForJustThisFrame();
		File_PathCombine3(&pathToTilesets, "data", "gfx", "tilesets");
		File_GetFilenames(tilesetFilenames, MString_Text(pathToTilesets), "*.png", true);
        for (int i = 0; i < IStringArray_Length(tilesetFilenames); i += 1)
        {
			const char* tilesetFilename = IStringArray_Get(tilesetFilenames, i);
			bool isSelected = Utils_StringEqualTo(levelData->mTilesetName, tilesetFilename);
            if (ImGui::Selectable(tilesetFilename, isSelected))
            {
				Utils_strlcpy(levelData->mTilesetName, tilesetFilename, EE_FILENAME_MAX);
            }
        }
		ImGui::EndCombo();
    }

	MString* comboPreview = MString_CreateForJustThisFrame();
	MString_AddAssignInt(&comboPreview, _mCurrentLayerSelected);
	if (ImGui::BeginCombo(KEY_SETTINGS_CB_LAYER_SELECT, MString_Text(comboPreview)))
    {
        for (int i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
        {
			bool isSelected = (i == _mCurrentLayerSelected);
            MString* comboSelection = MString_CreateForJustThisFrame();
            MString_AddAssignInt(&comboSelection, i);
			if (ImGui::Selectable(MString_Text(comboSelection), isSelected))
            {
				_mCurrentLayerSelected = i;
            }
        }
    }

    if (ImGui::InputInt("Layer Depth", &levelData->mLayerData[_mCurrentLayerSelected].mDepth))
    {
		levelData->mLayerData[_mCurrentLayerSelected].mDepth = Math_ClampInt(levelData->mLayerData[_mCurrentLayerSelected].mDepth, 0, 100);
    }

	ImGui::InputFloat("Layer Speed X", &levelData->mLayerData[_mCurrentLayerSelected].mSpeedX);
	ImGui::InputFloat("Layer Speed Y", &levelData->mLayerData[_mCurrentLayerSelected].mSpeedY);

	if (ImGui::Button("Copy LayerData", ImVec2(125, 25)))
    {
        for (int i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
        {
            _mTempLayerData[i] = levelData->mLayerData[i];
        }
    }
    ImGui::SameLine();
	if (ImGui::Button("Paste LayerData", ImVec2(125, 25)))
    {
        for (int i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
        {
            levelData->mLayerData[i] = _mTempLayerData[i];
        }
    }
    if (ImGui::CollapsingHeader("STRING DATA"))
    {
        for (int i = 0; i < LEVELDATA_STRING_DATA_LENGTH; i += 1)
        {
			MString* tempLabel = MString_CreateForJustThisFrame();
			MString_AddAssignInt(&tempLabel, i);
			MString_AddAssignString(&tempLabel, " : ");
            MString_AddAssignString(&tempLabel, Editor_GetEditorDataName(i));
			ImGui::InputText(MString_Text(tempLabel), levelData->mStringData[i], EE_FILENAME_MAX);
        }
    }

    /*
    {
        string pathToAseFile = Path.Combine("data", "ase_lvl", Get_LevelFileName() + ".ase");
        string labelImportFailure = "labelImportFailure";
        string labelExportFailure = "labelExportFailure";
        string labelExportComplete = "labelExportComplete";

        ImGui::InputInt("Import Offset X", ref _mAseImportOffset.X);
        ImGui::InputInt("Import Offset Y", ref _mAseImportOffset.Y);
        if (ImGui::Button("IMPORT ASE", 125, 25))
        {
            if (!OeAseTool.ImportTilemap(pathToAseFile, Get_LevelData(), _mAseImportOffset.X, _mAseImportOffset.Y))
            {
                ImGui::OpenPopup(labelImportFailure);
            }
        }

        //if (ImGui::CollapsingHeader("WARNING! DESTRUCTIVE! EXPORT ASE!"))
        //{
        //    if (ImGui::Button("EXPORT ASE!!!", 300, 25))
        //    {
        //        if (!OeAseTool.ExportTilemap(pathToAseFile, Get_LevelData()))
        //        {
        //            ImGui::OpenPopup(labelExportFailure);
        //        }
        //        else
        //        {
        //            ImGui::OpenPopup(labelExportComplete);
        //        }
        //    }
		//}

        //ImGui::MessageModal(labelExportFailure, "Unable to export ASE file! File missing? " + pathToAseFile);
        //ImGui::MessageModal(labelImportFailure, "Unable to import ASE file! File missing? " + pathToAseFile);
        //ImGui::MessageModal(labelExportComplete, "Ase Export Complete!");
    }
    */

    ImGui::End();
}

void WindowSettings_Update()
{
    if (!_mIsActive)
    {
		return;
    }

    CreateSettingsWindow();
}
void WindowSettings_Activate()
{
	_mIsActive = true;
}
