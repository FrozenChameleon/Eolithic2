/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"
#include "LeaderboardEntry.h"
#include "LeaderboardRequestData.h"

//supposed to be private
bool Service_IsLeaderboardRequestQueued(void);
bool Service_IsLeaderboardRateLimited(void);
int32_t Service_GetPageThatYouAreOn(void);

//supposed to be protected
void Service_NullLeaderboardEntries(void);
void Service_CreateLeaderboardEntries(void);
int32_t Service_GetLeaderboardAmountOfRowsToRetrieve(void);
void Service_SetLeaderboardSendStatus(int32_t status);
void Service_SetLeaderboardSendStatus2(int32_t status, int32_t frames);
void Service_UpdateScoreKeepBestHelper(const char* leaderboard, int32_t score, bool isAscending, bool isMilliseconds);
const char* Service_GetCurrentLeaderboardName(void);
int32_t Service_GetCurrentLeaderboardScope(void);
bool Service_IsCurrentLeaderboardAscending(void);
bool Service_IsCurrentLeaderboardMilliseconds(void);
int32_t Service_GetHighestPossiblePage(void);
void Service_RetrieveLeaderboardEntriesHelper(int32_t leaderboardPage, bool isInitialRequest, int32_t direction);
int32_t Service_GetMinimumRank(void);
int32_t Service_GetMaximumRank(void);
void Service_ReadyLeaderboard(const LeaderboardEntry* entries, size_t entries_len);
const LeaderboardRequestData* Service_GetLeaderboardRequestData(void);

//Supposed to be public
typedef enum ServiceLeaderboardSendStatus
{
	SERVICE_LEADERBOARD_SEND_STATUS_NOTHING = 0,
	SERVICE_LEADERBOARD_SEND_STATUS_SENDING = 1,
	SERVICE_LEADERBOARD_SEND_STATUS_SENT = 2,
	SERVICE_LEADERBOARD_SEND_STATUS_FAILED = 3
} ServiceLeaderboardSendStatus;

typedef enum ServiceLeaderboardScope
{
	SERVICE_LEADERBOARD_SCOPE_GLOBAL = 0,
	SERVICE_LEADERBOARD_SCOPE_USER = 1,
	SERVICE_LEADERBOARD_SCOPE_FRIENDS = 2
} ServiceLeaderboardScope;

typedef enum ServiceLeaderboardQueueState
{
	SERVICE_LEADERBOARD_QUEUE_STATE_NOTHING = 0,
	SERVICE_LEADERBOARD_QUEUE_STATE_NEW_REQUEST = 1,
	SERVICE_LEADERBOARD_QUEUE_STATE_NEXT_PAGE = 2
} ServiceLeaderboardQueueState;

void Service_ResetLeaderboardRequestStuff(void);
void Service_SetSilenceLeaderboardSendStatusNotification(bool value);
void Service_FlushStatsToServiceForPlatformX(void);
void Service_SetLeaderboardAmountOfRowsToRetrieve(int32_t range);
bool Service_IsSimplifiedLeaderboard(void);
bool Service_IsShowingLeaderboardCannotConnectError(void);
void Service_DisableLeaderboards(void);
bool Service_AreLeaderboardDisabled(void);
void Service_UpdateScoreKeepBest(const char* leaderboard, int32_t score, bool isAscending, bool isMilliseconds);
void Service_RetrieveLeaderboardEntries(const char* leaderboard, int32_t scopeType, bool isAscending, bool isMilliseconds);
int32_t Service_CheckLeaderboardSendStatus(bool update);
void Service_SimulateSendingLeaderboardStatus(int32_t frames, int32_t status);
bool Service_IsLeaderboardRetrievalInProgress(void);
LeaderboardEntry* Service_GetLeaderboardEntries(void);
LeaderboardEntry Service_GetLeaderboardEntry(int32_t i);
bool Service_IsLeaderboardReady(void);
int64_t Service_GetLeaderboardEntryCount(void);
int32_t Service_GetCurrentHighestRank(void);
int32_t Service_GetCurrentLowestRank(void);
bool Service_IsLeaderboardNullOrEmpty(void);
bool Service_CanLeaderboardGoLeft(void);
bool Service_CanLeaderboardGoRight(void);
void Service_LeaderboardGoRight(void);
void Service_LeaderboardGoLeft(void);
double Service_GetPlatformLeaderboardDelayTime(void);
bool Service_PlatformDoesNotAllowLeaderboardScrollingForFriendScope(void);
void Service_TurnOffLeaderboardCannotConnectError(void);
bool Service_LeaveTheLeaderboardMenuRightNow(void);
void Service_TellServiceIfOnLeaderboardMenuRightNow(bool isOnLeaderboardMenuRightNow);
bool Service_IsOnLeaderboardMenuRightNow(void);