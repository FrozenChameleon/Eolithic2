/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysDefaultState.h"

#include "../globals/Globals.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"

static void Draw(void* givenData, SpriteBatch* spriteBatch)
{
	if (Globals_IsEditorActive())
	{
		return;
	}

	GameState_Draw(GameStateManager_GetGameState(), spriteBatch);
}
static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
	if (Globals_IsEditorActive())
	{
		return;
	}

	GameState_DrawHud(GameStateManager_GetGameState(), spriteBatch);
}
/*void OeGlobalSysDefaultState_DrawDebugHud(OeSpriteBatch* spriteBatch)
{
#ifdef EDITOR_MODE
	//WILLNOTDO 06262023 (EDITOR_MODE) 2023 OeGameStateManager.ActiveGameState.DrawDebugHud(spriteBatch);
#endif
}*/
static void Update(void* givenData)
{
	if (Globals_IsEditorActive())
	{
		return;
	}

	GameState_Update(GameStateManager_GetGameState());
}

System* GlobalSysDefaultState_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mDraw = Draw;
	sys->_mDrawHud = DrawHud;
	sys->_mUpdate = Update;
	return sys;
}
