/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Service.h"

#include "../utils/Macros.h"
#include "SDL3/SDL.h"
#include "LeaderboardRequestData.h"
#include "../io/File.h"
#include "../utils/Logger.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"

#define DEFAULT_SEND_STATUS_TIME 180

//Achievements
static bool _mIsOnLeaderboardMenuRightNow;
static int32_t _mAchievementMapLength;
static int32_t* _mAchievementMap;

//Leaderboard Retrieving
static int32_t _mLeaderboardAmountOfRowsToRetrieve;
static int32_t _mLeaderboardRequestQueueState;
static int32_t _mLeaderboardRequestQueuedPage;
static int32_t _mLeaderboardRequestDirection;
static double _mLeaderboardRequestDelayCounter;
static LeaderboardRequestData _mLeaderboardRequest;
static LeaderboardEntry* arr_leaderboard_entries;
static bool _mIsTheLeaderboardReady;

//Leaderboard Sending
static bool _mSilenceLeaderboardStatusNotifications;
static bool _mAreLeaderboardDisabled;
static int32_t _mLeaderboardSendStatus;
static int32_t _mLeaderboardSendStatusFrames;

//private
bool Service_IsLeaderboardRequestQueued(void)
{
	return (_mLeaderboardRequestQueueState != SERVICE_LEADERBOARD_QUEUE_STATE_NOTHING);
}
bool Service_IsLeaderboardRateLimited(void)
{
	return _mLeaderboardRequestDelayCounter < Service_GetPlatformLeaderboardDelayTime();
}
int32_t Service_GetPageThatYouAreOn(void)
{
	int32_t adjustedLowerRank = Service_GetCurrentLowestRank() - 1;
	int32_t page = adjustedLowerRank / Service_GetLeaderboardAmountOfRowsToRetrieve();
	return page;
}

