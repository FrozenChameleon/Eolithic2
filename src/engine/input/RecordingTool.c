/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "RecordingTool.h"

#include "../utils/Macros.h"
#include "../globals/Globals.h"
#include "../io/BufferReader.h"
#include "../utils/Cvars.h"
#include "../utils/Logger.h"
#include "../gamesave/GameSaveManager.h"
#include "../gamestate/GameStateManager.h"
#include "../render/SpriteBatch.h"
#include "../core/GameUpdater.h"
#include "../render/DrawTool.h"
#include "../input/Input.h"
#include "../input/InputAction.h"
#include "../input/ActionList.h"
#include "../io/File.h"
#include "../service/Service.h"
#include "../core/Game.h"
#include "../core/ServiceHelper.h"
//#include "../utils/Tuning.h" //UNUSED
#include "../utils/MString.h"
#include "../core/GameHelper.h"
#include "../../third_party/stb_ds.h"
#include "../render/Color.h"
#include "../utils/Utils.h"

enum ToolState
{
	TOOL_STATE_OFF = 0,
	TOOL_STATE_WRITING = 1,
	TOOL_STATE_READING = 2
};

enum
{
	TOOL_DATA_LENGTH = 60 * 60 * 60 * 3
};

#define READOUT_LIMIT_FOR_LOW_FPS 20

static int32_t _mToolState;
static int32_t _mCurrentPriority;
static int32_t _mPlayerSwitchCounter;
static RecordingData _mDummy;
static RecordingData* _mData;
static HeaderData _mHeaderData;
static ReaderData _mReaderData;
//static std_shared_ptr<OeIniWriter> _mWriter; //UNUSED
static int32_t _mDisplaySuccessCounter;
static MString* _mDisplaySuccessCounterStr;
static int32_t _mDisplayFailureCounter;
static MString* _mDisplayFailureCounterStr;
static IStringArray* _mDisplayReadout;
static MString* _mLastReadRecordingFilename;
static bool _mGoFastFlag;
static int32_t _mFromArgumentsPlaybackState;

static void InitData(void)
{
	if (_mData != NULL)
	{
		return;
	}

	_mData = (RecordingData*)Utils_calloc(60 * 60 * 60 * 3, sizeof(RecordingData)); //3 hours
	MString_AssignString(&_mDisplaySuccessCounterStr, "");
	MString_AssignString(&_mDisplayFailureCounterStr, "");
	MString_AssignString(&_mLastReadRecordingFilename, "");
	_mDisplayReadout = IStringArray_Create();
}

