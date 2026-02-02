/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "System.h"

#include "../utils/Utils.h"

static void DummyInitStringSettingsHere(void* givenData)
{
}
static void DummyInit(void* givenData, ComponentType givenType, Entity owner)
{
}
static void DummyUpdate(void* givenData)
{
}
static void DummyDraw(void* givenData, SpriteBatch* spriteBatch)
{
}
static void DummyDrawHud(void* givenData, SpriteBatch* spriteBatch)
{
}
static void DummyDrawDebugHud(void* givenData, SpriteBatch* spriteBatch)
{
}
static void DummyUpdateLastRenderPosition(void* givenData, GameState* gameState)
{
}
static void DummyReceiveBroadcast(void* givenData, int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3)
{
}

System* System_Create(void)
{
	System* sys = (System*)Utils_calloc(1, sizeof(System));
	sys->_mInitStringSettingsHere = DummyInitStringSettingsHere;
	sys->_mInit = DummyInit;
	sys->_mUpdate = DummyUpdate;
	sys->_mDraw = DummyDraw;
	sys->_mDrawHud = DummyDrawHud;
	sys->_mDrawDebugHud = DummyDrawDebugHud;
	sys->_mUpdateLastRenderPosition = DummyUpdateLastRenderPosition;
	sys->_mReceiveBroadcast = DummyReceiveBroadcast;
	return sys;
}
