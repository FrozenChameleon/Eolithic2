/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifdef SERVICE_DUMMY

#include "SDL3/SDL.h"
#include "Service.h"
#include "../io/File.h"
#include "../core/Platform.h"
#include "../utils/Strings.h"

//protected
void Service_UpdateScoreKeepBestHelper(const char* leaderboard, int32_t score, bool isAscending, bool isMilliseconds)
{

}
int32_t Service_GetMinimumRank(void)
{
	return 0;
}
int32_t Service_GetMaximumRank(void)
{
	return 0;
}
void Service_RetrieveLeaderboardEntriesHelper(int32_t leaderboardPage, bool isInitialRequest, int32_t direction)
{
}

//public
void Service_Init(void)
{
}
BufferRequest Service_AskToRetrieveBuffer(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName,
	const char* path)
{
	return Service_AskToRetrieveBufferForPC(isPurelyGameSaveData, containerDisplayName, containerName, path);
}
void Service_SaveBuffer(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path,
	FixedByteBuffer* buffer)
{
	Service_SaveBufferForPC(isPurelyGameSaveData, containerDisplayName, containerName, path, buffer);
}
const char* Service_GetSignedInUserName(void)
{
	return "NOT_SET";
}
void Service_SignIn(bool trySilentSignIn, bool forceSignIn)
{
}
bool Service_HasSignedIn(void)
{
	return true;
}
void Service_HandleException(const char* e)
{
}
void Service_HandleSetAchievement(int32_t i, const char* achievement)
{
}
void Service_HandleApplicationExit(void)
{
}
void Service_Create(void)
{
}
void Service_UpdateHelper(double delta)
{
}
void Service_Resize(int32_t width, int32_t height)
{
}
void Service_Pause(void)
{
}
void Service_Resume(void)
{
}
void Service_Dispose(void)
{
}
bool Service_IsOverlayEnabled(void)
{
	return false;
}

int32_t Service_GetPlatformType(void)
{
	return PLATFORM_STEAM;
}
bool Service_IsSimplifiedLeaderboard(void)
{
	return false;
}
void Service_FlushStatsToServiceForPlatformX(void)
{
}
bool Service_IsLeaderboardRetrievalInProgress(void)
{
	return false;
}
bool Service_IsShowingLeaderboardCannotConnectError(void)
{
	return true;
}
bool Service_IsSingleUserApplication(void)
{
	return true;
}
bool Service_PlatformHidesOptionChangeControllerGlyphs(void)
{
	return false;
}
bool Service_PlatformHidesOptionExitGame(void)
{
	return false;
}
bool Service_PlatformHidesAllVideoOptions(void)
{
	return false;
}
bool Service_PlatformHidesKeyboardBindings(void)
{
	return false;
}
bool Service_PlatformForcesRelyOnVsync(void)
{
	return false;
}
bool Service_PlatformHidesResetAllBindings(void)
{
	return false;
}
bool Service_PlatformHidesMenuBindings(void)
{
	return false;
}
bool Service_PlatformHidesFocusLossToggles(void)
{
	return false;
}
bool Service_PlatformForcesControllerOnPressStartScreen(void)
{
	return false;
}
bool Service_PlatformDisablesKeyboardUse(void)
{
	return false;
}
bool Service_PlatformHidesResetAllData(void)
{
	return false;
}
bool Service_PlatformForcesSpecificGlyph(void)
{
	return false;
}
int32_t Service_PlatformGetForcedSpecificGlyph(void)
{
	return -1;
}
bool Service_PlatformForcesPlatformNGlyphs(void)
{
	return false;
}
bool Service_PlatformDisablesWritingRecordings(void)
{
	return false;
}
bool Service_PlatformUsesLocalStorageForSaveData(void)
{
	return true;
}
Rectangle Service_PlatformGetForcedBackBufferSize(void)
{
	return Rectangle_Empty;
}
Rectangle Service_PlatformGetForcedWindowSize(void)
{
	return Rectangle_Empty;
}
bool Service_PlatformForcesVsync(void)
{
	return false;
}
bool Service_PlatformForcesFullscreen(void)
{
	return false;
}
void Service_ResetAchievements(void)
{
}
bool Service_PlatformDisablesSimultaneousInputForController(void)
{
	return false;
}
bool Service_PlatformForcesControllerGlyphs(void)
{
	return false;
}
bool Service_PlatformShowsControllerDisconnectScreen(void)
{
	return false;
}
bool Service_PlatformHidesWindowSizeOption(void)
{
	return false;
}
int32_t Service_PlatformAchievementCrawlingTime(void)
{
	return 5;
}
bool Service_PlatformDoesNotDoNormalPausing(void)
{
	return false;
}
bool Service_PlatformGoesToPauseMenuWhenGameIsNotActive(void)
{
	return false;
}
bool Service_PlatformMutesInputWhenGameIsNotActive(void)
{
	return false;
}
bool Service_PlatformStopsPollingInputWhenGameIsNotActive(void)
{
	return false;
}
bool Service_IsSigningIn(void)
{
	return false;
}
double Service_GetPlatformLeaderboardDelayTime(void)
{
	return 0.5;
}
const char* Service_GetPlatformLanguage(void)
{
	return Platform_GetPreferredLocalesEFIGS();
}
bool Service_ShowSignInFailure(void)
{
	return false;
}
int32_t Service_PlatformCrashText(void)
{
	return PLATFORM_CRASH_TEXT_WITH_LOG;
}
bool Service_IsOnlineRightNow(void)
{
	return true;
}
bool Service_PlatformDoesNotAllowLeaderboardScrollingForFriendScope(void)
{
	return false;
}
bool Service_AreNetworkFeaturesAreAvailableRightNow(bool isSilent)
{
	return true;
}
bool Service_LeaveTheLeaderboardMenuRightNow(void)
{
	return false;
}
void Service_TurnOffLeaderboardCannotConnectError(void)
{

}
bool Service_IsWaitingOnServiceToFinishLoading(double delta)
{
	return false;
}
bool Service_SuppressDrawing(void)
{
	return false;
}
bool Service_PlatformDisablesDepthBufferForRender(void)
{
	return true;
}
bool Service_PlatformRequiresOffscreenTargetForRender(void)
{
	return true;
}
bool Service_PlatformCanShowWebsites(void)
{
	return true;
}
bool Service_PlatformShouldLoadWindowIcon(void)
{
	return true;
}
bool Service_PlatformForcesWindowedModeOnBoot(void)
{
	return false;
}
bool Service_PlatformNeedsShaderStutterFix(void)
{
	return false;
}
bool Service_PlatformIsSayingToPauseTheGameIfPossible(void)
{
	return false;
}
void Service_SignalThatCampaignHasBeenStarted(void)
{

}
void Service_SignalThatCampaignHasBeenCompleted(void)
{

}
void Service_SignalThatCampaigHasBeenAbandoned(void)
{

}
void Service_SignalThatCampaigHasBeenFailed(void)
{

}
const char* Service_PlatformSpecificTerminologyFilter(const char* key)
{
	return Strings_Get(key);
}
#endif