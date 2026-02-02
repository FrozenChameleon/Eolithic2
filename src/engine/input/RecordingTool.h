/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../utils/IStringArray.h"
#include "../utils/MString.h"

typedef struct SpriteBatch SpriteBatch;

typedef struct RecordingPlayerData
{
	bool mBool0;
	bool mBool1;
	bool mBool2;
	bool mBool3;
	bool mBool4;
	bool mBool5;
	bool mBool6;
	bool mBool7;
	bool mBool8;
	bool mBool9;
	bool mBool10;
	bool mBool11;
	bool mBool12;
	bool mBool13;
	bool mBool14;
	bool mBool15;
	Vector2 mLeftAxis;
	Vector2 mRightAxis;
	float mLeftTrigger;
	float mRightTrigger;
} RecordingPlayerData;

typedef struct RecordingData
{
	bool mIsInUse;
	RecordingPlayerData mPlayerOne;
	RecordingPlayerData mPlayerTwo;
	RecordingPlayerData mPlayerThree;
	RecordingPlayerData mPlayerFour;
} RecordingData;

typedef struct PerMapReaderData
{
	int32_t mLowestFps;
	int32_t mPlayerReadCounter;
	int32_t mReaderFailsafe;
	bool mWasSessionSuccessCounterTicked;
	bool mIsAtEndOfRecording;
} PerMapReaderData;

typedef struct ReaderData
{
	PerMapReaderData mPerLevelData;
	int32_t mSessionCounter;
	bool mIsReadyForNext;
	char mCurrentRecording[EE_FILENAME_MAX];
	IStringArray* mRecordings;
} ReaderData;

typedef struct HeaderData
{
	char mLevelFileName[EE_FILENAME_MAX];
	int32_t mAmountOfPlayers;
	int32_t mBoolLength;
	bool mUseLeftAxis;
	bool mUseRightAxis;
	bool mUseLeftTrigger;
	bool mUseRightTrigger;
	int32_t mSuccessNumber;
	int32_t mRecordingVersion;
} HeaderData;

enum FromArguments
{
	RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_OFF = 0,
	RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_NORMAL = 1,
	RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_FAST = 2,
	RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_NORMAL = 3,
	RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_FAST = 4
};

const char* RecordingTool_GetCurrentRecordingName(void);
bool RecordingTool_IsFromArgumentsPlaybackEnabled(void);
void RecordingTool_EnableFromArgumentsPlayback(int32_t state);
bool RecordingTool_IsDisplayingSessionReadout(void);
void RecordingTool_DrawReadSessionReadout(SpriteBatch* spriteBatch, const char* font);
void RecordingTool_CheckForDebugReadSessionCode(void);
bool RecordingTool_IsWriting(void);
bool RecordingTool_IsReading(void);
RecordingPlayerData* RecordingTool_Get(int32_t amountOfPlayers, int32_t playerNumber, int32_t boolLength, bool useLeftAxis, bool useRightAxis, bool useLeftTrigger, bool useRightTrigger);
int32_t RecordingTool_GetPlayerReadCounter(void);
void RecordingTool_SetPlayerReadCounter(int32_t value);
bool RecordingTool_IsRunning(void);
void RecordingTool_SetupWriteSession(int32_t priority, const char* mapToLoad);
bool RecordingTool_LoadNextRecordingIfAtEndOfRecording(bool isMapLoad);
void RecordingTool_SetupReadSession(IStringArray* givenRecordings, int32_t priority, bool goFast);
bool RecordingTool_IsGoFastFlagSet(void);
void RecordingTool_TickReadSessionSuccessCounter(int32_t successNumber);
void RecordingTool_RewriteAllRecordings(void);
int32_t RecordingTool_GetCurrentRecordingVersion(void);