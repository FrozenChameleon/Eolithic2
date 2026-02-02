/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Logger.h"

#include "../utils/Macros.h"
#include "SDL3/SDL.h"
#include "../globals/Globals.h"
#include "../utils/Timer.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../render/Color.h"
#include "../render/SpriteBatch.h"
#ifndef GLOBAL_DEF_DISABLE_ALL_LOGGING
#include "stdio.h"
#endif

typedef struct LoggerNotification
{
	Vector2 mPosition;
	Timer mTimer;
	Timer mFadeTimer;
	char mText[EE_FILENAME_MAX];
} LoggerNotification;

static const char* _mMessageStr = "INFORMATION : ";
static const char* _mWarningStr = "WARNING : ";
static const char* _mErrorStr = "ERROR : ";

#define TOTAL_TIME 150
#define FADE_OUT_TIME 30
#define HARD_LIMIT_ON_NOTIFICATIONS 10

static int32_t _mLoggerLevel = LOGGER_LEVEL_DEBUG;
static LoggerNotification* arr_notifications;

void Logger_SetLevel(int32_t level)
{
	_mLoggerLevel = level;
}
static void LogHelper(int32_t type, const char* message, bool isSilent)
{
#ifdef GLOBAL_DEF_DISABLE_ALL_LOGGING
	//DO NOTHING
#else
	if ((_mLoggerLevel < type) || message == NULL)
	{
		return;
	}

	const char* strType;
	switch (type)
	{
	case LOGGER_LEVEL_INFORMATION:
		strType = _mMessageStr;
		break;
	case LOGGER_LEVEL_WARNING:
		strType = _mWarningStr;
		break;
	case LOGGER_LEVEL_ERROR:
		strType = _mErrorStr;
		break;
	case LOGGER_LEVEL_DEBUG:
		strType = NULL;
		break;
	default:
		strType = NULL;
		break;
	}

	if (strType == NULL)
	{
		SDL_Log("%s", message);
	}
	else
	{
		SDL_Log("%s%s", strType, message);
	}

#ifdef DEBUG_DEF_ENABLE_CONSOLE_NOTIFICATIONS
	if (!isSilent && Globals_IsGameLoaded() && (arrlen(arr_notifications) < HARD_LIMIT_ON_NOTIFICATIONS))
	{
		LoggerNotification notification = { 0 };
		notification.mPosition = Vector2_Zero;
		notification.mTimer.mLimit = TOTAL_TIME;
		notification.mFadeTimer.mLimit = FADE_OUT_TIME;
		Utils_strlcpy(notification.mText, message, EE_FILENAME_MAX);
		arrput(arr_notifications, notification);
	}
#endif

#endif
}

void Logger_LogInformation(const char* message)
{
	LogHelper(LOGGER_LEVEL_INFORMATION, message, false);
}
void Logger_LogInformationSilently(const char* message)
{
	LogHelper(LOGGER_LEVEL_INFORMATION, message, true);
}
void Logger_LogWarning(const char* message)
{
	LogHelper(LOGGER_LEVEL_WARNING, message, false);
}
void Logger_LogWarningSilently(const char* message)
{
	LogHelper(LOGGER_LEVEL_WARNING, message, true);
}
void Logger_LogError(const char* message)
{
	LogHelper(LOGGER_LEVEL_ERROR, message, false);
}
void Logger_LogErrorSilently(const char* message)
{
	LogHelper(LOGGER_LEVEL_ERROR, message, true);
}
void Logger_LogDebug(const char* message)
{
	LogHelper(LOGGER_LEVEL_DEBUG, message, false);
}
void Logger_LogDebugSilently(const char* message)
{
	LogHelper(LOGGER_LEVEL_DEBUG, message, true);
}
static bool UpdateNotification(LoggerNotification* data, Vector2 initialPosition, int32_t i)
{
	data->mPosition = Vector2_Add(initialPosition, Vector2_Create(0, (float)(i * 8)));

	if (Timer_Update(&data->mTimer))
	{
		return true;
	}
	else
	{
		if (data->mTimer.mCurrent > (TOTAL_TIME - FADE_OUT_TIME))
		{
			Timer_Update(&data->mFadeTimer);
		}
		return false;
	}
}
void Logger_Update(void)
{
	if (arrlen(arr_notifications) <= 0)
	{
		return;
	}

	Vector2 initialPosition = Vector2_Zero;
	/*if (OeGlobals.DEBUG_IS_EDITOR_MODE) //UNUSED
	{
		initialPosition = new Vector2(0, 24);
	}*/
	for (int32_t i = 0; i < arrlen(arr_notifications); i += 1)
	{
		if (UpdateNotification(&arr_notifications[i], initialPosition, i))
		{
			arrdel(arr_notifications, i);
			i -= 1;
		}
	}
}
static void DrawNotification(const LoggerNotification* data, SpriteBatch* spriteBatch)
{
	Color color = COLOR_YELLOW;
	color.A = (uint8_t)(0xFF * Timer_GetInversePercentage(&data->mFadeTimer));
	SpriteBatch_DrawString(spriteBatch, "editor", data->mText, color, 100, data->mPosition);
}
void Logger_DrawHud(SpriteBatch* spriteBatch)
{
	if (arrlen(arr_notifications) <= 0)
	{
		return;
	}

	for (int i = 0; i < arrlen(arr_notifications); i += 1)
	{
		DrawNotification(&arr_notifications[i], spriteBatch);
	}
}
void Logger_printf(const char* fmt, ...)
{
#ifdef GLOBAL_DEF_DISABLE_ALL_LOGGING
	//Do Nothing.
#else
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#endif
}