static void DisposeWriter(void)
{
	/*if (_mWriter == null) //UNUSED
	{
		return;
	}

	_mWriter.Dispose();
	_mWriter = null;*/
}
static int32_t GetAmountInUse(void)
{
	int32_t counter = 0;
	for (int32_t i = 0; i < TOOL_DATA_LENGTH; i += 1)
	{
		RecordingData* data = &_mData[i];
		if (data->mIsInUse)
		{
			counter += 1;
		}
		else
		{
			return counter;
		}
	}
	return counter;
}
static bool IsSoakTesting(void)
{
	return (_mFromArgumentsPlaybackState == RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_NORMAL) || 
		(_mFromArgumentsPlaybackState == RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_FAST);
}
static void ResetRecordingData(void)
{
	_mPlayerSwitchCounter = 0;
	Utils_memset(&_mDummy, 0, sizeof(RecordingData));
	Utils_memset(_mData, 0, sizeof(RecordingData) * TOOL_DATA_LENGTH);
}
static void ResetAllData(void)
{
	_mToolState = TOOL_STATE_OFF;
	_mCurrentPriority = 0;
	ResetRecordingData();
	Utils_memset(&_mHeaderData, 0, sizeof(HeaderData));
	Utils_memset(&_mReaderData, 0, sizeof(ReaderData));
	DisposeWriter();
}
static RecordingData* GetNext(void)
{
	if (!RecordingTool_IsRunning())
	{
		return &_mDummy;
	}
	for (int32_t i = 0; i < TOOL_DATA_LENGTH; i += 1)
	{
		RecordingData* data = &_mData[i];
		if (!data->mIsInUse)
		{
			data->mIsInUse = true;
			return data;
		}
	}
	return &_mDummy;
}
static void WriteData(void)
{
	/*if (_mWriter == null) //UNUSED
	{
	return;
	}

	int32_t len = GetAmountInUse();
	if (len == 0)
	{
	return;
	}

	_mWriter.WriteInt32(Globals.GetNewestRecordingVersion());
	_mWriter.WriteInt32(_mHeaderData.mAmountOfPlayers);
	_mWriter.WriteInt32(_mHeaderData.mBoolLength);
	_mWriter.WriteBoolean(_mHeaderData.mUseLeftAxis);
	_mWriter.WriteBoolean(_mHeaderData.mUseRightAxis);
	_mWriter.WriteBoolean(_mHeaderData.mUseLeftTrigger);
	_mWriter.WriteBoolean(_mHeaderData.mUseRightTrigger);
	_mWriter.WriteInt32(len);
	for (int32_t i = 0; i < len; i += 1)
	{
	ref RecordingData recordingData = ref _mData[i];
	for (int32_t j = 0; j < _mHeaderData.mAmountOfPlayers; j += 1)
	{
	ref RecordingPlayerData playerData = ref GetPlayerData(ref recordingData, j);
	if (_mHeaderData.mBoolLength > 0)
	{
	_mWriter.WriteBoolean(playerData.mBool0);
	}
	if (_mHeaderData.mBoolLength > 1)
	{
	_mWriter.WriteBoolean(playerData.mBool1);
	}
	if (_mHeaderData.mBoolLength > 2)
	{
	_mWriter.WriteBoolean(playerData.mBool2);
	}
	if (_mHeaderData.mBoolLength > 3)
	{
	_mWriter.WriteBoolean(playerData.mBool3);
	}
	if (_mHeaderData.mBoolLength > 4)
	{
	_mWriter.WriteBoolean(playerData.mBool4);
	}
	if (_mHeaderData.mBoolLength > 5)
	{
	_mWriter.WriteBoolean(playerData.mBool5);
	}
	if (_mHeaderData.mBoolLength > 6)
	{
	_mWriter.WriteBoolean(playerData.mBool6);
	}
	if (_mHeaderData.mBoolLength > 7)
	{
	_mWriter.WriteBoolean(playerData.mBool7);
	}
	if (_mHeaderData.mBoolLength > 8)
	{
	_mWriter.WriteBoolean(playerData.mBool8);
	}
	if (_mHeaderData.mBoolLength > 9)
	{
	_mWriter.WriteBoolean(playerData.mBool9);
	}
	if (_mHeaderData.mBoolLength > 10)
	{
	_mWriter.WriteBoolean(playerData.mBool10);
	}
	if (_mHeaderData.mBoolLength > 11)
	{
	_mWriter.WriteBoolean(playerData.mBool11);
	}
	if (_mHeaderData.mBoolLength > 12)
	{
	_mWriter.WriteBoolean(playerData.mBool12);
	}
	if (_mHeaderData.mBoolLength > 13)
	{
	_mWriter.WriteBoolean(playerData.mBool13);
	}
	if (_mHeaderData.mBoolLength > 14)
	{
	_mWriter.WriteBoolean(playerData.mBool14);
	}
	if (_mHeaderData.mBoolLength > 15)
	{
	_mWriter.WriteBoolean(playerData.mBool15);
	}
	if (_mHeaderData.mUseLeftAxis)
	{
	_mWriter.WriteSingle(playerData.mLeftAxis.X);
	_mWriter.WriteSingle(playerData.mLeftAxis.Y);
	}
	if (_mHeaderData.mUseRightAxis)
	{
	_mWriter.WriteSingle(playerData.mRightAxis.X);
	_mWriter.WriteSingle(playerData.mRightAxis.Y);
	}
	if (_mHeaderData.mUseLeftTrigger)
	{
	_mWriter.WriteSingle(playerData.mLeftTrigger);
	}
	if (_mHeaderData.mUseRightTrigger)
	{
	_mWriter.WriteSingle(playerData.mRightTrigger);
	}
	}
	}

	string lastFileName = _mHeaderData.mLevelFileName;
	string currentDate = DateTime.Now.ToString("yyyyMMddhhmmss");
	string targetFilename = currentDate + "_" + lastFileName + OeUtils.EXTENSION_BIN;
	string path = OeFile.Combine("recordings", targetFilename);
	if (toDataDirectory)
	{
	path = OeFile.Combine("data", "recordings", recordingFilenameWithoutExtension + ".bin");
	}
	Logger.LogInformation("Writing recording to " + path);
	if (toDataDirectory)
	{
	byte[] memory = ((MemoryStream)_mWriter.GetBinaryWriter().BaseStream).ToArray();
	using (OeIniWriter writer = new OeIniWriter(true, OeFile.Create(path)))
	{
	for (int32_t i = 0; i < memory.Length; i += 1)
	{
	writer.WriteByte(memory[i]);
	}
	}
	}
	else
	{
	#ifndef DISABLE_STORAGE_CONTAINER
	using (StorageContainer container = OeFile.OpenStorageContainer())
	{
	byte[] memory = ((MemoryStream)_mWriter.GetBinaryWriter().BaseStream).ToArray();
	using (OeIniWriter writer = new OeIniWriter(true, container.CreateFile(path)))
	{
	for (int32_t i = 0; i < memory.Length; i += 1)
	{
	writer.WriteByte(memory[i]);
	}
	}
	}
	#endif
	}


	DisposeWriter();*/
}
static void FinishPreviousWritingSession(void)
{
	if (_mToolState != TOOL_STATE_WRITING)
	{
		return;
	}

	WriteData();
}
static void ContinueReadSession(bool isFirst)
{
	if (!isFirst)
	{
		int32_t threshold = Cvars_GetAsInt(CVARS_ENGINE_RECORDING_FPS_THRESHOLD);
		if (_mReaderData.mPerLevelData.mLowestFps <= threshold)
		{
			Logger_LogInformation("Low FPS for:");
			Logger_LogInformation(MString_Text(_mLastReadRecordingFilename));
#ifndef DEBUG_DEF_DISABLE_LOW_FPS_READOUT_IN_RECORDING_TOOL
			if (!IsSoakTesting())
			{
				if (IStringArray_Length(_mDisplayReadout) < READOUT_LIMIT_FOR_LOW_FPS)
				{
					MString* tempString = NULL;
					MString_AssignString(&tempString, "Low FPS: ");
					MString_AddAssignString(&tempString, MString_Text(_mLastReadRecordingFilename));
					MString_AddAssignString(&tempString, ", FPS: ");
					MString_AddAssignInt(&tempString, _mReaderData.mPerLevelData.mLowestFps);
					IStringArray_Add(_mDisplayReadout, MString_Text(tempString));
					MString_Dispose(&tempString);
				}
			}
#endif
		}
		if (_mReaderData.mPerLevelData.mWasSessionSuccessCounterTicked)
		{
			_mDisplaySuccessCounter += 1;
			MString* tempString = NULL;
			MString_AssignString(&tempString, "SUCCESS: ");
			MString_AddAssignInt(&tempString, _mDisplaySuccessCounter);
			MString_AssignString(&_mDisplaySuccessCounterStr, MString_Text(tempString));
			_mReaderData.mPerLevelData.mWasSessionSuccessCounterTicked = false;
			MString_Dispose(&tempString);
		}
		else
		{
			_mDisplayFailureCounter += 1;
			MString* tempString = NULL;
			MString_AssignString(&tempString, "FAILURE: ");
			MString_AddAssignInt(&tempString, _mDisplayFailureCounter);
			MString* tempString2 = NULL;
			MString_AssignString(&tempString2, "FAIL: ");
			MString_AddAssignString(&tempString2, MString_Text(_mLastReadRecordingFilename));
			IStringArray_Add(_mDisplayReadout, MString_Text(tempString2));
			Logger_LogInformation("Session success miss for:");
			Logger_LogInformation(MString_Text(_mLastReadRecordingFilename));
			MString_Dispose(&tempString);
			MString_Dispose(&tempString2);
		}
	}

	ResetRecordingData();

	Logger_LogInformation("Reading next in session");
	for (int32_t i = 0; i < IStringArray_Length(_mReaderData.mRecordings); i += 1)
	{
		if (i == _mReaderData.mSessionCounter)
		{
			Utils_memset(&_mReaderData.mPerLevelData, 0, sizeof(PerMapReaderData));
			_mReaderData.mIsReadyForNext = true;
			Utils_strlcpy(_mReaderData.mCurrentRecording, IStringArray_Get(_mReaderData.mRecordings, i), EE_FILENAME_MAX);
			_mReaderData.mSessionCounter += 1;
			return;
		}
	}

	if (IsSoakTesting())
	{
		_mReaderData.mSessionCounter = 0;
		ContinueReadSession(true);
		return;
	}

	_mFromArgumentsPlaybackState = RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_OFF;

	IStringArray_Add(_mDisplayReadout, "Read session has ended");

	for (int32_t i = 0; i < IStringArray_Length(_mDisplayReadout); i += 1)
	{
		Logger_LogInformation(IStringArray_Get(_mDisplayReadout, i));
	}

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "SUCCESS:");
		MString_AddAssignInt(&tempString, _mDisplaySuccessCounter);
		Logger_LogInformation(MString_Text(tempString));

		MString_AssignString(&tempString, "FAILURE:");
		MString_AddAssignInt(&tempString, _mDisplayFailureCounter);
		Logger_LogInformation(MString_Text(tempString));

		MString_Dispose(&tempString);
	}

	ResetAllData();
	GameSaveManager_ResetSaveData();
	GameStateManager_LoadMap("777");
	GLOBALS_DEBUG_IS_GOD_MODE = false;
	_mGoFastFlag = false;
}
static RecordingPlayerData* GetPlayerData(RecordingData* recordingData, int32_t playerNumber)
{
	switch (playerNumber)
	{
	case 0:
		return &recordingData->mPlayerOne;
	case 1:
		return &recordingData->mPlayerTwo;
	case 2:
		return &recordingData->mPlayerThree;
	case 3:
		return &recordingData->mPlayerFour;
	}
	return &_mDummy.mPlayerOne;
}
static void WriteHeader(void)
{
	/*_mWriter = OeIniWriter(true, new MemoryStream()); //UNUSED
	_mWriter.WriteBoolean(Globals.DEBUG_IS_GOD_MODE);
	_mWriter.WriteInt32(OeGameStateManager.GetUniqueMapSeed());
	_mWriter.WriteInt32(OeTuning.GetCurrentDifficulty());
	_mWriter.WriteString(_mHeaderData.mLevelFileName);
	OeGameSaveManager.WriteAllForRecording(_mWriter);
	OeGame.GetHelper().WriteSessionData(_mWriter);
	Logger.LogInformation("Writing recording has begun for " + _mHeaderData.mLevelFileName);*/
}
static void Read(const char* recordingFilenameWithoutExtension)
{
	MString_AssignString(&_mLastReadRecordingFilename, recordingFilenameWithoutExtension);
	MString_AddAssignString(&_mLastReadRecordingFilename, ".bin");

	BufferReader* br = NULL;
	{
		MString* path = NULL;
		File_PathCombine3(&path, "data", "recordings", MString_Text(_mLastReadRecordingFilename));
		br = BufferReader_CreateFromPath(MString_Text(path));
		MString_Dispose(&path);
	}

	GLOBALS_DEBUG_IS_GOD_MODE = BufferReader_ReadBoolean(br);
	GameStateManager_SetUniqueMapSeed(BufferReader_ReadI32(br));
	BufferReader_ReadI32(br); //Tuning_SetCurrentDifficulty(BufferReader_ReadI32()); //ORIGINALLY WAS THIS, NOW UNUSED
	BufferReader_ReadString(br, _mHeaderData.mLevelFileName, EE_FILENAME_MAX);
#ifndef GLOBAL_DEF_RECORDING_TOOL_DO_NOT_WRITE_SAVE_FILES
	OeGameSaveManager_ResetAllDataAndReadAllForRecording(reader);
#endif
#ifndef GLOBAL_DEF_RECORDING_TOOL_DO_NOT_WRITE_SESSION_DATA
	OeGameHelper_ReadSessionData(reader);
#endif
	_mHeaderData.mRecordingVersion = BufferReader_ReadI32(br);
	if (_mHeaderData.mRecordingVersion >= 1)
	{
		_mHeaderData.mSuccessNumber = BufferReader_ReadI32(br);
	}
	_mHeaderData.mAmountOfPlayers = BufferReader_ReadI32(br);
	_mHeaderData.mBoolLength = BufferReader_ReadI32(br);
	_mHeaderData.mUseLeftAxis = BufferReader_ReadBoolean(br);
	_mHeaderData.mUseRightAxis = BufferReader_ReadBoolean(br);
	_mHeaderData.mUseLeftTrigger = BufferReader_ReadBoolean(br);
	_mHeaderData.mUseRightTrigger = BufferReader_ReadBoolean(br);
	int32_t len = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < len; i += 1)
	{
		RecordingData* recordingData = &_mData[i];
		recordingData->mIsInUse = true;
		for (int32_t j = 0; j < _mHeaderData.mAmountOfPlayers; j += 1)
		{
			RecordingPlayerData* playerData = GetPlayerData(recordingData, j);
			if (_mHeaderData.mBoolLength > 0)
			{
				playerData->mBool0 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 1)
			{
				playerData->mBool1 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 2)
			{
				playerData->mBool2 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 3)
			{
				playerData->mBool3 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 4)
			{
				playerData->mBool4 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 5)
			{
				playerData->mBool5 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 6)
			{
				playerData->mBool6 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 7)
			{
				playerData->mBool7 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 8)
			{
				playerData->mBool8 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 9)
			{
				playerData->mBool9 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 10)
			{
				playerData->mBool10 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 11)
			{
				playerData->mBool11 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 12)
			{
				playerData->mBool12 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 13)
			{
				playerData->mBool13 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 14)
			{
				playerData->mBool14 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mBoolLength > 15)
			{
				playerData->mBool15 = BufferReader_ReadBoolean(br);
			}
			if (_mHeaderData.mUseLeftAxis)
			{
				playerData->mLeftAxis.X = BufferReader_ReadFloat(br);
				playerData->mLeftAxis.Y = BufferReader_ReadFloat(br);
			}
			if (_mHeaderData.mUseRightAxis)
			{
				playerData->mRightAxis.X = BufferReader_ReadFloat(br);
				playerData->mRightAxis.Y = BufferReader_ReadFloat(br);
			}
			if (_mHeaderData.mUseLeftTrigger)
			{
				playerData->mLeftTrigger = BufferReader_ReadFloat(br);
			}
			if (_mHeaderData.mUseRightTrigger)
			{
				playerData->mRightTrigger = BufferReader_ReadFloat(br);
			}
		}
	}
}

