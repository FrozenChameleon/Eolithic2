/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#ifdef CONTROLLER_DUMMY

#include "../utils/Macros.h"
#include "ControllerState.h"

void ControllerData_PollControllerStatus(ControllerData* cd, PlayerIndex index)
{
}
void ControllerData_PollInput(ControllerData* cd, bool* isButtonMutedArray, bool* isAnalogMutedArray)
{
}
void ControllerState_SetVibration(ControllerState* cs, float leftMotor, float rightMotor)
{
}
int32_t ControllerState_AddControllerInstance(int32_t dev)
{
	return 0;
}
int32_t ControllerState_RemoveControllerInstance(int32_t dev)
{
	return 0;
}
void ControllerState_BeforePollingUpdate(void)
{

}
bool ControllerState_PlatformUsesControllerPrompt(void)
{
	return false;
}
bool ControllerState_GetControllerPrompt(int32_t minPlayer, int32_t maxPlayer, bool retainConnection, bool singlePlayer, bool spinlockUntilSuccess)
{
	return false;
}
bool ControllerState_PlatformIsForcingCustomInputActions(int32_t player)
{
	return false;
}
//std::vector<OeInputAction>& OeControllerState_GetForcedCustomInputActions(int32_t player)
//{
//	return _mDummyAction;
//}
bool ControllerState_IsUsingControllerForbiddenToFullRebinding(void)
{
	return false;
}
bool ControllerState_IsUsingControllerPrompt(void)
{
	return false;
}
const char* ControllerState_PlatformGetForcedSpecificGlyphAsString(void)
{
	return EE_STR_EMPTY;
}

#endif

typedef int compiler_warning_compliance;