/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "ServiceLeaderboard.h"
#include "SaveBlobDataRequest.h"
#include "../io/FixedByteBuffer.h"
#include "../math/Rectangle.h"
#include "../input/InputChecks.h"
#include "../input/InputBindings.h"

//supposed to be protected
const int32_t* Service_GetAchievementMap(void);
BufferRequest Service_AskToRetrieveBufferForPC(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path);
int32_t Service_SaveBufferForPC(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path,
	FixedByteBuffer* buffer);

//supposed to be public
typedef enum ServicePlatformCrashText
{
	PLATFORM_CRASH_TEXT_NOTHING = 0,
	PLATFORM_CRASH_TEXT_WITH_LOG = 1,
	PLATFORM_CRASH_TEXT_NO_LOG = 2
} ServicePlatformCrashText;

typedef enum ServicePlatform
{
	PLATFORM_STEAM = 0,
	PLATFORM_X = 1,
	PLATFORM_P5 = 2,
	PLATFORM_N = 3
} ServicePlatform;

void Service_Init(void);
bool Service_IsPlatformSteam(void);
void Service_ResetAchievements(void);
int32_t Service_GetPlatformType(void);
void Service_SetAchievementMap(int32_t* achievementMap, int32_t achievementMapLength);
BufferRequest Service_AskToRetrieveBuffer(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path);
void Service_SaveBuffer(bool isPurelyGameSaveData, const char* containerDisplayName, const char* containerName, const char* path,
	FixedByteBuffer* buffer);
const char* Service_GetSignedInUserName(void);
void Service_SignIn(bool trySilentSignIn, bool forceSignIn);
bool Service_HasSignedIn(void);
bool Service_IsSigningIn(void);
bool Service_IsOverlayEnabled(void);
void Service_Create(void);
void Service_Update(double deltaTime);
void Service_UpdateHelper(double deltaTime);
void Service_Resize(int32_t width, int32_t height);
void Service_Pause(void);
void Service_Resume(void);
void Service_Dispose(void);
void Service_HandleException(const char* e);
void Service_HandleSetAchievement(int32_t index, const char* achievement);
void Service_HandleApplicationExit(void);
bool Service_PlatformDisablesKeyboardUse(void);
bool Service_IsSingleUserApplication(void);
bool Service_PlatformForcesControllerOnPressStartScreen(void);
bool Service_PlatformForcesControllerGlyphs(void);
bool Service_PlatformForcesRelyOnVsync(void);
bool Service_PlatformHidesOptionChangeControllerGlyphs(void);
bool Service_PlatformHidesOptionExitGame(void);
bool Service_PlatformHidesAllVideoOptions(void);
bool Service_PlatformHidesKeyboardBindings(void);
bool Service_PlatformHidesResetAllBindings(void);
bool Service_PlatformHidesMenuBindings(void);
bool Service_PlatformHidesFocusLossToggles(void);
bool Service_PlatformHidesResetAllData(void);
bool Service_PlatformForcesSpecificGlyph(void);
int32_t Service_PlatformGetForcedSpecificGlyph(void);
bool Service_PlatformForcesPlatformNGlyphs(void);
bool Service_PlatformDisablesWritingRecordings(void);
bool Service_PlatformUsesLocalStorageForSaveData(void);
Rectangle Service_PlatformGetForcedWindowSize(void);
Rectangle Service_PlatformGetForcedBackBufferSize(void);
bool Service_PlatformForcesVsync(void);
bool Service_PlatformForcesFullscreen(void);
bool Service_PlatformDisablesSimultaneousInputForController(void);
bool Service_PlatformShowsControllerDisconnectScreen(void);
bool Service_PlatformHidesWindowSizeOption(void);
int32_t Service_PlatformAchievementCrawlingTime(void);
bool Service_PlatformDoesNotDoNormalPausing(void);
bool Service_PlatformGoesToPauseMenuWhenGameIsNotActive(void);
bool Service_PlatformMutesInputWhenGameIsNotActive(void);
bool Service_PlatformStopsPollingInputWhenGameIsNotActive(void);
const char* Service_GetPlatformLanguage(void);
bool Service_ShowSignInFailure(void);
int32_t Service_PlatformCrashText(void);
void Service_LogMessage(const char* message);
bool Service_IsOnlineRightNow(void);
bool Service_AreNetworkFeaturesAreAvailableRightNow(bool isSilent);
bool Service_IsWaitingOnServiceToFinishLoading(double deltaTime);
bool Service_SuppressDrawing(void);
bool Service_PlatformDisablesDepthBufferForRender(void);
bool Service_PlatformRequiresOffscreenTargetForRender(void);
bool Service_PlatformCanShowWebsites(void);
bool Service_PlatformShouldLoadWindowIcon(void);
bool Service_PlatformForcesWindowedModeOnBoot(void);
bool Service_PlatformNeedsShaderStutterFix(void);
bool Service_PlatformIsSayingToPauseTheGameIfPossible(void);
void Service_SignalThatCampaignHasBeenStarted(void);
void Service_SignalThatCampaignHasBeenCompleted(void);
void Service_SignalThatCampaigHasBeenAbandoned(void);
void Service_SignalThatCampaigHasBeenFailed(void);
const char* Service_PlatformSpecificTerminologyFilter(const char* key);