const char* RecordingTool_GetCurrentRecordingName(void)
{
	return MString_Text(_mLastReadRecordingFilename);
}
bool RecordingTool_IsFromArgumentsPlaybackEnabled(void)
{
	return _mFromArgumentsPlaybackState != RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_OFF;
}
void RecordingTool_EnableFromArgumentsPlayback(int32_t state)
{
	Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards();
	_mFromArgumentsPlaybackState = state;
}
bool RecordingTool_IsDisplayingSessionReadout(void)
{
	if ((_mToolState == TOOL_STATE_READING) || (IStringArray_Length(_mDisplayReadout) > 0))
	{
		return true;
	}
	return false;
}
void RecordingTool_DrawReadSessionReadout(SpriteBatch* spriteBatch, const char* font)
{
	if (!RecordingTool_IsDisplayingSessionReadout())
	{
		return;
	}

	int32_t distY = 16;
	int32_t posY = distY;
	if (!MString_EqualToString(_mLastReadRecordingFilename, EE_STR_EMPTY) && (_mToolState == TOOL_STATE_READING))
	{
		SpriteBatch_DrawString(spriteBatch, font, MString_Text(_mLastReadRecordingFilename), Color_Yellow, 100, Vector2_Create(0, (float)posY));
		posY += distY;
		SpriteBatch_DrawString(spriteBatch, font, GameUpdater_GetFpsString(), Color_Yellow, 100, Vector2_Create(0, (float)posY));
		posY += distY;
	}

	if (!MString_EqualToString(_mDisplaySuccessCounterStr, EE_STR_EMPTY))
	{
		SpriteBatch_DrawString(spriteBatch, font, MString_Text(_mDisplaySuccessCounterStr), Color_Yellow, 100, Vector2_Create(0, (float)posY));
		posY += distY;
	}

	if (!MString_EqualToString(_mDisplayFailureCounterStr, EE_STR_EMPTY))
	{
		SpriteBatch_DrawString(spriteBatch, font, MString_Text(_mDisplayFailureCounterStr), Color_Yellow, 100, Vector2_Create(0, (float)posY));
		posY += distY;
	}

	Vector2 offset = Vector2_Create(0, (float)posY);
	for (int32_t i = 0; i < IStringArray_Length(_mDisplayReadout); i += 1)
	{
		const char* displayString = IStringArray_Get(_mDisplayReadout, i);
		Rectangle bounds = DrawTool_GetBounds(displayString, font);
		SpriteBatch_DrawString(spriteBatch, font, displayString, COLOR_BLACK, 100, Vector2_Sub(offset, Vector2_Create(-1, -1)));
		SpriteBatch_DrawString(spriteBatch, font, displayString, COLOR_YELLOW, 100, offset);
		offset.Y += bounds.Height + 4;
	}

	if (_mToolState != TOOL_STATE_READING)
	{
		if (Input_GetPlayerOneAction(ACTIONLIST_GAME_MENU_SELECT)->mIsPressed)
		{
			_mDisplayFailureCounter = 0;
			MString_AssignString(&_mDisplayFailureCounterStr, EE_STR_EMPTY);
			_mDisplaySuccessCounter = 0;
			MString_AssignString(&_mDisplaySuccessCounterStr, EE_STR_EMPTY);
			IStringArray_Clear(_mDisplayReadout);
		}
	}
}
void RecordingTool_CheckForDebugReadSessionCode(void)
{
	if (_mToolState == TOOL_STATE_READING)
	{
		return;
	}

	InputAction* action1 = Input_GetPlayerOneAction(ACTIONLIST_GAME_A);
	InputAction* action2 = Input_GetPlayerOneAction(ACTIONLIST_GAME_START);
	InputAction* action3 = Input_GetPlayerOneAction(ACTIONLIST_GAME_LS_UP);

	int32_t time = 60 * 5;
	if (action1->mTimeHeld > time && action2->mTimeHeld > time && action3->mTimeHeld > time)
	{
		RecordingTool_SetupReadSession(NULL, INT_MAX, false);
	}
}
bool RecordingTool_IsWriting(void)
{
	return _mToolState == TOOL_STATE_WRITING;
}
bool RecordingTool_IsReading(void)
{
	return _mToolState == TOOL_STATE_READING;
}
RecordingPlayerData* RecordingTool_Get(int32_t amountOfPlayers, int32_t playerNumber, int32_t boolLength, bool useLeftAxis, bool useRightAxis, bool useLeftTrigger, bool useRightTrigger)
{
	int32_t currentFps = GameUpdater_GetFPS();
	if (_mReaderData.mPerLevelData.mLowestFps == 0 || currentFps < _mReaderData.mPerLevelData.mLowestFps)
	{
		_mReaderData.mPerLevelData.mLowestFps = currentFps;
	}

	_mReaderData.mPerLevelData.mReaderFailsafe = 0;

	_mHeaderData.mAmountOfPlayers = amountOfPlayers;
	_mHeaderData.mBoolLength = boolLength;
	_mHeaderData.mUseLeftAxis = useLeftAxis;
	_mHeaderData.mUseRightAxis = useRightAxis;
	_mHeaderData.mUseLeftTrigger = useLeftTrigger;
	_mHeaderData.mUseRightTrigger = useRightTrigger;

	if (_mPlayerSwitchCounter >= amountOfPlayers)
	{
		_mPlayerSwitchCounter = 0;
	}
	bool isFirstGetInSequence = _mPlayerSwitchCounter == 0;
	bool isLastGetInSequence = _mPlayerSwitchCounter == amountOfPlayers - 1;
	_mPlayerSwitchCounter += 1;

	if (_mToolState == TOOL_STATE_WRITING)
	{
		if (isFirstGetInSequence)
		{
			return GetPlayerData(GetNext(), playerNumber);
		}
		else
		{
			int32_t top = GetAmountInUse() - 1;
			return GetPlayerData(&_mData[top], playerNumber);
		}
	}
	else if (_mToolState == TOOL_STATE_READING)
	{
		RecordingData* data = &_mData[_mReaderData.mPerLevelData.mPlayerReadCounter];
		if (data->mIsInUse)
		{
			if (isLastGetInSequence)
			{
				_mReaderData.mPerLevelData.mPlayerReadCounter += 1;
			}
			return GetPlayerData(data, playerNumber);
		}
		else
		{
			_mReaderData.mPerLevelData.mIsAtEndOfRecording = true;
		}
	}
	return &_mDummy.mPlayerOne;
}
int32_t RecordingTool_GetPlayerReadCounter(void)
{
	if (_mToolState != TOOL_STATE_READING)
	{
		return 0;
	}

	return _mReaderData.mPerLevelData.mPlayerReadCounter;
}
void RecordingTool_SetPlayerReadCounter(int32_t value)
{
	if (_mToolState != TOOL_STATE_READING)
	{
		return;
	}

	_mReaderData.mPerLevelData.mPlayerReadCounter = value;
}
bool RecordingTool_IsRunning(void)
{
	return _mToolState != TOOL_STATE_OFF;
}
void RecordingTool_SetupWriteSession(int32_t priority, const char* mapToLoad)
{
	if (!ServiceHelper_HasLoadedEverything())
	{
		return;
	}

	if (Utils_StringEqualTo(mapToLoad, "777")) //Press start screen
	{
		return;
	}

	if (!Globals_IsDebugFileMode() && !Cvars_GetAsBool(CVARS_ENGINE_ALLOW_RECORDINGS_IN_USER))
	{
		return;
	}

	if (Service_PlatformDisablesWritingRecordings())
	{
		return;
	}

	if (priority < _mCurrentPriority)
	{
		return;
	}

	InitData();

	FinishPreviousWritingSession();

	ResetAllData();

	_mCurrentPriority = priority;
	_mToolState = TOOL_STATE_WRITING;
	Utils_strlcpy(_mHeaderData.mLevelFileName, mapToLoad, EE_FILENAME_MAX);

	WriteHeader();
}
bool RecordingTool_LoadNextRecordingIfAtEndOfRecording(bool isMapLoad)
{
	if (_mToolState != TOOL_STATE_READING)
	{
		return false;
	}

	bool readyForNextRecording = false;

	_mReaderData.mPerLevelData.mReaderFailsafe += 1;
	if (_mReaderData.mPerLevelData.mReaderFailsafe >= 60 * 60 * 15) //15 minutes
	{
		Logger_LogInformation("No input retrievals for a long time, forcing next recording to play.");
		ContinueReadSession(false);
		readyForNextRecording = true;
	}
	else
	{
		if (isMapLoad || _mReaderData.mPerLevelData.mIsAtEndOfRecording)
		{
			RecordingData* data = &_mData[_mReaderData.mPerLevelData.mPlayerReadCounter];
			if (!data->mIsInUse)
			{
				ContinueReadSession(false);
				readyForNextRecording = true;
			}
		}
	}

	if (!_mReaderData.mIsReadyForNext)
	{
		if (readyForNextRecording) //At end, map 777 should have been loaded
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	_mToolState = TOOL_STATE_READING;
	Read(_mReaderData.mCurrentRecording);
	GameStateManager_LoadMap(_mHeaderData.mLevelFileName);
	_mReaderData.mIsReadyForNext = false;
	Logger_LogInformation("Reading recording has begun for:");
	Logger_LogInformation(_mHeaderData.mLevelFileName);
	return true;
}
void RecordingTool_SetupReadSession(IStringArray* givenRecordings, int32_t priority, bool goFast)
{
	IStringArray* recordingsToRead = NULL;
	if ((givenRecordings == NULL) || (IStringArray_Length(givenRecordings) == 0))
	{
		recordingsToRead = GameHelper_GetAllTheRecordings();
	}
	else
	{
		recordingsToRead = givenRecordings;
	}

	if (recordingsToRead == NULL)
	{
		return;
	}

	if (_mFromArgumentsPlaybackState != RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_OFF)
	{
		if ((_mFromArgumentsPlaybackState == RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_NORMAL) ||
			(_mFromArgumentsPlaybackState == RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_NORMAL))
		{
			goFast = false;
		}
		else
		{
			goFast = true;
		}
	}

	if (priority < _mCurrentPriority)
	{
		return;
	}

	InitData();

	if (goFast)
	{
		_mGoFastFlag = true;
	}

	FinishPreviousWritingSession();

	ResetAllData();

	_mReaderData.mRecordings = recordingsToRead;
	Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards();
	_mCurrentPriority = priority;
	_mToolState = TOOL_STATE_READING;
	Logger_LogInformation("Read session has begun");
	ContinueReadSession(true);
}
bool RecordingTool_IsGoFastFlagSet(void)
{
	return _mGoFastFlag;
}
void RecordingTool_TickReadSessionSuccessCounter(int32_t successNumber)
{
	if (_mToolState == TOOL_STATE_READING)
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Given success number is ");
		MString_AddAssignInt(&tempString, successNumber);
		Logger_LogInformation(MString_Text(tempString));

		MString_AssignString(&tempString, "Given success number is ");
		MString_AddAssignInt(&tempString, _mHeaderData.mSuccessNumber);
		if (successNumber == _mHeaderData.mSuccessNumber)
		{
			_mReaderData.mPerLevelData.mWasSessionSuccessCounterTicked = true;
			Logger_LogInformation("Session success counter ticked");
		}
		else
		{
			Logger_LogInformation("Success number mismatch");
		}

		MString_Dispose(&tempString);
	}
	else if (_mToolState == TOOL_STATE_WRITING)
	{
		_mHeaderData.mSuccessNumber = successNumber;
	}
}
void RecordingTool_RewriteAllRecordings(void)
{
	/*InitData(); //UNUSED
	string[] files = OeFile.GetFiles(OeFile.Combine("data", "recordings"));
	for (int32_t i = 0; i < files.Length; i += 1)
	{
		ResetAllData();
		string fileNameWithoutExtension = OeFile.GetFileNameWithoutExtension(files[i]);
		Read(fileNameWithoutExtension);
		WriteHeader();
		WriteData(true, fileNameWithoutExtension);
	}*/
}
int32_t RecordingTool_GetCurrentRecordingVersion(void)
{
	if (!RecordingTool_IsReading())
	{
		return Globals_GetNewestRecordingVersion();
	}
	else
	{
		return _mHeaderData.mRecordingVersion;
	}
}
