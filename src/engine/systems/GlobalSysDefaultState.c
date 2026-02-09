/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysDefaultState.h"

#include "../editor/Editor.h"
#include "../globals/Globals.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"

static void Draw(void* givenData, SpriteBatch* spriteBatch)
{
#ifdef EDITOR_MODE
	if (Globals_DebugIsEditorMode())
	{
		Editor_Draw(spriteBatch);
	}
	else
	{
		GameState_Draw(GameStateManager_GetGameState(), spriteBatch);
	}
#else
	GameState_Draw(GameStateManager_GetGameState(), spriteBatch);
#endif
}
static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
#ifdef EDITOR_MODE
	if (Globals_DebugIsEditorMode())
	{
		Editor_DrawHud(spriteBatch);
	}
	else
	{
		GameState_DrawHud(GameStateManager_GetGameState(), spriteBatch);
	}
#else
	GameState_DrawHud(GameStateManager_GetGameState(), spriteBatch);
#endif
}
/*void OeGlobalSysDefaultState_DrawDebugHud(OeSpriteBatch* spriteBatch)
{
#ifdef EDITOR_MODE
	//WILLNOTDO 06262023 (EDITOR_MODE) 2023 OeGameStateManager.ActiveGameState.DrawDebugHud(spriteBatch);
#endif
}*/
static void Update(void* givenData)
{
#ifdef EDITOR_MODE
	if (Globals_DebugIsEditorMode())
	{
		Editor_Update();
	}
	else
	{
		//WILLNOTDO 06262023 (EDITOR_MODE) 2023 OeGuiThingViewer.Update();
		//WILLNOTDO 06262023 (EDITOR_MODE) 2023 if (!OeConsole.IsPresent())
		//WILLNOTDO 06262023 (EDITOR_MODE) 2023 {
		GameState_Update(GameStateManager_GetGameState());
		//WILLNOTDO 06262023 (EDITOR_MODE) 2023 }
	}
#else
	GameState_Update(GameStateManager_GetGameState());
#endif
}

System* GlobalSysDefaultState_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mDraw = Draw;
	sys->_mDrawHud = DrawHud;
	sys->_mUpdate = Update;
	return sys;
}
