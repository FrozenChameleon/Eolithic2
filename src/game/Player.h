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