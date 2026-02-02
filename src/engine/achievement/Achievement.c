/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Achievement.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../service/Service.h"
#include "../globals/Globals.h"
#include "../utils/Cvars.h"

void Achievement_Init(Achievement* achievement, bool (*condition)(void), const char* name, const char* description, const char* lockedDescription, const char* image, bool isHidden)
{
	Utils_memset(achievement, 0, sizeof(Achievement));

	achievement->mIsCertified = false;
	achievement->mIsHidden = isHidden;
	achievement->mIsUnlocked = false;
	Utils_strlcpy(achievement->mName, name, EE_FILENAME_MAX);
	Utils_strlcpy(achievement->mDescription, description, EE_FILENAME_MAX);
	Utils_strlcpy(achievement->mLockedDescription, lockedDescription, EE_FILENAME_MAX);
	achievement->mCondition = condition;
	achievement->mSheet = Sheet_GetDefaultSheet();
	if (!Utils_StringEqualTo(image, EE_STR_EMPTY))
	{
		achievement->mSheet = Sheet_GetSheet(image);
	}
}
void Achievement_Update(bool doNotCertify, Achievement* achievement, int32_t index)
{
	if (!achievement->mIsUnlocked)
	{
		if (achievement->mCondition())
		{
			Achievement_UnlockAchievement(doNotCertify, achievement, index);
		}
	}
}
void Achievement_UnlockAchievement(bool doNotCertify, Achievement* achievement, int32_t index)
{
	if (achievement->mIsUnlocked)
	{
		return;
	}

	achievement->mIsUnlocked = true;

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Achievement ");
		MString_AddAssignInt(&tempString, index);
		MString_AddAssignString(&tempString, " - ");
		MString_AddAssignString(&tempString, achievement->mName);
		MString_AddAssignString(&tempString, " has been unlocked!");
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	if (!doNotCertify)
	{
		Achievement_CertifyAchievement(achievement, index);
	}
}
void Achievement_CertifyAchievement(Achievement* achievement, int32_t index)
{
	if (!achievement->mIsUnlocked || achievement->mIsCertified || !Service_HasSignedIn() || Globals_AreAchievementsDisabled())
	{
		return;
	}

	achievement->mIsCertified = true;

	if (Cvars_GetAsBool(CVARS_ENGINE_DISABLE_SERVICE_ACHIEVEMENTS))
	{
		return;
	}

	Service_HandleSetAchievement(index, achievement->mName);
}