//protected
void Service_NullLeaderboardEntries(void)
{
	_mIsTheLeaderboardReady = false;
	arrsetlen(arr_leaderboard_entries, 0);
}
void Service_CreateLeaderboardEntries(void)
{
	arrsetlen(arr_leaderboard_entries, 0);
}
int32_t Service_GetLeaderboardAmountOfRowsToRetrieve(void)
{
	return _mLeaderboardAmountOfRowsToRetrieve;
}
void Service_SetLeaderboardSendStatus(int32_t status)
{
	Service_SetLeaderboardSendStatus2(status, DEFAULT_SEND_STATUS_TIME);
}
void Service_SetLeaderboardSendStatus2(int32_t status, int32_t frames)
{
	if (_mSilenceLeaderboardStatusNotifications)
	{
		return;
	}

	_mLeaderboardSendStatus = status;
	_mLeaderboardSendStatusFrames = frames;
}
const int32_t* Service_GetAchievementMap(void)
{
	return _mAchievementMap;
}
const char* Service_GetCurrentLeaderboardName(void)
{
	return _mLeaderboardRequest.mName;
}
int32_t Service_GetCurrentLeaderboardScope(void)
{
	return _mLeaderboardRequest.mScopeType;
}
bool Service_IsCurrentLeaderboardAscending(void)
{
	return _mLeaderboardRequest.mIsAscending;
}
bool Service_IsCurrentLeaderboardMilliseconds(void)
{
	return _mLeaderboardRequest.mIsMilliseconds;
}
int32_t Service_GetHighestPossiblePage(void)
{
	return Service_GetMaximumRank() / Service_GetLeaderboardAmountOfRowsToRetrieve();
}
void Service_ReadyLeaderboard(const LeaderboardEntry* entries, size_t entries_len)
{
	arrsetlen(arr_leaderboard_entries, 0);
	for (int32_t i = 0; i < entries_len; i += 1)
	{
		arrput(arr_leaderboard_entries, entries[i]);
	}
	_mIsTheLeaderboardReady = true;
}
const LeaderboardRequestData* Service_GetLeaderboardRequestData(void)
{
	return &_mLeaderboardRequest;
}
BufferRequest Service_AskToRetrieveBufferForPC(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path)
{
	BufferRequest request = { 0 };
	request.mIsBufferReady = true;

	const char* prefPath = File_GetPrefPath();
	SDL_IOStream* rwop = NULL;

	{
		MString* savePath = NULL;
		File_PathCombine2(&savePath, prefPath, path);
		rwop = SDL_IOFromFile(MString_Text(savePath), "rb");
		MString_Dispose(&savePath);
	}

	if (rwop == NULL) //Missing file
	{
		Logger_LogWarning("Retrieve buffer: missing file!");
		Logger_LogWarning(path);
		return request;
	}

	int64_t size = SDL_GetIOSize(rwop);
	if (size == -1) //???
	{
		Logger_LogWarning("Retrieve buffer: something went wrong!");
		Logger_LogWarning(path);
		return request;
	}

	FixedByteBuffer* buffer = FixedByteBuffer_Create(size);
	SDL_ReadIO(rwop, FixedByteBuffer_GetBuffer(buffer), FixedByteBuffer_GetLength(buffer));
	SDL_CloseIO(rwop);

	request.mBuffer = buffer;

	Logger_LogInformation("Retrieve buffer: loaded the buffer!");
	Logger_LogInformation(path);

	return request;
}
int32_t Service_SaveBufferForPC(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path, FixedByteBuffer* buffer)
{
	const char* prefPath = File_GetPrefPath();
	SDL_IOStream* rwop = NULL;
	{
		MString* savePath = NULL;
		File_PathCombine2(&savePath, prefPath, path);
		rwop = SDL_IOFromFile(MString_Text(savePath), "wb");
		MString_Dispose(&savePath);
	}

	if (rwop == NULL)
	{
		Logger_LogWarning("Save buffer: unable to open!");
		Logger_LogWarning(path);
		return -1;
	}

	SDL_WriteIO(rwop, FixedByteBuffer_GetBuffer(buffer), FixedByteBuffer_GetLength(buffer));
	SDL_CloseIO(rwop);

	Logger_LogInformation("Save buffer: saved the buffer!");
	Logger_LogInformation(path);

 	return 0;
}

