/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */
 
#pragma once

#include "../../engine/utils/Macros.h"
#include "../../engine/systems/SystemSimple.h"
#include "../../engine/math/Vector2.h"

typedef struct Player
{
	int mDirection;
} Player;

System* Player_CreateSystem();

void Player_InitRoutine(Entity owner, Player* data);
void Player_UpdateRoutine(Entity owner, Player* data);