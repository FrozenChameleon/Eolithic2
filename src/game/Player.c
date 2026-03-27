#include "Player.h"

#include "../../engine/globals/OeState.h"
#include "../../engine/globals/OePhase.h"

System* Player_CreateSystem()
{
    SystemSimple* ss = SystemSimple_Create(C_Player);
    ss->_mInitRoutine = (SystemSimple_InitFunc)Player_InitRoutine;
    ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)Player_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}

void Player_InitRoutine(Entity owner, Player* data)
{
}
void Player_UpdateRoutine(Entity owner, Player* data)
{
}
