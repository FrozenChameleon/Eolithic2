/* Mute Crimson DX C Source Code
 * Copyright 2026 Patrick Derosby
 * Released under a specialized, non-commercial-use license.
 * See LICENSE.md for details.
 */

#include "../engine/core/GameHelper.h"

#include "../engine/utils/Macros.h"
#include "../engine/globals/Align.h"
#include "../engine/render/DrawTool.h"
#include "../engine/core/ServiceHelper.h"
#include "../engine/gamestate/DebugRewindTester.h"
#include "../engine/systems/GlobalSysLoggerNotifications.h"
#include "../engine/systems/GlobalSysDrawFPS.h"
#include "../engine/systems/GlobalSysHideMouse.h"
#include "../engine/systems/GlobalSysHandleGameStateChange.h"
#include "../engine/systems/GlobalSysHandleLoadNextMap.h"
#include "../engine/systems/GlobalSysUpdateManagers.h"
#include "../engine/systems/GlobalSysTickSoundEffect.h"
#include "../engine/systems/GlobalSysTickMusic.h"
#include "../engine/systems/GlobalSysUpdateInput.h"
#include "../engine/systems/GlobalSysDefaultState.h"
#include "../engine/systems/GlobalSysUpdateMapLoadTicker.h"
#include "../engine/systems/GlobalSysSetJustChangedGameStateToFalse.h"
#include "../engine/systems/GlobalSysSaveIcon.h"
#include "../engine/systems/GlobalSysUpdateAchievementHandler.h"
#include "../engine/render/Color.h"
#include "../engine/systems/SysResetBounceData.h"
#include "../engine/systems/SysResetFloatyMovementData.h"
#include "../engine/systems/SysRemoveCompleteEntities.h"
#include "../engine/systems/SysDrawProps.h"
#include "../engine/systems/SysDrawTiles.h"
#include "../engine/components/SpecialMcFinalBossEffectSys.h"
#include "../engine/components/LevelFrameCounterSys.h"
#include "../engine/components/SpecialShakeCameraSys.h"
#include "../engine/components/LevelDataStub.h"
#include "../engine/components/LevelFrameCounter.h"
#include "../engine/components/Camera.h"
#include "../engine/components/CameraSys.h"
#include "../engine/components/DrawActorSys.h"
#include "../engine/components/FreezeEntityTillOnScreenSys.h"
#include "../engine/globals/Globals.h"
#include "../engine/input/Keys.h"
#include "../engine/input/Buttons.h"
#include "../engine/input/ActionList.h"
#include "../engine/input/InputBindings.h"
#include "../engine/input/Input.h"
#include "../engine/input/InputPlayer.h"
#include "../engine/input/InputAction.h"
#include "../engine/input/RecordingTool.h"
#include "../engine/utils/Cvars.h"
#include "../engine/collision/BodySys.h"
#include "../engine/leveldata/ParticleInstanceSys.h"
#include "../engine/utils/Utils.h"
#include "../engine/collision/CollisionEngineSys.h"
#include "../engine/leveldata/LevelData.h"
#include "../engine/core/Func.h"
#include "../engine/gamestate/GameStateManager.h"
#include "../engine/gamestate/GameState.h"
#include "../engine/globals/ObjectTypes.h"
#include "../engine/math/Math.h"
#include "../engine/math/Vectors.h"
#include "../engine/service/Service.h"
#include "../engine/utils/Strings.h"
#include "../engine/core/MenuFunc.h"
#include "../engine/achievement/AchievementHandler.h"
#include "../engine/io/File.h"
#include "../engine/systems/GlobalSysDrawQuickStats.h"
#include "../engine/resources/ResourceList.h"

#define START_CONTROLLER 0
#define START_KEYBOARD 3

static IStringArray* _mRecordings;

#ifdef EDITOR_MODE
void GameHelper_OnDebugFastResetPlusMove(void)
{
}
#endif

