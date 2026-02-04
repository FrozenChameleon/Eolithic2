/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "InputBindings.h"

#include "InputPlayer.h"
#include "../globals/Globals.h"
#include "Input.h"
#include "ActionList.h"
#include "../utils/Cvars.h"
#include "../utils/Logger.h"
#include "../core/Game.h"
#include "../core/GameHelper.h"
#include "../service/Service.h"
#include "../io/BufferReader.h"
#include "../utils/Utils.h"
#include "../utils/IStringArray.h"
#include "../io/DynamicByteBuffer.h"

#define CONTAINER_DISPLAY_NAME "GameBindings"
#define CONTAINER_NAME "GameBindingsContainer"

#define VERSION_INPUT_BINDINGS 7
#define data_version "data_version"
#define data_count "data_count"

#define PLAYERBINDINGS_LEN INPUT_MAXIMUM_PLAYER_COUNT

static InputAction _mDefaultBindings[PLAYERBINDINGDATA_BINDINGS_LEN];
static PlayerBindingData _mPlayerBindings[PLAYERBINDINGS_LEN];

static bool _mHasInit;
static MString* _mTempString;

const char* PlayerBindingData_GetFilePlayerPath(PlayerBindingData* pbd)
{
	MString_AssignString(&_mTempString, "player");
	MString_AddAssignString(&_mTempString, Utils_IntToStringGlobalBuffer(pbd->_mPlayerNumber));
	MString_AddAssignString(&_mTempString, ".txt");
	return MString_Text(_mTempString);
}
void PlayerBindingData_LogLoadSuccess(PlayerBindingData* pbd)
{
	MString_AssignString(&_mTempString, "Bindings have been loaded for player #");
	MString_AddAssignString(&_mTempString, Utils_IntToStringGlobalBuffer(pbd->_mPlayerNumber));
	Logger_LogInformation(MString_Text(_mTempString));
}
void PlayerBindingData_LogLoadFailure(PlayerBindingData* pbd)
{
	MString_AssignString(&_mTempString, "Unable to load bindings for player #");
	MString_AddAssignString(&_mTempString, Utils_IntToStringGlobalBuffer(pbd->_mPlayerNumber));
	Logger_LogInformation(MString_Text(_mTempString));
}
void PlayerBindingData_LoadFromStream(PlayerBindingData* pbd, BufferReader* reader)
{
	if (!BufferReader_ReadMagicNumber(reader))
	{
		return;
	}
	BufferReader_ReadVersionNumber(reader);

	int32_t count = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < count; i += 1)
	{
		InputAction data;
		InputAction_Init(EE_STR_NOT_SET, &data);
		InputAction_Read(&data,  reader);
		pbd->_mBindings[i] = data;
	}
}
FixedByteBuffer* PlayerBindingData_CreateBufferFromBindings(PlayerBindingData* pbd)
{
	DynamicByteBuffer* writer = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(writer, true);

	DynamicByteBuffer_WriteMagicNumber(writer);

	DynamicByteBuffer_WriteNewline(writer);

	DynamicByteBuffer_WriteVersionNumber(writer, VERSION_INPUT_BINDINGS);

	DynamicByteBuffer_WriteNewline(writer);

	DynamicByteBuffer_WriteI32(writer, PLAYERBINDINGDATA_BINDINGS_LEN);
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		DynamicByteBuffer_WriteNewline(writer);
		InputAction_Write(&pbd->_mBindings[i], writer);
	}

	DynamicByteBuffer_WriteEOF(writer);
	return DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(writer);
}

void PlayerBindingData_Init(PlayerBindingData* pbd, IStringArray* actionList, int32_t playerNumber)
{
	Utils_memset(pbd, 0, sizeof(PlayerBindingData));
	pbd->_mPlayerNumber = playerNumber;
}

