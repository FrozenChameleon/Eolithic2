/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef enum LoggerLevel
{
	LOGGER_LEVEL_NOTHING = 0,
	LOGGER_LEVEL_ERROR = 1,
	LOGGER_LEVEL_WARNING = 2,
	LOGGER_LEVEL_INFORMATION = 3,
	LOGGER_LEVEL_DEBUG = 4
} LoggerLevel;

typedef struct SpriteBatch SpriteBatch;

void Logger_SetLevel(int32_t level);
void Logger_LogInformation(const char* message);
void Logger_LogInformationSilently(const char* message);
void Logger_LogWarning(const char* message);
void Logger_LogWarningSilently(const char* message);
void Logger_LogError(const char* message);
void Logger_LogErrorSilently(const char* message);
void Logger_LogDebug(const char* message);
void Logger_LogDebugSilently(const char* message);
void Logger_Update(void);
void Logger_DrawHud(SpriteBatch* spriteBatch);
void Logger_printf(const char* fmt, ...);