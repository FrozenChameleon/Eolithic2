/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysDrawQuickStats.h"

#include "../utils/Cvars.h"
#include "../render/Color.h"
#include "../utils/Utils.h"
#include "../core/GameUpdater.h"
#include "../math/Rectangle.h"
#include "../render/DrawTool.h"
#include "../render/SpriteBatch.h"
#include "../globals/Globals.h"
#include "../render/Renderer.h"
#include "../gamestate/GameStateManager.h"
#include "../core/Func.h"

static int32_t GetAmountOfTotalActivePermanentEntity(void)
{
	int32_t count = 0;
	EntitySearch* search = EntitySearch_CreateNewAllEntitySearch();
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (!Is_Complete(target) && !Is_Child(target))
		{
			count += 1;
		}
	}
	return count;
}
static int32_t GetAmountOfTotalChildrenEntities(void)
{
	int32_t count = 0;
	EntitySearch* search = EntitySearch_CreateNewAllEntitySearch();
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (Is_Child(target))
		{
			count += 1;
		}
	}
	return count;
}

#ifdef EDITOR_MODE
static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
	int32_t depth = 150;

	Rectangle rect = Renderer_GetScreenBounds();
    float middleX = (float)Rectangle_Center(&rect).X;
	int32_t textOff = 0;
	Vector2 textDiff = Vector2_Create(0, 24);
    const char* font = "menu";
	Color color = COLOR_YELLOW;

    if (GLOBALS_DEBUG_IS_GOD_MODE)
    {
        SpriteBatch_DrawString(spriteBatch, font, "GOD_MODE_ON", COLOR_YELLOW, depth, Vector2_Create(middleX, 0));
		textOff += 1;
    }
    if (GLOBALS_DEBUG_IS_PAUSED)
    {
        //spriteBatch.DrawString(font, "IS_PAUSED", OeColors.YELLOW, depth, new Vector2(middleX, counter));
    }

	if (GLOBALS_DEBUG_SHOW_INFO != GAMESTATEMANAGER_DEBUG_INFO_SHOW_QUICK_STATS)
    {
		return;
	}

	MString* tempString = NULL;

	MString_Combine2(&tempString, "Speed=", Utils_DoubleToStringGlobalBuffer(Globals_GetDebugGameSpeedAsMul()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "SecondsSinceMapLoad=", Utils_UInt64ToStringGlobalBuffer(GameStateManager_GetTicksSinceMapLoad() / 60));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "FramesSinceMapLoad=", Utils_UInt64ToStringGlobalBuffer(GameStateManager_GetTicksSinceMapLoad()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "SkipFrames=", Utils_UInt64ToStringGlobalBuffer(GameUpdater_GetSkippedFrames()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "FPS=", Utils_IntToStringGlobalBuffer(GameUpdater_GetFPS()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "FPS=", Utils_IntToStringGlobalBuffer(GameState_GetAmountOfEntitiesInPlay(GameStateManager_GetGameState())));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "PermThingsOn=", Utils_IntToStringGlobalBuffer(GetAmountOfTotalActivePermanentEntity()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	MString_Combine2(&tempString, "SummonThingsOn=", Utils_IntToStringGlobalBuffer(GetAmountOfTotalChildrenEntities()));
	DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);

	if (Is_PlayerPresent(false))
	{
		Vector2 playerPosition = Get_PlayerPosition();
		MString_AssignString(&tempString, "Player=");
		MString_AddAssignString(&tempString, "X:");
		MString_AddAssignFloat(&tempString, playerPosition.X);
		MString_AddAssignString(&tempString, "Y:");
		MString_AddAssignFloat(&tempString, playerPosition.Y);
		DrawTool_DrawDebugInfoHelperMString(spriteBatch, tempString, font, color, depth, textDiff, &textOff);
	}

	DrawTool_DrawDebugRefCounts(spriteBatch, font, color, depth, textDiff, &textOff);

	MString_Dispose(&tempString);
}
#endif

System* GlobalSysDrawQuickStats_CreateSystem(void)
{
	System* sys = System_Create();
#ifdef EDITOR_MODE
	sys->_mDrawHud = (System_DrawFunc)DrawHud;
#endif
	return sys;
}