void PlayerBindingData_ResetCertainBindingsToDefault(PlayerBindingData* pbd, IStringArray* actions, int32_t indexStart, int32_t length)
{
	InputAction* defaultActions = InputBindings_GetDefaultActions();
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		for (int32_t j = 0; j < IStringArray_Length(actions); j += 1)
		{
			if (Utils_StringEqualTo(pbd->_mBindings[i].mName, IStringArray_Get(actions, j)))
			{
				InputAction* currentAction = &pbd->_mBindings[i];
				InputAction* defaultAction = &defaultActions[i];
				for (int32_t k = indexStart; k < (indexStart + length); k += 1)
				{
					InputCheck* defaultCheck = InputChecks_Get(&defaultAction->mChecks, k);
					InputChecks_Set(&currentAction->mChecks, k, *defaultCheck);
				}
			}
		}
	}

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Player #");
		MString_AddAssignInt(&tempString, pbd->_mPlayerNumber);
		MString_AddAssignString(&tempString, " certain bindings (");
		MString_AddAssignInt(&tempString, indexStart);
		MString_AddAssignString(&tempString, "-");
		MString_AddAssignInt(&tempString, (indexStart + (length - 1)));
		MString_AddAssignString(&tempString, ") have been reset to default");
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
bool PlayerBindingData_DoBindingsExistFor(PlayerBindingData* pbd, const char* action, int32_t indexStart, int32_t length)
{
	int32_t dummyCounter = 0;
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		if (Utils_StringEqualTo(pbd->_mBindings[i].mName, action))
		{
			InputAction* currentAction = &pbd->_mBindings[i];
			for (int32_t j = indexStart; j < indexStart + length; j += 1)
			{
				InputCheck* check = InputChecks_Get(&currentAction->mChecks, j);
				if (check->mType == INPUTCHECK_TYPE_DUMMY)
				{
					dummyCounter += 1;
				}
			}
		}
	}

	if (dummyCounter >= length)
	{
		return false;
	}
	else
	{
		return true;
	}
}
void PlayerBindingData_SetAsLoaded(PlayerBindingData* pbd)
{
	pbd->_mHasLoaded = true;
}
InputAction* PlayerBindingData_GetBindings(PlayerBindingData* pbd)
{
	return 	pbd->_mBindings;
}
void PlayerBindingData_ResetToDefault(PlayerBindingData* pbd)
{
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		pbd->_mBindings[i] = _mDefaultBindings[i];
	}
}
bool PlayerBindingData_HasLoaded(PlayerBindingData* pbd)
{
	return pbd->_mHasLoaded;
}
void PlayerBindingData_Load(PlayerBindingData* pbd)
{
	if (pbd->_mHasLoaded)
	{
		return;
	}

	const char* filename = PlayerBindingData_GetFilePlayerPath(pbd);
	BufferRequest request = Service_AskToRetrieveBuffer(false, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, filename);
	if (request.mIsBufferReady)
	{
		pbd->_mHasLoaded = true;
		if (request.mBuffer == NULL)
		{
			PlayerBindingData_LogLoadFailure(pbd);
			return;
		}
		else
		{
			//try
			//{
			
			BufferReader* reader = BufferReader_Create(request.mBuffer);
			BufferReader_SetIsReadingText(reader, true);
			PlayerBindingData_LoadFromStream(pbd, reader);
			PlayerBindingData_LogLoadSuccess(pbd);
			return;
			// }
			//catch (...)
			//{
			//	OeLogger_LogError("Unable to read binding data!");
			//	LogLoadFailure();
			//	return;
			//}
		}
	}
}
void PlayerBindingData_Save(PlayerBindingData* pbd)
{
	const char* filename = PlayerBindingData_GetFilePlayerPath(pbd);

	{
		FixedByteBuffer* bufferToSave = PlayerBindingData_CreateBufferFromBindings(pbd);
		Service_SaveBuffer(false, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, filename, bufferToSave);
		FixedByteBuffer_Dispose(bufferToSave);
	}

	{
		MString* tempString = NULL;
		MString_Combine4(&tempString, "Bindings have been saved for player #", Utils_IntToStringGlobalBuffer(pbd->_mPlayerNumber), ", ", filename);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	Utils_JustSaved();
}

static bool IsMirroringBindingsDisabled(void)
{
	return Cvars_GetAsBool(CVARS_ENGINE_DISABLE_PLAYER_BINDING_MIRRORING);
}
static void ResetAllPlayerBindingsToDefault(void)
{
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		PlayerBindingData_ResetToDefault(&_mPlayerBindings[i]);
	}
}
static void CreateDefaultBindings(InputAction* inputData, IStringArray* strings)
{
	Utils_memset(inputData, 0, sizeof(InputAction) * PLAYERBINDINGDATA_BINDINGS_LEN);
	for (int32_t i = 0; i < IStringArray_Length(strings); i += 1)
	{
		InputAction_Init(IStringArray_Get(strings, i), &inputData[i]);
	}
	GameHelper_DefaultBindings(strings, inputData);
}
static void CopyActions(const InputAction* copyFrom, InputAction* copyTo)
{
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		copyTo[i] = copyFrom[i];
	}
}
static void UpdateAllBindingsToMatchPlayerOne(void)
{
	InputAction* playerOneBindings = PlayerBindingData_GetBindings(&_mPlayerBindings[0]);
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		if (i != 0)
		{
			CopyActions(playerOneBindings, PlayerBindingData_GetBindings(&_mPlayerBindings[i]));
		}
	}
}

