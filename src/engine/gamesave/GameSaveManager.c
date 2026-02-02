/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameSaveManager.h"

#include "../utils/Macros.h"
#include "../core/GameHelper.h"
#include "../core/Game.h"
#include "../utils/Logger.h"
#include "../utils/Cvars.h"
#include "../utils/Utils.h"
#include "../service/Service.h"
#include "../globals/Globals.h"
#include "../io/FixedByteBuffer.h"
#include "../io/DynamicByteBuffer.h"
#include "../io/BufferReader.h"

#define CONTAINER_DISPLAY_NAME "GameSave"
#define CONTAINER_NAME "GameSaveContainer"

static void* _mData;
static bool _mWasDataJustLoaded;
static bool _mHasLoaded;
static bool _mHasInit;

void GameSaveManager_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mData = GameHelper_CreateGameSaveData();

	_mHasInit = true;
}
bool GameSaveManager_HasInit(void)
{
	return _mHasInit;
}
static FixedByteBuffer* CreateBufferFromSaveData(void)
{
	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	GameHelper_WriteGameSaveData(_mData, dbb);
	return DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
}
void* GameSaveManager_GetCurrentSaveData(void)
{
	return _mData;
}
void GameSaveManager_ResetSaveData(void)
{
	if (_mData != NULL)
	{
		Utils_free(_mData);
		_mData = NULL;
	}

	_mData = GameHelper_CreateGameSaveData();
}
void GameSaveManager_Save(void)
{
	if (Globals_IsSavingUserDataDisabled())
	{
		return;
	}

	Utils_JustSaved();

	FixedByteBuffer* fbb = CreateBufferFromSaveData();
	Service_SaveBuffer(true, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, GLOBAL_DEF_SAVE_FILE_NAME, fbb);
	FixedByteBuffer_Dispose(fbb);

	Logger_LogInformation("Save file saved to save0.bin");
}
void GameSaveManager_Load(void)
{
	_mWasDataJustLoaded = false;

	if (Globals_IsLoadingUserDataDisabled())
	{
		_mHasLoaded = true;
		Logger_LogInformation("Loading game save data is disabled");
		return;
	}

	BufferRequest request = Service_AskToRetrieveBuffer(true, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, GLOBAL_DEF_SAVE_FILE_NAME);
	if (request.mIsBufferReady)
	{
		_mHasLoaded = true;
		if (request.mBuffer == NULL)
		{
			Logger_LogInformation("No game save data to load...");
			return;
		}
		else
		{
			GameSaveManager_ResetSaveData();
			BufferReader* reader = BufferReader_Create(request.mBuffer);
			GameHelper_ReadGameSaveData(_mData, reader);
			BufferReader_Dispose(reader);
			_mWasDataJustLoaded = true;
			Logger_LogInformation("Loaded game save data successfully!");
			return;
		}
	}
}
bool GameSaveManager_HasLoaded(void)
{
	return _mHasLoaded;
}
void GameSaveManager_ResetWasSaveDataJustLoaded(void)
{
	_mWasDataJustLoaded = false;
}
bool GameSaveManager_WasSaveDataJustLoaded(void)
{
	return _mWasDataJustLoaded;
}
