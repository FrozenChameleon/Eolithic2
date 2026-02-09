/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameHelper.h"

#include "../gamestate/GameStateManager.h"
#include "../systems/System.h"
#include "../input/InputBindings.h"
#include "../input/ActionList.h"
#include "../input/Keys.h"
#include "../input/Buttons.h"
#include "../leveldata/ThingInstance.h"
#include "../systems/GlobalSysDrawFPS.h"
#include "../systems/GlobalSysHideMouse.h"
#include "../systems/GlobalSysHandleGameStateChange.h"
#include "../systems/GlobalSysHandleLoadNextMap.h"
#include "../systems/GlobalSysUpdateManagers.h"
#include "../systems/GlobalSysTickSoundEffect.h"
#include "../systems/GlobalSysTickMusic.h"
#include "../systems/GlobalSysUpdateInput.h"
#include "../systems/GlobalSysDefaultState.h"
#include "../systems/GlobalSysUpdateMapLoadTicker.h"
#include "../systems/GlobalSysSetJustChangedGameStateToFalse.h"
#include "../systems/GlobalSysSaveIcon.h"
#include "../systems/GlobalSysUpdateAchievementHandler.h"
#include "../gamestate/GameState.h"
#include "../core/Func.h"
#include "../components/LevelDataStubFunc.h"
#include "../components/Camera.h"
#include "../../engine/io/File.h"
#include "../../engine/utils/Utils.h"
#include "../systems/GlobalSysDrawQuickStats.h"
#include "../input/Axes.h"

 //static struct { int32_t key; int32_t value; } _mControllerComponentTypeMap; //UNUSED
//static IStringArray* _mControllerComponentStrArray; //UNUSED
//static std_unordered_map<std_string, std_unordered_map<std_string, int32_t>> _mComponentSizes; //UNUSED

const float GAMEHELPER_DEFAULT_GRAVITY_DECELERATION = 0.233f;
const float GAMEHELPER_DEFAULT_GRAVITY_DECELERATION_MAX = 5.25f;
const int32_t GAMEHELPER_DEFAULT_MOVING_PLATFORM_LEEWAY = 1;

int32_t GAMEHELPER_PLATFORM_UP = -1;
int32_t GAMEHELPER_PLATFORM_DOWN = -1;
int32_t GAMEHELPER_PLATFORM_RIGHT = -1;
int32_t GAMEHELPER_PLATFORM_LEFT = -1;

