/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"
#include "ActionList.h"
#include "InputAction.h"

typedef struct FixedByteBuffer FixedByteBuffer;
typedef struct BufferReader BufferReader;
typedef struct IStringArray IStringArray;

#define PLAYERBINDINGDATA_BINDINGS_LEN ACTIONLIST_LENGTH

typedef struct PlayerBindingData
{
	int32_t _mPlayerNumber;
	bool _mHasLoaded;
	InputAction _mBindings[PLAYERBINDINGDATA_BINDINGS_LEN];
} PlayerBindingData;

void PlayerBindingData_Init(PlayerBindingData* pbd, IStringArray* actionList, int32_t playerNumber);

const char* PlayerBindingData_GetFilePlayerPath(PlayerBindingData* pbd);
void PlayerBindingData_LogLoadSuccess(PlayerBindingData* pbd);
void PlayerBindingData_LogLoadFailure(PlayerBindingData* pbd);
void PlayerBindingData_LoadFromStream(PlayerBindingData* pbd, BufferReader* reader);
FixedByteBuffer* PlayerBindingData_CreateBufferFromBindings(PlayerBindingData* pbd);
void PlayerBindingData_ResetCertainBindingsToDefault(PlayerBindingData* pbd, IStringArray* actions, int32_t indexStart, int32_t length);
bool PlayerBindingData_DoBindingsExistFor(PlayerBindingData* pbd, const char* action, int32_t indexStart, int32_t length);
void PlayerBindingData_SetAsLoaded(PlayerBindingData* pbd);
InputAction* PlayerBindingData_GetBindings(PlayerBindingData* pbd);
void PlayerBindingData_ResetToDefault(PlayerBindingData* pbd);
bool PlayerBindingData_HasLoaded(PlayerBindingData* pbd);
void PlayerBindingData_Load(PlayerBindingData* pbd);
void PlayerBindingData_Save(PlayerBindingData* pbd);

void InputBindings_Init(void);
InputAction* InputBindings_GetDefaultActions(void);
void InputBindings_ResetAllPlayerBindingsToDefaultAndSave(void);
void InputBindings_SetupKey(InputAction* data, int32_t place, const char* s, const char* check, int32_t key);
void InputBindings_SetupMouseButton(InputAction* data, int32_t place, const char* s, const char* check, int32_t mouseButton);
void InputBindings_SetupButton(InputAction* data, int32_t place, const char* s, const char* check, int32_t button);
void InputBindings_SetupAxis(InputAction* data, int32_t place, const char* s, const char* check, int32_t axis, int32_t direction);
bool InputBindings_HaveAllPlayersLoadedBindings(void);
void InputBindings_LoadAllPlayerBindings(void);
void InputBindings_SaveAllBindings(void);
void InputBindings_ResetAllPlayerCertainBindingsToDefaultAndSave(IStringArray* actions, int32_t indexStart, int32_t length);
bool InputBindings_DoBindingsExistFor(int32_t player, const char* action, int32_t indexStart, int32_t length);
void InputBindings_SyncAllBindingsToAllPlayers(void);
InputAction* InputBindings_GetBindings(int32_t player);
InputAction* InputBindings_GetActionFromBindings(int32_t player, const char* name);
InputAction* InputBindings_GetActionFromArray(InputAction* actions, const char* name);