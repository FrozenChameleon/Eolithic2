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
	LOGGER_NOTHING = 0,
	LOGGER_ERROR = 1,
	LOGGER_WARNING = 2,
	LOGGER_INFORMATION = 3,
	LOGGER_DEBUG = 4
} LoggerLevel;

typedef struct SpriteBatch SpriteBatch;

void Logger_SetLevel(LoggerLevel level);
void Logger_Log(LoggerLevel level, const char* message);
void Logger_LogSilently(LoggerLevel level, const char* message);
void Logger_Update(void);
void Logger_DrawHud(SpriteBatch* spriteBatch);
void Logger_printf(const char* fmt, ...);