Resource* GameHelper_GetDefaultLevelDataResource(void)
{
	LevelDataStub* stub = (LevelDataStub*)GameState_GetFirstSetComponent(Get_ActiveGameState(), C_LevelDataStub);
	return LevelDataStubFunc_GetLevelDataResource(stub);
}
LevelData* GameHelper_GetDefaultLevelData(void)
{
	LevelDataStub* stub = (LevelDataStub*)GameState_GetFirstSetComponent(Get_ActiveGameState(), C_LevelDataStub);
	return LevelDataStubFunc_GetLevelData(stub);
}
void GameHelper_UpdateLastRenderPositionNormally(void)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	GameState* gameState = GameStateManager_GetGameState();

	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mUpdateLastRenderPosition(stateSys->_mData, gameState);
	}
}
Camera* GameHelper_GetDefaultCameraForRender(void)
{
	return (Camera*)GameStateManager_GetFirstSetComponent(C_Camera);
}
float GameHelper_GetDefaultPostGameBrightness(void)
{
	return 1;
}
void GameHelper_UpdateGlobalSystemsNormally(void)
{
	int32_t globalSystemsLen = GameStateManager_GetGlobalSystemsLength();
	System** globalSystems = GameStateManager_GetGlobalSystems();
	for (int32_t i = 0; i < globalSystemsLen; i += 1)
	{
		System* globalSys = globalSystems[i];
		globalSys->_mUpdate(globalSys->_mData);
	}
}
void GameHelper_DrawGlobalSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t globalSystemsLen = GameStateManager_GetGlobalSystemsLength();
	System** globalSystems = GameStateManager_GetGlobalSystems();
	for (int32_t i = 0; i < globalSystemsLen; i += 1)
	{
		System* globalSys = globalSystems[i];
		globalSys->_mDraw(globalSys->_mData, spriteBatch);
	}
}
void GameHelper_DrawHudGlobalSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t globalSystemsLen = GameStateManager_GetGlobalSystemsLength();
	System** globalSystems = GameStateManager_GetGlobalSystems();
	for (int32_t i = 0; i < globalSystemsLen; i += 1)
	{
		System* globalSys = globalSystems[i];
		globalSys->_mDrawHud(globalSys->_mData, spriteBatch);
	}
}
void GameHelper_DrawDebugHudGlobalSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t globalSystemsLen = GameStateManager_GetGlobalSystemsLength();
	System** globalSystems = GameStateManager_GetGlobalSystems();
	for (int32_t i = 0; i < globalSystemsLen; i += 1)
	{
		System* globalSys = globalSystems[i];
		globalSys->_mDrawDebugHud(globalSys->_mData, spriteBatch);
	}
}
void GameHelper_UpdateStateSystemsNormally(void)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mUpdate(stateSys->_mData);
	}
}
void GameHelper_DrawStateSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mDraw(stateSys->_mData, spriteBatch);
	}
}
void GameHelper_DrawHudStateSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mDrawHud(stateSys->_mData, spriteBatch);
	}
}
void GameHelper_DrawDebugHudStateSystemsNormally(SpriteBatch* spriteBatch)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mDrawDebugHud(stateSys->_mData, spriteBatch);
	}
}
void GameHelper_CreateDefaultGlobalSystems(void)
{
#ifdef EDITOR_MODE
	/* //WILLNOTDO 06242023 DEBUG STUFF
	GameStateManager_AddGlobalSystem(GlobalSysMetaMapHack_CreateSystem()));
	GameStateManager_AddGlobalSystem(GlobalSysUpdateFontMap_CreateSystem()));
	GameStateManager_AddGlobalSystem(GlobalSysDebugMenu_CreateSystem()));	*/
	GameStateManager_AddGlobalSystem(GlobalSysDrawQuickStats_CreateSystem());
#endif

	GameStateManager_AddGlobalSystem(GlobalSysHideMouse_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysHandleGameStateChange_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysHandleLoadNextMap_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysUpdateManagers_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysTickSoundEffect_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysTickMusic_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysUpdateInput_CreateSystem());
	//GameStateManager_AddGlobalSystem(GlobalSysConsole_CreateSystem()); //WILLNOTDO 06242023 DEBUG CONSOLE STUFF
	GameStateManager_AddGlobalSystem(GlobalSysDefaultState_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysUpdateMapLoadTicker_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysSetJustChangedGameStateToFalse_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysSaveIcon_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysUpdateAchievementHandler_CreateSystem());
	GameStateManager_AddGlobalSystem(GlobalSysDrawFPS_CreateSystem());
}
void GameHelper_CreateDefaultBindings(IStringArray* strings, InputAction* input)
{
	int64_t len = IStringArray_Length(strings);

	for (int32_t i = 0; i < len; i += 1)
	{
		const char* s = IStringArray_Get(strings, i);

		InputAction* data = &input[i];

		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_A, KEYS_Z);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_X, KEYS_X);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_B, KEYS_C);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_Y, KEYS_V);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LB, KEYS_A);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LT, KEYS_S);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LS, KEYS_D);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RB, KEYS_Q);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RT, KEYS_W);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RS, KEYS_E);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_START, KEYS_ENTER);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_SELECT, KEYS_BACK);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LS_UP, KEYS_UP);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LS_RIGHT, KEYS_RIGHT);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LS_DOWN, KEYS_DOWN);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_LS_LEFT, KEYS_LEFT);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_DIGITAL_UP, KEYS_T);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_DIGITAL_RIGHT, KEYS_H);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_DIGITAL_DOWN, KEYS_G);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_DIGITAL_LEFT, KEYS_F);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RS_UP, KEYS_I);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RS_RIGHT, KEYS_L);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RS_DOWN, KEYS_K);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_RS_LEFT, KEYS_J);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_MENU_SELECT, KEYS_Z);
		InputBindings_SetupKey(data, 0, s, ACTIONLIST_GAME_MENU_BACK, KEYS_X);

		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_A, BUTTONS_A);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_X, BUTTONS_X);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_B, BUTTONS_B);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_Y, BUTTONS_Y);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_LB, BUTTONS_LEFTSHOULDER);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_RB, BUTTONS_RIGHTSHOULDER);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_LT, AXES_LEFT_TRIGGER, 1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_RT, AXES_RIGHT_TRIGGER, -1);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_LS, BUTTONS_LEFTSTICK);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_RS, BUTTONS_RIGHTSTICK);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_START, BUTTONS_START);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_SELECT, BUTTONS_BACK);

		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_LS_LEFT, AXES_LEFT_STICK_X, -1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_LS_RIGHT, AXES_LEFT_STICK_X, 1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_LS_UP, AXES_LEFT_STICK_Y, 1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_LS_DOWN, AXES_LEFT_STICK_Y, -1);

		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_RS_LEFT, AXES_RIGHT_STICK_X, -1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_RS_RIGHT, AXES_RIGHT_STICK_X, 1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_RS_UP, AXES_RIGHT_STICK_Y, -1);
		InputBindings_SetupAxis(data, 1, s, ACTIONLIST_GAME_RS_DOWN, AXES_RIGHT_STICK_Y, 1);

		InputBindings_SetupButton(data, 2, s, ACTIONLIST_GAME_DIGITAL_LEFT, BUTTONS_DPADLEFT);
		InputBindings_SetupButton(data, 2, s, ACTIONLIST_GAME_DIGITAL_RIGHT, BUTTONS_DPADRIGHT);
		InputBindings_SetupButton(data, 2, s, ACTIONLIST_GAME_DIGITAL_UP, BUTTONS_DPADUP);
		InputBindings_SetupButton(data, 2, s, ACTIONLIST_GAME_DIGITAL_DOWN, BUTTONS_DPADDOWN);

		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_MENU_SELECT, BUTTONS_A);
		InputBindings_SetupButton(data, 1, s, ACTIONLIST_GAME_MENU_BACK, BUTTONS_B);
	}
}
void GameHelper_InitGameStateData(const char* name, GameStateData* initThis)
{
	GameStateData_Ctor(initThis);
	
	/*if (!_mComponentSizes.count(name)) //UNUSED
	{
		bool isBinary = true;
		std_string extension = OeUtils_GetExtension(isBinary);
		std_string filePath = OeFile_PathCombine("data", "componentsizes_" + name + extension);
		if (!OeFile_FileExists(filePath))
		{
			return temp;
		}
		std_shared_ptr<OeIniReader> reader = OeIniReader_CreateNewOeIniReader(filePath);
		_mComponentSizes[name] = reader->ReadAsStringIntDictionary();
	}
	
	CreateGameStateDataHelper(temp, _mComponentSizes[name]);*/
}
void GameHelper_AddStrings(void)
{
	GameHelper_AddStringsHelper();
}