//public
bool Service_IsPlatformSteam(void)
{
	return (Service_GetPlatformType() == PLATFORM_STEAM);
}
void Service_ResetLeaderboardRequestStuff(void)
{
	_mIsTheLeaderboardReady = false;
	arrsetlen(arr_leaderboard_entries, 0);
	_mLeaderboardRequestQueueState = SERVICE_LEADERBOARD_QUEUE_STATE_NOTHING;
	_mLeaderboardRequestQueuedPage = 0;
	Utils_memset(&_mLeaderboardRequest, 0, sizeof(LeaderboardRequestData));
}
void Service_SetSilenceLeaderboardSendStatusNotification(bool value)
{
	_mSilenceLeaderboardStatusNotifications = value;
}
void Service_SetLeaderboardAmountOfRowsToRetrieve(int32_t range)
{
	_mLeaderboardAmountOfRowsToRetrieve = range;
}
void Service_SetAchievementMap(int32_t* achievementMap, int32_t achievementMapLength)
{
	_mAchievementMap = achievementMap;
	_mAchievementMapLength = achievementMapLength;
}
void Service_Update(double delta)
{
	if (Service_IsLeaderboardRateLimited())
	{
		_mLeaderboardRequestDelayCounter += delta;
	}
	else
	{
		if (Service_IsLeaderboardRequestQueued())
		{
			if (_mLeaderboardRequestQueueState == SERVICE_LEADERBOARD_QUEUE_STATE_NEW_REQUEST)
			{
				Service_RetrieveLeaderboardEntriesHelper(0, true, 0);
			}
			else if (_mLeaderboardRequestQueueState == SERVICE_LEADERBOARD_QUEUE_STATE_NEXT_PAGE)
			{
				Service_RetrieveLeaderboardEntriesHelper(_mLeaderboardRequestQueuedPage, false, _mLeaderboardRequestDirection);
			}
			_mLeaderboardRequestQueueState = SERVICE_LEADERBOARD_QUEUE_STATE_NOTHING;
			_mLeaderboardRequestDelayCounter = 0;
		}
	}

	Service_UpdateHelper(delta);
}
void Service_DisableLeaderboards(void)
{
#ifdef DEBUG_DEF_DISABLE_RECORDING_SAFETY_LEADERBOARDS
	//DO NOTHING
#else
	_mAreLeaderboardDisabled = true;
#endif
}
bool Service_AreLeaderboardDisabled(void)
{
	return _mAreLeaderboardDisabled;
}
void Service_UpdateScoreKeepBest(const char* leaderboard, int32_t score, bool isAscending, bool isMilliseconds)
{
	if (_mAreLeaderboardDisabled)
	{
		return;
	}

	Service_UpdateScoreKeepBestHelper(leaderboard, score, isAscending, isMilliseconds);
}
void Service_RetrieveLeaderboardEntries(const char* leaderboard, int32_t scopeType, bool isAscending, bool isMilliseconds)
{
	if (_mAreLeaderboardDisabled)
	{
		return;
	}

	if (!Service_HasSignedIn())
	{
		return;
	}

	if (Service_IsLeaderboardRetrievalInProgress())
	{
		return;
	}

	if ((leaderboard == _mLeaderboardRequest.mName) && (scopeType == _mLeaderboardRequest.mScopeType) &&
		(isAscending == _mLeaderboardRequest.mIsAscending) && (isMilliseconds == _mLeaderboardRequest.mIsMilliseconds))
	{
		return;
	}

	Service_NullLeaderboardEntries();

	Utils_strlcpy(_mLeaderboardRequest.mName, leaderboard, EE_FILENAME_MAX);
	_mLeaderboardRequest.mScopeType = scopeType;
	_mLeaderboardRequest.mIsAscending = isAscending;
	_mLeaderboardRequest.mIsMilliseconds = isMilliseconds;

	_mLeaderboardRequestQueueState = SERVICE_LEADERBOARD_QUEUE_STATE_NEW_REQUEST;

	if (Service_IsLeaderboardRateLimited()) //Player is requesting new leaderboard entries too fast
	{
		_mLeaderboardRequestDelayCounter = 0;
	}
}
int32_t Service_CheckLeaderboardSendStatus(bool update)
{
	if (_mLeaderboardSendStatusFrames > 0)
	{
		if (update)
		{
			_mLeaderboardSendStatusFrames -= 1;
		}
	}
	else
	{
		_mLeaderboardSendStatus = SERVICE_LEADERBOARD_SEND_STATUS_NOTHING;
	}
	return _mLeaderboardSendStatus;
}
void Service_SimulateSendingLeaderboardStatus(int32_t frames, int32_t status)
{
	_mLeaderboardSendStatusFrames = frames;
	_mLeaderboardSendStatus = status;
}
LeaderboardEntry* Service_GetLeaderboardEntries(void)
{
	return arr_leaderboard_entries;
}
LeaderboardEntry Service_GetLeaderboardEntry(int32_t i)
{
	return arr_leaderboard_entries[i];
}
bool Service_IsLeaderboardReady(void)
{
	if (!_mIsTheLeaderboardReady)
	{
		return false;
	}
	if (Service_IsLeaderboardRequestQueued())
	{
		return false;
	}
	if (Service_IsShowingLeaderboardCannotConnectError())
	{
		return false;
	}
	if (Service_IsLeaderboardRetrievalInProgress())
	{
		return false;
	}
	return true;
}
int64_t Service_GetLeaderboardEntryCount(void)
{
	return arrlen(arr_leaderboard_entries);
}
int32_t Service_GetCurrentHighestRank(void)
{
	if (!_mIsTheLeaderboardReady)
	{
		return 0;
	}

	int32_t highest = INT_MIN;
	for (int32_t i = 0; i < arrlen(arr_leaderboard_entries); i += 1)
	{
		int32_t rank = arr_leaderboard_entries[i].mGlobalRank;
		if (rank > highest)
		{
			highest = rank;
		}
	}
	return highest;
}
int32_t Service_GetCurrentLowestRank(void)
{
	if (!_mIsTheLeaderboardReady)
	{
		return 0;
	}

	int32_t lowest = INT_MAX;
	for (int32_t i = 0; i < arrlen(arr_leaderboard_entries); i += 1)
	{
		int32_t rank = arr_leaderboard_entries[i].mGlobalRank;
		if (rank < lowest)
		{
			lowest = rank;
		}
	}
	return lowest;
}
bool Service_IsLeaderboardNullOrEmpty(void)
{
	if (!_mIsTheLeaderboardReady)
	{
		return true;
	}

	if (arrlen(arr_leaderboard_entries) == 0)
	{
		return true;
	}

	return false;
}
bool Service_CanLeaderboardGoLeft(void)
{
	if ((_mLeaderboardRequest.mScopeType == SERVICE_LEADERBOARD_SCOPE_FRIENDS) && Service_PlatformDoesNotAllowLeaderboardScrollingForFriendScope())
	{
		return false;
	}

	if (Service_IsLeaderboardNullOrEmpty() || !Service_IsLeaderboardReady())
	{
		return false;
	}

	if (Service_GetCurrentLowestRank() <= Service_GetMinimumRank())
	{
		return false;
	}

	return true;
}
bool Service_CanLeaderboardGoRight(void)
{
	if ((_mLeaderboardRequest.mScopeType == SERVICE_LEADERBOARD_SCOPE_FRIENDS) && Service_PlatformDoesNotAllowLeaderboardScrollingForFriendScope())
	{
		return false;
	}

	if (Service_IsLeaderboardNullOrEmpty() || !Service_IsLeaderboardReady())
	{
		return false;
	}

	if (Service_GetCurrentHighestRank() >= Service_GetMaximumRank())
	{
		return false;
	}

	return true;
}
void Service_LeaderboardGoRight(void)
{
	if (!Service_CanLeaderboardGoRight())
	{
		return;
	}

	int32_t leaderboardPage = Service_GetPageThatYouAreOn();
	leaderboardPage += 1;
	int32_t highestPossiblePage = Service_GetHighestPossiblePage();
	if (leaderboardPage > highestPossiblePage)
	{
		leaderboardPage = highestPossiblePage;
	}
	Service_NullLeaderboardEntries();
	_mLeaderboardRequestQueuedPage = leaderboardPage;
	_mLeaderboardRequestQueueState = SERVICE_LEADERBOARD_QUEUE_STATE_NEXT_PAGE;
	_mLeaderboardRequestDirection = 1;
}
void Service_LeaderboardGoLeft(void)
{
	if (!Service_CanLeaderboardGoLeft())
	{
		return;
	}

	int32_t leaderboardPage = Service_GetPageThatYouAreOn();
	leaderboardPage -= 1;
	if (leaderboardPage < 0)
	{
		leaderboardPage = 0;
	}
	Service_NullLeaderboardEntries();
	_mLeaderboardRequestQueuedPage = leaderboardPage;
	_mLeaderboardRequestQueueState = SERVICE_LEADERBOARD_QUEUE_STATE_NEXT_PAGE;
	_mLeaderboardRequestDirection = -1;
}
void Service_LogMessage(const char* message)
{
	Logger_LogInformation(message);
}
void Service_TellServiceIfOnLeaderboardMenuRightNow(bool isOnLeaderboardMenuRightNow)
{
	_mIsOnLeaderboardMenuRightNow = isOnLeaderboardMenuRightNow;
}
bool Service_IsOnLeaderboardMenuRightNow(void)
{
	return _mIsOnLeaderboardMenuRightNow;
}