void InputBindings_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	IStringArray* actionList = ActionList_GetArray();
	CreateDefaultBindings(_mDefaultBindings, actionList);
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		PlayerBindingData_Init(&_mPlayerBindings[i], actionList, i);
	}

	ResetAllPlayerBindingsToDefault();

	_mHasInit = true;
}
InputAction* InputBindings_GetDefaultActions(void)
{
	return _mDefaultBindings;
}
void InputBindings_ResetAllPlayerBindingsToDefaultAndSave(void)
{
	ResetAllPlayerBindingsToDefault();
	InputBindings_SaveAllBindings();
}
void InputBindings_SetupKey(InputAction* data, int32_t place, const char* s, const char* check, Key key)
{
	if (Utils_StringEqualTo(s, check))
	{
		InputChecks_Set(&data->mChecks, place, InputCheck_CreateCheckKey(key));
	}
}
void InputBindings_SetupMouseButton(InputAction* data, int32_t place, const char* s, const char* check, MouseButton mouseButton)
{
	if (Utils_StringEqualTo(s, check))
	{
		InputChecks_Set(&data->mChecks, place, InputCheck_CreateCheckMouseButton(mouseButton));
	}
}
void InputBindings_SetupButton(InputAction* data, int32_t place, const char* s, const char* check, Button button)
{
	if (Utils_StringEqualTo(s, check))
	{
		InputChecks_Set(&data->mChecks, place, InputCheck_CreateCheckButton(button));
	}
}
void InputBindings_SetupAxis(InputAction* data, int32_t place, const char* s, const char* check, Axis axis, int32_t direction)
{
	if (Utils_StringEqualTo(s, check))
	{
		InputChecks_Set(&data->mChecks, place, InputCheck_CreateCheckAxis(axis, direction));
	}
}
bool InputBindings_HaveAllPlayersLoadedBindings(void)
{
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		if (!PlayerBindingData_HasLoaded(&_mPlayerBindings[i]))
		{
			return false;
		}
	}
	return true;
}
void InputBindings_LoadAllPlayerBindings(void)
{
	bool bindingsAreMirrored = !IsMirroringBindingsDisabled();

	if (Cvars_GetAsBool(CVARS_ENGINE_LOADING_SAVE_DATA_DISABLED) || Globals_IsLoadingUserDataDisabled())
	{
		for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
		{
			PlayerBindingData_SetAsLoaded(&_mPlayerBindings[i]);
		}
		InputBindings_SyncAllBindingsToAllPlayers();
		if (bindingsAreMirrored)
		{
			UpdateAllBindingsToMatchPlayerOne();
		}
		return;
	}

	int32_t len = PLAYERBINDINGS_LEN;
	if (bindingsAreMirrored)
	{
		len = 1;
	}
	bool isDone = true;
	for (int32_t i = 0; i < len; i += 1)
	{
		PlayerBindingData* player = &_mPlayerBindings[i];
		if (!PlayerBindingData_HasLoaded(player))
		{
			PlayerBindingData_Load(player);
			if (!PlayerBindingData_HasLoaded(player))
			{
				isDone = false;
			}
		}
	}

	if (isDone)
	{
		for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
		{
			PlayerBindingData_SetAsLoaded(&_mPlayerBindings[i]);
		}
		if (bindingsAreMirrored)
		{
			UpdateAllBindingsToMatchPlayerOne();
		}
		InputBindings_SyncAllBindingsToAllPlayers();
	}
}
void InputBindings_SaveAllBindings(void)
{
	if (Cvars_GetAsBool(CVARS_ENGINE_SAVING_SAVES_DISABLED) || Globals_IsSavingUserDataDisabled())
	{
		InputBindings_SyncAllBindingsToAllPlayers();
		return;
	}

	bool bindingsAreMirrored = !IsMirroringBindingsDisabled();
	int32_t len = PLAYERBINDINGS_LEN;
	if (bindingsAreMirrored)
	{
		len = 1;
	}
	for (int32_t i = 0; i < len; i += 1)
	{
		PlayerBindingData_Save(&_mPlayerBindings[i]);
	}

	if (bindingsAreMirrored)
	{
		UpdateAllBindingsToMatchPlayerOne();
	}
	InputBindings_SyncAllBindingsToAllPlayers();
}
void InputBindings_ResetAllPlayerCertainBindingsToDefaultAndSave(IStringArray* actions, int32_t indexStart, int32_t length)
{
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		PlayerBindingData_ResetCertainBindingsToDefault(&_mPlayerBindings[i], actions, indexStart, length);
	}

	InputBindings_SaveAllBindings();
}
bool InputBindings_DoBindingsExistFor(int32_t player, const char* action, int32_t indexStart, int32_t length)
{
	return PlayerBindingData_DoBindingsExistFor(&_mPlayerBindings[player], action, indexStart, length);
}
void InputBindings_SyncAllBindingsToAllPlayers(void)
{
	for (int32_t i = 0; i < PLAYERBINDINGS_LEN; i += 1)
	{
		CopyActions(PlayerBindingData_GetBindings(&_mPlayerBindings[i]), 
			InputPlayer_GetActionsForBindingsSync(Input_GetPlayer(i)));
	}
}
InputAction* InputBindings_GetBindings(int32_t player)
{
	return PlayerBindingData_GetBindings(&_mPlayerBindings[player]);
}
InputAction* InputBindings_GetActionFromBindings(int32_t player, const char* name)
{
	return  InputBindings_GetActionFromArray(PlayerBindingData_GetBindings(&_mPlayerBindings[player]), name);
}
InputAction* InputBindings_GetActionFromArray(InputAction* actions, const char* name)
{
	for (int32_t i = 0; i < PLAYERBINDINGDATA_BINDINGS_LEN; i += 1)
	{
		InputAction* action = &actions[i];
		if (Utils_StringEqualTo(action->mName, name))
		{
			return action;
		}
	}

	MString* tempStr = NULL;
	MString_Combine2(&tempStr, "Unable to find action: ", name);
	Logger_LogError(MString_Text(tempStr));
	MString_Dispose(&tempStr);
	return &INPUTACTION_DUMMY_ACTION;
}