void GameHelper_OnLoadingAfterResources(void)
{
}
void GameHelper_SetupMusicAndSfxList(StringPair* music, StringPair* sfx)
{
}
void GameHelper_BuildAchievementList(void)
{
}
void GameHelper_OnNormalGameStateChange(void)
{
}
void GameHelper_CreateGameStates(void)
{
}
const char* GameHelper_DebugGiveNextLevel(void)
{
	return NULL;
}
void GameHelper_CreateGlobalSystems(void)
{
#ifdef EDITOR_MODE
	/* //WILLNOTDO 06242023 DEBUG STUFF
	GameStateManager_AddGlobalSystem(new GlobalSysMetaMapHack());
	GameStateManager_AddGlobalSystem(new GlobalSysUpdateFontMap());
	GameStateManager_AddGlobalSystem(new GlobalSysDebugMenu());
		*/
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
	GameStateManager_AddGlobalSystem(GlobalSysLoggerNotifications_CreateSystem());
}
void GameHelper_CreateStateSystems(void)
{
	GameStateManager_AddStateSystem(FreezeEntityTillOnScreenSys_CreateSystem());

	GameStateManager_AddStateSystem(ParticleInstanceSys_CreateSystem());
	
	GameStateManager_AddStateSystem(SpecialShakeCameraSys_CreateSystem());

	GameStateManager_AddStateSystem(BodySys_CreateSystem());
	GameStateManager_AddStateSystem(CollisionEngineSys_CreateSystem());

	GameStateManager_AddStateSystem(CameraSys_CreateSystem());

	GameStateManager_AddStateSystem(SysDrawTiles_CreateSystem());
	GameStateManager_AddStateSystem(SysDrawProps_CreateSystem());

	GameStateManager_AddStateSystem(SysResetBounceData_CreateSystem());
	GameStateManager_AddStateSystem(SysResetFloatyMovementData_CreateSystem());

	GameStateManager_AddStateSystem(DrawActorSys_CreateSystem());

	GameStateManager_AddStateSystem(LevelFrameCounterSys_CreateSystem());

	GameStateManager_AddStateSystem(SysRemoveCompleteEntities_CreateSystem());
}
ComponentType GameHelper_BuildControllerComponent(int32_t number, Entity entity)
{
	switch (number)
	{
	case 0:
		Do_InitComponent(C_Player, entity);
		return C_Player;
	default:
		return C_Player;
	}
}
void GameHelper_InitGameState(const char* levelToLoad)
{
}
bool GameHelper_HandleRewindBeforeUpdatingStateSystems(void)
{
	return false;
}
void GameHelper_UpdateStateSystems(void)
{
}
void GameHelper_DrawStateSystems(SpriteBatch* spriteBatch)
{
}
void GameHelper_DrawHudStateSystems(SpriteBatch* spriteBatch)
{

}
void GameHelper_DefaultBindings(IStringArray* strings, InputAction* input)
{
	int64_t len = IStringArray_Length(strings);
	for (int32_t i = 0; i < len; i += 1)
	{
		const char* s = IStringArray_Get(strings, i);

		InputAction* data = &input[i];

		//JUMP
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_A, KEYS_Z);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_A, BUTTONS_A);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_A, BUTTONS_B);

		//ATTACK
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_X, KEYS_X);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_X, BUTTONS_X);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_X, BUTTONS_Y);

		//START
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_START, KEYS_BACK);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_START, BUTTONS_START);

		//REWIND
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LB, KEYS_TAB);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_LB, BUTTONS_LEFTSHOULDER);
		InputBindings_SetupAxis(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_LB, AXES_LEFT_TRIGGER, 1);

		//QUICKSAVE
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LS, KEYS_F1);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_LS, BUTTONS_LEFTSTICK);

		//QUICKLOAD
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_RS, KEYS_F2);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_RS, BUTTONS_RIGHTSTICK);

		//LEFT STICK UP
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LS_UP, KEYS_UP);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_LS_UP, AXES_LEFT_STICK_Y, 1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_LS_UP, BUTTONS_DPADUP);

		//LEFT STICK RIGHT
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LS_RIGHT, KEYS_RIGHT);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_LS_RIGHT, AXES_LEFT_STICK_X, 1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_LS_RIGHT, BUTTONS_DPADRIGHT);

		//LEFT STICK DOWN
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LS_DOWN, KEYS_DOWN);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_LS_DOWN, AXES_LEFT_STICK_Y, -1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_LS_DOWN, BUTTONS_DPADDOWN);

		//LEFT STICK LEFT
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_LS_LEFT, KEYS_LEFT);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_LS_LEFT, AXES_LEFT_STICK_X, -1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_LS_LEFT, BUTTONS_DPADLEFT);

		//MENU SELECT
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_SELECT, KEYS_Z);
		InputBindings_SetupKey(data, START_KEYBOARD + 1, s, ACTIONLIST_GAME_MENU_SELECT, KEYS_ENTER);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_SELECT, BUTTONS_A);

		//MENU BACK
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_BACK, KEYS_X);
		InputBindings_SetupButton(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_BACK, BUTTONS_B);

		//MENU UP
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_UP, KEYS_UP);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_UP, AXES_LEFT_STICK_Y, 1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_MENU_UP, BUTTONS_DPADUP);

		//MENU RIGHT
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_RIGHT, KEYS_RIGHT);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_RIGHT, AXES_LEFT_STICK_X, 1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_MENU_RIGHT, BUTTONS_DPADRIGHT);

		//MENU DOWN
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_DOWN, KEYS_DOWN);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_DOWN, AXES_LEFT_STICK_Y, -1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_MENU_DOWN, BUTTONS_DPADDOWN);

		//MENU LEFT
		InputBindings_SetupKey(data, START_KEYBOARD, s, ACTIONLIST_GAME_MENU_LEFT, KEYS_LEFT);
		InputBindings_SetupAxis(data, START_CONTROLLER, s, ACTIONLIST_GAME_MENU_LEFT, AXES_LEFT_STICK_X, -1);
		InputBindings_SetupButton(data, START_CONTROLLER + 1, s, ACTIONLIST_GAME_MENU_LEFT, BUTTONS_DPADLEFT);
	}
}
float GameHelper_GetGravityDeceleration(void)
{
	return 0;
}
float GameHelper_GetGravityDecelerationMax(void)
{
	return 0;
}
void* GameHelper_CreateGameSaveData(void)
{
	return NULL;
}
void GameHelper_AddStringsHelper(void)
{
}
void GameHelper_SetupPlatformTypes(void)
{
}
float GameHelper_GetMovingPlatformLeeway(void)
{
	return 4;
}
void GameHelper_BakedCollisionCheck(float x, float y, int32_t collisionToCheck, Body* bodyRef, bool vertical, CollisionCheckData* data)
{
	
}
float GameHelper_GetPostGameBrightness(void)
{
	return 1.0f;
}
void GameHelper_Initialize(void)
{
	GlobalSysDrawFPS_SetFont("menu");
	Service_SetLeaderboardAmountOfRowsToRetrieve(10);
	//Service_SetAchievementMap(GetAchievementMap(), ACHIEVEMENT_MAP_LEN);
}
void GameHelper_OnLoadingStart(void)
{
}
void GameHelper_WriteSessionData(BufferWriter* writer)
{
}
void GameHelper_ReadSessionData(BufferReader* reader)
{
}
void GameHelper_AddDefaultThingSettings(StringPair* pairs)
{
}
bool GameHelper_IsCollisionSolidForPathFinding(int32_t collision, int32_t currentX, int32_t currentY, int32_t newX, int32_t newY)
{
	return false;
}
bool GameHelper_IsCollisionSolidForLineOfSight(int32_t collision)
{
	return true;
}
const char* GameHelper_ReturnInputDisplayMask(const char* displayName)
{
	if (Utils_StringEqualTo(displayName, "GAME_LS_UP"))
	{
		return Strings_Get("INPUT_MASK_MOVE_UP");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_LS_RIGHT"))
	{
		return Strings_Get("INPUT_MASK_MOVE_RIGHT");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_LS_DOWN"))
	{
		return Strings_Get("INPUT_MASK_MOVE_DOWN");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_LS_LEFT"))
	{
		return Strings_Get("INPUT_MASK_MOVE_LEFT");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_A"))
	{
		return Strings_Get("INPUT_MASK_JUMP");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_X"))
	{
		return Strings_Get("INPUT_MASK_MELEE");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_LB"))
	{
		return Strings_Get("INPUT_MASK_REWIND");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_START"))
	{
		return Strings_Get("INPUT_MASK_START");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_UP"))
	{
		return Strings_Get("INPUT_MASK_MENU_UP");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_RIGHT"))
	{
		return Strings_Get("INPUT_MASK_MENU_RIGHT");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_DOWN"))
	{
		return Strings_Get("INPUT_MASK_MENU_DOWN");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_LEFT"))
	{
		return Strings_Get("INPUT_MASK_MENU_LEFT");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_SELECT"))
	{
		return Strings_Get("INPUT_MASK_MENU_SELECT");
	}
	else if (Utils_StringEqualTo(displayName, "GAME_MENU_BACK"))
	{
		return Strings_Get("INPUT_MASK_MENU_BACK");
	}

	return displayName;
}
LevelData* GameHelper_GetLevelData(void)
{
	return GameHelper_GetDefaultLevelData();
}
Resource* GameHelper_GetLevelDataResource(void)
{
	return GameHelper_GetDefaultLevelDataResource();
}
void GameHelper_InitPoolsForGame(void)
{
}
Camera* GameHelper_GetCameraForRender(void)
{
	return GameHelper_GetDefaultCameraForRender();
}
void GameHelper_UpdateLastRenderPosition(void)
{
	GameHelper_UpdateLastRenderPositionNormally();
}
void GameHelper_UpdateGlobalSystems(void)
{
	//REWIND HUD UPDATE

	//NEED TO SAVE WITH DELAY TOOL UPDATE

	Service_CheckLeaderboardSendStatus(true);

	GameHelper_UpdateGlobalSystemsNormally();
}
void GameHelper_DrawGlobalSystems(SpriteBatch* spriteBatch)
{
	GameHelper_DrawGlobalSystemsNormally(spriteBatch);
}
void GameHelper_DrawHudGlobalSystems(SpriteBatch* spriteBatch)
{
	GameHelper_DrawHudGlobalSystemsNormally(spriteBatch);
}
void GameHelper_DrawDebugHudGlobalSystems(SpriteBatch* spriteBatch)
{
	GameHelper_DrawDebugHudGlobalSystemsNormally(spriteBatch);
}
void GameHelper_DrawDebugHudStateSystems(SpriteBatch* spriteBatch)
{
	GameHelper_DrawDebugHudStateSystemsNormally(spriteBatch);
}
IStringArray* GameHelper_GetAllTheRecordings(void)
{
	if (_mRecordings == NULL)
	{
		_mRecordings = IStringArray_Create();
		{
			MString* path = NULL;
			File_PathCombine2(&path, "data", "recordings");
			File_GetFilenames(_mRecordings, MString_Text(path), "*.bin", true);
			MString_Dispose(&path);
		}
	}
	return _mRecordings;
}
void GameHelper_WriteGameSaveData(void* gsd, DynamicByteBuffer* writer)
{
}
void GameHelper_ReadGameSaveData(void* gsd, BufferReader* reader)
{
}
void GameHelper_AfterHighPriorityBodyStep(bool isVertical)
{

}
void GameHelper_AfterHighPriorityBodyStep2(bool isVertical)
{

}